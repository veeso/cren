#include <cren.h>
#include <lib/log.h>
#include <manifest/features.h>

cren_manifest_features_t *cren_manifest_features_init()
{
    cren_manifest_features_t *features = (cren_manifest_features_t *)malloc(sizeof(cren_manifest_features_t));
    if (features == NULL)
    {
        log_fatal("Failed to allocate memory for manifest features");
        return NULL;
    }

    features->default_features = NULL;
    features->features = NULL;
    features->features_len = 0;

    log_trace("Initialized manifest features");

    return features;
}

void cren_manifest_features_free(cren_manifest_features_t *features)
{
    if (features == NULL)
    {
        log_trace("Manifest features is NULL");
        return;
    }

    if (features->default_features != NULL)
    {
        log_trace("Freeing default features");
        string_list_free(features->default_features);
    }

    if (features->features != NULL)
    {
        log_trace("Freeing features");
        for (size_t i = 0; i < features->features_len; i++)
        {
            log_trace("Freeing feature %zu", i);
            cren_manifest_feature_free(features->features[i]);
        }
        free(features->features);
    }

    free(features);

    log_trace("Freed manifest features");
}

void cren_manifest_feature_free(cren_manifest_feature_t *feature)
{
    if (feature == NULL)
    {
        log_trace("Manifest feature is NULL");
        return;
    }

    string_free(feature->name);
    string_list_free(feature->dependencies);
    string_list_free(feature->defines);

    free(feature);

    log_trace("Freed manifest feature");
}

cren_manifest_feature_t *cren_manifest_feature_init()
{
    cren_manifest_feature_t *feature = (cren_manifest_feature_t *)malloc(sizeof(cren_manifest_feature_t));
    if (feature == NULL)
    {
        log_fatal("Failed to allocate memory for manifest feature");
        return NULL;
    }

    feature->name = NULL;
    feature->dependencies = NULL;
    feature->defines = NULL;

    log_trace("Initialized manifest feature");

    return feature;
}

int cren_manifest_features_add_feature(cren_manifest_features_t *features, cren_manifest_feature_t *feature)
{
    if (features == NULL || feature == NULL)
    {
        log_error("One or more arguments are NULL");
        return CREN_NOK;
    }

    cren_manifest_feature_t **new_features = (cren_manifest_feature_t **)realloc(features->features, (features->features_len + 1) * sizeof(cren_manifest_feature_t *));
    if (new_features == NULL)
    {
        log_error("Failed to reallocate memory for features");
        return CREN_NOK;
    }

    features->features = new_features;
    features->features[features->features_len] = feature;
    features->features_len++;

    log_trace("Added feature `%.*s`", feature->name->length, feature->name->data);

    return CREN_OK;
}
