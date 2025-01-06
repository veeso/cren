#include <manifest.h>

cren_manifest_t *cren_manifest_init()
{
    cren_manifest_t *manifest = malloc(sizeof(cren_manifest_t));

    if (manifest == NULL)
    {
        return NULL;
    }

    manifest->package = cren_manifest_package_init();
    manifest->targets = cren_manifest_targets_init();
    manifest->dependencies = cren_manifest_dependencies_init();
    manifest->features = cren_manifest_features_init();

    // Check if any of the sub-manifests failed to init
    if (manifest->package == NULL || manifest->targets == NULL || manifest->dependencies == NULL || manifest->features == NULL)
    {
        cren_manifest_free(manifest);
        return NULL;
    }

    return manifest;
}

void cren_manifest_free(cren_manifest_t *manifest)
{
    if (manifest == NULL)
    {
        return NULL;
    }

    cren_manifest_package_free(manifest->package);
    cren_manifest_targets_free(manifest->targets);
    cren_manifest_features_free(manifest->features);
    cren_manifest_dependencies_free(manifest->dependencies);

    free(manifest);
}
