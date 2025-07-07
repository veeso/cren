#include <stdbool.h>

#ifdef __STDC_NO_THREADS__
#include <lib/tinycthread.h>
#else
#include <threads.h>
#endif // __STDC_NO_THREADS__

#include <build/compile.h>
#include <build/dependency_output.h>
#include <build/manifest.h>
#include <build/uri_loader.h>
#include <cren.h>
#include <lib/log.h>
#include <utils/cmd.h>
#include <utils/fs.h>
#include <utils/paths.h>
#include <utils/terminal.h>

typedef struct build_object_args_t
{
    const build_cfg_t *build;
    const build_environment_t *env;
    const build_compiler_t *compiler;
    const string_t *objects_dir;
    const source_t *source;
    size_t progress_steps;
} build_object_args_t;

int build_compile_project(build_cfg_t *project_build, const manifest_build_config_t *manifest_build_config, const build_environment_t *build_env, dependency_outputs_t *outputs);
int build_compile_project_dependencies(build_cfg_t *project_build, build_cfg_t **dependencies, manifest_build_config_t **dependencies_manifests_cfg, const size_t dependencies_len, const build_environment_t *build_env, dependency_outputs_t *outputs);
int configure_dependencies_build(const build_cfg_t *project_build, const manifest_build_config_t *manifest_build_config, build_cfg_t ***dependencies, manifest_build_config_t ***dependencies_manifests_cfg, size_t *dependencies_len);
build_cfg_t *configure_dependency_build(const build_cfg_t *project_build, const manifest_build_config_t *manifest_build_config, const build_dependency_t *dep, manifest_build_config_t **dep_manifest_cfg);
int build_project(const build_cfg_t *build, const build_environment_t *env, const size_t progress_steps, const string_t *objects_dir);
int build_target_objects(const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir, target_t **targets, size_t targets_len, size_t progress_steps);
int build_objects(const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir, source_t **sources, size_t sources_len, size_t progress_steps);
int build_object(void *args);
int link_target(const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir, const target_t *target, const size_t progress_steps);
int archive_objects(const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir, const target_t *target, const size_t progress_steps);
int link_objects(const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir, const target_t *target, const size_t progress_steps);
string_t *compile_object_command(const build_object_args_t *args);
build_compiler_t *get_build_compiler(const build_cfg_t *build, const build_environment_t *env);
bool should_build_object(const source_t *source);
void advance_build_object_progress(const build_cfg_t *build, const source_t *source, const size_t progress_steps);
size_t get_progress_steps(const build_cfg_t *build);
void append_release_opts(string_t *command, const build_compiler_t *compiler);

#define OBJECTS_DIR "objects"
#define DEPS_DIR "deps"

// mutex for build objects
mtx_t build_objects_mutex;

// THREAD_SAFE objects count
int build_progress = 0;

int build_compile(build_cfg_t *project_build, const manifest_build_config_t *manifest_build_config)
{
    // get build env
    int rc = CREN_OK;
    build_environment_t *build_env = build_environment_init();
    if (build_env == NULL)
    {
        log_error("Failed to initialize build environment.");
        rc = CREN_NOK;
        goto cleanup;
    }

    rc = build_compile_project(project_build, manifest_build_config, build_env, NULL);

cleanup:

    if (build_env != NULL)
        build_environment_free(build_env);

    return rc;
}

/// @brief Compile a project with the provided build configuration. Dependencies are recursively built
/// @param project_build
/// @param manifest_build_config
/// @param dependency
/// @return
int build_compile_project(build_cfg_t *project_build, const manifest_build_config_t *manifest_build_config, const build_environment_t *build_env, dependency_outputs_t *outputs)
{
    bool is_dependency = outputs != NULL;
    // init vars
    int rc = CREN_OK;
    string_t *objects_dir = NULL;
    build_cfg_t **dependencies = NULL;
    size_t dependencies_len = 0;
    manifest_build_config_t **dependencies_manifests_cfg = NULL;

    objects_dir = string_clone(project_build->target_dir);
    string_append_path(objects_dir, OBJECTS_DIR);

    if (project_build == NULL)
    {
        log_error("Attempted to compile a NULL build object.");
        return CREN_NOK;
    }

    // if outputs is not NULL, fill it with the includes and objects
    if (outputs != NULL)
    {
        if ((rc = dependency_outputs_insert(outputs, project_build, build_env, objects_dir)) != CREN_OK)
        {
            log_error("Failed to insert dependency outputs.");
            goto cleanup;
        }
    }

    log_info("Compiling project...");

    // Configure dependencies build
    if ((rc = configure_dependencies_build(project_build, manifest_build_config, &dependencies, &dependencies_manifests_cfg, &dependencies_len)) != CREN_OK)
    {
        log_error("Failed to configure dependencies build.");
        goto cleanup;
    }

    size_t progress_steps = get_progress_steps(project_build);

    // build dependencies
    if ((rc = build_compile_project_dependencies(
             project_build, dependencies, dependencies_manifests_cfg, dependencies_len, build_env, outputs)) != CREN_OK)
    {
        log_error("Failed to compile project dependencies.");
        goto cleanup;
    }

    // build project
    if ((rc = build_project(project_build, build_env, progress_steps, objects_dir)) != CREN_OK)
    {
        log_error("Failed to build project.");
        goto cleanup;
    }

    // Build OK
    if (!is_dependency)
        print_outcome("Finished", "cren build");

cleanup:
    string_free(objects_dir);

    if (dependencies != NULL)
    {
        for (size_t i = 0; i < dependencies_len; i++)
        {
            build_free(dependencies[i]);
            if (dependencies_manifests_cfg != NULL && dependencies_manifests_cfg[i] != NULL)
            {
                manifest_build_config_free(dependencies_manifests_cfg[i]);
            }
        }
        free(dependencies);
        free(dependencies_manifests_cfg);
    }

    return rc;
}

/// @brief Build and compile the dependencies of a project. It also adds the outputs to the project build.
/// @param project_build
/// @param dependencies
/// @param dependencies_manifests_cfg
/// @param dependencies_len
/// @param build_env
/// @param outputs
/// @return CREN_OK on success, CREN_NOK on failure.
int build_compile_project_dependencies(build_cfg_t *project_build, build_cfg_t **dependencies, manifest_build_config_t **dependencies_manifests_cfg, const size_t dependencies_len, const build_environment_t *build_env, dependency_outputs_t *outputs)
{
    int rc = CREN_OK;

    for (size_t i = 0; i < dependencies_len; i++)
    {
        // prepare dependency output
        dependency_outputs_t *dependency_outputs = dependency_outputs_init();
        if (dependency_outputs == NULL)
        {
            log_error("Failed to initialize dependency outputs.");
            rc = CREN_NOK;
            goto cleanup;
        }
        const manifest_build_config_t *dep_manifest_build_config = dependencies_manifests_cfg[i];
        if ((rc = build_compile_project(dependencies[i], dep_manifest_build_config, build_env, dependency_outputs)) != CREN_OK)
        {
            log_error("Failed to build dependency.");
            goto cleanup;
        }
        // concatenate dependency outputs
        if (outputs != NULL)
        {
            if ((rc = dependency_outputs_concat(outputs, dependency_outputs)) != CREN_OK)
            {
                log_error("Failed to concatenate dependency outputs.");
                dependency_outputs_free(dependency_outputs);
                goto cleanup;
            }
        }

        // add dependency outputs to project build
        for (size_t i = 0; i < dependency_outputs->includes->nitems; i++)
        {
            string_t *include = string_clone(dependency_outputs->includes->items[i]);
            if (include == NULL)
            {
                log_error("Failed to clone include %s from dependency outputs.", dependency_outputs->includes->items[i]->data);
                dependency_outputs_free(dependency_outputs);
                rc = CREN_NOK;
                goto cleanup;
            }
            log_debug("Adding include %s from dependency outputs.", include->data);
            if (string_list_push(project_build->include_dirs, include) != CREN_OK)
            {
                log_error("Failed to add include %s to project build.", include->data);
                dependency_outputs_free(dependency_outputs);
                rc = CREN_NOK;
                goto cleanup;
            }
        }

        for (size_t i = 0; i < dependency_outputs->objects->nitems; i++)
        {
            string_t *object = string_clone(dependency_outputs->objects->items[i]);
            if (object == NULL)
            {
                log_error("Failed to clone object %s from dependency outputs.", dependency_outputs->objects->items[i]->data);
                dependency_outputs_free(dependency_outputs);
                rc = CREN_NOK;
                goto cleanup;
            }
            log_debug("Adding object %s from dependency outputs.", object->data);
            if (string_list_push(project_build->objects, object) != CREN_OK)
            {
                log_error("Failed to add object %s to project build.", object->data);
                dependency_outputs_free(dependency_outputs);
                rc = CREN_NOK;
                goto cleanup;
            }
        }

        // free dependency outputs
        dependency_outputs_free(dependency_outputs);
    }

cleanup:

    return rc;
}

/// @brief Configure the dependencies to build for the project.
/// @param project_build
/// @param env
/// @param dependencies
/// @param dependencies_len
/// @return CREN_OK on success, CREN_NOK on failure.
int configure_dependencies_build(const build_cfg_t *project_build, const manifest_build_config_t *manifest_build_config, build_cfg_t ***dependencies, manifest_build_config_t ***dependencies_manifests_cfg, size_t *dependencies_len)
{
    for (size_t i = 0; i < project_build->dependencies_len; i++)
    {
        const build_dependency_t *dep = project_build->dependencies[i];

        manifest_build_config_t *dep_manifest_cfg = NULL;
        build_cfg_t *dep_config = configure_dependency_build(project_build, manifest_build_config, dep, &dep_manifest_cfg);
        if (dep_config == NULL)
        {
            log_error("Failed to configure dependency %s", dep->uri->data);
            return CREN_NOK;
        }
        // add dependency to list
        build_cfg_t **tmp = (build_cfg_t **)realloc(*dependencies, sizeof(build_cfg_t *) * (*dependencies_len + 1));
        if (tmp == NULL)
        {
            log_error("Failed to allocate memory for dependencies.");
            build_free(dep_config);
            return CREN_NOK;
        }
        *dependencies = tmp;
        (*dependencies)[*dependencies_len] = dep_config;

        // add manifest config to list
        manifest_build_config_t **tmp_manifest = (manifest_build_config_t **)realloc(*dependencies_manifests_cfg, sizeof(manifest_build_config_t *) * (*dependencies_len + 1));
        if (tmp_manifest == NULL)
        {
            log_error("Failed to allocate memory for dependencies manifest configs.");
            build_free(dep_config);
            manifest_build_config_free(dep_manifest_cfg);
            return CREN_NOK;
        }
        *dependencies_manifests_cfg = tmp_manifest;
        (*dependencies_manifests_cfg)[*dependencies_len] = dep_manifest_cfg;

        (*dependencies_len)++;
    }

    return CREN_OK;
}

/// @brief Configure the build configuration for a dependency.
/// @param project_build
/// @param manifest_build_config
/// @param dep
/// @return A pointer to the configured build configuration for the dependency, or NULL on failure.
build_cfg_t *configure_dependency_build(const build_cfg_t *project_build, const manifest_build_config_t *manifest_build_config, const build_dependency_t *dep, manifest_build_config_t **dep_manifest_cfg)
{
    log_debug("Configuring dependency build for %s", dep->uri->data);
    build_cfg_t *dep_config = NULL;
    cren_manifest_t *manifest = NULL;
    string_t *dependency_dir = NULL;
    string_t *dependency_manifest_path = NULL;
    manifest_build_config_t *dep_manifest_build_config = NULL;

    // load dependency manifest
    manifest = load_dependency_manifest(dep->name, dep->uri, dep->type, project_build->project_dir, project_build->target_dir, &dependency_dir, &dependency_manifest_path);
    if (manifest == NULL)
    {
        log_error("Failed to load dependency manifest for %s", dep->uri->data);
        goto cleanup;
    }

    // make manifest build configuration
    dep_manifest_build_config = build_from_manifest_init();
    if (dep_manifest_build_config == NULL)
    {
        log_error("Failed to initialize manifest build configuration.");
        goto cleanup;
    }
    dep_manifest_build_config->all_features = false;
    dep_manifest_build_config->no_default_features = !dep->default_features;
    dep_manifest_build_config->release = project_build->release;
    dep_manifest_build_config->all_targets = false;
    dep_manifest_build_config->bins = false;
    dep_manifest_build_config->examples = false;
    dep_manifest_build_config->lib = true; // always build libs for dependencies
    dep_manifest_build_config->manifest_path = string_clone(dependency_manifest_path);
    if (dep_manifest_build_config->manifest_path == NULL)
    {
        log_error("Failed to clone dependency manifest path.");
        goto cleanup;
    }
    dep_manifest_build_config->target_dir = string_clone(project_build->target_dir);
    if (dep_manifest_build_config->target_dir == NULL)
    {
        log_error("Failed to clone target directory for dependency %s", dep->name->data);
        goto cleanup;
    }
    // push 'deps' if target_dir doesn't end with it
    if (!str_ends_with(dep_manifest_build_config->target_dir->data, DEPS_DIR))
    {
        // append DEPS_DIR to target_dir
        string_append_path(dep_manifest_build_config->target_dir, DEPS_DIR);
    }

    // load the manifest into the dependency build configuration
    dep_config = build_config_from_manifest(manifest, dep_manifest_build_config, dependency_dir);
    if (dep_config == NULL)
    {
        log_error("Failed to initialize dependency build configuration.");
        goto cleanup;
    }

    *dep_manifest_cfg = dep_manifest_build_config;
    dep_manifest_build_config = NULL; // prevent double free in cleanup

cleanup:
    if (dep_config == NULL)
    {
        log_error("Failed to initialize dependency build configuration.");
        return NULL;
    }
    if (dependency_dir != NULL)
    {
        string_free(dependency_dir);
    }
    if (dependency_manifest_path != NULL)
    {
        string_free(dependency_manifest_path);
    }
    if (manifest != NULL)
    {
        cren_manifest_free(manifest);
    }
    if (dep_manifest_build_config != NULL)
    {
        manifest_build_config_free(dep_manifest_build_config);
    }

    log_debug("Configured dependency build for %s", dep->name->data);

    return dep_config;
}

/// @brief build provided project.
/// @param build
/// @param env
/// @param progress_steps
/// @return CREN_OK on success, CREN_NOK on failure.
int build_project(const build_cfg_t *build, const build_environment_t *env, const size_t progress_steps, const string_t *objects_dir)
{
    // make objects dir
    int rc = CREN_OK;

    // build objects
    if (build_objects(build, env, objects_dir, build->sources, build->sources_len, progress_steps) != CREN_OK)
    {
        log_error("Failed to build objects.");
        rc = CREN_NOK;
        goto cleanup;
    }

    // build targets
    if (build_target_objects(build, env, objects_dir, build->targets, build->targets_len, progress_steps) != CREN_OK)
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

cleanup:

    return rc;
}

/// @brief Build the target objects from the provided targets.
/// @param build
/// @param env
/// @param objects_dir
/// @param targets
/// @param targets_len
/// @param progress_steps
/// @return CREN_OK on success, CREN_NOK on failure.
int build_target_objects(const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir, target_t **targets, size_t targets_len, size_t progress_steps)
{
    // convert targets to sources
    int rc = CREN_OK;
    source_t **sources = malloc(sizeof(source_t *) * targets_len);
    size_t sources_len = 0;
    if (sources == NULL)
    {
        log_error("Failed to allocate memory for sources.");
        return CREN_NOK;
    }

    for (size_t i = 0; i < targets_len; i++)
    {
        sources[i] = source_from_target(targets[i]);
        if (sources[i] == NULL)
        {
            log_error("Failed to convert target %s to source.", targets[i]->target_name->data);
            rc = CREN_NOK;
            goto cleanup;
        }
        sources_len++;
    }

    // build objects
    if (build_objects(build, env, objects_dir, sources, sources_len, progress_steps) != CREN_OK)
    {
        log_error("Failed to build objects for targets.");
        rc = CREN_NOK;
        goto cleanup;
    }

cleanup:
    // clean all sources
    for (size_t i = 0; i < sources_len; i++)
    {
        if (sources[i] != NULL)
        {
            source_free(sources[i]);
        }
    }
    free(sources);

    return rc;
}

/// @brief Builds the object files for the provided sources. Each object is built in a separate thread.
/// @param build
/// @param env
/// @param objects_dir
/// @param sources
/// @param sources_len
/// @param progress_steps
/// @return CREN_OK on success, CREN_NOK on failure.
int build_objects(const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir, source_t **sources, size_t sources_len, const size_t progress_steps)
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

/// @brief Builds an object file from a source file.
/// @param ctx Context containing the build information. It is a `build_object_args_t` structure.
/// @return CREN_OK on success, CREN_NOK on failure.
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

/// @brief Compose the command to compile an object file from a source file.
/// @param args
/// @return A string containing the command to compile the object file, or NULL on failure.
string_t *compile_object_command(const build_object_args_t *args)
{
    string_t *object_path = get_object_path(args->objects_dir, args->source->obj);
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
            " %cc %cWall %cstd=%s %co %s ",
            option_symbol,
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

    // release opts
    if (args->build->release)
    {
        append_release_opts(command, args->compiler);
    }

    // push source
    string_append(command, " ");
    string_append(command, args->source->src->data);

    string_free(object_path);

    return command;
}

/// @brief Links the target. If the target is a library, it archives the objects first; in any case, it links the objects.
/// @param build
/// @param env
/// @param objects_dir
/// @param target
/// @param progress_steps
/// @return CREN_OK on success, CREN_NOK on failure.
int link_target(const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir, const target_t *target, const size_t progress_steps)
{
    // if target is a library, archive objects
    if (target->type == TARGET_TYPE_LIBRARY && archive_objects(build, env, objects_dir, target, progress_steps) != CREN_OK)
    {
        log_error("Failed to link static library %s", target->target_name->data);
        return CREN_NOK;
    }

    // in any case, link objects
    return link_objects(build, env, objects_dir, target, progress_steps);
}

/// @brief Builds the static library from the objects.
/// @param build
/// @param env
/// @param objects_dir
/// @param target
/// @param progress_steps
/// @return CREN_OK on success, CREN_NOK on failure.
int archive_objects(const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir, const target_t *target, const size_t progress_steps)
{
    int rc = CREN_OK;
    int exit_rc = 0;

    build_compiler_t *ar = env->ar;
    string_t *command = NULL;
    string_t *target_path = get_lib_path(env, build, target);
    if (target_path == NULL)
    {
        log_error("Failed to get library path for target %s", target->target_name->data);
        rc = CREN_NOK;
        goto cleanup;
    }

    command = string_from_cstr(ar->path->data);
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
        "%sLinking (static)%s %s",
        COLOR_HEADER,
        COLOR_RESET,
        target->target_name->data);

    // make command
    string_append(command, " ");

    if (ar->family == COMPILER_FAMILY_MSVC)
    {
        // MSVC uses /OUT: for outputs
        string_append(command, "/OUT:");
        string_append(command, target_path->data);
    }
    else
    {
        string_append(command, "rcs ");
        string_append(command, target_path->data);
    }

    // push objects
    for (size_t i = 0; i < build->sources_len; i++)
    {
        string_t *object_path = get_object_path(objects_dir, build->sources[i]->obj);
        if (object_path == NULL)
        {
            log_error("Failed to clone object path.");
            rc = CREN_NOK;
            goto cleanup;
        }

        string_append(command, " ");
        string_append(command, object_path->data);

        string_free(object_path);
    }

    // push object for target
    string_t *target_object_path = get_object_path(objects_dir, target->obj);
    if (target_object_path == NULL)
    {
        log_error("Failed to clone target object path.");
        rc = CREN_NOK;
        goto cleanup;
    }

    string_append(command, " ");
    string_append(command, target_object_path->data);

    // push dependency objects
    // NOTE: this MUST come AFTER the objects
    for (size_t i = 0; i < build->objects->nitems; i++)
    {
        string_t *object_path = build->objects->items[i];
        string_append(command, " ");
        string_append(command, object_path->data);
    }

    // free
    string_free(target_object_path);

    // execute
    log_info("LINK %s", command->data);
    exit_rc = cmd_exec(command->data);

cleanup:
    if (command != NULL)
        string_free(command);
    if (target_path != NULL)
        string_free(target_path);

    if (rc != CREN_OK)
    {
        log_error("Failed to link static target %s", target->target_name->data);
    }

    if (exit_rc != 0)
    {
        log_error("Failed to link static target %s", target->target_name->data);
        rc = CREN_NOK;
    }

    return rc;
}

/// @brief Links the target. If the target is a library, it archives the objects first; in any case, it links the objects.
/// @param build
/// @param env
/// @param objects_dir
/// @param target
/// @param progress_steps
/// @return
int link_objects(const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir, const target_t *target, const size_t progress_steps)
{
    int rc = CREN_OK;
    int exit_rc = 0;

    string_t *target_path = NULL;
    build_compiler_t *compiler = get_build_compiler(build, env);
    string_t *target_name = NULL;

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
    target_name = string_clone(target->target_name);
    if (target_name == NULL)
    {
        log_error("Failed to clone target name.");
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

    // append '.so' or '.dll' extension based on compiler family if the target is a library
    if (target->type == TARGET_TYPE_LIBRARY)
    {
        if (compiler->family == COMPILER_FAMILY_GCC)
        {
            string_append(target_name, ".so");
        }
        else if (compiler->family == COMPILER_FAMILY_MSVC)
        {
            string_append(target_name, ".dll");
        }
    }

    string_append_path(target_path, target_name->data);

    // push command
    string_append(command, " ");
    string_append_char(command, option_symbol);
    string_append(command, "o ");
    string_append(command, target_path->data);

    // if the target is a library, push shared option
    if (target->type == TARGET_TYPE_LIBRARY)
    {
        if (compiler->family == COMPILER_FAMILY_GCC)
        {
            string_append(command, " ");
            string_append_char(command, option_symbol);
            string_append(command, "shared");
        }
        else if (compiler->family == COMPILER_FAMILY_MSVC)
        {
            // MSVC uses /DLL for shared libraries
            string_append(command, " ");
            string_append_char(command, option_symbol);
            string_append(command, "DLL");
        }
    }

    // Wall
    string_append(command, " ");
    string_append_char(command, option_symbol);
    string_append(command, "Wall");

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

    // release opts
    if (build->release)
    {
        append_release_opts(command, compiler);
    }

    // push objects
    for (size_t i = 0; i < build->sources_len; i++)
    {
        string_t *object_path = get_object_path(objects_dir, build->sources[i]->obj);
        if (object_path == NULL)
        {
            log_error("Failed to clone object path.");
            rc = CREN_NOK;
            goto cleanup;
        }

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

    // push static library objects
    // NOTE: this MUST come AFTER the objects
    for (size_t i = 0; i < build->objects->nitems; i++)
    {
        string_t *lib_path = build->objects->items[i];
        string_append(command, " ");
        string_append(command, lib_path->data);
    }

    // free
    string_free(target_object_path);

    // execute
    log_info("LINK %s", command->data);
    exit_rc = cmd_exec(command->data);

cleanup:

    string_free(command);
    string_free(target_path);
    string_free(target_name);

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

/// @brief Appends the release options to the command string based on the compiler family.
/// @param command The command string to append the options to.
/// @param compiler The compiler to use for building the project.
/// @note For MSVC, it appends `/O2 /GL /Gy /Ot`; for GCC, it appends `-O3 -march=native -s -flto`.
void append_release_opts(string_t *command, const build_compiler_t *compiler)
{

    if (compiler->family == COMPILER_FAMILY_MSVC)
    {
        string_append(command, " /O2 /GL /Gy /Ot");
    }
    else
    {
        string_append(command, " -O3 -march=native -s -flto");
    }
}

/// @brief Tells whether an object file should be built based on the source file's modification time.
/// @param source
/// @return true if the object file should be built, false if it is up to date.
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

/// @brief Gets the compiler to use for building the project based on the language.
/// If the language is C, it returns the C compiler; if it is C++, it returns the C++ compiler.
/// @param build
/// @param env
/// @return The compiler to use for building the project.
build_compiler_t *get_build_compiler(const build_cfg_t *build, const build_environment_t *env)
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

/// @brief Advances the build progress for building an object file.
/// @param build
/// @param source
/// @param progress_steps
void advance_build_object_progress(const build_cfg_t *build, const source_t *source, const size_t progress_steps)
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

/// @brief Get the total number of progress steps for the build process.
/// @param build
/// @return The total number of progress steps for the build process.
size_t get_progress_steps(const build_cfg_t *build)
{
    size_t steps = 0;

    steps += build->sources_len;

    // for each target, add the number of target it has
    for (size_t i = 0; i < build->targets_len; i++)
    {
        target_t *target = build->targets[i];
        if (target->type == TARGET_TYPE_LIBRARY)
        {
            steps += 2; // for libraries we count two steps: one for archiving and one for linking
        }
        else
        {
            steps += 1; // for executable targets, we just count it as one step
        }
    }

    return steps;
}
