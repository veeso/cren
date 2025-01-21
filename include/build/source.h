// Source files list

#ifndef CREN_BUILD_SOURCE_H
#define CREN_BUILD_SOURCE_H

#include <utils/string.h>

typedef struct source_t source_t;

struct source_t
{
    string_t *src;
    string_t *obj;
    string_t *target_name;
};

/// @brief Initialize the source object.
/// @param src The source file.
/// @return The source object.
source_t *source_init(const char *src, const char *target_dir);

/// @brief Initialize the target object.
/// @param src The source file.
/// @return The source object.
source_t *target_init(const char *src, const char *target_dir, const char *target_name);

/// @brief Free the source object.
/// @param source The source object to free.
void source_free(source_t *source);

#endif // CREN_BUILD_SOURCE_H
