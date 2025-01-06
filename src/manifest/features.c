#include <manifest/features.h>

cren_manifest_features_t *cren_manifest_features_init()
{
    cren_manifest_features_t *features = (cren_manifest_features_t *)malloc(sizeof(cren_manifest_features_t));
    if (features == NULL)
    {
        return NULL;
    }

    features->default_features = string_list_init();
    features->features = NULL;
    features->features_len = 0;

    // check if default features is initialized
    if (features->default_features == NULL)
    {
        cren_manifest_features_free(features);
        return NULL;
    }

    return features;
}

void cren_manifest_features_free(cren_manifest_features_t *features)
{
    if (features == NULL)
    {
        return;
    }

    if (features->default_features != NULL)
    {
        string_list_free(features->default_features);
    }

    if (features->features != NULL)
    {
        for (size_t i = 0; i < features->features_len; i++)
        {
            cren_manifest_feature_free(features->features[i]);
        }
        free(features->features);
    }

    free(features);
}

void cren_manifest_feature_free(cren_manifest_feature_t *feature)
{
    if (feature == NULL)
    {
        return;
    }

    string_free(feature->name);
    string_list_free(feature->dependencies);
    string_list_free(feature->defines);

    free(feature);
}
