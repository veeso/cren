#include <build.h>
#include <command/build.h>
#include <cren.h>
#include <manifest.h>
#include <manifest/path.h>
#include <lib/log.h>
#include <utils/paths.h>

void log_opts(const args_build_t *args);
build_t *init_build_args(const args_build_t *args, const cren_manifest_t *manifest);
string_t *get_project_dir(const args_build_t *args, const cren_manifest_t *manifest);
int configure_include_dirs(build_t *build_args, const string_t *project_dir);
int configure_target_dir(build_t *build_args, const args_build_t *args, const string_t *project_dir);
int configure_source_files(build_t *build_args, const args_build_t *args, const cren_manifest_t *manifest);
int configure_defines(build_t *build_args, const cren_manifest_feature_t **features, size_t len);
int configure_targets(build_t *build_args, const args_build_t *args, const cren_manifest_t *manifest);
cren_manifest_feature_t **get_enabled_feature(const args_build_t *args, const cren_manifest_t *manifest, size_t *len);

int command_build(const args_build_t *args)
{
    int rc = CREN_OK;
    cren_manifest_t *manifest = NULL;
    build_t *build_args = NULL;
    // log
    log_opts(args);
    // load manifest
    manifest = cren_manifest_load(args->manifest_path != NULL ? args->manifest_path->data : NULL);
    if (manifest == NULL)
    {
        log_error("Error loading manifest");
        rc = CREN_NOK;
        goto cleanup;
    }
    // load build options
    build_args = init_build_args(args, manifest);
    if (build_args == NULL)
    {
        log_error("Error initializing build args");
        rc = CREN_NOK;
        goto cleanup;
    }

    // Build the project
    rc = build_compile(build_args);

cleanup:
    if (manifest)
        cren_manifest_free(manifest);
    if (build_args)
        build_free(build_args);

    return rc;
}

build_t *init_build_args(const args_build_t *args, const cren_manifest_t *manifest)
{
    log_debug("Initializing build args");
    string_t *project_dir = NULL;
    build_t *build_args = build_init();
    cren_manifest_feature_t **enabled_features = NULL;
    size_t enabled_features_len = 0;
    int rc = CREN_OK;
    if (build_args == NULL)
    {
        log_error("Error initializing build args");
        return NULL;
    }

    // set release flag
    build_args->release = args->release;
    log_debug("Release: %d", build_args->release);
    // set language
    build_args->language = manifest->package->language;
    log_debug("Language: %s", language_to_string(build_args->language));

    // get project dir
    project_dir = get_project_dir(args, manifest);
    if (project_dir == NULL)
    {
        log_error("Error getting project dir");
        rc = CREN_NOK;
        goto cleanup;
    }

    // include dir
    if (configure_include_dirs(build_args, project_dir) != CREN_OK)
    {
        log_error("Error configuring include dirs");
        rc = CREN_NOK;
        goto cleanup;
    }

    // target dir
    if (configure_target_dir(build_args, args, project_dir) != CREN_OK)
    {
        log_error("Error configuring target dir");
        rc = CREN_NOK;
        goto cleanup;
    }

    // source files
    if (configure_source_files(build_args, args, manifest) != CREN_OK)
    {
        log_error("Error configuring source files");
        rc = CREN_NOK;
        goto cleanup;
    }

    // targets
    if (configure_targets(build_args, args, manifest) != CREN_OK)
    {
        log_error("Error configuring targets");
        rc = CREN_NOK;
        goto cleanup;
    }

    // enabled features
    enabled_features = get_enabled_feature(args, manifest, &enabled_features_len);
    if (enabled_features == NULL)
    {
        log_error("Error getting enabled features");
        rc = CREN_NOK;
        goto cleanup;
    }

    // configure defines
    // defines
    if (configure_defines(build_args, enabled_features, enabled_features_len) != CREN_OK)
    {
        log_error("Error configuring defines");
        rc = CREN_NOK;
        goto cleanup;
    }

cleanup:
    if (rc != CREN_OK)
    {
        build_free(build_args);
        build_args = NULL;
    }
    string_free(project_dir);
    for (size_t i = 0; i < enabled_features_len; i++)
    {
        cren_manifest_feature_free(enabled_features[i]);
    }
    if (enabled_features)
        free(enabled_features);

    return build_args;
}

string_t *get_project_dir(const args_build_t *args, const cren_manifest_t *manifest)
{
    string_t *manifest_path_s = args->manifest_path != NULL ? args->manifest_path : manifest_path();
    if (manifest_path_s == NULL)
    {
        log_error("Error getting manifest path");
        return NULL;
    }
    string_t *project_dir = parent_dir(manifest_path_s->data);
    if (project_dir == NULL)
    {
        log_error("Error getting project dir");
        string_free(manifest_path_s);
        return NULL;
    }

    string_free(manifest_path_s);

    log_debug("Project dir: %s", project_dir->data);

    return project_dir;
}

int configure_include_dirs(build_t *build_args, const string_t *project_dir)
{
    build_args->include_dirs = string_list_init();
    if (build_args->include_dirs == NULL)
    {
        log_error("Error initializing include dirs");
        return CREN_NOK;
    }
    string_t *include_dir = string_clone(project_dir);
    if (include_dir == NULL)
    {
        log_error("Error cloning project dir");
        return CREN_NOK;
    }
    string_append_path(include_dir, CREN_MANIFEST_INCLUDE);
    string_list_push(build_args->include_dirs, include_dir);
    for (size_t i = 0; i < build_args->include_dirs->nitems; i++)
    {
        log_debug("Include dir: %s", build_args->include_dirs->items[i]->data);
    }

    return CREN_OK;
}

int configure_target_dir(build_t *build_args, const args_build_t *args, const string_t *project_dir)
{
    log_debug("Configuring target dir");
    if (args->target_dir != NULL)
    {
        build_args->target_dir = string_clone(args->target_dir);
        if (build_args->target_dir == NULL)
        {
            log_error("Error cloning target dir");
            return CREN_NOK;
        }
        log_debug("Target dir: %s", build_args->target_dir->data);

        return CREN_OK;
    }

    string_t *target_dir = string_clone(project_dir);
    if (target_dir == NULL)
    {
        log_error("Error cloning project dir");
        return CREN_NOK;
    }
    string_append_path(target_dir, TARGET_DIR);
    if (args->release)
    {
        string_append_path(target_dir, TARGET_DIR_RELEASE);
    }
    else
    {
        string_append_path(target_dir, TARGET_DIR_DEBUG);
    }

    build_args->target_dir = target_dir;
    log_debug("Target dir: %s", build_args->target_dir->data);

    return CREN_OK;
}

int configure_defines(build_t *build_args, const cren_manifest_feature_t **features, size_t len)
{
    log_debug("Configuring defines");
    int rc = CREN_OK;
    build_args->defines = string_list_init();
    if (build_args->defines == NULL)
    {
        log_error("Error initializing defines");
        return CREN_NOK;
    }

    for (size_t i = 0; i < len; i++)
    {
        cren_manifest_feature_t *feature = features[i];
        for (size_t j = 0; j < feature->defines->nitems; j++)
        {
            string_t *define = string_clone(feature->defines->items[j]);
            if (define == NULL)
            {
                log_error("Error cloning define");
                return CREN_NOK;
            }
            string_list_push(build_args->defines, define);
        }
    }

    return CREN_OK;
}

cren_manifest_feature_t **get_enabled_feature(const args_build_t *args, const cren_manifest_t *manifest, size_t *len)
{
    // get enabled features
    int rc = CREN_OK;
    cren_manifest_feature_t **enabled_features_obj = NULL;
    string_list_t *enabled_features = string_list_init();
    if (enabled_features == NULL)
    {
        log_error("Error initializing enabled features");
        return CREN_NOK;
    }
    // if default features are enabled, add them
    if (args->no_default_features == false)
    {
        for (size_t i = 0; i < manifest->features->default_features->nitems; i++)
        {
            string_t *feature = string_clone(manifest->features->default_features->items[i]);
            if (feature == NULL)
            {
                log_error("Error cloning feature");
                rc = CREN_NOK;
                goto cleanup;
            }
            string_list_push(enabled_features, feature);
        }
    }
    // add other features
    for (size_t i = 0; i < args->features->nitems; i++)
    {
        if (!string_list_contains(enabled_features, args->features->items[i]->data))
        {
            string_t *feature = string_clone(args->features->items[i]);
            if (feature == NULL)
            {
                log_error("Error cloning feature");
                rc = CREN_NOK;
                goto cleanup;
            }
            string_list_push(enabled_features, feature);
        }
    }

    // iterate over enabled features
    for (size_t i = 0; i < enabled_features->nitems; i++)
    {
        // get feature
        cren_manifest_feature_t *feature = cren_manifest_features_get_feature(manifest->features, enabled_features->items[i]->data);
        if (feature == NULL)
        {
            log_error("Error getting feature %s", enabled_features->items[i]->data);
            rc = CREN_NOK;
            goto cleanup;
        }
        cren_manifest_feature_t *feature_clone = cren_manifest_feature_clone(feature);
        if (feature_clone == NULL)
        {
            log_error("Error cloning feature");
            rc = CREN_NOK;
            goto cleanup;
        }

        // add feature to enabled features
        cren_manifest_feature_t **tmp = realloc(enabled_features_obj, sizeof(cren_manifest_feature_t *) * (i + 1));
        if (tmp == NULL)
        {
            log_error("Error reallocating enabled features");
            rc = CREN_NOK;
            goto cleanup;
        }
        enabled_features_obj = tmp;
        enabled_features_obj[i] = feature_clone;
    }

    *len = enabled_features->nitems;

cleanup:
    string_list_free(enabled_features);
    if (rc != CREN_OK)
    {
        for (size_t i = 0; i < *len; i++)
        {
            cren_manifest_feature_free(enabled_features_obj[i]);
        }
        return NULL;
    }

    return enabled_features_obj;
}

void log_opts(const args_build_t *args)
{
    log_debug("Build options:");
    log_debug("all-features: %d", args->all_features);
    log_debug("no-default-features: %d", args->no_default_features);
    log_debug("release: %d", args->release);
    log_debug("all-targets: %d", args->all_targets);
    log_debug("bins: %d", args->bins);
    log_debug("examples: %d", args->examples);
    log_debug("lib: %d", args->lib);
    if (args->bin)
        log_debug("bin: %s", args->bin->data);
    if (args->example)
        log_debug("example: %s", args->example->data);
    if (args->target_dir)
        log_debug("target-dir: %s", args->target_dir->data);
    log_debug("features:");
    for (size_t i = 0; i < args->features->nitems; i++)
    {
        log_debug("- %s", args->features->items[i]->data);
    }
}
