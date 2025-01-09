#include <stdio.h>
#include <string.h>

#include <lib/log.h>
#include <manifest.h>
#include <manifest/path.h>
#include <utils/fs.h>

string_t *find_manifest_path_in(const char *path);
char *parent_dir(const char *path);
char *get_current_dir(void);

string_t *manifest_path(void)
{
    // get current dir
    char *current_dir = get_current_dir();
    if (current_dir == NULL)
    {
        log_error("Failed to get current directory");
        return NULL;
    }

    // find manifest in current dir
    string_t *manifest_path = find_manifest_path_in(current_dir);
    free(current_dir);

    if (manifest_path != NULL)
    {
        log_info("Found manifest at %.*s", manifest_path->length, manifest_path->data);
    }
    else
    {
        log_error("Failed to find manifest");
    }

    return manifest_path;
}

string_t *find_manifest_path_in(const char *path)
{
    if (path == NULL)
    {
        log_debug("Path is NULL");
        return NULL;
    }

    log_debug("Finding manifest in %s", path);

    string_t *manifest_path = string_from_cstr(path);
    if (manifest_path == NULL)
    {
        log_error("Failed to create string from path");
        return NULL;
    }

    // push manifest name
    string_append_path(manifest_path, CREN_MANIFEST_NAME);

    // check if manifest exists
    FILE *file = fopen(manifest_path->data, "r");
    if (file == NULL)
    {
        log_debug("Manifest not found in %s", path);
        string_free(manifest_path);

        // search in parent dir
        char *parent = parent_dir(path);
        if (parent == NULL)
        {
            log_error("Failed to find parent directory");
            return NULL;
        }
        string_t *res = find_manifest_path_in(parent);
        free(parent);
        return res;
    }

    // return manifest path
    fclose(file);
    return manifest_path;
}

char *parent_dir(const char *path)
{
    if (path == NULL)
    {
        log_error("Path is NULL");
        return NULL;
    }

    size_t len = strlen(path);
    if (len == 0)
    {
        log_error("Path is empty");
        return NULL;
    }

    // find last slash
    size_t last_slash = 0;
    for (size_t i = len - 1; i > 0; i--)
    {
        if (path[i] == '/' || path[i] == '\\')
        {
            last_slash = i;
            break;
        }
    }

    // check if slash was found
    if (last_slash == 0)
    {
        log_error("Failed to find parent directory");
        return NULL;
    }

    // copy parent dir
    char *parent = (char *)malloc(sizeof(char) * last_slash + 1);
    if (parent == NULL)
    {
        log_error("Failed to allocate memory for parent directory");
        return NULL;
    }

    strncpy(parent, path, last_slash);
    parent[last_slash] = '\0';

    return parent;
}
