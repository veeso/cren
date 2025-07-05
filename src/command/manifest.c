#include <command/manifest.h>
#include <cren.h>
#include <lib/log.h>
#include <manifest.h>
#include <manifest/path.h>

int manifest_check(const args_manifest_t *args);

int command_manifest(const args_manifest_t *args)
{
    switch (args->cmd)
    {
    case MANIFEST_CMD_CHECK:
        return manifest_check(args);
    default:
        return CREN_NOK;
    }
}

int manifest_check(const args_manifest_t *args)
{
    int rc = CREN_OK;

    string_t *manifest_filepath = NULL;
    cren_manifest_t *manifest = NULL;

    if (args->path != NULL)
    {
        manifest_filepath = string_clone(args->path);
    }
    else
    {
        manifest_filepath = manifest_path();
    }

    if (manifest_filepath == NULL)
    {
        log_error("Error getting manifest path");
        rc = CREN_NOK;
        goto cleanup;
    }

    // load manifest
    manifest = cren_manifest_load(manifest_filepath);
    if (manifest == NULL)
    {
        log_error("Failed to load manifest");
        return CREN_NOK;
    }

    rc = cren_manifest_write(manifest, NULL);

    puts("Manifest OK");

cleanup:
    if (manifest_filepath != NULL)
        string_free(manifest_filepath);

    if (manifest != NULL)
        cren_manifest_free(manifest);

    return rc;
}
