// Build contains the main logic for building the project.

#ifndef CREN_BUILD_H
#define CREN_BUILD_H

#include <utils/string.h>
#include <utils/string_list.h>

typedef struct build_t build_t;

/// @brief Initialize the build object.
/// @return The build object.
build_t *build_init(void);

/// @brief Free the build object.
/// @param build The build object to free.
void build_free(build_t *build);

/// @brief compile the project.
/// @param build The build object.
/// @return CREN_OK on success, CREN_NOK on failure.
int build_compile(build_t *build);

#endif // CREN_BUILD_H
