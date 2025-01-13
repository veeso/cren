// Cren paths utils

#ifndef CREN_PATHS_H
#define CREN_PATHS_H

#include <utils/string.h>

#define TARGET_DIR "target"
#define TARGET_DIR_DEBUG "debug"
#define TARGET_DIR_RELEASE "release"

string_t *parent_dir(const char *path);
string_t *file_name(const char *path);
string_t *base_name(const char *path);
string_t *get_project_dir(const char *manifest_path);
string_t *target_dir(void);
string_t *target_debug_dir(void);
string_t *target_release_dir(void);

#endif // CREN_PATHS_H
