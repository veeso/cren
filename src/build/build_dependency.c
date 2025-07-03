#include <build/build_dependency.h>

build_dependency_t *build_dependency_init(const char *uri, build_dependency_type_t type)
{
    build_dependency_t *dependency = (build_dependency_t *)malloc(sizeof(build_dependency_t));
    if (dependency == NULL)
    {
        return NULL;
    }

    dependency->uri = string_from_cstr(uri);
    if (dependency->uri == NULL)
    {
        free(dependency);
        return NULL;
    }

    dependency->type = type;

    return dependency;
}

void build_dependency_free(build_dependency_t *dependency)
{
    if (dependency == NULL)
    {
        return;
    }

    string_free(dependency->uri);
    free(dependency);
}
