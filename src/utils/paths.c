#include <string.h>

#include <lib/log.h>
#include <manifest/path.h>
#include <utils/paths.h>

string_t *parent_dir(const char *path)
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
    bool found_any_non_slash = false;
    size_t last_slash = 0;
    for (size_t i = len - 1; i > 0; i--)
    {
        bool is_slash = (path[i] == '/' || path[i] == '\\');
        if (is_slash && found_any_non_slash)
        {
            last_slash = i;
            break;
        }
        else if (!is_slash)
        {
            found_any_non_slash = true;
        }
    }

    // check if slash was found
    if (last_slash == 0)
    {
        log_debug("Failed to find parent directory");
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

    string_t *parent_dir = string_from_cstr(parent);
    free(parent);

    return parent_dir;
}

string_t *file_name(const char *path)
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
        log_error("Failed to find file name");
        return NULL;
    }

    // copy file name
    char *file = (char *)malloc(sizeof(char) * (len - last_slash + 1));
    if (file == NULL)
    {
        log_error("Failed to allocate memory for file name");
        return NULL;
    }

    strncpy(file, path + last_slash + 1, len - last_slash);
    file[len - last_slash] = '\0';

    string_t *file_name = string_from_cstr(file);
    free(file);

    return file_name;
}

string_t *base_name(const char *path)
{
    string_t *filename = file_name(path);
    if (filename == NULL)
    {
        return NULL;
    }

    size_t len = filename->length;
    if (len == 0)
    {
        log_error("File name is empty");
        string_free(filename);
        return NULL;
    }

    // find last dot
    size_t last_dot = 0;
    for (size_t i = len - 1; i > 0; i--)
    {
        if (filename->data[i] == '.')
        {
            last_dot = i;
            break;
        }
    }

    // check if dot was found
    if (last_dot == 0)
    {
        log_error("Failed to find base name");
        string_free(filename);
        return NULL;
    }

    // copy base name
    char *base = (char *)malloc(sizeof(char) * last_dot + 1);
    if (base == NULL)
    {
        log_error("Failed to allocate memory for base name");
        string_free(filename);
        return NULL;
    }

    strncpy(base, filename->data, last_dot);

    string_t *base_name = string_from_cstr(base);
    string_free(filename);
    free(base);

    return base_name;
}

string_t *target_dir(void)
{
    // get manifest path
    string_t *target_dir = NULL;
    string_t *manifest_toml_path = manifest_path();
    char *manifest_dir_cstr = NULL;

    if (manifest_toml_path == NULL)
    {
        log_error("Failed to get manifest path");
        return NULL;
    }

    // get manifest directory (remove /Cren.toml from path)
    size_t to_rem = strlen("/Cren.toml");
    manifest_dir_cstr = (char *)malloc(sizeof(char) * (manifest_toml_path->length + 1 - to_rem));
    if (manifest_dir_cstr == NULL)
    {
        log_error("Failed to allocate memory for manifest directory");
        goto cleanup;
    }

    // copy the path without the /Cren.toml file
    strncpy(manifest_dir_cstr, manifest_toml_path->data, manifest_toml_path->length - to_rem);
    manifest_dir_cstr[manifest_toml_path->length - to_rem] = '\0';

    target_dir = string_from_cstr(manifest_dir_cstr);
    string_append_path(target_dir, TARGET_DIR);

cleanup:
    string_free(manifest_toml_path);
    if (manifest_dir_cstr != NULL)
        free(manifest_dir_cstr);

    return target_dir;
}

string_t *target_debug_dir(void)
{
    string_t *target_dir_path = target_dir();
    if (target_dir_path == NULL)
    {
        return NULL;
    }

    string_append_path(target_dir_path, TARGET_DIR_DEBUG);

    return target_dir_path;
}

string_t *target_release_dir(void)
{
    string_t *target_dir_path = target_dir();
    if (target_dir_path == NULL)
    {
        return NULL;
    }

    string_append_path(target_dir_path, TARGET_DIR_RELEASE);

    return target_dir_path;
}

string_t *to_absolute_path(const string_t *path, const string_t *base_path)
{
    if (path == NULL || base_path == NULL)
    {
        log_error("Path or base path is NULL");
        return NULL;
    }

    log_debug("Converting path '%s' to absolute path with base '%s'", path->data, base_path->data);

    // Check if the path is already absolute
    if (path->data[0] == '/' || path->data[0] == '\\')
    {
        return string_clone(path);
    }

    string_t *absolute_path = string_clone(base_path);
    if (absolute_path == NULL)
    {
        log_error("Failed to clone base path");
        return NULL;
    }

    // iterate over the path components
    const char *path_data = path->data;
    // strtok to split the path by '/' or '\\'
#ifndef _WIN32
    const char *separator = "/";
#else
    const char *separator = "\\";
#endif
    char *token = strtok((char *)path_data, separator);
    while (token != NULL)
    {
        // check the token if it is '.' or '..'
        if (strcmp(token, ".") == 0)
        {
            // skip '.' as it refers to the current directory
        }
        else if (strcmp(token, "..") == 0)
        {
            // go up one directory
            string_t *parent = parent_dir(absolute_path->data);
            if (parent == NULL)
            {
                log_error("Failed to get parent directory");
                string_free(absolute_path);
                return NULL;
            }
            log_debug("Going up one directory from %s to %s", absolute_path->data, parent->data);

            string_free(absolute_path);
            absolute_path = parent;
        }
        else
        {
            // append the token to the absolute path
            log_debug("Appending token '%s' to absolute path '%s'", token, absolute_path->data);
            string_append_path(absolute_path, token);
        }
        // Get the next token
        token = strtok(NULL, separator);
    }

    return absolute_path;
}
