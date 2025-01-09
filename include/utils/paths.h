// Cren paths utils

#ifndef CREN_PATHS_H
#define CREN_PATHS_H

#include <utils/string.h>

#define TARGET_DIR "target"
#define TARGET_DIR_DEBUG "debug"
#define TARGET_DIR_RELEASE "release"

string_t *target_dir();
string_t *target_debug_dir();
string_t *target_release_dir();

#endif // CREN_PATHS_H
