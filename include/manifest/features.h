// Manifest features

#ifndef CREN_MANIFEST_FEATURES_H
#define CREN_MANIFEST_FEATURES_H

#include <stdlib.h>

#include <utils/string_list.h>

/// Cren manifest features
typedef struct cren_manifest_features_t cren_manifest_features_t;

struct cren_manifest_features_t
{
    string_list_t *default_features;
    cren_manifest_feature_t **features;
    size_t features_len;
};

/// Feature item in feature section of the manifest
typedef struct cren_manifest_feature_t cren_manifest_feature_t;

struct cren_manifest_feature_t
{
    string_t *name;
    string_list_t *dependencies;
    string_list_t *defines;
};

/// @brief init cren manifest features
/// @return features
cren_manifest_features_t *cren_manifest_features_init();

/// @brief cleanup manifest features
/// @param features
void cren_manifest_features_free(cren_manifest_features_t *features);

#endif // CREN_MANIFEST_FEATURES_H
