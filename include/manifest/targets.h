// Manifest Targets section config

#ifndef CREN_MANIFEST_TARGETS_H
#define CREN_MANIFEST_TARGETS_H

#include <stdlib.h>

#include <utils/string_list.h>
#include <utils/string.h>

/// Config of a manifest target
typedef struct cren_manifest_target_cfg_t cren_manifest_target_cfg_t;

struct cren_manifest_target_cfg_t
{
    string_t *name;
    string_t *path;
    string_list_t *required_features;
};

/// manifest targets
typedef struct cren_manifest_targets_t cren_manifest_targets_t;

struct cren_manifest_targets_t
{
    cren_manifest_target_cfg_t *lib;
    cren_manifest_target_cfg_t **bin;
    size_t bin_len;
    cren_manifest_target_cfg_t **examples;
    size_t examples_len;
};

/// @brief initializes a new cren_manifest_targets
/// @return pointer to targets
cren_manifest_targets_t *cren_manifest_targets_init(void);

/// @brief initializes a new cren_manifest_target_cfg
/// @return pointer to target cfg
cren_manifest_target_cfg_t *cren_manifest_target_cfg_init(void);

/// @brief add new target to targets
/// @param targets
/// @param cfg
/// @return 0 on success, 1 on failure
int cren_manifest_targets_add_cfg(cren_manifest_target_cfg_t ***dest, size_t *len, cren_manifest_target_cfg_t *cfg);

/// @brief free cren manifest targets
/// @param package
void cren_manifest_targets_free(cren_manifest_targets_t *targets);

/// @brief free cren manifest target cfg
/// @param cfg
void cren_manifest_target_cfg_free(cren_manifest_target_cfg_t *cfg);

#endif // CREN_MANIFEST_TARGETS_H
