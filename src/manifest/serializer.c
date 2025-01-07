#include <stdarg.h>

#include <cren.h>
#include <manifest.h>
#include <manifest/dependencies.h>
#include <manifest/features.h>
#include <manifest/targets.h>

int serialize_package(cren_manifest_package_t *package, FILE *file);
int serialize_targets(cren_manifest_targets_t *targets, FILE *file);
void serialize_target_cfg(cren_manifest_target_cfg_t *cfg, FILE *file);
int serialize_dependencies(cren_manifest_dependencies_t *dependencies, FILE *file);
int serialize_dependency(cren_manifest_dependency_t *dependency, FILE *file);
int serialize_features(cren_manifest_features_t *features, FILE *file);
int serialize_feature(cren_manifest_feature_t *feature, FILE *file);
void print(FILE *file, const char *fmt, ...);
void println(FILE *file, const char *fmt, ...);

int cren_manifest_write(cren_manifest_t *manifest, FILE *file)
{
    if (serialize_package(manifest->package, file) != CREN_OK)
    {
        return CREN_NOK;
    }
    if (serialize_targets(manifest->targets, file) != CREN_OK)
    {
        return CREN_NOK;
    }
    if (serialize_dependencies(manifest->dependencies, file) != CREN_OK)
    {
        return CREN_NOK;
    }
    if (serialize_features(manifest->features, file) != CREN_OK)
    {
        return CREN_NOK;
    }

    return CREN_OK;
}

int serialize_package(cren_manifest_package_t *package, FILE *file)
{

    if (package->name == NULL || package->name->length == 0)
    {
        return CREN_NOK;
    }

    println(file, "[package]");
    println(file, "name = \"%s\"", package->name->data);
    if (package->version.major != 0 &&
        package->version.minor != 0 &&
        package->version.patch != 0)
    {
        char *semver = semver_to_str(&package->version);
        println(file, "version = \"%s\"", semver);
        free(semver);
    }
    if (package->description != NULL)
        println(file, "description = \"%s\"", package->description->data);

    println(file, "edition = \"%s\"", edition_to_string(package->edition));
    if (package->language != LANGUAGE_UNKNOWN)
        println(file, "language = \"%s\"", language_to_string(package->language));

    if (package->authors != NULL)
    {
        println(file, "authors = [");
        for (int i = 0; i < package->authors->nitems; i++)
        {
            println(file, "    \"%s\"", package->authors->items[i]->data);
        }
        println(file, "]");
    }

    if (package->license != LICENSE_NONE)
        println(file, "license = \"%s\"", license_to_string(package->license));
    if (package->license_file != NULL)
        println(file, "license-file = \"%s\"", package->license_file->data);
    if (package->repository != NULL)
        println(file, "repository = \"%s\"", package->repository->data);
    if (package->homepage != NULL)
        println(file, "homepage = \"%s\"", package->homepage->data);
    if (package->documentation != NULL)
        println(file, "documentation = \"%s\"", package->documentation->data);

    println(file, "");

    return CREN_OK;
}

int serialize_targets(cren_manifest_targets_t *targets, FILE *file)
{
    if (targets->lib != NULL)
    {
        println(file, "[lib]");
        serialize_target_cfg(targets->lib, file);
    }

    for (size_t i = 0; i < targets->bin_len; i++)
    {
        println(file, "[[bin]]");
        serialize_target_cfg(targets->bin[i], file);
    }

    for (size_t i = 0; i < targets->examples_len; i++)
    {
        println(file, "[[example]]");
        serialize_target_cfg(targets->examples[i], file);
    }

    println(file, "");

    return CREN_OK;
}

void serialize_target_cfg(cren_manifest_target_cfg_t *cfg, FILE *file)
{
    if (cfg->name != NULL)
        println(file, "name = \"%s\"", cfg->name->data);
    if (cfg->path != NULL)
        println(file, "path = \"%s\"", cfg->path->data);

    if (cfg->required_features != NULL)
    {
        println(file, "required-features = [");
        for (int i = 0; i < cfg->required_features->nitems; i++)
        {
            println(file, "    \"%s\"", cfg->required_features->items[i]->data);
        }
        println(file, "]");
    }
}

int serialize_dependencies(cren_manifest_dependencies_t *dependencies, FILE *file)
{
    if (dependencies->dependencies_len > 0)
    {
        println(file, "[dependencies]");
        for (size_t i = 0; i < dependencies->dependencies_len; i++)
        {
            if (serialize_dependency(dependencies->dependencies[i], file) != CREN_OK)
            {
                return CREN_NOK;
            }
        }
    }

    if (dependencies->dev_dependencies_len > 0)
    {
        println(file, "[dev-dependencies]");
        for (size_t i = 0; i < dependencies->dev_dependencies_len; i++)
        {
            if (serialize_dependency(dependencies->dev_dependencies[i], file) != CREN_OK)
            {
                return CREN_NOK;
            }
        }
    }

    println(file, "");

    return CREN_OK;
}

int serialize_dependency(cren_manifest_dependency_t *dependency, FILE *file)
{
    if (dependency->name == NULL)
    {
        return CREN_NOK;
    }

    print(file, "%s = { ", dependency->name->data);
    int first = 1;
    if (dependency->git != NULL)
    {
        print(file, "git = \"%s\"", dependency->git->data);
        first = 0;
    }

    if (dependency->link != NULL)
    {
        if (!first)
        {
            print(file, ", ");
        }
        print(file, "link = \"%s\"", dependency->link->data);
        first = 0;
    }

    if (dependency->optional)
    {
        if (!first)
        {
            print(file, ", ");
        }
        print(file, "optional = true");
        first = 0;
    }

    if (dependency->defines != NULL)
    {
        if (!first)
        {
            print(file, ", ");
        }
        print(file, "defines = [");
        for (int i = 0; i < dependency->defines->nitems; i++)
        {
            print(file, "\"%s\"", dependency->defines->items[i]->data);
            if (i < dependency->defines->nitems - 1)
            {
                print(file, ", ");
            }
        }
        print(file, "]");
    }

    println(file, " }");

    return CREN_OK;
}

int serialize_features(cren_manifest_features_t *features, FILE *file)
{
    if (features->default_features != NULL || features->features_len > 0)
        println(file, "[features]");

    if (features->default_features != NULL)
    {
        print(file, "default = [");
        for (int i = 0; i < features->default_features->nitems; i++)
        {
            print(file, "\"%s\"", features->default_features->items[i]->data);
            if (i < features->default_features->nitems - 1)
            {
                print(file, ", ");
            }
        }
        println(file, "]");
    }

    for (size_t i = 0; i < features->features_len; i++)
    {
        if (serialize_feature(features->features[i], file) != CREN_OK)
        {
            return CREN_NOK;
        }
    }

    println(file, "");

    return CREN_OK;
}

int serialize_feature(cren_manifest_feature_t *feature, FILE *file)
{
    if (feature->name == NULL)
    {
        return CREN_NOK;
    }

    print(file, "%s = { ", feature->name->data);

    int first = 1;
    if (feature->defines != NULL)
    {
        print(file, "defines = [");
        for (int i = 0; i < feature->defines->nitems; i++)
        {
            print(file, "\"%s\"", feature->defines->items[i]->data);
            if (i < feature->defines->nitems - 1)
            {
                print(file, ", ");
            }
        }
        print(file, "]");
        first = 0;
    }

    if (feature->dependencies != NULL)
    {
        if (!first)
        {
            print(file, ", ");
        }
        print(file, "dependencies = [");
        for (int i = 0; i < feature->dependencies->nitems; i++)
        {
            print(file, "\"%s\"", feature->dependencies->items[i]->data);
            if (i < feature->dependencies->nitems - 1)
            {
                print(file, ", ");
            }
        }
        print(file, "]");
    }

    println(file, " }");

    return CREN_OK;
}

void print(FILE *file, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if (file == NULL)
    {
        vprintf(fmt, ap);
    }
    else
    {
        vfprintf(file, fmt, ap);
    }

    va_end(ap);
}

void println(FILE *file, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    // get length of formatted string

    if (file == NULL)
    {
        vprintf(fmt, ap);
        printf("\n");
    }
    else
    {
        vfprintf(file, fmt, ap);
        fprintf(file, "\n");
    }

    va_end(ap);
}
