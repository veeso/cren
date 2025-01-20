#include <lib/log.h>
#include <utils/env.h>
#include <utils/fs.h>

string_t *env_get(const char *name)
{
    if (name == NULL)
    {
        log_error("name is NULL");
    }
#if defined(_WIN32) || defined(_WIN64)
    // Per Windows, usiamo _dupenv_s
    char *value = NULL;
    size_t len = 0;
    if (_dupenv_s(&value, &len, name) == 0 && value != NULL)
    {

        string_t *env_value = string_from_cstr(name);
        free(value);
        return env_value;
    }
    else
    {
        return NULL;
    }
#else
    // Per sistemi POSIX (Linux, macOS, ecc.)
    const char *value = getenv(name);
    if (value != NULL)
    {
        return string_from_cstr(value);
    }
    else
    {
        return NULL; // Variabile non trovata
    }
#endif
}

string_t *env_get_from_path(const char *name)
{
    log_debug("Searching for %s in PATH", name);
    // get PATH
    string_t *path = env_get("PATH");
    if (path == NULL)
    {
        return NULL;
    }
#ifdef _WIN32
    const char *delimiter = ";"; // WINDOWS
#else
    const char *delimiter = ":"; // POSIX
#endif

    // split PATH
    char *dir = strtok(path->data, delimiter);
    while (dir != NULL)
    {
        log_debug("Searching in %s", dir);
        // copy dir
        string_t *dir_string = string_from_cstr(dir);
        // search in each path
        dirent_t *path_dir = scan_dir_with_depth(dir_string->data, 1);
        if (path_dir == NULL)
        {
            log_warn("Failed to scan directory %s", dir_string->data);
            dir = strtok(NULL, delimiter);
            string_free(dir_string);
            continue;
        }

        // search in children
        for (size_t i = 0; i < path_dir->children_count; i++)
        {
            dirent_t *child = path_dir->children[i];
            if (child->is_dir)
            {
                continue;
            }

            // get filename
            string_t *filename = dirent_filename(child);
            if (filename == NULL)
            {
                log_error("Failed to get filename for %s", child->path);
                continue;
            }

            if (strcmp(filename->data, name) == 0)
            {
                log_debug("Found %s in %s", name, dir_string->data);
                string_t *bin_path = string_from_cstr(child->path);
                dirent_free(path_dir);
                string_free(filename);
                string_free(dir_string);
                string_free(path);

                return bin_path;
            }
            string_free(filename);
        }
        string_free(dir_string);
        dirent_free(path_dir);

        dir = strtok(NULL, delimiter);
    }

    string_free(path);

    return NULL;
}
