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

    string_t *cren_manifest_path = NULL;
    FILE *file = NULL;
    cren_manifest_t *manifest = NULL;
    // get cren manifest or use provided arg
    if (args->path == NULL)
    {
        cren_manifest_path = manifest_path();
    }
    else
    {
        cren_manifest_path = string_clone(args->path);
    }

    if (cren_manifest_path == NULL)
    {
        log_fatal("Failed to get cren manifest path");
        rc = CREN_NOK;
        goto cleanup;
    }

    log_info("Checking cren manifest: %.*s", cren_manifest_path->length, cren_manifest_path->data);

    // open file
    file = fopen(cren_manifest_path->data, "r");
    if (file == NULL)
    {
        log_fatal("Failed to open cren manifest: %.*s", cren_manifest_path->length, cren_manifest_path->data);
        rc = CREN_NOK;
        goto cleanup;
    }

    // read manifest
    manifest = cren_manifest_init();
    if (manifest == NULL)
    {
        log_fatal("Failed to init cren manifest");
        rc = CREN_NOK;
        goto cleanup;
    }

    // parse manifest
    char error[4096];
    if (cren_manifest_parse(manifest, file, error, 4096) != CREN_OK)
    {
        log_fatal("Failed to parse cren manifest: %s", error);
        rc = CREN_NOK;
        goto cleanup;
    }

    puts("Manifest OK");

cleanup:
    if (file != NULL)
        fclose(file);
    string_free(cren_manifest_path);
    cren_manifest_free(manifest);

    return rc;
}
