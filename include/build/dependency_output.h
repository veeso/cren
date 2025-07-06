#ifndef CREN_BUILD_DEPENDENCY_OUTPUT_H
#define CREN_BUILD_DEPENDENCY_OUTPUT_H

#include <build.h>
#include <utils/string_list.h>

/// @brief Structure to hold the includes and objects for a dependency.
/// These fields must then be included by the project using the dependency.
typedef struct dependency_outputs_t dependency_outputs_t;

struct dependency_outputs_t
{
    string_list_t *includes;
    string_list_t *objects;
};

/// @brief Initialize the dependency outputs.
/// @return initialized dependency outputs or NULL on failure.
dependency_outputs_t *dependency_outputs_init(void);

/// @brief Add an include to the dependency outputs.
/// @param outputs The dependency outputs.
void dependency_outputs_free(dependency_outputs_t *outputs);

/// @brief Initialize the dependency outputs with the provided build configuration.
/// @param build The build configuration to use.
/// @param objects_dir The directory where the objects are stored.
/// @return CREN_OK on success, CREN_NOK on failure.
int dependency_outputs_insert(dependency_outputs_t *outputs, const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir);

/// @brief Concatenate the other dependency outputs to the current one.
/// @param outputs The dependency outputs to concatenate to.
/// @param other The other dependency outputs to concatenate.
/// @return CREN_OK on success, CREN_NOK on failure.
int dependency_outputs_concat(dependency_outputs_t *outputs, const dependency_outputs_t *other);

/// @brief get the path of an object in the objects directory.
string_t *get_object_path(const string_t *objects_dir, const string_t *object_name);

/// @brief Get static library target path.
/// @param build
/// @param target
/// @return static library target path or NULL if the target is not a library.
string_t *get_lib_path(const build_environment_t *env, const build_cfg_t *build, const target_t *target);

#endif // CREN_BUILD_DEPENDENCY_OUTPUT_H
