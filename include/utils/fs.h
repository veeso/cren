// FS utils

#ifndef UTILS_FS_H
#define UTILS_FS_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct dirent_t dirent_t;

struct dirent_t
{
    char *path;
    bool is_dir;
    dirent_t **children;
    size_t children_count;
    dirent_t *parent;
};

/// @brief Get the current directory
/// @return The current directory or NULL if failed. Must be freed by the caller
char *get_current_dir();

/// @brief Create a directory
/// @param path
/// @return CREN_OK on success, CREN_NOK on failure
int make_dir(const char *path);

/// @brief Remove a directory and its content
/// @param path
/// @return CREN_OK on success, CREN_NOK on failure
int rmdir_all(const char *path, void (*on_remove_cb)(void *ctx, const char *path), void *ctx);

/// @brief Scan a directory
/// @param path
/// @return
dirent_t *scan_dir(const char *path);

/// @brief Stat dirent_t at path
/// @param path
/// @return dirent_t or NULL if doesn't exist
dirent_t *dirent_stat(const char *path);

/// @brief Free a dirent_t
/// @param dir
void dirent_free(dirent_t *dir);

/// @brief Count the number of children in a dirent_t
/// @param dir
size_t dirent_count(dirent_t *dir);

#endif // UTILS_FS_H
