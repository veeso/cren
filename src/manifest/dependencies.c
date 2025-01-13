#include <cren.h>
#include <lib/log.h>
#include <manifest/dependencies.h>

cren_manifest_dependencies_t *cren_manifest_dependencies_init(void)
{
    cren_manifest_dependencies_t *dependencies = (cren_manifest_dependencies_t *)malloc(sizeof(cren_manifest_dependencies_t));
    if (dependencies == NULL)
    {
        log_fatal("Failed to allocate memory for manifest dependencies");
        return NULL;
    }

    dependencies->dependencies = NULL;
    dependencies->dependencies_len = 0;
    dependencies->dev_dependencies = NULL;
    dependencies->dev_dependencies_len = 0;

    log_trace("Initialized manifest dependencies");

    return dependencies;
}

void cren_manifest_dependencies_free(cren_manifest_dependencies_t *dependencies)
{
    if (dependencies == NULL)
    {
        log_trace("Manifest dependencies is NULL");
        return;
    }

    for (size_t i = 0; i < dependencies->dependencies_len; i++)
    {
        log_trace("Freeing dependency %zu", i);
        cren_manifest_dependency_free(dependencies->dependencies[i]);
    }

    for (size_t i = 0; i < dependencies->dev_dependencies_len; i++)
    {
        log_trace("Freeing dev dependency %zu", i);
        cren_manifest_dependency_free(dependencies->dev_dependencies[i]);
    }

    free(dependencies->dependencies);
    free(dependencies->dev_dependencies);
    free(dependencies);

    log_trace("Freed manifest dependencies");
}

cren_manifest_dependency_t *cren_manifest_dependency_init(void)
{
    cren_manifest_dependency_t *dependency = (cren_manifest_dependency_t *)malloc(sizeof(cren_manifest_dependency_t));
    if (dependency == NULL)
    {
        log_fatal("Failed to allocate memory for manifest dependency");
        return NULL;
    }

    dependency->name = NULL;
    dependency->git = NULL;
    dependency->link = NULL;
    dependency->optional = false;
    dependency->defines = NULL;

    log_trace("Initialized manifest dependency");

    return dependency;
}

int cren_manifest_dependencies_add_dependency(cren_manifest_dependency_t ***dest, size_t *len, cren_manifest_dependency_t *dependency)
{
    cren_manifest_dependency_t **new_dependencies = (cren_manifest_dependency_t **)realloc(*dest, (*len + 1) * sizeof(cren_manifest_dependency_t *));
    if (new_dependencies == NULL)
    {
        log_fatal("Failed to reallocate memory for dependencies");
        return CREN_NOK;
    }

    *dest = new_dependencies;
    (*dest[*len]) = dependency;
    (*len)++;

    log_trace("Added dependency `%.*s`", dependency->name->length, dependency->name->data);

    return CREN_OK;
}

void cren_manifest_dependency_free(cren_manifest_dependency_t *dependency)
{
    if (dependency == NULL)
    {
        log_trace("Manifest dependency is NULL");
        return;
    }

    string_free(dependency->name);
    string_free(dependency->git);
    string_free(dependency->link);
    string_list_free(dependency->defines);
    free(dependency);

    log_trace("Freed manifest dependency");
}

cren_manifest_dependency_t *cren_manifest_dependency_clone(cren_manifest_dependency_t *dependency)
{
    if (dependency == NULL)
    {
        log_error("Feature is NULL");
        return NULL;
    }

    cren_manifest_dependency_t *clone = cren_manifest_dependency_init();
    if (clone == NULL)
    {
        log_error("Failed to clone feature");
        return NULL;
    }

    if (dependency->name != NULL)
    {
        clone->name = string_clone(dependency->name);
        if (clone->name == NULL)
        {
            log_error("Failed to clone feature name");
            cren_manifest_dependency_free(clone);
            return NULL;
        }
    }

    if (dependency->git != NULL)
    {
        clone->git = string_clone(dependency->git);
        if (clone->git == NULL)
        {
            log_error("Failed to clone feature git");
            cren_manifest_dependency_free(clone);
            return NULL;
        }
    }

    if (dependency->link != NULL)
    {
        clone->link = string_clone(dependency->link);
        if (clone->link == NULL)
        {
            log_error("Failed to clone feature link");
            cren_manifest_dependency_free(clone);
            return NULL;
        }
    }

    clone->optional = dependency->optional;

    if (dependency->defines != NULL)
    {
        clone->defines = string_list_init();
        if (clone->defines == NULL)
        {
            log_error("Failed to clone feature defines");
            cren_manifest_dependency_free(clone);
            return NULL;
        }
        for (size_t i = 0; i < dependency->defines->nitems; i++)
        {
            string_t *define = string_clone(dependency->defines->items[i]);
            if (define == NULL)
            {
                log_error("Failed to clone feature define");
                cren_manifest_dependency_free(clone);
                return NULL;
            }
            string_list_push(clone->defines, define);
        }
    }

    return clone;
}
