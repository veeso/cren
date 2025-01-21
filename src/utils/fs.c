#include <stdlib.h>

#include <cren.h>
#include <lib/log.h>
#include <utils/fs.h>
#include <utils/paths.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

char *get_current_dir(void)
{
    char *buffer = (char *)malloc(sizeof(char) * 4096);
    if (buffer == NULL)
    {
        log_error("Failed to allocate memory for current directory");
        return NULL;
    }

#if defined(_WIN32) || defined(_WIN64)
    if (GetCurrentDirectoryA(4096, buffer) == 0)
    {
        log_error("Failed to get current directory");
        free(buffer);
        return NULL;
    }
#else
    if (getcwd(buffer, 4096) == NULL)
    {
        log_error("Failed to get current directory");
        free(buffer);
        return NULL;
    }

#endif

    return buffer;
}

int make_dir(const char *path)
{
#if defined(_WIN32) || defined(_WIN64)
    if (CreateDirectoryA(path, NULL) == 0)
    {
        log_warn("Failed to create directory %s", path);
        return CREN_NOK;
    }
#else
    if (mkdir(path, 0755) == -1)
    {
        log_warn("Failed to create directory %s", path);
        return CREN_NOK;
    }
#endif

    log_debug("Created directory %s", path);

    return CREN_OK;
}

int make_dir_recursive(const char *path)
{
    // get the parent directory
    dirent_t *parent_stat = NULL;
    string_t *parent = parent_dir(path);
    int rc = CREN_OK;
    if (parent == NULL)
    {
        log_error("Failed to get parent directory of %s", path);
        rc = CREN_NOK;
        goto cleanup;
    }

    // if the parent directory doesn't exist, create it
    parent_stat = dirent_stat(parent->data);
    if (parent_stat == NULL)
    {
        if (make_dir_recursive(parent->data) == CREN_NOK)
        {
            log_error("Failed to create parent directory %s", parent->data);
            rc = CREN_NOK;
            goto cleanup;
        }
    }

cleanup:

    dirent_free(parent_stat);
    string_free(parent);

    if (rc != CREN_OK)
    {
        return CREN_NOK;
    }

    return make_dir(path);
}

int rmdir_all(const char *path, void (*on_remove_cb)(void *ctx, const char *path), void *ctx)
{
    dirent_t *dir = scan_dir(path);
    if (dir == NULL)
    {
        log_error("Failed to scan directory %s", path);
        return CREN_NOK;
    }

    for (size_t i = 0; i < dir->children_count; i++)
    {
        dirent_t *child = dir->children[i];
        if (child->is_dir)
        {
            rmdir_all(child->path, on_remove_cb, ctx);
        }
        else
        {
            if (remove(child->path) == -1)
            {
                log_error("Failed to remove file %s", child->path);
                dirent_free(dir);
                return CREN_NOK;
            }

            if (on_remove_cb != NULL)
            {
                on_remove_cb(ctx, child->path);
            }
        }
    }

    if (rmdir(path) == -1)
    {
        log_error("Failed to remove directory %s", path);
        dirent_free(dir);
        return CREN_NOK;
    }

    if (on_remove_cb != NULL)
    {
        on_remove_cb(ctx, path);
    }

    dirent_free(dir);

    return CREN_OK;
}
