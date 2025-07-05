#include <stdbool.h>

#include <cren.h>
#include <lib/log.h>
#include <manifest.h>
#include <manifest/path.h>
#include <utils/chars.h>

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

cren_manifest_t *cren_manifest_load(const string_t *path)
{
    int rc = CREN_OK;
    FILE *file = NULL;
    cren_manifest_t *manifest = NULL;

    log_info("Checking cren manifest: %.*s", path->length, path->data);

    // open file
    file = fopen(path->data, "r");
    if (file == NULL)
    {
        log_fatal("Failed to open cren manifest: %.*s", path->length, path->data);
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

    // check manifest package name
    if (cren_manifest_validate_name(manifest->package->name->data) != CREN_OK)
    {
        log_fatal("Invalid package name: %s", manifest->package->name->data);
        rc = CREN_NOK;
        goto cleanup;
    }

cleanup:

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

int cren_manifest_validate_name(const char *package_name)
{
    // Validate package name according to Cren's rules
    // - Must start with a letter
    // - Can contain letters, numbers,  and underscores

    if (package_name == NULL || package_name[0] == '\0')
    {
        log_error("Package name cannot be empty");
        return CREN_NOK;
    }

    if (!is_alpha(package_name[0]))
    {
        log_error("Package name must start with a letter");
        return CREN_NOK;
    }

    for (size_t i = 0; package_name[i] != '\0'; i++)
    {
        char c = package_name[i];
        if (!is_alpha(c) && !is_digit(c) && c != '_')
        {
            log_error("Package name can only contain letters, numbers, and underscores");
            return CREN_NOK;
        }
    }

    log_debug("Package name '%s' is valid", package_name);

    return CREN_OK;
}
