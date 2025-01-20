// Platform checker

#ifndef UTILS_PLATFORM_H
#define UTILS_PLATFORM_H

#include <stdbool.h>

#include <utils/string.h>

typedef struct platform_t platform_t;

struct platform_t
{
    string_t *arch;
    string_t *vendor;
    string_t *os;
    string_t *abi;
    string_t *alias;
};

/// @brief Parse a platform string
/// @param str
/// @return platform
platform_t *platform_parse(const char *str);

/// @brief Clone a platform
/// @param platform
/// @return platform
platform_t *platform_clone(const platform_t *platform);

/// @brief Convert a platform to a string
/// @param platform
/// @return string
string_t *platform_to_string(const platform_t *platform);

/// @brief free a platform
/// @param platform
void platform_free(platform_t *platform);

/// @brief Check if target platform matches local platform
/// @param platforms
/// @return bool
bool platform_matches(const platform_t *local, const platform_t *target);

/// @brief Get the local platform
/// @return platform
platform_t *platform_local(void);

#endif // UTILS_PLATFORM_H
