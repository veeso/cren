#include <stdbool.h>

#include <cren.h>
#include <lib/log.h>
#include <manifest.h>
#include <manifest/path.h>

cren_manifest_t *cren_manifest_init(void)
{
    cren_manifest_t *manifest = (cren_manifest_t *)malloc(sizeof(cren_manifest_t));

    if (manifest == NULL)
    {
        log_fatal("Failed to allocate memory for manifest");
        return NULL;
    }

    manifest->package = cren_manifest_package_init();
    manifest->targets = cren_manifest_targets_init();
    manifest->dependencies = cren_manifest_dependencies_init();
    manifest->features = cren_manifest_features_init();

    // Check if any of the sub-manifests failed to init
    if (manifest->package == NULL || manifest->targets == NULL || manifest->dependencies == NULL || manifest->features == NULL)
    {
        log_fatal("Failed to init sub-manifests");
        cren_manifest_free(manifest);
        return NULL;
    }

    log_trace("Manifest initialized");

    return manifest;
}

void cren_manifest_free(cren_manifest_t *manifest)
{
    if (manifest == NULL)
    {
        log_trace("Manifest is NULL");
        return;
    }

    cren_manifest_package_free(manifest->package);
    cren_manifest_targets_free(manifest->targets);
    cren_manifest_features_free(manifest->features);
    cren_manifest_dependencies_free(manifest->dependencies);

    free(manifest);

    log_trace("Manifest freed");
}

cren_manifest_t *cren_manifest_load(const char *path)
{
    int rc = CREN_OK;
    string_t *cren_manifest_path = NULL;
    FILE *file = NULL;
    cren_manifest_t *manifest = NULL;
    // get cren manifest or use provided arg
    if (path == NULL)
    {
        cren_manifest_path = manifest_path();
    }
    else
    {
        cren_manifest_path = string_from_cstr(path);
    }

    if (cren_manifest_path == NULL)
    {
        log_fatal("Failed to get cren manifest path");
        rc = CREN_NOK;
        goto cleanup;
    }

    log_info("Checking cren manifest: %.*s", cren_manifest_path->length, cren_manifest_path->data);

    // open file
    file = fopen(cren_manifest_path->data, "r");
    if (file == NULL)
    {
        log_fatal("Failed to open cren manifest: %.*s", cren_manifest_path->length, cren_manifest_path->data);
        rc = CREN_NOK;
        goto cleanup;
    }

    // read manifest
    manifest = cren_manifest_init();
    if (manifest == NULL)
    {
        log_fatal("Failed to init cren manifest");
        goto cleanup;
    }

    // parse manifest
    char error[4096];
    if (cren_manifest_parse(manifest, file, error, 4096) != CREN_OK)
    {
        log_fatal("Failed to parse cren manifest: %s", error);
        rc = CREN_NOK;
        goto cleanup;
    }

cleanup:
    if (cren_manifest_path)
        string_free(cren_manifest_path);
    if (file)
        fclose(file);

    if (rc != CREN_OK)
    {
        if (manifest)
            cren_manifest_free(manifest);
        return NULL;
    }

    return manifest;
}
