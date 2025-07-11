#include <command/clean.h>
#include <cren.h>
#include <lib/log.h>
#include <manifest.h>
#include <manifest/path.h>
#include <utils/fs.h>
#include <utils/paths.h>
#include <utils/terminal.h>

#define CLEANING COLOR_HEADER "Cleaning" COLOR_TEXT

struct clean_progress_ctx_t
{
    size_t total;
    size_t count;
};

void print_clean_progress(void *ctx, const char *path);

int command_clean(const args_clean_t *args)
{
    int rc = CREN_OK;
    cren_manifest_t *manifest = NULL;
    string_t *dir_to_clean = NULL;
    dirent_t *target_dirent = NULL;
    string_t *manifest_filepath = NULL;
    if (args->manifest_path != NULL)
    {
        manifest_filepath = string_clone(args->manifest_path);
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
        log_error("Error loading manifest");
        rc = CREN_NOK;
        goto cleanup;
    }
    // get target dir or provided
    if (args->target_dir != NULL)
    {
        dir_to_clean = string_clone(args->target_dir);
    }
    else
    {
        dir_to_clean = target_dir();
    }

    // if NULL
    if (dir_to_clean == NULL)
    {
        log_error("Error getting target dir");
        rc = CREN_NOK;
        goto cleanup;
    }

    // check if exists
    dirent_t *dir_stat = dirent_stat(dir_to_clean->data);
    if (dir_stat == NULL)
    {
        log_debug("Target dir does not exist: %s", dir_to_clean->data);
        rc = CREN_OK;
        goto cleanup;
    }
    dirent_free(dir_stat);

    // if release is true, clean only debug
    if (args->release)
    {
        string_append_path(dir_to_clean, TARGET_DIR_DEBUG);
    }

    // get the target dir
    target_dirent = scan_dir(dir_to_clean->data);
    if (target_dirent == NULL)
    {
        log_error("Error scanning target dir: %s", dir_to_clean->data);
        rc = CREN_NOK;
        goto cleanup;
    }

    // get items count
    size_t files_to_remove = dirent_count(target_dirent);
    struct clean_progress_ctx_t progress_ctx = {
        .total = files_to_remove,
        .count = 0,
    };

    // clean targets
    log_debug("Cleaning target dir: %s; %zu items to remove", dir_to_clean->data, files_to_remove);
    rmdir_all(dir_to_clean->data, print_clean_progress, (void *)&progress_ctx);

cleanup:
    print_outcome("Finished", "cren clean");

    string_free(dir_to_clean);
    string_free(manifest_filepath);
    cren_manifest_free(manifest);
    dirent_free(target_dirent);

    return rc;
}

void print_clean_progress(void *ctx, const char *path)
{
    struct clean_progress_ctx_t *progress_ctx = (struct clean_progress_ctx_t *)ctx;
    progress_ctx->count++;
    print_line_and_progress(progress_ctx->count, progress_ctx->total, CLEANING, "%sRemoving%s %s", COLOR_HEADER, COLOR_TEXT, path);
}
