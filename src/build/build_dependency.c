#include <build/build_dependency.h>

build_dependency_t *build_dependency_init(const string_t *name, const string_t *uri, const string_list_t *features, bool default_features, build_dependency_type_t type)
{
    build_dependency_t *dependency = (build_dependency_t *)malloc(sizeof(build_dependency_t));
    if (dependency == NULL)
    {
        return NULL;
    }
    dependency->name = NULL;
    dependency->uri = NULL;
    dependency->features = NULL;

        dependency->name = string_clone(name);
    if (dependency->name == NULL)
    {
        build_dependency_free(dependency);
        return NULL;
    }

    dependency->uri = string_clone(uri);
    if (dependency->uri == NULL)
    {
        build_dependency_free(dependency);
        return NULL;
    }

    dependency->features = string_list_clone(features);
    dependency->default_features = default_features;
    dependency->type = type;

    return dependency;
}

void build_dependency_free(build_dependency_t *dependency)
{
    if (dependency == NULL)
    {
        return;
    }

    if (dependency->name != NULL)
        string_free(dependency->name);
    if (dependency->uri != NULL)
        string_free(dependency->uri);
    if (dependency->features != NULL)
        string_list_free(dependency->features);
    free(dependency);
}
