#include <manifest/targets.h>

cren_manifest_targets_t *cren_manifest_targets_init()
{
    cren_manifest_targets_t *targets = (cren_manifest_targets_t *)malloc(sizeof(cren_manifest_targets_t));
    if (targets == NULL)
    {
        return NULL;
    }

    targets->lib = NULL;
    targets->bin = NULL;
    targets->bin_len = 0;
    targets->examples = NULL;
    targets->examples_len = 0;

    return targets;
}

void cren_manifest_targets_free(cren_manifest_targets_t *targets)
{
    if (targets == NULL)
    {
        return;
    }

    if (targets->lib != NULL)
    {
        cren_manifest_target_cfg_free(targets->lib);
    }

    if (targets->bin != NULL)
    {
        for (size_t i = 0; i < targets->bin_len; i++)
        {
            cren_manifest_target_cfg_free(targets->bin[i]);
        }
        free(targets->bin);
    }

    if (targets->examples != NULL)
    {
        for (size_t i = 0; i < targets->examples_len; i++)
        {
            cren_manifest_target_cfg_free(targets->examples[i]);
        }
        free(targets->examples);
    }

    free(targets);
}

cren_manifest_target_cfg_t *cren_manifest_target_cfg_init()
{
    cren_manifest_target_cfg_t *cfg = (cren_manifest_target_cfg_t *)malloc(sizeof(cren_manifest_target_cfg_t));
    if (cfg == NULL)
    {
        return NULL;
    }

    cfg->name = NULL;
    cfg->path = NULL;
    cfg->required_features = string_list_init();

    return cfg;
}

int cren_manifest_targets_add_cfg(cren_manifest_target_cfg_t **dest, size_t *len, cren_manifest_target_cfg_t *cfg)
{
    size_t new_size = sizeof(cren_manifest_target_cfg_t *) * (*len + 1);
    cren_manifest_target_cfg_t **new_bin = (cren_manifest_target_cfg_t **)realloc(*dest, new_size);
    if (new_bin == NULL)
    {
        return 1;
    }

    dest = new_bin;
    dest[*len] = cfg;
    (*len)++;

    return 0;
}

void cren_manifest_target_cfg_free(cren_manifest_target_cfg_t *cfg)
{
    if (cfg == NULL)
    {
        return;
    }

    string_free(cfg->name);
    string_free(cfg->path);
    string_list_free(cfg->required_features);

    free(cfg);
}
