// Cren paths utils

#ifndef CREN_PATHS_H
#define CREN_PATHS_H

#include <utils/string.h>

#define TARGET_DIR "target"
#define TARGET_DIR_DEBUG "debug"
#define TARGET_DIR_RELEASE "release"

/// @brief Get the parent directory of a given path.
/// @param path
/// @return the parent directory as a string_t or NULL if the path is invalid.
string_t *parent_dir(const char *path);
/// @brief Get the file name from a given path.
/// @param path
/// @return the file name as a string_t or NULL if the path is invalid.
string_t *file_name(const char *path);
/// @brief Get the base name of a file from a given path.
/// @param path
/// @return the base name as a string_t or NULL if the path is invalid.
string_t *base_name(const char *path);
/// @brief Get the project directory based on the manifest path.
/// @param manifest_path
/// @return project directory as a string_t or NULL if the manifest path is invalid.
string_t *get_project_dir(const char *manifest_path);
/// @brief Get the target directory based on the current directory.
string_t *target_dir(void);
/// @brief Get the target debug directory based on the current directory.
/// @return target debug directory as a string_t or NULL if the target directory cannot be determined
string_t *target_debug_dir(void);
/// @brief Get the target release directory based on the current directory.
/// @return target release directory as a string_t or NULL if the target directory cannot be determined
string_t *target_release_dir(void);
/// @brief Get the absolute path from a relative path and a base path.
/// @param path The relative path.
/// @param base_path The base path to resolve against.
/// @return The absolute path as a string_t or NULL if the path is invalid.
string_t *to_absolute_path(const string_t *path, const string_t *base_path);

#endif // CREN_PATHS_H
