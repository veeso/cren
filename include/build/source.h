// Source files list

#ifndef CREN_BUILD_SOURCE_H
#define CREN_BUILD_SOURCE_H

#include <utils/string.h>

typedef struct source_t source_t;

struct source_t
{
    string_t *src;
    string_t *obj;
};

/// @brief Initialize the source object.
/// @param src The source file.
/// @return The source object.
source_t *source_init(const char *src);

/// @brief Free the source object.
/// @param source The source object to free.
void source_free(source_t *source);

#endif // CREN_BUILD_SOURCE_H
