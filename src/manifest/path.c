#include <stdio.h>
#include <string.h>

#include <lib/log.h>
#include <manifest.h>
#include <manifest/path.h>
#include <utils/fs.h>
#include <utils/paths.h>

string_t *find_manifest_path_in(const char *path);
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
        string_t *parent = parent_dir(path);
        if (parent == NULL)
        {
            log_error("Failed to find parent directory");
            return NULL;
        }
        string_t *res = find_manifest_path_in(parent->data);
        string_free(parent);
        return res;
    }

    // return manifest path
    fclose(file);
    return manifest_path;
}

string_t *get_project_dir(const char *manifest)
{
    string_t *manifest_path_s = manifest != NULL ? string_from_cstr(manifest) : manifest_path();
    if (manifest_path_s == NULL)
    {
        log_error("Error getting manifest path");
        return NULL;
    }
    string_t *project_dir = parent_dir(manifest_path_s->data);
    if (project_dir == NULL)
    {
        log_error("Error getting project dir");
        string_free(manifest_path_s);
        return NULL;
    }

    string_free(manifest_path_s);

    log_debug("Project dir: %s", project_dir->data);

    return project_dir;
}
