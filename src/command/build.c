#include <stdbool.h>

#include <build.h>
#include <build/compile.h>
#include <build/manifest.h>
#include <command/build.h>
#include <cren.h>
#include <manifest.h>
#include <manifest/path.h>
#include <lib/log.h>
#include <utils/fs.h>
#include <utils/paths.h>

void log_opts(const args_build_t *args);
manifest_build_config_t *build_from_manifest_from_args(const args_build_t *args);

int command_build(const args_build_t *args)
{
    int rc = CREN_OK;
    manifest_build_config_t *manifest_build_args = NULL;
    string_t *manifest_filepath = NULL;
    // log
    log_opts(args);

    if (args->manifest_path == NULL)
    {
        manifest_filepath = manifest_path();
    }
    else
    {
        manifest_filepath = string_clone(args->manifest_path);
    }

    if (manifest_filepath == NULL)
    {
        log_error("Failed to get manifest path");
        rc = CREN_NOK;
        goto cleanup;
    }

    // build from manifest
    manifest_build_args = build_from_manifest_from_args(args);
    if (manifest_build_args == NULL)
    {
        log_error("Error building from manifest");
        rc = CREN_NOK;
        goto cleanup;
    }

    // Build the project
    rc = load_manifest_and_build(manifest_build_args, manifest_filepath);

cleanup:
    if (manifest_build_args)
        manifest_build_config_free(manifest_build_args);
    if (manifest_filepath)
        string_free(manifest_filepath);

    return rc;
}

manifest_build_config_t *build_from_manifest_from_args(const args_build_t *args)
{
    manifest_build_config_t *build_args = build_from_manifest_init();
    if (build_args == NULL)
    {
        log_error("Error initializing build args");
        return NULL;
    }

    build_args->all_targets = args->all_targets;
    build_args->all_features = args->all_features;
    build_args->no_default_features = args->no_default_features;
    build_args->release = args->release;
    build_args->bins = args->bins;
    build_args->examples = args->examples;
    build_args->lib = args->lib;

    build_args->bin = string_clone(args->bin);
    build_args->example = string_clone(args->example);
    build_args->target_dir = string_clone(args->target_dir);
    build_args->manifest_path = string_clone(args->manifest_path);

    if (args->features != NULL)
    {
        build_args->features = string_list_init();
        for (size_t i = 0; i < args->features->nitems; i++)
        {
            string_t *feature = string_clone(args->features->items[i]);
            if (feature == NULL)
            {
                log_error("Error cloning feature");
                manifest_build_config_free(build_args);
                return NULL;
            }
            string_list_push(build_args->features, feature);
        }
    }

    return build_args;
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
    if (args->features != NULL)
    {
        for (size_t i = 0; i < args->features->nitems; i++)
        {
            log_debug("- %s", args->features->items[i]->data);
        }
    }
}
