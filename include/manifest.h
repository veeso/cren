// Cren manifest types

#ifndef CREN_MANIFEST_H
#define CREN_MANIFEST_H

#include <stdio.h>
#include <stdlib.h>

#include <manifest/dependencies.h>
#include <manifest/features.h>
#include <manifest/package.h>
#include <manifest/targets.h>

#define CREN_MANIFEST_NAME "Cren.toml"
#define CREN_MANIFEST_SRC "src"
#define CREN_MANIFEST_INCLUDE "include"

// struct containing the layout of the manifest
typedef struct cren_manifest_t cren_manifest_t;

struct cren_manifest_t
{
    cren_manifest_package_t *package;
    cren_manifest_targets_t *targets;
    cren_manifest_dependencies_t *dependencies;
    cren_manifest_features_t *features;
};

/// @brief Init a new manifest
/// @return cren_manifest
cren_manifest_t *cren_manifest_init();

/// @brief Parse a cren manifest file
/// @param manifest
/// @param file to parse
/// @return `CREN_OK` on success
int cren_manifest_parse(cren_manifest_t *manifest, FILE *file, char *error, size_t error_sz);

/// @brief Write manifest to file
/// @param manifest
/// @param file to write; if NULL, write to stdout
/// @return `CREN_OK` on success
int cren_manifest_write(cren_manifest_t *manifest, FILE *file);

/// @brief Free manifest
/// @param manifest
void cren_manifest_free(cren_manifest_t *manifest);

#endif // CREN_MANIFEST_H
