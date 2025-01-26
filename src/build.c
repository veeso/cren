#include <stdbool.h>
#include <threads.h>

#include <build.h>
#include <build/environment.h>
#include <cren.h>
#include <lib/log.h>
#include <lib/tinycthread.h>
#include <utils/cmd.h>
#include <utils/fs.h>
#include <utils/paths.h>
#include <utils/terminal.h>

typedef struct build_object_args_t
{
    const build_t *build;
    const build_environment_t *env;
    const build_compiler_t *compiler;
    const string_t *objects_dir;
    const source_t *source;
    size_t progress_steps;
} build_object_args_t;

int build_objects(const build_t *build, const build_environment_t *env, const string_t *objects_dir, source_t **sources, size_t sources_len, size_t progress_steps);
int build_object(void *args);
int link_target(const build_t *build, const build_environment_t *env, const string_t *objects_dir, source_t *target, const size_t progress_steps);
string_t *compile_object_command(const build_object_args_t *args);
build_compiler_t *get_build_compiler(const build_t *build, const build_environment_t *env);
bool should_build_object(const source_t *source);
void advance_build_object_progress(const build_t *build, const source_t *source, const size_t progress_steps);
size_t get_progress_steps(const build_t *build);

#define OBJECTS_DIR "objects"

// mutex for build objects
mtx_t build_objects_mutex;

// THREAD_SAFE objects count
int build_progress = 0;

build_t *build_init(void)
{
    build_t *build = malloc(sizeof(build_t));
    if (build == NULL)
    {
        log_fatal("Failed to allocate memory for build object.");
        return NULL;
    }

    build->language = C11;
    build->defines = NULL;
    build->include_dirs = NULL;
    build->target_dir = NULL;
    build->links = NULL;
    build->sources = NULL;
    build->sources_len = 0;
    build->targets = NULL;
    build->targets_len = 0;
    build->release = false;

    return build;
}

void build_free(build_t *build)
{
    if (build == NULL)
    {
        log_debug("Attempted to free a NULL build object.");
        return;
    }

    string_list_free(build->defines);
    string_list_free(build->include_dirs);
    string_free(build->target_dir);
    string_list_free(build->links);

    if (build->sources != NULL)
    {
        for (size_t i = 0; i < build->sources_len; i++)
        {
            source_free(build->sources[i]);
        }
        free(build->sources);
    }

    if (build->targets != NULL)
    {
        for (size_t i = 0; i < build->targets_len; i++)
        {
            source_free(build->targets[i]);
        }
        free(build->targets);
    }

    free(build);
}

int build_add_source(build_t *build, const char *src)
{
    if (build == NULL || build->target_dir == NULL)
    {
        log_error("Attempted to add a source file to a NULL build object.");
        return CREN_NOK;
    }

    source_t *source = source_init(src, build->target_dir->data);
    if (source == NULL)
    {
        log_error("Failed to initialize source object.");
        return CREN_NOK;
    }

    build->sources = realloc(build->sources, sizeof(source_t *) * (build->sources_len + 1));
    if (build->sources == NULL)
    {
        log_error("Failed to reallocate memory for sources.");
        source_free(source);
        return CREN_NOK;
    }

    build->sources[build->sources_len] = source;
    build->sources_len++;

    return CREN_OK;
}

int build_add_target(build_t *build, const char *target, const char *src_path, const char *project_dir)
{
    if (build == NULL || build->target_dir == NULL || target == NULL || src_path == NULL || project_dir == NULL)
    {
        log_error("Attempted to add a target file to a NULL build object.");
        return CREN_NOK;
    }

    string_t *source_path = string_from_cstr(project_dir);
    string_append_path(source_path, src_path);

    source_t *source = target_init(source_path->data, build->target_dir->data, target);
    string_free(source_path);
    if (source == NULL)
    {
        log_error("Failed to initialize source object.");
        return CREN_NOK;
    }

    build->targets = realloc(build->targets, sizeof(source_t *) * (build->targets_len + 1));
    if (build->targets == NULL)
    {
        log_error("Failed to reallocate memory for targets.");
        source_free(source);
        return CREN_NOK;
    }

    build->targets[build->targets_len] = source;
    build->targets_len++;

    return CREN_OK;
}

int build_compile(build_t *build)
{
    if (build == NULL)
    {
        log_error("Attempted to compile a NULL build object.");
        return CREN_NOK;
    }

    log_info("Compiling project...");
    // get build env
    int rc = CREN_OK;
    build_environment_t *env = build_environment_init();
    string_t *objects_dir = NULL;
    if (env == NULL)
    {
        log_error("Failed to initialize build environment.");
        rc = CREN_NOK;
        goto cleanup;
    }

    const size_t progress_steps = get_progress_steps(build);

    // TODO: build dependencies

    // make objects dir
    objects_dir = string_clone(build->target_dir);
    string_append_path(objects_dir, OBJECTS_DIR);

    // build objects
    if (build_objects(build, env, objects_dir, build->sources, build->sources_len, progress_steps) != CREN_OK)
    {
        log_error("Failed to build objects.");
        rc = CREN_NOK;
        goto cleanup;
    }

    // build targets
    if (build_objects(build, env, objects_dir, build->targets, build->targets_len, progress_steps) != CREN_OK)
    {
        log_error("Failed to build targets.");
        rc = CREN_NOK;
        goto cleanup;
    }

    // link targets
    for (size_t i = 0; i < build->targets_len; i++)
    {
        if (link_target(build, env, objects_dir, build->targets[i], progress_steps) != CREN_OK)
        {
            log_error("Failed to link target %s", build->targets[i]->target_name->data);
            rc = CREN_NOK;
            goto cleanup;
        }
    }

    // TODO: release build

    // Build OK
    print_outcome("Finished", "cren build");

cleanup:
    string_free(objects_dir);
    build_environment_free(env);

    return rc;
}

int build_objects(const build_t *build, const build_environment_t *env, const string_t *objects_dir, source_t **sources, size_t sources_len, const size_t progress_steps)
{
    int rc = CREN_OK;
    thrd_t *threads = NULL;
    size_t threads_len = 0;
    build_compiler_t *compiler = get_build_compiler(build, env);
    if (compiler == NULL)
    {
        log_error("Failed to get build compiler for language %s", language_to_string(build->language));
        rc = CREN_NOK;
        goto cleanup;
    }

    // make objects_dir
    dirent_t *objects_stat = dirent_stat(objects_dir->data);
    if (objects_stat == NULL)
    {
        log_debug("Objects directory (%s) does not exist, creating it.", objects_dir->data);
        if (make_dir_recursive(objects_dir->data) != CREN_OK)
        {
            log_error("Failed to create objects directory.");
            rc = CREN_NOK;
            goto cleanup;
        }
    }
    dirent_free(objects_stat);

    // init mutex
    if (mtx_init(&build_objects_mutex, mtx_plain) != thrd_success)
    {
        log_error("Failed to initialize build objects mutex.");
        rc = CREN_NOK;
        goto cleanup;
    }

    // create thread list
    threads = (thrd_t *)malloc(sizeof(thrd_t) * sources_len);
    if (threads == NULL)
    {
        log_error("Failed to allocate memory for threads.");
        rc = CREN_NOK;
        goto cleanup;
    }

    for (size_t i = 0; i < sources_len; i++)
    {
        source_t *source = sources[i];

        // build object args
        build_object_args_t *build_object_args = malloc(sizeof(build_object_args_t));
        if (build == NULL)
        {
            log_error("Failed to allocate memory for build object args.");
            rc = CREN_NOK;
            goto cleanup;
        }
        build_object_args->build = build;
        build_object_args->env = env;
        build_object_args->compiler = compiler;
        build_object_args->objects_dir = objects_dir;
        build_object_args->source = source;
        build_object_args->progress_steps = progress_steps;

        // start thread
        if (thrd_create(&threads[i], build_object, (void *)build_object_args) != thrd_success)
        {
            log_error("Failed to create thread for source %s", source->src->data);
            rc = CREN_NOK;
            goto cleanup;
        }
        threads_len++;
    }

cleanup:
    if (threads != NULL)
    {
        // join threads
        for (size_t i = 0; i < threads_len; i++)
        {
            log_debug("joining thread %zu", i);
            int thread_res = 0;
            thrd_join(threads[i], &thread_res);
            log_debug("joined thread %zu", i);
            if (thread_res != CREN_OK)
            {
                log_error("Thread %zu failed with code %d", i, thread_res);
                rc = CREN_NOK;
            }
        }
        // free threads
        free(threads);
    }

    // free mutex
    mtx_destroy(&build_objects_mutex);

    return rc;
}

int build_object(void *ctx)
{
    string_t *command = NULL;
    string_t *object_path = NULL;
    string_t *parent = NULL;
    build_object_args_t *args = (build_object_args_t *)ctx;
    log_debug("building src %s", args->source->src->data);
    int rc = CREN_OK;

    if (!should_build_object(args->source))
    {
        log_info("skipping source %s, because unchanged since last build", args->source->src->data);
        advance_build_object_progress(args->build, args->source, args->progress_steps);
        rc = CREN_OK;
        goto cleanup;
    }

    // init dir
    object_path = string_clone(args->objects_dir);
    string_append_path(object_path, args->source->obj->data);
    if (object_path == NULL)
    {
        log_error("Failed to create object path.");
        rc = CREN_NOK;
        goto cleanup;
    }
    // get parent dir
    log_debug("getting parent dir for object %s", object_path->data);
    parent = parent_dir(object_path->data);
    if (parent == NULL)
    {
        log_error("Failed to get parent dir for object %s", object_path->data);
        rc = CREN_NOK;
        goto cleanup;
    }
    // create parent dir
    log_debug("creating object dir %s", parent->data);
    if (make_dir_recursive(parent->data) != CREN_OK)
    {
        log_warn("Failed to create parent dir for object %s", object_path->data);
    }

    command = compile_object_command(args);
    if (command == NULL)
    {
        log_error("Failed to create compile command for source %s", args->source->src->data);
        rc = CREN_NOK;
        goto cleanup;
    }

    // execute
    log_info("CC %s", command->data);
    const int exec_rc = cmd_exec(command->data);
    if (exec_rc != 0)
    {
        rc = CREN_NOK;
        log_error("Failed to build object %s", args->source->src->data);
        goto cleanup;
    }

    advance_build_object_progress(args->build, args->source, args->progress_steps);

cleanup:
    free(args);
    string_free(command);
    string_free(object_path);
    string_free(parent);

    return rc;
}

string_t *compile_object_command(const build_object_args_t *args)
{
    string_t *object_path = string_clone(args->objects_dir);
    string_append_path(object_path, args->source->obj->data);
    if (object_path == NULL)
    {
        log_error("Failed to create object path.");
        return NULL;
    }

    char option_symbol = '-';
    if (args->compiler->family == COMPILER_FAMILY_MSVC)
    {
        option_symbol = '/';
    }

    // build command
    string_t *command = string_from_cstr(args->compiler->path->data);
    if (command == NULL)
    {
        log_error("Failed to create command string.");
        string_free(object_path);
        return NULL;
    }
    // std options
    char common_opts[2048] = {0};
    sprintf(common_opts,
            " %cc %cstd=%s %co %s ",
            option_symbol,
            option_symbol,
            language_to_string(args->build->language),
            option_symbol,
            object_path->data);

    // push common options
    string_append(command, common_opts);
    // push defines
    for (size_t i = 0; i < args->build->defines->nitems; i++)
    {
        string_append(command, " ");
        string_append_char(command, option_symbol);
        string_append(command, "D");
        string_append(command, args->build->defines->items[i]->data);
    }

    // push include dirs
    for (size_t i = 0; i < args->build->include_dirs->nitems; i++)
    {
        string_append(command, " ");
        string_append_char(command, option_symbol);
        string_append(command, "I");
        string_append(command, args->build->include_dirs->items[i]->data);
    }

    // push source
    string_append(command, " ");
    string_append(command, args->source->src->data);

    string_free(object_path);

    return command;
}

int link_target(const build_t *build, const build_environment_t *env, const string_t *objects_dir, source_t *target, const size_t progress_steps)
{
    int rc = CREN_OK;
    int exit_rc = 0;

    string_t *target_path = NULL;
    build_compiler_t *compiler = get_build_compiler(build, env);
    string_t *command = string_from_cstr(compiler->path->data);
    if (command == NULL)
    {
        log_error("Failed to create command string.");
        return CREN_NOK;
    }
    if (target->target_name == NULL)
    {
        log_error("Target name is NULL.");
        rc = CREN_NOK;
        goto cleanup;
    }

    // print progress
    print_line_and_progress(
        ++build_progress,
        progress_steps,
        "Building objects",
        "%sLinking%s %s",
        COLOR_HEADER,
        COLOR_RESET,
        target->target_name->data);

    char option_symbol = '-';
    if (compiler->family == COMPILER_FAMILY_MSVC)
    {
        option_symbol = '/';
    }

    // make command

    target_path = string_clone(build->target_dir);
    if (target_path == NULL)
    {
        log_error("Failed to clone target path.");
        rc = CREN_NOK;
        goto cleanup;
    }

    string_append_path(target_path, target->target_name->data);

    // push command
    string_append(command, " ");
    string_append_char(command, option_symbol);
    string_append(command, "o ");
    string_append(command, target_path->data);

    // push standard
    string_append(command, " ");
    string_append_char(command, option_symbol);
    string_append(command, "std=");
    string_append(command, language_to_string(build->language));

    // push dependencies
    for (size_t i = 0; i < build->links->nitems; i++)
    {
        string_append(command, " ");
        string_append_char(command, option_symbol);
        string_append(command, "l");
        string_append(command, build->links->items[i]->data);
    }

    // link libc
    string_append(command, " ");
    string_append_char(command, option_symbol);
    string_append(command, "lc");

    // push objects
    for (size_t i = 0; i < build->sources_len; i++)
    {
        string_t *object_path = string_clone(objects_dir);
        if (object_path == NULL)
        {
            log_error("Failed to clone object path.");
            rc = CREN_NOK;
            goto cleanup;
        }
        string_append_path(object_path, build->sources[i]->obj->data);

        string_append(command, " ");
        string_append(command, object_path->data);

        string_free(object_path);
    }
    // push object for target
    string_t *target_object_path = string_clone(objects_dir);
    if (target_object_path == NULL)
    {
        log_error("Failed to clone target object path.");
        rc = CREN_NOK;
        goto cleanup;
    }
    string_append_path(target_object_path, target->obj->data);

    string_append(command, " ");
    string_append(command, target_object_path->data);

    // free
    string_free(target_object_path);

    // execute
    exit_rc = cmd_exec(command->data);

cleanup:

    string_free(command);
    string_free(target_path);

    if (rc != CREN_OK)
    {
        log_error("Failed to link target %s", target->target_name->data);
    }

    if (exit_rc != 0)
    {
        log_error("Failed to link target %s", target->target_name->data);
        rc = CREN_NOK;
    }

    return rc;
}

bool should_build_object(const source_t *source)
{
    // stat object and source
    dirent_t *src_stat = dirent_stat(source->src->data);
    if (src_stat == NULL)
    {
        log_error("Failed to stat source file %s", source->src->data);
        return true;
    }
    dirent_t *obj_stat = dirent_stat(source->obj->data);
    // if object exists and is newer than source, skip
    if (obj_stat != NULL && obj_stat->mtime >= src_stat->mtime)
    {
        log_debug("Object file %s is up to date.", source->obj->data);
        dirent_free(obj_stat);
        dirent_free(src_stat);
        return false;
    }

    dirent_free(obj_stat);
    dirent_free(src_stat);

    return true;
}

build_compiler_t *get_build_compiler(const build_t *build, const build_environment_t *env)
{
    if (language_is_c(build->language))
    {
        return env->cc;
    }
    else
    {
        return env->cxx;
    }
}

void advance_build_object_progress(const build_t *build, const source_t *source, const size_t progress_steps)
{
    // lock
    mtx_lock(&build_objects_mutex);

    build_progress++;

    print_line_and_progress(
        build_progress,
        progress_steps,
        "Building objects",
        "%sBuilding%s object %s",
        COLOR_HEADER,
        COLOR_RESET,
        source->obj->data);

    mtx_unlock(&build_objects_mutex);
}

size_t get_progress_steps(const build_t *build)
{
    size_t steps = 0;

    steps += build->sources_len;
    steps += build->targets_len;
    steps += 1; // for linking

    return steps;
}
