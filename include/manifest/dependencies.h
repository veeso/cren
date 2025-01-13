// Cren manifest dependencies

#ifndef CREN_MANIFEST_DEPENDENCIES_H
#define CREN_MANIFEST_DEPENDENCIES_H

#include <stdlib.h>
#include <stdbool.h>

#include <utils/string_list.h>
#include <utils/string.h>

/// @brief Cren manifest dependency
typedef struct cren_manifest_dependency_t cren_manifest_dependency_t;

/// @brief Cren manifest dependencies
typedef struct cren_manifest_dependencies_t cren_manifest_dependencies_t;

struct cren_manifest_dependencies_t
{
    cren_manifest_dependency_t **dependencies;
    size_t dependencies_len;
    cren_manifest_dependency_t **dev_dependencies;
    size_t dev_dependencies_len;
};

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
cren_manifest_dependencies_t *cren_manifest_dependencies_init(void);

/// @brief Init a dependency cfg
/// @return pointer to dependency cfg
cren_manifest_dependency_t *cren_manifest_dependency_init(void);

/// @brief clone cren manifest dependency
/// @param dep
/// @return cloned dep
cren_manifest_dependency_t *cren_manifest_dependency_clone(cren_manifest_dependency_t *dep);

/// @brief Add new dependency to dependencies
/// @param dest
/// @param len
/// @param dependency
/// @return CREN_OK on success, CREN_NOK on failure
int cren_manifest_dependencies_add_dependency(cren_manifest_dependency_t ***dest, size_t *len, cren_manifest_dependency_t *dependency);

/// @brief free cren manifest dependencies
/// @param dependencies
void cren_manifest_dependencies_free(cren_manifest_dependencies_t *dependencies);

/// @brief free cren manifest dependency
/// @param dependency
void cren_manifest_dependency_free(cren_manifest_dependency_t *dependency);

#endif // CREN_MANIFEST_DEPENDENCIES_H
