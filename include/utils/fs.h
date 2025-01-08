// FS utils

#ifndef UTILS_FS_H
#define UTILS_FS_H

/// @brief Get the current directory
/// @return The current directory or NULL if failed. Must be freed by the caller
char *get_current_dir();

/// @brief Create a directory
/// @param path
/// @return CREN_OK on success, CREN_NOK on failure
int make_dir(const char *path);

#endif // UTILS_FS_H
