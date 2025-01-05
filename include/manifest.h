// Cren manifest types

#ifndef CREN_MANIFEST_H
#define CREN_MANIFEST_H

#include <stdio.h>
#include <stdlib.h>

#include <manifest/dependencies.h>
#include <manifest/error.h>
#include <manifest/features.h>
#include <manifest/package.h>
#include <manifest/targets.h>

// struct containing the layout of the manifest
typedef struct cren_manifest cren_manifest;

struct cren_manifest
{
    cren_manifest_package *package;
    cren_manifest_targets *targets;
    cren_manifest_dependencies *dependencies;
    cren_manifest_features *features;
};

/// @brief Init a new manifest
/// @return cren_manifest
cren_manifest *cren_manifest_init();

/// @brief Parse a cren manifest file
/// @param manifest
/// @param file to parse
/// @return manifest_parse_error_msg
manifest_parse_error cren_manifest_parse(cren_manifest *manifest, FILE *file);

/// @brief Free manifest
/// @param manifest
void cren_manifest_free(cren_manifest *manifest);

#endif // CREN_MANIFEST_H
