// Cren manifest dependencies

#ifndef CREN_MANIFEST_DEPENDENCIES_H
#define CREN_MANIFEST_DEPENDENCIES_H

#include <stdlib.h>
#include <stdbool.h>

#include <utils/char_list.h>

/// Cren manifest dependencies
typedef struct cren_manifest_dependencies cren_manifest_dependencies;

struct cren_manifest_dependencies
{
    cren_manifest_dependency **dependencies;
    size_t dependencies_len;
};

typedef struct cren_manifest_dependency cren_manifest_dependency;

struct cren_manifest_dependency
{
    char *name;
    size_t name_len;
    char *git;
    size_t git_len;
    char *link;
    size_t link_len;
    bool optional;
    char_list_t *defines;
};

/// @brief init cren manifest dependencies
/// @return cren manifest dependencies
cren_manifest_dependencies *cren_manifest_dependencies_init();

/// @brief free cren manifest dependencies
/// @param dependencies
void cren_manifest_dependencies_free(cren_manifest_dependencies *dependencies);

#endif // CREN_MANIFEST_DEPENDENCIES_H