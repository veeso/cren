
#include <build/manifest.h>
#include <cren.h>
#include <lib/log.h>
#include <manifest.h>
#include <manifest/path.h>
#include <utils/fs.h>
#include <utils/paths.h>

int configure_include_dirs(build_t *build_args, const string_t *project_dir);
int configure_target_dir(build_t *build_args, const build_from_manifest_t *args, const string_t *project_dir);
int configure_source_files(build_t *build_args, const build_from_manifest_t *args, const cren_manifest_t *manifest, const string_t *project_dir);
int add_source_files(build_t *build_args, const dirent_t *source_files);
int configure_defines(build_t *build_args, cren_manifest_feature_t **features, size_t len);
int configure_targets(build_t *build_args, const build_from_manifest_t *args, const cren_manifest_t *manifest, const char *project_dir);
int configure_links(build_t *build_args, cren_manifest_dependency_t **dependencies, size_t len);
int get_enabled_feature(const build_from_manifest_t *args, const cren_manifest_t *manifest, cren_manifest_feature_t ***enabled_features, size_t *len);
int get_enabled_dependencies(const build_from_manifest_t *args, const cren_manifest_t *manifest, cren_manifest_dependency_t ***dependencies, size_t *len, cren_manifest_feature_t **enabled_features, size_t enabled_features_len);
bool is_platform_enabled(platform_t *local, platform_t **platforms, size_t len);

build_t *build_from_manifest(const cren_manifest_t *manifest, const build_from_manifest_t *args)
{
    log_debug("Initializing build args");
    string_t *project_dir = NULL;
    build_t *build_args = build_init();
    cren_manifest_feature_t **enabled_features = NULL;
    size_t enabled_features_len = 0;
    cren_manifest_dependency_t **enabled_dependencies = NULL;
    size_t enabled_dependencies_len = 0;
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
    project_dir = get_project_dir(args->manifest_path ? args->manifest_path->data : NULL);
    if (project_dir == NULL)
    {
        log_error("Error getting project dir");
        rc = CREN_NOK;
        goto cleanup;
    }
    log_debug("Project dir: %s", project_dir->data);

    // include dir
    log_debug("Configuring include dirs");
    if (configure_include_dirs(build_args, project_dir) != CREN_OK)
    {
        log_error("Error configuring include dirs");
        rc = CREN_NOK;
        goto cleanup;
    }

    // target dir
    log_debug("Configuring target dir");
    if (configure_target_dir(build_args, args, project_dir) != CREN_OK)
    {
        log_error("Error configuring target dir");
        rc = CREN_NOK;
        goto cleanup;
    }

    // targets
    log_debug("Configuring targets");
    if (configure_targets(build_args, args, manifest, project_dir->data) != CREN_OK)
    {
        log_error("Error configuring targets");
        rc = CREN_NOK;
        goto cleanup;
    }

    // source files
    log_debug("Configuring source files");
    if (configure_source_files(build_args, args, manifest, project_dir) != CREN_OK)
    {
        log_error("Error configuring source files");
        rc = CREN_NOK;
        goto cleanup;
    }

    // enabled features
    log_debug("Configuring enabled features");
    if (get_enabled_feature(args, manifest, &enabled_features, &enabled_features_len) != CREN_OK)
    {
        log_error("Error getting enabled features");
        rc = CREN_NOK;
        goto cleanup;
    }

    // defines
    log_debug("Configuring defines");
    if (configure_defines(build_args, enabled_features, enabled_features_len) != CREN_OK)
    {
        log_error("Error configuring defines");
        rc = CREN_NOK;
        goto cleanup;
    }

    // resolve dependencies
    log_debug("Configuring enabled dependencies");
    if (get_enabled_dependencies(args, manifest, &enabled_dependencies, &enabled_dependencies_len, enabled_features, enabled_features_len) != CREN_OK)
    {
        log_error("Error getting enabled dependencies");
        rc = CREN_NOK;
        goto cleanup;
    }

    // links
    log_debug("Configuring links");
    if (configure_links(build_args, enabled_dependencies, enabled_dependencies_len) != CREN_OK)
    {
        log_error("Error configuring links");
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
    for (size_t i = 0; i < enabled_dependencies_len; i++)
    {
        cren_manifest_dependency_free(enabled_dependencies[i]);
    }
    if (enabled_dependencies)
        free(enabled_dependencies);

    return build_args;
}

build_from_manifest_t *build_from_manifest_init(void)
{
    build_from_manifest_t *args = (build_from_manifest_t *)malloc(sizeof(build_from_manifest_t));
    if (args == NULL)
    {
        return NULL;
    }

    args->all_targets = false;
    args->all_features = false;
    args->no_default_features = false;
    args->bins = false;
    args->examples = false;
    args->lib = false;
    args->release = false;
    args->bin = NULL;
    args->example = NULL;
    args->target_dir = NULL;
    args->features = NULL;
    args->manifest_path = NULL;

    return args;
}

void build_from_manifest_free(build_from_manifest_t *args)
{
    if (args == NULL)
    {
        return;
    }

    string_free(args->bin);
    string_free(args->example);
    string_free(args->target_dir);
    string_list_free(args->features);
    string_free(args->manifest_path);

    free(args);
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

int configure_target_dir(build_t *build_args, const build_from_manifest_t *args, const string_t *project_dir)
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

int configure_defines(build_t *build_args, cren_manifest_feature_t **features, size_t len)
{
    log_debug("Configuring defines");
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

int get_enabled_feature(const build_from_manifest_t *args, const cren_manifest_t *manifest, cren_manifest_feature_t ***enabled_features, size_t *len)
{
    // get enabled features
    int rc = CREN_OK;
    cren_manifest_feature_t **enabled_features_obj = NULL;
    string_list_t *enabled_features_str = string_list_init();
    if (enabled_features == NULL)
    {
        log_error("Error initializing enabled features");
        rc = CREN_NOK;
        goto cleanup;
    }
    // if default features are enabled, add them
    if (!args->no_default_features && manifest->features->default_features != NULL)
    {
        log_debug("Adding default features");
        for (size_t i = 0; i < manifest->features->default_features->nitems; i++)
        {
            string_t *feature = string_clone(manifest->features->default_features->items[i]);
            log_debug("Feature: %s", feature->data);
            if (feature == NULL)
            {
                log_error("Error cloning feature");
                rc = CREN_NOK;
                goto cleanup;
            }
            string_list_push(enabled_features_str, feature);
        }
    }
    // add other features
    if (args->features != NULL)
    {
        log_debug("Adding features");
        for (size_t i = 0; i < args->features->nitems; i++)
        {
            if (!string_list_contains(enabled_features_str, args->features->items[i]->data))
            {
                string_t *feature = string_clone(args->features->items[i]);
                if (feature == NULL)
                {
                    log_error("Error cloning feature");
                    rc = CREN_NOK;
                    goto cleanup;
                }
                string_list_push(enabled_features_str, feature);
            }
        }
    }

    log_debug("Enabled features: %lu", enabled_features_str->nitems);

    // iterate over enabled features
    for (size_t i = 0; i < enabled_features_str->nitems; i++)
    {
        // get feature
        cren_manifest_feature_t *feature = cren_manifest_features_get_feature(manifest->features, enabled_features_str->items[i]->data);
        if (feature == NULL)
        {
            log_error("Error getting feature %s", enabled_features_str->items[i]->data);
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

cleanup:
    *len = enabled_features_str->nitems;
    string_list_free(enabled_features_str);
    if (rc != CREN_OK)
    {
        for (size_t i = 0; i < *len; i++)
        {
            cren_manifest_feature_free(enabled_features_obj[i]);
        }
        if (enabled_features_obj)
            free(enabled_features_obj);
        return CREN_NOK;
    }

    *enabled_features = enabled_features_obj;

    return rc;
}

int get_enabled_dependencies(const build_from_manifest_t *args, const cren_manifest_t *manifest, cren_manifest_dependency_t ***dependencies, size_t *len, cren_manifest_feature_t **enabled_features, size_t enabled_features_len)
{
    // get enabled features
    int rc = CREN_OK;
    *len = 0;
    cren_manifest_dependency_t **enabled_dependencies = NULL;
    platform_t *local = platform_local();
    if (local == NULL)
    {
        log_error("Error getting local platform");
        return CREN_NOK;
    }
    // log platform
    string_t *local_platform_str = platform_to_string(local);
    if (local_platform_str == NULL)
    {
        log_error("Error getting local platform string");
        platform_free(local);
        return CREN_NOK;
    }
    log_info("Local platform: %s", local_platform_str->data);
    string_free(local_platform_str);

    // add dependencies
    for (size_t i = 0; i < manifest->dependencies->dependencies_len; i++)
    {
        // if optional, check if feature is enabled
        cren_manifest_dependency_t *dep = manifest->dependencies->dependencies[i];
        if (dep->optional)
        {
            // check if in enabled features
            for (size_t j = 0; j < enabled_features_len; j++)
            {
                cren_manifest_feature_t *feature = enabled_features[j];
                if (string_list_contains(feature->dependencies, dep->name->data))
                {
                    log_debug("Dependency %s is enabled", dep->name->data);
                    goto dep_checked;
                }
            }

            log_debug("Dependency %s is not enabled", dep->name->data);
            continue;
        }

        // if platform is defined, check if it matches
        if (dep->platforms != NULL && !is_platform_enabled(local, dep->platforms, dep->platforms_len))
        {
            log_debug("Platform not enabled");
            continue;
        }

    dep_checked:;

        // add dependency
        cren_manifest_dependency_t **tmp = realloc(enabled_dependencies, sizeof(cren_manifest_dependency_t *) * (*len + 1));
        if (tmp == NULL)
        {
            log_error("Error reallocating enabled dependencies");
            rc = CREN_NOK;
            goto cleanup;
        }
        enabled_dependencies = tmp;
        enabled_dependencies[*len] = cren_manifest_dependency_clone(dep);
        if (enabled_dependencies[*len] == NULL)
        {
            log_error("Error cloning dependency");
            rc = CREN_NOK;
            goto cleanup;
        }
        (*len)++;
    }

cleanup:
    if (rc != CREN_OK)
    {
        for (size_t i = 0; i < *len; i++)
        {
            cren_manifest_dependency_free(enabled_dependencies[i]);
        }
        if (enabled_dependencies)
            free(enabled_dependencies);
        return CREN_NOK;
    }
    platform_free(local);

    *dependencies = enabled_dependencies;

    return rc;
}

int configure_targets(build_t *build_args, const build_from_manifest_t *args, const cren_manifest_t *manifest, const char *project_dir)
{
    // bins enabled
    if (args->all_targets || args->bins)
    {
        log_debug("Adding bins");
        for (size_t i = 0; i < manifest->targets->bin_len; i++)
        {
            // check if target is enabled
            if (args->bin == NULL || strcmp(args->bin->data, manifest->targets->bin[i]->name->data) == 0)
            {
                cren_manifest_target_cfg_t *cfg = manifest->targets->bin[i];
                if (build_add_target(build_args, cfg->name->data, cfg->path->data, project_dir) != CREN_OK)
                {
                    log_error("Error adding target %s", manifest->targets->bin[i]->name->data);
                    return CREN_NOK;
                }
            }
        }
    }

    // examples enabled
    if (args->all_targets || args->examples)
    {
        log_debug("Adding examples");
        for (size_t i = 0; i < manifest->targets->examples_len; i++)
        {
            // check if target is enabled
            if (args->example == NULL || strcmp(args->example->data, manifest->targets->examples[i]->name->data) == 0)
            {
                cren_manifest_target_cfg_t *cfg = manifest->targets->examples[i];
                if (build_add_target(build_args, cfg->name->data, cfg->path->data, project_dir) != CREN_OK)
                {
                    log_error("Error adding target %s", manifest->targets->examples[i]->name->data);
                    return CREN_NOK;
                }
            }
        }
    }

    // lib enabled
    if (manifest->targets->lib != NULL && (args->all_targets || args->lib))
    {
        log_debug("Adding lib");
        cren_manifest_target_cfg_t *cfg = manifest->targets->lib;
        if (build_add_target(build_args, cfg->name->data, cfg->path->data, project_dir) != CREN_OK)
        {
            log_error("Error adding target %s", manifest->targets->lib->name->data);
            return CREN_NOK;
        }
    }

    return CREN_OK;
}

int configure_source_files(build_t *build_args, const build_from_manifest_t *args, const cren_manifest_t *manifest, const string_t *project_dir)
{
    if (build_args->targets_len == 0)
    {
        log_error("No targets specified");
        return CREN_NOK;
    }

    // get source dir
    string_t *src_dir = string_clone(project_dir);
    if (src_dir == NULL)
    {
        log_error("Error cloning project dir");
        return CREN_NOK;
    }
    string_append_path(src_dir, CREN_MANIFEST_SRC);
    // scan source files
    log_debug("Scanning source files in %s", src_dir->data);
    dirent_t *source_files = scan_dir(src_dir->data);
    string_free(src_dir);
    if (source_files == NULL)
    {
        log_error("Error scanning source files");
        return CREN_NOK;
    }

    // recurse over source files
    int rc = add_source_files(build_args, source_files);
    dirent_free(source_files);

    return rc;
}

int add_source_files(build_t *build_args, const dirent_t *source_files)
{
    // iterate over source files
    for (size_t i = 0; i < source_files->children_count; i++)
    {
        dirent_t *child = source_files->children[i];
        if (child->is_dir)
        {
            if (add_source_files(build_args, child) != CREN_OK)
            {
                return CREN_NOK;
            }
        }
        else if (str_ends_with(child->path, ".c") || str_ends_with(child->path, ".cpp")) // if ends with .c or .cpp
        {
            // source must not be a target
            for (size_t i = 0; i < build_args->targets_len; i++)
            {
                log_trace("Comparing %s with %s", build_args->targets[i]->src->data, child->path);
                if (strcmp(build_args->targets[i]->src->data, child->path) == 0)
                {
                    log_debug("Skipping source file %s (is a target)", child->path);
                    goto next;
                }
            }

            if (build_add_source(build_args, child->path) != CREN_OK)
            {
                log_error("Error adding source file %s", child->path);
                return CREN_NOK;
            }
        }

    next:
        continue;
    }

    return CREN_OK;
}

int configure_links(build_t *build_args, cren_manifest_dependency_t **dependencies, size_t len)
{
    if (build_args->links == NULL)
    {
        build_args->links = string_list_init();
        if (build_args->links == NULL)
        {
            log_error("Error initializing links");
            return CREN_NOK;
        }
    }

    for (size_t i = 0; i < len; i++)
    {
        cren_manifest_dependency_t *dep = dependencies[i];
        if (dep->link != NULL)
        {
            string_t *link = string_clone(dep->link);
            if (link == NULL)
            {
                log_error("Error cloning link");
                return CREN_NOK;
            }
            string_list_push(build_args->links, link);
            log_debug("Added link %s", link->data);
        }
    }

    return CREN_OK;
}

bool is_platform_enabled(platform_t *local, platform_t **platforms, size_t len)
{
    if (len == 0)
    {
        return true;
    }

    for (size_t i = 0; i < len; i++)
    {
        if (platform_matches(local, platforms[i]))
        {
            string_t *platform_str = platform_to_string(platforms[i]);
            log_debug("Platform matches %s", platform_str->data);
            string_free(platform_str);

            return true;
        }
    }

    return false;
}
