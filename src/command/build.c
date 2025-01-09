#include <command/build.h>
#include <cren.h>
#include <manifest.h>
#include <lib/log.h>

void log_opts(args_build_t *args);

int command_build(args_build_t *args)
{
    // log
    log_opts(args);
    // load manifest
    cren_manifest_t *manifest = cren_manifest_load(args->manifest_path != NULL ? args->manifest_path->data : NULL);
    if (manifest == NULL)
    {
        log_error("Error loading manifest");
        return CREN_NOK;
    }
    // load build options
    // Build the project

    return CREN_OK;
}

void log_opts(args_build_t *args)
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
