// Build contains the main logic for building the project.

#ifndef CREN_BUILD_H
#define CREN_BUILD_H

#include <stdbool.h>

#include <build/source.h>
#include <manifest/package/language.h>
#include <utils/string.h>
#include <utils/string_list.h>

typedef struct build_t build_t;

struct build_t
{
    /// @brief Language standard
    language_t language;
    /// @brief Source files without main files
    source_t **sources;
    /// @brief Amount of source files
    size_t sources_len;
    /// @brief targets
    source_t **targets;
    /// @brief Amount of targets
    size_t targets_len;
    /// @brief defines
    string_list_t *defines;
    /// @brief include dirs
    string_list_t *include_dirs;
    /// @brief target dir
    string_t *target_dir;
    /// @brief Release build
    bool release;
};

/// @brief Initialize the build object.
/// @return The build object.
build_t *build_init(void);

/// @brief Free the build object.
/// @param build The build object to free.
void build_free(build_t *build);

/// @brief Add a source file to the build.
/// @param build
/// @param src
/// @return
int add_source(build_t *build, const char *src);

/// @brief compile the project.
/// @param build The build object.
/// @return CREN_OK on success, CREN_NOK on failure.
int build_compile(build_t *build);

#endif // CREN_BUILD_H
