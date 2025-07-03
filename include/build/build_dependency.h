// Build contains the main logic for building the project.

#ifndef CREN_BUILD_BUILD_DEPENDENCY_H
#define CREN_BUILD_BUILD_DEPENDENCY_H

#include <utils/string.h>

/// @brief Type of build dependency.
/// This enum defines the types of build dependencies that can be used in a project.
/// - BUILD_DEPENDENCY_TYPE_PATH: A dependency that is specified by a local path.
/// - BUILD_DEPENDENCY_TYPE_GIT: A dependency that is specified by a Git repository
typedef enum build_dependency_type_t
{
    BUILD_DEPENDENCY_TYPE_PATH,
    BUILD_DEPENDENCY_TYPE_GIT
} build_dependency_type_t;

typedef struct build_dependency_t build_dependency_t;

/// @brief Build dependency structure.
/// This structure represents a build dependency in a project.
/// It contains the URI of the dependency and its type.
/// - uri: The URI of the dependency, which can be a local path or a Git repository URL.
/// - type: The type of the dependency, which can be either a local path or
struct build_dependency_t
{
    /// @brief URI of the dependency. Can be either a local path or a Git repository URL.
    string_t *uri;
    /// @brief Type of the dependency.
    build_dependency_type_t type;
};

/// @brief Initializes a build dependency.
/// @param uri The URI of the dependency, which can be a local path or a Git repository URL.
/// @param type The type of the dependency, which can be either a local path or a Git repository.
/// @return A pointer to the initialized build dependency structure, or NULL on failure.
build_dependency_t *build_dependency_init(const char *uri, build_dependency_type_t type);

/// @brief Frees a build dependency.
/// @param dependency The build dependency to free.
void build_dependency_free(build_dependency_t *dependency);

#endif // CREN_BUILD_BUILD_DEPENDENCY_H
