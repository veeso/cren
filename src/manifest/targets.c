#include <cren.h>
#include <lib/log.h>
#include <manifest/targets.h>

cren_manifest_targets_t *cren_manifest_targets_init()
{
    cren_manifest_targets_t *targets = (cren_manifest_targets_t *)malloc(sizeof(cren_manifest_targets_t));
    if (targets == NULL)
    {
        log_fatal("Failed to allocate memory for targets");
        return NULL;
    }

    targets->lib = NULL;
    targets->bin = NULL;
    targets->bin_len = 0;
    targets->examples = NULL;
    targets->examples_len = 0;

    log_trace("Initialized new manifest targets");

    return targets;
}

void cren_manifest_targets_free(cren_manifest_targets_t *targets)
{
    if (targets == NULL)
    {
        log_trace("Attempted to free NULL targets");
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

    log_trace("Freed manifest targets");
}

cren_manifest_target_cfg_t *cren_manifest_target_cfg_init()
{
    cren_manifest_target_cfg_t *cfg = (cren_manifest_target_cfg_t *)malloc(sizeof(cren_manifest_target_cfg_t));
    if (cfg == NULL)
    {
        log_fatal("Failed to allocate memory for target cfg");
        return NULL;
    }

    cfg->name = NULL;
    cfg->path = NULL;
    cfg->required_features = NULL;

    log_trace("Initialized new manifest target cfg");

    return cfg;
}

int cren_manifest_targets_add_cfg(cren_manifest_target_cfg_t ***dest, size_t *len, cren_manifest_target_cfg_t *cfg)
{
    size_t new_size = sizeof(cren_manifest_target_cfg_t *) * (*len + 1);
    cren_manifest_target_cfg_t **new_target = (cren_manifest_target_cfg_t **)realloc(*dest, new_size);
    if (new_target == NULL)
    {
        log_fatal("Failed to reallocate memory for targets");
        return CREN_NOK;
    }

    *dest = new_target;
    (*dest)[*len] = cfg;
    (*len)++;

    log_trace("Added new target to targets; new length: %zu", *len);

    return CREN_OK;
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
