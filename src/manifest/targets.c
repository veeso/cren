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
