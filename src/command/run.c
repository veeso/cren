#include <build.h>
#include <build/compile.h>
#include <build/manifest.h>
#include <command/run.h>
#include <cren.h>
#include <manifest.h>
#include <manifest/path.h>
#include <lib/log.h>
#include <utils/cmd.h>
#include <utils/paths.h>

void log_run_opts(const args_run_t *args);
manifest_build_config_t *build_from_manifest_from_run_args(const args_run_t *args, const string_t *manifest_path);
string_t *get_default_target(const string_t *manifest_path);
int execute_binary(const manifest_build_config_t *build_args, const args_run_t *args);
string_t *build_get_binary_path(const manifest_build_config_t *build_args);

int command_run(const args_run_t *args)
{
    int rc = CREN_OK;
    manifest_build_config_t *manifest_build_args = NULL;
    string_t *manifest_filepath = NULL;

    log_run_opts(args);

    if (args->manifest_path == NULL)
    {
        manifest_filepath = manifest_path();
    }
    else
    {
        manifest_filepath = string_clone(args->manifest_path);
    }

    if (manifest_filepath == NULL)
    {
        log_error("Failed to get manifest path");
        rc = CREN_NOK;
        goto cleanup;
    }

    // build from manifest
    manifest_build_args = build_from_manifest_from_run_args(args, manifest_filepath);
    if (manifest_build_args == NULL)
    {
        log_error("Error building from manifest");
        rc = CREN_NOK;
        goto cleanup;
    }

    // Build the project
    if ((rc = load_manifest_and_build(manifest_build_args, manifest_filepath) != CREN_OK))
    {
        log_error("Failed to compile the project");
        goto cleanup;
    }

    // execute the binary
    rc = execute_binary(manifest_build_args, args);

cleanup:
    if (manifest_build_args)
        manifest_build_config_free(manifest_build_args);
    if (manifest_filepath)
        string_free(manifest_filepath);

    return rc;
}

/// @brief Log the run options.
/// @param args
void log_run_opts(const args_run_t *args)
{
    log_debug("Build options:");
    log_debug("all-features: %d", args->all_features);
    log_debug("no-default-features: %d", args->no_default_features);
    log_debug("release: %d", args->release);
    if (args->bin)
        log_debug("bin: %s", args->bin->data);
    if (args->example)
        log_debug("example: %s", args->example->data);
    if (args->target_dir)
        log_debug("target-dir: %s", args->target_dir->data);
    log_debug("features:");
    if (args->features != NULL)
    {
        for (size_t i = 0; i < args->features->nitems; i++)
        {
            log_debug("- %s", args->features->items[i]->data);
        }
    }
    log_debug("args:");
    if (args->args != NULL)
    {
        for (size_t i = 0; i < args->args->nitems; i++)
        {
            log_debug("- %s", args->args->items[i]->data);
        }
    }
}

/// @brief Get the build configuration from the manifest and command line arguments.
/// @param args
/// @param manifest_path
/// @return The build configuration or NULL on failure.
manifest_build_config_t *build_from_manifest_from_run_args(const args_run_t *args, const string_t *manifest_path)
{
    manifest_build_config_t *build_args = build_from_manifest_init();
    if (build_args == NULL)
    {
        log_error("Error initializing build args");
        return NULL;
    }

    build_args->all_targets = false;
    build_args->all_features = args->all_features;
    build_args->no_default_features = args->no_default_features;
    build_args->release = args->release;
    build_args->bins = false;
    build_args->examples = false;
    build_args->lib = false;

    build_args->bin = string_clone(args->bin);
    build_args->example = string_clone(args->example);
    build_args->target_dir = string_clone(args->target_dir);
    build_args->manifest_path = string_clone(args->manifest_path);

    if (args->features != NULL)
    {
        build_args->features = string_list_init();
        for (size_t i = 0; i < args->features->nitems; i++)
        {
            string_t *feature = string_clone(args->features->items[i]);
            if (feature == NULL)
            {
                log_error("Error cloning feature");
                manifest_build_config_free(build_args);
                return NULL;
            }
            string_list_push(build_args->features, feature);
        }
    }

    if (build_args->bin == NULL && build_args->example == NULL)
    {
        log_debug("No binary or example specified, trying to get default target from manifest");
        // if no bin or example is specified, try to get the default target from the manifest
        string_t *default_target = get_default_target(manifest_path);
        if (default_target == NULL)
        {
            log_error("No default target found in manifest");
            manifest_build_config_free(build_args);
            return NULL;
        }
        build_args->bin = default_target;
        build_args->bins = true;
    }

    return build_args;
}

/// @brief Get the default target from the manifest path.
/// @param manifest_path The path to the manifest file.
/// @return The default target or NULL if not found.
string_t *get_default_target(const string_t *manifest_path)
{
    log_debug("Searching for default target in manifest: %s", manifest_path->data);
    cren_manifest_t *manifest = cren_manifest_load(manifest_path);
    if (manifest == NULL)
    {
        log_error("Failed to load manifest from path: %s", manifest_path->data);
        return NULL;
    }

    // get the default target
    if (manifest->targets == NULL || manifest->targets->bin_len != 1)
    {
        log_error("Unable to determine the default target from manifest: %s", manifest_path->data);
        cren_manifest_free(manifest);
        return NULL;
    }

    string_t *default_target = string_clone(manifest->targets->bin[0]->name);
    cren_manifest_free(manifest);
    if (default_target == NULL)
    {
        log_error("Failed to clone default target name from manifest: %s", manifest_path->data);
        return NULL;
    }

    log_info("Found default target: %s", default_target->data);

    return default_target;
}

/// @brief Execute the binary with the given build arguments.
/// @param build_args The build configuration arguments.
/// @param args The run arguments containing additional arguments to pass to the binary.
/// @return the exit code of the binary or an error code on failure.
int execute_binary(const manifest_build_config_t *build_args, const args_run_t *args)
{
    int rc = CREN_OK;
    string_t *command = NULL;
    string_t *binary_path = NULL;
    // get the path to the binary
    binary_path = build_get_binary_path(build_args);
    if (binary_path == NULL)
    {
        log_error("Failed to get binary path");
        rc = CREN_NOK;
        goto cleanup;
    }

    // make command from binary path
    command = string_clone(binary_path);
    if (command == NULL)
    {
        log_error("Failed to clone binary path");
        rc = CREN_NOK;
        goto cleanup;
    }
    if (args->args != NULL)
    {
        for (size_t i = 0; i < args->args->nitems; i++)
        {
            string_append_char(command, ' ');
            string_append(command, args->args->items[i]->data);
        }
    }

    log_info("Executing binary: %s", command->data);
    rc = cmd_exec(command->data);

cleanup:
    if (binary_path != NULL)
        string_free(binary_path);
    if (command != NULL)
        string_free(command);

    return rc;
}

/// @brief Get binary path based on the build configuration.
/// @param build_args
/// @return The path to the binary or NULL on failure.
string_t *build_get_binary_path(const manifest_build_config_t *build_args)
{
    string_t *bin_path = NULL;

    if (build_args->release)
    {
        bin_path = target_release_dir();
    }
    else
    {
        bin_path = target_debug_dir();
    }

    if (bin_path == NULL)
    {
        log_error("Failed to get target directory for binary");
        return NULL;
    }

    string_t *target_name = NULL;
    if (build_args->bin != NULL)
    {
        target_name = build_args->bin;
    }
    else if (build_args->example != NULL)
    {
        target_name = build_args->example;
    }
    else
    {
        log_error("No binary or example specified in build arguments");
        string_free(bin_path);
        return NULL;
    }

    string_append_path(bin_path, target_name->data);
    log_debug("Binary path: %s", bin_path->data);
    return bin_path;
}
