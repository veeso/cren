#include <manifest/dependencies.h>
#include <cren.h>

cren_manifest_dependencies_t *cren_manifest_dependencies_init()
{
    cren_manifest_dependencies_t *dependencies = (cren_manifest_dependencies_t *)malloc(sizeof(cren_manifest_dependencies_t));
    if (dependencies == NULL)
    {
        return NULL;
    }

    dependencies->dependencies = NULL;
    dependencies->dependencies_len = 0;
    dependencies->dev_dependencies = NULL;
    dependencies->dev_dependencies_len = 0;
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

    for (size_t i = 0; i < dependencies->dev_dependencies_len; i++)
    {
        cren_manifest_dependency_free(dependencies->dev_dependencies[i]);
    }

    free(dependencies->dependencies);
    free(dependencies->dev_dependencies);
    free(dependencies);
}

cren_manifest_dependency_t *cren_manifest_dependency_init()
{
    cren_manifest_dependency_t *dependency = (cren_manifest_dependency_t *)malloc(sizeof(cren_manifest_dependency_t));
    if (dependency == NULL)
    {
        return NULL;
    }

    dependency->name = NULL;
    dependency->git = NULL;
    dependency->link = NULL;
    dependency->optional = false;
    dependency->defines = NULL;
    return dependency;
}

int cren_manifest_dependencies_add_dependency(cren_manifest_dependency_t **dest, size_t *len, cren_manifest_dependency_t *dependency)
{
    cren_manifest_dependency_t **new_dependencies = (cren_manifest_dependency_t **)realloc(*dest, (*len + 1) * sizeof(cren_manifest_dependency_t *));
    if (new_dependencies == NULL)
    {
        return CREN_NOK;
    }

    dest = new_dependencies;
    dest[*len] = dependency;
    (*len)++;
    return CREN_OK;
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
