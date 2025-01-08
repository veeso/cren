// Path utils for Manifest

#ifndef CREN_MANIFEST_PATH_H
#define CREN_MANIFEST_PATH_H

#include <utils/string.h>

/// @brief Find the manifest path in any of the current or parent directories
/// @return The manifest path or NULL if not found
string_t *manifest_path();

#endif // CREN_MANIFEST_PATH_H
