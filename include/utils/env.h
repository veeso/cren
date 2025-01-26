// Environment utils

#ifndef UTILS_ENV_H
#define UTILS_ENV_H

#include <utils/string.h>

/// @brief Get the environment variable.
/// @param name The name of the environment variable.
/// @return The value of the environment variable. NULL if not found
string_t *env_get(const char *name);

/// @brief get a path from PATH environment variable.
/// @param name The name of binary to find.
/// @return The path to the binary. NULL if not found.
string_t *env_get_from_path(const char *name);

#endif // UTILS_ENV_H
