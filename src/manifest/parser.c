#include <stdbool.h>
#include <string.h>

#include <cren.h>
#include <lib/log.h>
#include <lib/toml-c.h>
#include <manifest.h>

const char *KEY_PACKAGE = "package";
const char *KEY_TARGETS = "targets";
const char *KEY_FEATURES = "features";

const char *KEY_PACKAGE_NAME = "name";
const char *KEY_PACKAGE_DESCRIPTION = "description";
const char *KEY_PACKAGE_VERSION = "version";
const char *KEY_PACKAGE_EDITION = "edition";
const char *KEY_PACKAGE_LANGUAGE = "language";
const char *KEY_PACKAGE_AUTHORS = "authors";
const char *KEY_PACKAGE_DOCUMENTATION = "documentation";
const char *KEY_PACKAGE_HOMEPAGE = "homepage";
const char *KEY_PACKAGE_REPOSITORY = "repository";
const char *KEY_PACKAGE_LICENSE = "license";
const char *KEY_PACKAGE_LICENSE_FILE = "license-file";

const char *KEY_TARGETS_LIB = "lib";
const char *KEY_TARGETS_BIN = "bin";
const char *KEY_TARGETS_EXAMPLES = "example";
const char *KEY_TARGETS_NAME = "name";
const char *KEY_TARGETS_PATH = "path";
const char *KEY_TARGETS_REQUIRED_FEATURES = "required-features";

const char *KEY_DEPENDENCIES = "dependencies";
const char *KEY_DEV_DEPENDENCIES = "dev-dependencies";
const char *KEY_DEPENDENCY_GIT = "git";
const char *KEY_DEPENDENCY_LINK = "link";
const char *KEY_DEPENDENCY_OPTIONAL = "optional";

const char *KEY_FEATURES_DEFAULT = "default";
const char *KEY_FEATURES_DEFINES = "defines";
const char *KEY_FEATURES_DEPENDENCIES = "dependencies";

// declare functions
int cren_manifest_parse_package(toml_table_t *manifest, cren_manifest_package_t *manifest_package, char *error, size_t error_sz);
int cren_manifest_parse_targets(toml_table_t *manifest, cren_manifest_targets_t *targets, char *error, size_t error_sz);
int cren_manifest_parse_dependencies(toml_table_t *manifest, cren_manifest_dependencies_t *dependencies, char *error, size_t error_sz);
int cren_manifest_parse_features(toml_table_t *manifest, cren_manifest_features_t *features, char *error, size_t error_sz);
int get_string(toml_table_t *table, const char *key, string_t **dest, bool required);
int get_string_list(toml_table_t *table, const char *key, string_list_t **dest, bool required);
int get_semver(toml_table_t *table, const char *key, semver_t *dest);
int get_edition(toml_table_t *table, const char *key, edition_t *dest);
int get_language(toml_table_t *table, const char *key, language_t *dest);
int get_license(toml_table_t *table, const char *key, license_t *dest);
int parse_target_cfg(toml_table_t *table, cren_manifest_target_cfg_t *cfg, char *error, size_t error_sz);
int parse_dependencies(toml_table_t *deps, cren_manifest_dependency_t **dest, size_t *dest_len, char *error, size_t error_sz);
int parse_dependency(toml_table_t *table, string_t *key, cren_manifest_dependency_t *dependency, char *error, size_t error_sz);
int parse_feature(toml_table_t *table, string_t *name, cren_manifest_features_t *features, char *error, size_t error_sz);
void parse_error(char *error, size_t error_sz, const char *msg);

int cren_manifest_parse(cren_manifest_t *manifest, FILE *file, char *error, size_t error_sz)
{
    if (manifest == NULL || file == NULL)
    {
        parse_error(error, error_sz, "One or more arguments are NULL");
        log_error("One or more arguments are NULL");
        return CREN_NOK;
    }

    toml_table_t *toml = toml_parse_file(file, error, (int)error_sz);

    if (toml == NULL)
    {
        log_error("Error parsing toml file: %s", error);
        return CREN_NOK;
    }

    // TODO: parse edition first for future compatibility

    // parse `package`
    if (cren_manifest_parse_package(toml, manifest->package, error, error_sz) != CREN_OK)
    {
        toml_free(toml);
        return CREN_NOK;
    }

    // parse `targets`
    if (cren_manifest_parse_targets(toml, manifest->targets, error, error_sz) != CREN_OK)
    {
        toml_free(toml);
        return CREN_NOK;
    }

    // parse `dependencies`
    if (cren_manifest_parse_dependencies(toml, manifest->dependencies, error, error_sz) != CREN_OK)
    {
        toml_free(toml);
        return CREN_NOK;
    }

    // parse `features`
    if (cren_manifest_parse_features(toml, manifest->features, error, error_sz) != CREN_OK)
    {
        toml_free(toml);
        return CREN_NOK;
    }

    toml_free(toml);
    log_info("Cren.toml manifest parsed successfully");

    return CREN_OK;
}

/// @brief Parse the `package` table in the manifest
/// @param manifest
/// @param manifest_package
/// @param error
/// @param error_sz
/// @return CREN_OK on success, CREN_NOK on failure
int cren_manifest_parse_package(toml_table_t *manifest, cren_manifest_package_t *manifest_package, char *error, size_t error_sz)
{
    log_debug("Parsing `package` table");
    if (manifest == NULL || manifest_package == NULL)
    {
        parse_error(error, error_sz, "One or more arguments are NULL");
        return CREN_NOK;
    }
    toml_table_t *package = toml_table_table(manifest, KEY_PACKAGE);
    if (package == NULL)
    {
        parse_error(error, error_sz, "`package` table not found");
        return CREN_NOK;
    }

    // Parse `name`
    if (get_string(package, KEY_PACKAGE_NAME, &manifest_package->name, true) != CREN_OK)
    {
        parse_error(error, error_sz, "`name` key not found in `package` table");
        return CREN_NOK;
    }
    // Parse `description`
    if (get_string(package, KEY_PACKAGE_DESCRIPTION, &manifest_package->description, false) != CREN_OK)
    {
        parse_error(error, error_sz, "invalid `description` found in `package` table");
        return CREN_NOK;
    }
    // Parse `version`
    if (get_semver(package, KEY_PACKAGE_VERSION, &manifest_package->version) != CREN_OK)
    {
        parse_error(error, error_sz, "invalid `version` found in `package` table");
        return CREN_NOK;
    }
    // Parse `edition`
    if (get_edition(package, KEY_PACKAGE_EDITION, &manifest_package->edition) != CREN_OK)
    {
        parse_error(error, error_sz, "`edition` key not found in `package` table");
        return CREN_NOK;
    }
    if (manifest_package->edition == MANIFEST_EDITION_UNKNOWN)
    {
        parse_error(error, error_sz, "invalid `edition` found in `package` table");
        return CREN_NOK;
    }
    // Parse `language`'
    if (get_language(package, KEY_PACKAGE_LANGUAGE, &manifest_package->language) != CREN_OK)
    {
        parse_error(error, error_sz, "invalid `language` found in `package` table");
        return CREN_NOK;
    }
    if (manifest_package->language == LANGUAGE_UNKNOWN)
    {
        parse_error(error, error_sz, "invalid `language` found in `package` table");
        return CREN_NOK;
    }
    // Parse `authors`
    if (get_string_list(package, KEY_PACKAGE_AUTHORS, &manifest_package->authors, false) != CREN_OK)
    {
        parse_error(error, error_sz, "invalid `authors` found in `package` table");
        return CREN_NOK;
    }
    // Parse `documentation`
    if (get_string(package, KEY_PACKAGE_DOCUMENTATION, &manifest_package->documentation, false) != CREN_OK)
    {
        parse_error(error, error_sz, "invalid `documentation` found in `package` table");
        return CREN_NOK;
    }
    // Parse `homepage`
    if (get_string(package, KEY_PACKAGE_HOMEPAGE, &manifest_package->homepage, false) != CREN_OK)
    {
        parse_error(error, error_sz, "invalid `homepage` found in `package` table");
        return CREN_NOK;
    }
    // Parse `repository`
    if (get_string(package, KEY_PACKAGE_REPOSITORY, &manifest_package->repository, false) != CREN_OK)
    {
        parse_error(error, error_sz, "invalid `repository` found in `package` table");
        return CREN_NOK;
    }
    // Parse `license`
    if (get_license(package, KEY_PACKAGE_LICENSE, &manifest_package->license) != CREN_OK)
    {
        parse_error(error, error_sz, "invalid `license` found in `package` table");
        return CREN_NOK;
    }
    if (manifest_package->license == LICENSE_UNKNOWN)
    {
        parse_error(error, error_sz, "invalid `license` found in `package` table");
        return CREN_NOK;
    }
    // Parse `license-file`
    if (manifest_package->license == LICENSE_NONE && get_string(package, KEY_PACKAGE_LICENSE_FILE, &manifest_package->license_file, false) != CREN_OK)
    {
        parse_error(error, error_sz, "invalid `license-file` found in `package` table");
        return CREN_NOK;
    }

    log_debug("Parsed `package` table");

    return CREN_OK;
}

/// @brief Parse the `targets` table in the manifest
/// @param manifest
/// @param targets
/// @param error
/// @param error_sz
/// @return CREN_OK on success, CREN_NOK on failure
int cren_manifest_parse_targets(toml_table_t *manifest, cren_manifest_targets_t *targets, char *error, size_t error_sz)
{
    log_debug("Parsing `targets` table");
    if (manifest == NULL || targets == NULL)
    {
        parse_error(error, error_sz, "One or more arguments are NULL");
        return CREN_NOK;
    }

    // parse `lib`
    toml_table_t *lib = toml_table_table(manifest, KEY_TARGETS_LIB);
    if (lib != NULL)
    {
        log_debug("Parsing `lib` table");
        targets->lib = cren_manifest_target_cfg_init();
        if (parse_target_cfg(lib, targets->lib, error, error_sz) != CREN_OK)
        {
            parse_error(error, error_sz, "Error parsing `lib` table");
            return CREN_NOK;
        }
        log_debug("Parsed `lib` table");
    }

    // parse `bin` array
    toml_array_t *bin = toml_table_array(manifest, KEY_TARGETS_BIN);
    if (bin != NULL)
    {
        log_debug("Parsing `bin` table");
        const int len = toml_array_len(bin);
        log_debug("Found %d `bin` targets", len);
        for (int i = 0; i < len; i++)
        {
            log_debug("Parsing `bin` target %d", i);
            toml_table_t *bin_table = toml_array_table(bin, i);
            cren_manifest_target_cfg_t *cfg = cren_manifest_target_cfg_init();
            if (parse_target_cfg(bin_table, cfg, error, error_sz) != CREN_OK)
            {
                parse_error(error, error_sz, "Error parsing `bin` table");
                cren_manifest_target_cfg_free(cfg);
                return CREN_NOK;
            }
            log_trace("Parsed `bin` cfg %p", cfg);
            if (cren_manifest_targets_add_cfg(&targets->bin, &targets->bin_len, cfg) != CREN_OK)
            {
                parse_error(error, error_sz, "Error adding `bin` target");
                cren_manifest_target_cfg_free(cfg);
                return CREN_NOK;
            }
            log_debug("Parsed `bin` target %d", i);
        }
    }

    // parse `examples` array
    toml_array_t *examples = toml_table_array(manifest, KEY_TARGETS_EXAMPLES);
    if (examples != NULL)
    {
        log_debug("Parsing `examples` table");
        const int len = toml_array_len(examples);
        log_debug("Found %d `examples` targets", len);
        for (int i = 0; i < len; i++)
        {
            toml_table_t *examples_table = toml_array_table(examples, i);
            cren_manifest_target_cfg_t *cfg = cren_manifest_target_cfg_init();
            if (parse_target_cfg(examples_table, cfg, error, error_sz) != CREN_OK)
            {
                parse_error(error, error_sz, "Error parsing `examples` table");
                cren_manifest_target_cfg_free(cfg);
                return CREN_NOK;
            }
            if (cren_manifest_targets_add_cfg(&targets->examples, &targets->examples_len, cfg) != CREN_OK)
            {
                parse_error(error, error_sz, "Error adding `examples` target");
                cren_manifest_target_cfg_free(cfg);
                return CREN_NOK;
            }
            log_debug("Parsed `examples` target %d", i);
        }
    }

    log_debug("Parsed `targets` table");

    return CREN_OK;
}

/// @brief Parse the `dependencies` table in the manifest
/// @param manifest
/// @param dependencies
/// @param error
/// @param error_sz
/// @return CREN_OK on success, CREN_NOK on failure
int cren_manifest_parse_dependencies(toml_table_t *manifest, cren_manifest_dependencies_t *dependencies, char *error, size_t error_sz)
{
    log_debug("Parsing `dependencies` table");
    if (manifest == NULL || dependencies == NULL)
    {
        parse_error(error, error_sz, "One or more arguments are NULL");
        return CREN_NOK;
    }

    // get `dependencies` table
    toml_table_t *deps = toml_table_table(manifest, KEY_DEPENDENCIES);
    if (deps != NULL)
    {
        log_debug("Parsing `dependencies` table");
        if (parse_dependencies(deps, dependencies->dependencies, &dependencies->dependencies_len, error, error_sz) != CREN_OK)
        {
            parse_error(error, error_sz, "Error parsing `dependencies` table");
            return CREN_NOK;
        }
        log_debug("Parsed `dependencies` table");
    }

    // get `dev-dependencies` table
    toml_table_t *dev_deps = toml_table_table(manifest, KEY_DEV_DEPENDENCIES);
    if (dev_deps != NULL)
    {
        log_debug("Parsing `dev-dependencies` table");
        if (parse_dependencies(dev_deps, dependencies->dev_dependencies, &dependencies->dev_dependencies_len, error, error_sz) != CREN_OK)
        {
            parse_error(error, error_sz, "Error parsing `dev-dependencies` table");
            return CREN_NOK;
        }
        log_debug("Parsed `dev-dependencies` table");
    }

    log_debug("Parsed `dependencies` and `dev-dependencies` tables");

    return CREN_OK;
}

/// @brief Parse the `features` table in the manifest
/// @param manifest
/// @param manifest_features
/// @param error
/// @param error_sz
/// @return CREN_OK on success, CREN_NOK on failure
int cren_manifest_parse_features(toml_table_t *manifest, cren_manifest_features_t *manifest_features, char *error, size_t error_sz)
{
    log_debug("Parsing `features` table");
    if (manifest == NULL || manifest_features == NULL)
    {
        parse_error(error, error_sz, "One or more arguments are NULL");
        return CREN_NOK;
    }
    toml_table_t *features = toml_table_table(manifest, KEY_FEATURES);
    if (features == NULL)
    {
        log_debug("`features` table not found");
        return CREN_OK;
    }

    // iterate over features
    const int len = toml_table_len(features);
    log_debug("Found %d features", len);
    for (int i = 0; i < len; i++)
    {
        int keylen;
        const char *key = toml_table_key(features, i, &keylen);
        log_debug("Parsing feature `%.*s`", keylen, key);
        // check if `key` is `default`
        if (strncmp(key, KEY_FEATURES_DEFAULT, keylen) == 0)
        {
            log_debug("Parsing `default` features");
            // get as array
            toml_array_t *default_features = toml_table_array(features, key);
            if (default_features == NULL)
            {
                parse_error(error, error_sz, "Error parsing `default` features");
                return CREN_NOK;
            }
            // iterate over default features
            const int default_len = toml_array_len(default_features);
            log_debug("Found %d default features", default_len);
            string_list_t *default_list = string_list_init();
            if (default_list == NULL)
            {
                parse_error(error, error_sz, "Error allocating memory for default features");
                return CREN_NOK;
            }
            for (int j = 0; j < default_len; j++)
            {
                log_debug("Parsing default feature %d", j);
                toml_value_t value = toml_array_string(default_features, j);
                if (!value.ok)
                {
                    parse_error(error, error_sz, "Error parsing default feature");
                    string_list_free(default_list);
                    return CREN_NOK;
                }

                log_debug("Found default feature `%s`", value.u.s);

                string_t *name = string_from_cstr(value.u.s);
                free(value.u.s);
                if (name == NULL)
                {
                    parse_error(error, error_sz, "Error allocating memory for default feature");
                    string_list_free(default_list);
                    return CREN_NOK;
                }
                if (string_list_push(default_list, name) != CREN_OK)
                {
                    parse_error(error, error_sz, "Error adding default feature");
                    string_list_free(default_list);
                    return CREN_NOK;
                }
                log_debug("Parsed default feature %d", j);
            }
            manifest_features->default_features = default_list;
            log_debug("Parsed `default` features");
        }
        else
        {
            log_debug("Parsing feature `%.*s`", keylen, key);
            string_t *name = string_from_cstr(key);
            // parse feature
            toml_table_t *feature_table = toml_table_table(features, key);
            if (parse_feature(feature_table, name, manifest_features, error, error_sz) != CREN_OK)
            {
                parse_error(error, error_sz, "Error parsing feature table");
                string_free(name);
                return CREN_NOK;
            }
            log_debug("Parsed feature `%.*s`", keylen, key);
        }
    }

    log_debug("Parsed `features` table");

    return CREN_OK;
}

/// @brief Get a string from a toml table
/// @param table
/// @param key
/// @param dest
/// @return CREN_OK on success, CREN_NOK on failure
int get_string(toml_table_t *table, const char *key, string_t **dest, bool required)
{
    log_debug("Getting string `%s` from table", key);
    toml_value_t value = toml_table_string(table, key);
    if (!value.ok)
    {
        log_debug("Key `%s` not found in table (was required? %d)", key, required);
        return required ? CREN_NOK : CREN_OK;
    }

    string_free(*dest);
    *dest = string_from_cstr(value.u.s);
    free(value.u.s);

    log_debug("Got string `%s` from table", key);

    return CREN_OK;
}

int get_string_list(toml_table_t *table, const char *key, string_list_t **dest, bool required)
{
    log_debug("Getting string list `%s` from table", key);
    toml_array_t *array = toml_table_array(table, key);
    if (array == NULL)
    {
        log_debug("Key `%s` not found in table (was required? %d)", key, required);
        return required ? CREN_NOK : CREN_OK;
    }

    string_list_free(*dest);
    *dest = string_list_init();
    if (*dest == NULL)
    {
        log_error("Error allocating memory for string list");
        return CREN_NOK;
    }

    int len = toml_array_len(array);
    log_debug("Found %d strings in array", len);
    for (int i = 0; i < len; i++)
    {
        log_debug("Parsing string at index %d", i);
        toml_value_t value = toml_array_string(array, i);
        if (!value.ok)
        {
            log_debug("Error parsing string at index %d", i);
            return CREN_NOK;
        }

        log_debug("Found string `%s` at index %d", value.u.s, i);

        string_t *item = string_from_cstr(value.u.s);
        free(value.u.s);
        if (item == NULL)
        {
            log_error("Error allocating memory for string at index %d", i);
            return CREN_NOK;
        }

        if (string_list_push(*dest, item) != CREN_OK)
        {
            log_error("Error adding string at index %d", i);
            free(value.u.s);
            return CREN_NOK;
        }
    }

    log_debug("Got string list `%s` from table", key);

    return CREN_OK;
}

int get_semver(toml_table_t *table, const char *key, semver_t *dest)
{
    log_debug("Getting semver `%s` from table", key);
    toml_value_t value = toml_table_string(table, key);
    if (!value.ok)
    {
        log_debug("Key `%s` not found in table", key);
        return CREN_OK; // value is optional
    }

    log_debug("Found semver `%s` in table", value.u.s);
    semver_from_str(value.u.s, dest);
    free(value.u.s);

    log_debug("Got semver `%s` from table", key);

    return CREN_OK;
}

int get_edition(toml_table_t *table, const char *key, edition_t *dest)
{
    log_debug("Getting edition `%s` from table", key);
    toml_value_t value = toml_table_string(table, key);
    if (!value.ok)
    {
        log_error("Key `%s` not found in table", key);
        return CREN_NOK; // value is NOT optional
    }

    log_debug("Got edition `%s` from table: %s", key, value.u.s);
    *dest = edition_from_str(value.u.s);
    free(value.u.s);

    return CREN_OK;
}

int get_license(toml_table_t *table, const char *key, license_t *dest)
{
    log_debug("Getting license `%s` from table", key);
    toml_value_t value = toml_table_string(table, key);
    if (!value.ok)
    {
        log_debug("Key `%s` not found in table", key);
        return CREN_OK; // value is optional
    }

    log_debug("Got license `%s` from table: %s", key, value.u.s);
    *dest = license_from_str(value.u.s);
    free(value.u.s);

    return CREN_OK;
}

int get_language(toml_table_t *table, const char *key, language_t *dest)
{
    log_debug("Getting language `%s` from table", key);
    toml_value_t value = toml_table_string(table, key);
    if (!value.ok)
    {
        log_debug("Key `%s` not found in table", key);
        return CREN_OK; // value is optional
    }

    log_debug("Got language `%s` from table: %s", key, value.u.s);
    *dest = language_from_str(value.u.s);
    free(value.u.s);

    return CREN_OK;
}

int parse_target_cfg(toml_table_t *table, cren_manifest_target_cfg_t *cfg, char *error, size_t error_sz)
{
    if (table == NULL || cfg == NULL)
    {
        parse_error(error, error_sz, "One or more arguments are NULL");
        return CREN_NOK;
    }

    // Parse `name`
    if (get_string(table, KEY_TARGETS_NAME, &cfg->name, true) != CREN_OK)
    {
        parse_error(error, error_sz, "`name` key not found in target table");
        return CREN_NOK;
    }

    // Parse `path`
    if (get_string(table, KEY_TARGETS_PATH, &cfg->path, false) != CREN_OK)
    {
        parse_error(error, error_sz, "Invalid `path` found in target table");
        return CREN_NOK;
    }

    // Parse `required-features`
    if (get_string_list(table, KEY_TARGETS_REQUIRED_FEATURES, &cfg->required_features, false) != CREN_OK)
    {
        parse_error(error, error_sz, "Invalid `required-features` found in target table");
        return CREN_NOK;
    }

    log_debug("Parsed target cfg");

    return CREN_OK;
}

int parse_dependencies(toml_table_t *deps, cren_manifest_dependency_t **dest, size_t *dest_len, char *error, size_t error_sz)
{
    int table_len = toml_table_len(deps);
    log_debug("Found %d dependencies", table_len);
    // init deps
    for (int i = 0; i < table_len; i++)
    {
        int keylen;
        const char *key = toml_table_key(deps, i, &keylen);
        log_debug("Parsing dependency `%.*s`", keylen, key);
        string_t *name = string_from_cstr(key);
        if (name == NULL)
        {
            parse_error(error, error_sz, "Cannot allocate memory for dependency name");
            return CREN_NOK;
        }

        toml_table_t *dep_table = toml_table_table(deps, key);
        cren_manifest_dependency_t *dependency = cren_manifest_dependency_init();
        if (parse_dependency(dep_table, name, dependency, error, error_sz) != CREN_OK)
        {
            parse_error(error, error_sz, "Error parsing dependency table");
            cren_manifest_dependency_free(dependency);
            return CREN_NOK;
        }
        // push dependency
        if (cren_manifest_dependencies_add_dependency(dest, dest_len, dependency) != CREN_OK)
        {
            parse_error(error, error_sz, "Error adding dependency");
            cren_manifest_dependency_free(dependency);
            return CREN_NOK;
        }
        log_debug("Parsed dependency `%.*s`", keylen, key);
    }

    log_debug("Parsed dependencies");

    return CREN_OK;
}

int parse_dependency(toml_table_t *table, string_t *key, cren_manifest_dependency_t *dependency, char *error, size_t error_sz)
{
    if (table == NULL || dependency == NULL)
    {
        parse_error(error, error_sz, "One or more arguments are NULL");
        return CREN_NOK;
    }

    dependency->name = key;
    log_debug("Parsing dependency `%s`", key->data);
    if (get_string(table, KEY_DEPENDENCY_GIT, &dependency->git, false) != CREN_OK)
    {
        parse_error(error, error_sz, "Invalid `git` found in dependency table");
        return CREN_NOK;
    }

    if (get_string(table, KEY_DEPENDENCY_LINK, &dependency->link, false) != CREN_OK)
    {
        parse_error(error, error_sz, "Invalid `link` found in dependency table");
        return CREN_NOK;
    }

    toml_value_t value = toml_table_bool(table, KEY_DEPENDENCY_OPTIONAL);
    if (value.ok)
    {
        log_debug("Found `optional` in dependency table");
        dependency->optional = value.u.b;
    }
    else
    {
        log_debug("`optional` not found in dependency table");
        dependency->optional = false;
    }

    log_debug("Parsed dependency `%s`", key->data);

    return CREN_OK;
}

int parse_feature(toml_table_t *table, string_t *name, cren_manifest_features_t *features, char *error, size_t error_sz)
{
    log_debug("Parsing feature `%s`", name->data);
    if (table == NULL || features == NULL)
    {
        parse_error(error, error_sz, "One or more arguments are NULL");
        return CREN_NOK;
    }

    cren_manifest_feature_t *feature = cren_manifest_feature_init();
    if (feature == NULL)
    {
        parse_error(error, error_sz, "Cannot allocate memory for feature");
        return CREN_NOK;
    }

    feature->name = name;
    if (get_string_list(table, KEY_FEATURES_DEPENDENCIES, &feature->dependencies, false) != CREN_OK)
    {
        parse_error(error, error_sz, "Invalid `dependencies` found in feature table");
        cren_manifest_feature_free(feature);
        return CREN_NOK;
    }

    if (get_string_list(table, KEY_FEATURES_DEFINES, &feature->defines, false) != CREN_OK)
    {
        parse_error(error, error_sz, "Invalid `defines` found in feature table");
        cren_manifest_feature_free(feature);
        return CREN_NOK;
    }

    if (cren_manifest_features_add_feature(features, feature) != CREN_OK)
    {
        parse_error(error, error_sz, "Error adding feature");
        cren_manifest_feature_free(feature);
        return CREN_NOK;
    }

    log_debug("Parsed feature `%s`", name->data);

    return CREN_OK;
}

/// @brief Copy string literal to error buffer
/// @param error
/// @param error_sz
/// @param msg
void parse_error(char *error, size_t error_sz, const char *msg)
{
    snprintf(error, error_sz, "Error parsing Cren.toml manifest: %s", msg);
    log_error("%s", error);
}
