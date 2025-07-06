// Run command arguments

#ifndef CREN_ARGS_RUN_H
#define CREN_ARGS_RUN_H

#include <stdbool.h>

#include <utils/string_list.h>
#include <utils/string.h>

/// @brief Run command arguments
typedef struct args_run_t args_run_t;

struct args_run_t
{
    /// @brief Build with all features
    bool all_features;
    /// @brief Build with no default features
    bool no_default_features;
    /// @brief Release build
    bool release;
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
    /// @brief Additional arguments to pass to the binary
    string_list_t *args;
};

void args_run_free(args_run_t *args);

#endif // CREN_ARGS_RUN_H
