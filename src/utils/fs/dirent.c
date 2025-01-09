#include <stdlib.h>
#include <string.h>

#include <cren.h>
#include <lib/log.h>
#include <utils/fs.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

dirent_t *dirent_init(dirent_t *parent, const char *path, bool is_dir);
dirent_t *scan_dir_r(dirent_t *parent);
int add_child(dirent_t *parent, dirent_t *child);
int is_directory(const char *path, bool *is_dir);

dirent_t *scan_dir(const char *path)
{
    // init parent
    dirent_t *parent = dirent_init(NULL, path, true);
    if (parent == NULL)
    {
        log_error("Failed to initialize parent dirent");
        return NULL;
    }

    // check if path is a directory
    bool is_dir = false;
    if (is_directory(path, &is_dir) != CREN_OK)
    {
        log_error("Failed to check if %s is a directory", path);
        return NULL;
    }

    if (!is_dir)
    {
        log_error("%s is not a directory", path);
        dirent_free(parent);
        return NULL;
    }

    // scan directory recursively
    return scan_dir_r(parent);
}

dirent_t *scan_dir_r(dirent_t *parent)
{

    if (parent == NULL || !parent->is_dir || parent->path == NULL)
    {
        log_error("Invalid parent dirent");
        return NULL;
    }

#if defined(_WIN32) || defined(_WIN64)

    WIN32_FIND_DATA find_data;
    HANDLE find_handle = FindFirstFile(parent->path, &find_data);
    if (find_handle == INVALID_HANDLE_VALUE)
    {
        log_error("Failed to open directory %s", path);
        return NULL;
    }

    do
    {
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0)
        {
            continue;
        }

        char *full_path = (char *)malloc(sizeof(char) * (strlen(parent->path) + strlen(find_data.cFileName) + 2));
        if (full_path == NULL)
        {
            log_error("Failed to allocate memory for full path");
            return NULL;
        }

        sprintf(full_path, "%s\\%s", parent->path, find_data.cFileName);

        bool is_dir = false;
        if (is_directory(full_path, &is_dir) != CREN_OK)
        {
            log_error("Failed to check if %s is a directory", full_path);
            free(full_path);
            return NULL;
        }

        dirent_t *entry = dirent_init(parent, full_path, is_dir);
        if (entry == NULL)
        {
            log_error("Failed to initialize dirent for %s", full_path);
            free(full_path);
            return NULL;
        }

        // add entry to parent
        if (add_child(parent, entry) != CREN_OK)
        {
            log_error("Failed to add child to parent");
            free(full_path);
            return NULL;
        }

        // scan recursively if directory
        if (is_dir && scan_dir_r(entry) == NULL)
        {
            log_error("Failed to scan directory %s", full_path);
            free(full_path);
            return NULL;
        }

    } while (FindNextFile(find_handle, &find_data));
#else

    struct dirent *entry;

    DIR *dir = opendir(parent->path);
    if (dir == NULL)
    {
        log_error("Failed to open directory %s", parent->path);
        return NULL;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        char *full_path = (char *)malloc(sizeof(char) * (strlen(parent->path) + strlen(entry->d_name) + 2));
        if (full_path == NULL)
        {
            log_error("Failed to allocate memory for full path");
            return NULL;
        }

        sprintf(full_path, "%s/%s", parent->path, entry->d_name);

        bool is_dir = false;
        if (is_directory(full_path, &is_dir) != CREN_OK)
        {
            log_error("Failed to check if %s is a directory", full_path);
            free(full_path);
            return NULL;
        }

        dirent_t *child = dirent_init(parent, full_path, is_dir);
        if (child == NULL)
        {
            log_error("Failed to initialize dirent for %s", full_path);
            free(full_path);
            return NULL;
        }

        // add child to parent
        if (add_child(parent, child) != CREN_OK)
        {
            log_error("Failed to add child to parent");
            free(full_path);
            return NULL;
        }

        // scan recursively if directory
        if (is_dir && scan_dir_r(child) == NULL)
        {
            log_error("Failed to scan directory %s", full_path);
            free(full_path);
            return NULL;
        }
    }

#endif

    return parent;
}

int add_child(dirent_t *parent, dirent_t *child)
{
    size_t new_count = parent->children_count + 1;
    dirent_t **new_children = (dirent_t **)realloc(parent->children, sizeof(dirent_t *) * new_count);

    if (new_children == NULL)
    {
        log_error("Failed to reallocate memory for children");
        return CREN_NOK;
    }

    parent->children = new_children;
    parent->children[parent->children_count] = child;
    parent->children_count = new_count;

    log_trace("Added child %s to parent %s", child->path, parent->path);

    return CREN_OK;
}

int is_directory(const char *path, bool *is_dir)
{
#if defined(_WIN32) || defined(_WIN64)
    DWORD attributes = GetFileAttributes(path);
    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        return CREN_NOK; // Error accessing the path
    }
    *is_dir = attributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
#else
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
    {
        return CREN_NOK; // Error accessing the path
    }
    *is_dir = S_ISDIR(statbuf.st_mode) ? true : false;
#endif

    return CREN_OK;
}

dirent_t *dirent_init(dirent_t *parent, const char *path, bool is_dir)
{
    dirent_t *entry = (dirent_t *)malloc(sizeof(dirent_t));
    entry->children = NULL;

    entry->children_count = 0;
    entry->is_dir = is_dir;

    size_t path_len = strlen(path);
    entry->path = (char *)malloc(sizeof(char) * (path_len + 1));
    if (entry->path == NULL)
    {
        log_error("Failed to allocate memory for dirent path");
        free(entry);
        return NULL;
    }
    memcpy(entry->path, path, path_len);
    entry->path[path_len] = '\0';

    entry->parent = parent;

    log_trace("Initialized dirent %s", path);

    return entry;
}

void dirent_free(dirent_t *dirent)
{
    if (dirent == NULL)
    {
        log_trace("Attempted to free NULL dirent");
        return;
    }
    // call recursively on children
    for (size_t i = 0; i < dirent->children_count; i++)
    {
        dirent_free(dirent->children[i]);
    }
    // free children
    if (dirent->children != NULL)
    {
        free(dirent->children);
    }
    // free path
    if (dirent->path != NULL)
    {
        free(dirent->path);
    }
    // free dirent
    free(dirent);

    log_trace("Freed dirent");
}

size_t dirent_count(dirent_t *entry)
{
    if (entry == NULL)
    {
        return 0;
    }

    size_t count = 1;
    for (size_t i = 0; i < entry->children_count; i++)
    {
        count += dirent_count(entry->children[i]);
    }

    return count;
}
