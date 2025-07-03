#ifndef CREN_BUILD_MANIFEST_H
#define CREN_BUILD_MANIFEST_H

#include <stdbool.h>

#include <build.h>
#include <manifest.h>

typedef struct manifest_build_config_t manifest_build_config_t;

struct manifest_build_config_t
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

manifest_build_config_t *build_from_manifest_init(void);

void manifest_build_config_free(manifest_build_config_t *args);

/// @brief Get build configuration from manifest.
/// @param manifest
/// @param args
/// @return The build object or NULL on failure.
build_cfg_t *build_config_from_manifest(const cren_manifest_t *manifest, const manifest_build_config_t *args);

#endif // CREN_BUILD_MANIFEST_H
