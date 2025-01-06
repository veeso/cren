// Manifest package fields

#ifndef CREN_MANIFEST_PACKAGE_H
#define CREN_MANIFEST_PACKAGE_H

#include <stdlib.h>

#include <manifest/package/edition.h>
#include <manifest/package/language.h>
#include <manifest/package/license.h>
#include <manifest/package/semver.h>
#include <utils/string_list.h>
#include <utils/string.h>

/// manifest package
typedef struct cren_manifest_package_t cren_manifest_package_t;

struct cren_manifest_package_t
{
    string_t *name;
    string_t *description;
    semver_t version;
    edition_t edition;
    language_t language;
    string_list_t *authors;
    string_t *documentation;
    string_t *homepage;
    string_t *repository;
    license_t license;
    string_t *license_file;
};

/// @brief initializes a new cren_manifest_package
/// @return pointer to manifest_package
cren_manifest_package_t *cren_manifest_package_init();

/// @brief free cren manifest package
/// @param package
void cren_manifest_package_free(cren_manifest_package_t *package);

#endif // CREN_MANIFEST_PACKAGE_H
