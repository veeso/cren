#include <command/manifest.h>
#include <cren.h>
#include <lib/log.h>
#include <manifest.h>
#include <manifest/path.h>

int manifest_check(args_manifest_t *args);

int command_manifest(args_manifest_t *args)
{
    switch (args->cmd)
    {
    case MANIFEST_CMD_CHECK:
        return manifest_check(args);
    default:
        return CREN_NOK;
    }
}

int manifest_check(args_manifest_t *args)
{
    int rc = CREN_OK;

    cren_manifest_t *manifest = cren_manifest_load(args->path != NULL ? args->path->data : NULL);
    if (manifest == NULL)
    {
        log_error("Failed to load manifest");
        return CREN_NOK;
    }

    rc = cren_manifest_write(manifest, NULL);

    puts("Manifest OK");

    cren_manifest_free(manifest);

    return rc;
}
