#ifndef CREN_BUILD_MANIFEST_H
#define CREN_BUILD_MANIFEST_H

#include <stdbool.h>

#include <build.h>
#include <manifest.h>

typedef struct build_from_manifest_t build_from_manifest_t;

struct build_from_manifest_t
{
    /// @brief Build with all features
    bool all_features;
    /// @brief Build with no default features
    bool no_default_features;
    /// @brief Release build
    bool release;
    /// @brief Build all targets
    bool all_targets;
    /// @brief Build bins
    bool bins;
    /// @brief Build examples
    bool examples;
    /// @brief Build libs
    bool lib;
    /// @brief Build binary target
    string_t *bin;
    /// @brief Build example target
    string_t *example;
    /// @brief target dir
    string_t *target_dir;
    /// @brief features to build with
    string_list_t *features;
    /// @brief manifest path
    string_t *manifest_path;
};

build_from_manifest_t *build_from_manifest_init(void);

void build_from_manifest_free(build_from_manifest_t *args);

/// @brief Make a build object from a manifest.
/// @param manifest
/// @param args
/// @return The build object or NULL on failure.
build_t *build_from_manifest(const cren_manifest_t *manifest, const build_from_manifest_t *args);

#endif // CREN_BUILD_MANIFEST_H
