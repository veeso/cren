// Build environment

#ifndef CREN_BUILD_ENVIRONMENT_H
#define CREN_BUILD_ENVIRONMENT_H

#include <utils/string.h>

typedef struct build_environment_t build_environment_t;

struct build_environment_t
{
    string_t *cc;
    string_t *cxx;
    string_t *ld;
};

/// @brief Get the build environment.
/// @return returns the build environment.
build_environment_t *build_environment_init(void);

/// @brief Free the build environment.
/// @param env The build environment to free.
void build_environment_free(build_environment_t *env);

#endif // CREN_BUILD_ENVIRONMENT_H
