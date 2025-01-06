#include <manifest/package.h>

cren_manifest_package_t *cren_manifest_package_init()
{
    cren_manifest_package_t *package = (cren_manifest_package_t *)malloc(sizeof(cren_manifest_package_t));

    if (package == NULL)
    {
        return NULL;
    }

    package->name = string_init();
    package->description = string_init();
    package->version = (semver_t){0, 0, 0};
    package->edition = MANIFEST_EDITION_ONE;
    package->language = C11;
    package->authors = string_list_init();
    package->documentation = string_init();
    package->homepage = string_init();
    package->repository = string_init();
    package->license = LICENSE_NONE;
    package->license_file = string_init();

    // Check if any of the fields failed to initialize
    if (package->name == NULL || package->description == NULL || package->authors == NULL || package->documentation == NULL || package->homepage == NULL || package->repository == NULL || package->license_file == NULL)
    {
        cren_manifest_package_free(package);
        return NULL;
    }

    return package;
}

void cren_manifest_package_free(cren_manifest_package_t *package)
{
    if (package == NULL)
    {
        return;
    }

    string_free(package->name);
    string_free(package->description);
    string_list_free(package->authors);
    string_free(package->documentation);
    string_free(package->homepage);
    string_free(package->repository);
    string_free(package->license_file);

    free(package);
}
