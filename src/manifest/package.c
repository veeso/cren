#include <lib/log.h>
#include <manifest/package.h>

cren_manifest_package_t *cren_manifest_package_init(void)
{
    cren_manifest_package_t *package = (cren_manifest_package_t *)malloc(sizeof(cren_manifest_package_t));

    if (package == NULL)
    {
        log_fatal("Failed to allocate memory for manifest package");
        return NULL;
    }

    package->name = NULL;
    package->description = NULL;
    package->version = (semver_t){0, 0, 0};
    package->edition = MANIFEST_EDITION_ONE;
    package->language = C11;
    package->authors = NULL;
    package->documentation = NULL;
    package->homepage = NULL;
    package->repository = NULL;
    package->license = LICENSE_NONE;
    package->license_file = NULL;

    log_trace("Initialized manifest package");

    return package;
}

void cren_manifest_package_free(cren_manifest_package_t *package)
{
    if (package == NULL)
    {
        log_trace("Manifest package is NULL");
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

    log_trace("Freed manifest package");
}
