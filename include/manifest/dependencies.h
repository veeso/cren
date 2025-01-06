// Cren manifest dependencies

#ifndef CREN_MANIFEST_DEPENDENCIES_H
#define CREN_MANIFEST_DEPENDENCIES_H

#include <stdlib.h>
#include <stdbool.h>

#include <utils/string_list.h>
#include <utils/string.h>

/// Cren manifest dependencies
typedef struct cren_manifest_dependencies_t cren_manifest_dependencies_t;

struct cren_manifest_dependencies_t
{
    cren_manifest_dependency_t **dependencies;
    size_t dependencies_len;
};

typedef struct cren_manifest_dependency_t cren_manifest_dependency_t;

struct cren_manifest_dependency_t
{
    string_t *name;
    string_t *git;
    string_t *link;
    bool optional;
    string_list_t *defines;
};

/// @brief init cren manifest dependencies
/// @return cren manifest dependencies
cren_manifest_dependencies_t *cren_manifest_dependencies_init();

/// @brief free cren manifest dependencies
/// @param dependencies
void cren_manifest_dependencies_free(cren_manifest_dependencies_t *dependencies);

#endif // CREN_MANIFEST_DEPENDENCIES_H
