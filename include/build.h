// Build contains the main logic for building the project.

#ifndef CREN_BUILD_H
#define CREN_BUILD_H

#include <stdbool.h>

#include <build/build_dependency.h>
#include <build/environment.h>
#include <build/source.h>
#include <build/target.h>
#include <manifest/package/language.h>
#include <utils/string.h>
#include <utils/string_list.h>

typedef struct build_cfg_t build_cfg_t;

struct build_cfg_t
{
    /// @brief Language standard
    language_t language;
    /// @brief Source files without main files
    source_t **sources;
    /// @brief Amount of source files
    size_t sources_len;
    /// @brief targets
    target_t **targets;
    /// @brief Amount of targets
    size_t targets_len;
    /// @brief defines
    string_list_t *defines;
    /// @brief include dirs
    string_list_t *include_dirs;
    /// @brief target dir
    string_t *target_dir;
    /// @brief links
    string_list_t *links;
    /// @brief Build dependencies
    build_dependency_t **dependencies;
    /// @brief Amount of dependencies
    size_t dependencies_len;
    /// @brief Release build
    bool release;
};

/// @brief Initialize the build object.
/// @return The build object.
build_cfg_t *build_init(void);

/// @brief Free the build object.
/// @param build The build object to free.
void build_free(build_cfg_t *build);

/// @brief Add a source file to the build.
/// @param build
/// @param src
/// @return CREN_OK on success, CREN_NOK on failure.
int build_add_source(build_cfg_t *build, const char *src);

/// @brief Add a target file to the build.
/// @param build
/// @param target name
/// @param src path
/// @param project dir
/// @return CREN_OK on success, CREN_NOK on failure.
int build_add_target(build_cfg_t *build, const char *target, const char *src_path, const char *project_dir, target_type_t type);

/// @brief Add a build dependency.
/// @param build The build object.
/// @param uri The URI of the dependency, which can be a local path or a Git
/// repository URL.
/// @param type The type of the dependency, which can be either a local path or a
/// Git repository.
/// @return CREN_OK on success, CREN_NOK on failure.
int build_add_dependency(build_cfg_t *build, const char *uri, build_dependency_type_t type);

#endif // CREN_BUILD_H
