#ifndef CREN_BUILD_URI_LOADER_H
#define CREN_BUILD_URI_LOADER_H

#include <utils/string.h>

#include <manifest.h>
#include <build/build_dependency.h>

/// @brief Load a cren manifest from a URI.
/// @param name The name of the dependency.
/// @param uri The URI to load.
/// @param type The type of the build dependency (path or git).
/// @param project_dir The project directory where the dependency is located.
/// @param target_dir The target directory where the dependency will be loaded.
/// @param dependency_dir Pointer to a string that will hold the absolute path of the dependency directory.
/// @param manifest_path Pointer to a string that will hold the path to the loaded manifest file.
/// @return Loaded manifest or NULL on failure.
cren_manifest_t *load_dependency_manifest(const string_t *name, const string_t *uri, build_dependency_type_t type, const string_t *project_dir, const string_t *target_dir, string_t **dependency_dir, string_t **manifest_path);

#endif // CREN_BUILD_URI_LOADER_H
