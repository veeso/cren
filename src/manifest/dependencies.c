#include <manifest/dependencies.h>

cren_manifest_dependencies_t *cren_manifest_dependencies_init()
{
    cren_manifest_dependencies_t *dependencies = (cren_manifest_dependencies_t *)malloc(sizeof(cren_manifest_dependencies_t));
    if (dependencies == NULL)
    {
        return NULL;
    }

    dependencies->dependencies = NULL;
    dependencies->dependencies_len = 0;
    return dependencies;
}

void cren_manifest_dependencies_free(cren_manifest_dependencies_t *dependencies)
{
    if (dependencies == NULL)
    {
        return;
    }

    for (size_t i = 0; i < dependencies->dependencies_len; i++)
    {
        cren_manifest_dependency_free(dependencies->dependencies[i]);
    }

    free(dependencies->dependencies);
    free(dependencies);
}

void cren_manifest_dependency_free(cren_manifest_dependency_t *dependency)
{
    if (dependency == NULL)
    {
        return;
    }

    string_free(dependency->name);
    string_free(dependency->git);
    string_free(dependency->link);
    string_list_free(dependency->defines);
    free(dependency);
}
