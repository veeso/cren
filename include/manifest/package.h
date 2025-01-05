// Manifest package fields

#ifndef CREN_MANIFEST_PACKAGE_H
#define CREN_MANIFEST_PACKAGE_H

#include <stdlib.h>

#include <manifest/package/edition.h>
#include <manifest/package/language.h>
#include <manifest/package/license.h>
#include <manifest/package/semver.h>
#include <utils/char_list.h>

/// manifest package
typedef struct cren_manifest_package cren_manifest_package;

struct cren_manifest_package
{
  char *name;
  size_t name_len;
  char *description;
  size_t description_len;
  semver version;
  edition edition;
  language language;
  char_list_t *authors;
  char *documentation;
  size_t documentation_len;
  char *homepage;
  size_t homepage_len;
  char *repository;
  size_t repository_len;
  license license;
  char *license_file;
  size_t license_file_len;
};

/// @brief initializes a new cren_manifest_package
/// @return pointer to manifest_package
cren_manifest_package *cren_manifest_package_init();

/// @brief free cren manifest package
/// @param package
void cren_manifest_package_free(cren_manifest_package *package);

#endif // CREN_MANIFEST_PACKAGE_H