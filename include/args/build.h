// Build command arguments

#ifndef CREN_ARGS_BUILD_H
#define CREN_ARGS_BUILD_H

#include <stdbool.h>

#include <utils/string_list.h>
#include <utils/string.h>

/// @brief Build command arguments
typedef struct args_build_t args_build_t;

struct args_build_t
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

void args_build_free(args_build_t *args);

#endif // CREN_ARGS_BUILD_H
