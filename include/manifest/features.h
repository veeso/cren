// Manifest features

#ifndef CREN_MANIFEST_FEATURES_H
#define CREN_MANIFEST_FEATURES_H

#include <stdlib.h>

#include <utils/char_list.h>

/// Cren manifest features
typedef struct cren_manifest_features cren_manifest_features;

struct cren_manifest_features
{
    char_list_t *default_features;
    cren_manifest_feature **features;
    size_t features_len;
};

/// Feature item in feature section of the manifest
typedef struct cren_manifest_feature cren_manifest_feature;

struct cren_manifest_feature
{
    char *name;
    size_t name_len;
    char_list_t *dependencies;
    char_list_t *defines;
};

/// @brief init cren manifest features
/// @return features
cren_manifest_features *cren_manifest_features_init();

/// @brief cleanup manifest features
/// @param features
void cren_manifest_features_free(cren_manifest_features *features);

#endif // CREN_MANIFEST_FEATURES_H