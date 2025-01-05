// Manifest Targets section config

#ifndef CREN_MANIFEST_TARGETS_H
#define CREN_MANIFEST_TARGETS_H

#include <stdlib.h>

#include <utils/string_list.h>
#include <utils/string.h>

/// manifest targets
typedef struct cren_manifest_targets cren_manifest_targets;

struct cren_manifest_targets
{
    cren_manifest_target_cfg *lib;
    cren_manifest_target_cfg **bin;
    size_t bin_len;
    cren_manifest_target_cfg **examples;
    size_t examples_len;
};

/// Config of a manifest target
typedef struct cren_manifest_target_cfg cren_manifest_target_cfg;

struct cren_manifest_target_cfg
{
    string_t *name;
    string_t *path;
    string_list_t *required_features;
};

/// @brief initializes a new cren_manifest_targets
/// @return pointer to targets
cren_manifest_targets *cren_manifest_targets_init();

/// @brief free cren manifest targets
/// @param package
void cren_manifest_targets_free(cren_manifest_targets *targets);

#endif // CREN_MANIFEST_TARGETS_H
