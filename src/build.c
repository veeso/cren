#include <stdbool.h>
#include <threads.h>

#include <build.h>
#include <build/environment.h>
#include <cren.h>
#include <lib/log.h>
#include <utils/cmd.h>
#include <utils/fs.h>
#include <utils/terminal.h>

typedef struct build_object_args_t
{
    const build_t *build;
    const build_environment_t *env;
    const build_compiler_t *compiler;
    const string_t *objects_dir;
    const source_t *source;
} build_object_args_t;

int build_objects(const build_t *build, const build_environment_t *env, const string_t *objects_dir);
int build_object(void *args);
string_t *compile_object_command(const build_object_args_t *args);
build_compiler_t *get_build_compiler(const build_t *build, const build_environment_t *env);
bool should_build_object(const source_t *source);
void advance_build_object_progress(const build_t *build);

#define OBJECTS_DIR "objects"

// mutex for build objects
mtx_t build_objects_mutex;

// THREAD_SAFE objects count
int build_objects_count = 0;

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

int build_add_target(build_t *build, const char *target, const char *project_dir)
{
    if (build == NULL || build->target_dir == NULL || project_dir == NULL)
    {
        log_error("Attempted to add a target file to a NULL build object.");
        return CREN_NOK;
    }

    string_t *source_path = string_from_cstr(project_dir);
    string_append_path(source_path, target);

    source_t *source = source_init(source_path->data, build->target_dir->data);
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

    // TODO: build dependencies

    // build objects
    objects_dir = string_clone(build->target_dir);
    string_append_path(objects_dir, OBJECTS_DIR);
    if (build_objects(build, env, objects_dir) != CREN_OK)
    {
        log_error("Failed to build objects.");
        rc = CREN_NOK;
        goto cleanup;
    }

cleanup:
    string_free(objects_dir);
    build_environment_free(env);
    return rc;
}

int build_objects(const build_t *build, const build_environment_t *env, const string_t *objects_dir)
{
    int rc = CREN_OK;
    thrd_t **threads = NULL;
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
        if (make_dir(objects_dir->data) != CREN_OK)
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
    threads = (thrd_t **)malloc(sizeof(thrd_t *) * build->sources_len);
    if (threads == NULL)
    {
        log_error("Failed to allocate memory for threads.");
        rc = CREN_NOK;
        goto cleanup;
    }

    for (size_t i = 0; i < build->sources_len; i++)
    {
        source_t *source = build->sources[i];

        // start thread for `build_object`
        thrd_t *thread = (thrd_t *)malloc(sizeof(thrd_t));
        if (thread == NULL)
        {
            log_error("Failed to allocate memory for thread.");
            rc = CREN_NOK;
            goto cleanup;
        }
        threads_len++;

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

        // start thread
        if (thrd_create(thread, build_object, (void *)build_object_args) != thrd_success)
        {
            log_error("Failed to create thread for source %s", source->src->data);
            rc = CREN_NOK;
            goto cleanup;
        }
    }

cleanup:
    if (threads != NULL)
    {
        // join threads
        for (size_t i = 0; i < threads_len; i++)
        {
            log_debug("joining thread %zu", i);
            int thread_res = 0;
            thrd_join(*threads[i], &thread_res);
            free(threads[i]);
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
    build_object_args_t *args = (build_object_args_t *)ctx;
    log_debug("building src %s", args->source->src->data);
    int rc = CREN_OK;

    if (!should_build_object(args->source))
    {
        log_info("skipping source %s, because unchanged since last build", args->source->src->data);
        advance_build_object_progress(args->build);
        rc = CREN_OK;
        goto cleanup;
    }

    string_t *command = compile_object_command(args);
    if (command == NULL)
    {
        log_error("Failed to create compile command for source %s", args->source->src->data);
        rc = CREN_NOK;
        goto cleanup;
    }

    // execute
    const int exec_rc = cmd_exec(command->data);
    if (exec_rc != 0)
    {
        rc = CREN_NOK;
        log_error("Failed to build object %s", args->source->src->data);
        goto cleanup;
    }

    advance_build_object_progress(args->build);

cleanup:
    free(args);

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
            " %cstd=%s %co %s ",
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

void advance_build_object_progress(const build_t *build)
{
    // lock
    mtx_lock(&build_objects_mutex);

    build_objects_count++;

    print_line_and_progress(
        build_objects_count,
        build->sources_len,
        "Building objects",
        "Building object %zu/%zu",
        build_objects_count,
        build->sources_len);

    mtx_unlock(&build_objects_mutex);
}
