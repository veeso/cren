
#include <build/uri_loader.h>
#include <manifest.h>
#include <lib/log.h>
#include <utils/paths.h>

string_t *get_manifest_path_from_path(const string_t *dir);
string_t *download_git_repository(const string_t *name, const string_t *uri, const string_t *target_dir);

cren_manifest_t *load_dependency_manifest(const string_t *name, const string_t *uri, build_dependency_type_t type, const string_t *project_dir, const string_t *target_dir, string_t **dependency_dir, string_t **manifest_path)
{
    string_t *absolute_dependency_dir = NULL;
    string_t *dependency_project_dir = NULL;
    string_t *absolute_dependency_project_dir = NULL;
    string_t *m_manifest_path = NULL;
    cren_manifest_t *manifest = NULL;

    log_debug("Loading dependency manifest from URI: %s; project dir is %s", uri->data, project_dir->data);

    if (type == BUILD_DEPENDENCY_TYPE_PATH)
    {
        dependency_project_dir = string_clone(uri);
    }
    else if (type == BUILD_DEPENDENCY_TYPE_GIT)
    {
        dependency_project_dir = download_git_repository(name, uri, target_dir);
    }

    // check if the project dir is initialized
    if (dependency_project_dir == NULL)
    {
        log_error("Failed to get dependency from uri %s.", uri->data);
        goto cleanup;
    }

    absolute_dependency_project_dir = to_absolute_path(dependency_project_dir, project_dir);
    if (absolute_dependency_project_dir == NULL)
    {
        log_error("Failed to convert dependency project dir %s to absolute path.", dependency_project_dir->data);
        goto cleanup;
    }
    log_debug("Absolute dependency project dir: %s", absolute_dependency_project_dir->data);

    // get manifest path
    m_manifest_path = get_manifest_path_from_path(absolute_dependency_project_dir);
    if (m_manifest_path == NULL)
    {
        log_error("Failed to get manifest path from dependency project dir %s.", absolute_dependency_project_dir->data);
        goto cleanup;
    }
    log_debug("Loading Manifest from path: %s", m_manifest_path->data);
    // load manifest from file
    manifest = cren_manifest_load(m_manifest_path);
    // set dependency directory and prevent double free
    *dependency_dir = absolute_dependency_project_dir;
    absolute_dependency_project_dir = NULL;
    // set manifest path and prevent double free
    *manifest_path = m_manifest_path;
    m_manifest_path = NULL;

cleanup:
    if (dependency_project_dir != NULL)
    {
        string_free(dependency_project_dir);
    }
    if (absolute_dependency_project_dir != NULL)
    {
        string_free(absolute_dependency_project_dir);
    }
    if (m_manifest_path != NULL)
    {
        string_free(m_manifest_path);
    }
    if (absolute_dependency_dir != NULL)
    {
        string_free(absolute_dependency_dir);
    }

    return manifest;
}

/// @brief Get the manifest path from a directory.
/// @param dir The directory to search for the manifest.
/// @return The path to the manifest file or NULL if not found.
string_t *get_manifest_path_from_path(const string_t *dir)
{
    string_t *manifest_path = string_clone(dir);
    if (manifest_path == NULL)
    {
        log_error("Failed to clone directory path %s.", dir->data);
        return NULL;
    }

    string_append_path(manifest_path, CREN_MANIFEST_NAME);

    return manifest_path;
}

/// @brief Download a git repository.
/// @param name
/// @param uri
/// @param target_dir
/// @return NULL if failed, otherwise the path to the downloaded repository.
string_t *download_git_repository(const string_t *name, const string_t *uri, const string_t *target_dir)
{
    log_error("Git repository download is not implemented yet. URI: %s", uri->data);
    return NULL;
}
