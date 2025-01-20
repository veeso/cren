// Build environment

#ifndef CREN_BUILD_ENVIRONMENT_H
#define CREN_BUILD_ENVIRONMENT_H

#include <utils/string.h>

/// @brief The compiler family. Used to determine the compiler flags.
typedef enum build_compiler_family_t
{
    COMPILER_FAMILY_GCC,
    COMPILER_FAMILY_MSVC,
} build_compiler_family_t;

typedef struct build_compiler_t build_compiler_t;

struct build_compiler_t
{
    string_t *path;
    build_compiler_family_t family;
};

typedef struct build_environment_t build_environment_t;

struct build_environment_t
{
    build_compiler_t *cc;
    build_compiler_t *cxx;
    string_t *ld;
};

/// @brief Get the build environment.
/// @return returns the build environment.
build_environment_t *build_environment_init(void);

/// @brief Free the build environment.
/// @param env The build environment to free.
void build_environment_free(build_environment_t *env);

#endif // CREN_BUILD_ENVIRONMENT_H
