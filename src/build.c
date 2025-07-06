#include <build.h>
#include <cren.h>
#include <lib/log.h>

build_cfg_t *build_init(const string_t *project_dir)
{
    build_cfg_t *build = malloc(sizeof(build_cfg_t));
    if (build == NULL)
    {
        log_fatal("Failed to allocate memory for build object.");
        return NULL;
    }

    build->project_dir = string_clone(project_dir);
    if (build->project_dir == NULL)
    {
        log_error("Failed to clone project directory.");
        build_free(build);
        return NULL;
    }

    build->language = C11;
    build->defines = NULL;
    build->include_dirs = string_list_init();
    if (build->include_dirs == NULL)
    {
        log_error("Failed to initialize include directories.");
        build_free(build);
        return NULL;
    }
    build->links = string_list_init();
    if (build->links == NULL)
    {
        log_error("Failed to initialize links.");
        build_free(build);
        return NULL;
    }
    build->objects = string_list_init();
    if (build->objects == NULL)
    {
        log_error("Failed to initialize libraries.");
        build_free(build);
        return NULL;
    }
    build->target_dir = NULL;
    build->sources = NULL;
    build->sources_len = 0;
    build->targets = NULL;
    build->targets_len = 0;
    build->dependencies = NULL;
    build->dependencies_len = 0;
    build->release = false;

    return build;
}

void build_free(build_cfg_t *build)
{
    if (build == NULL)
    {
        log_debug("Attempted to free a NULL build object.");
        return;
    }

    string_free(build->project_dir);
    string_list_free(build->defines);
    string_list_free(build->include_dirs);
    string_list_free(build->objects);
    string_free(build->target_dir);
    string_list_free(build->links);

    if (build->sources != NULL)
    {
        for (size_t i = 0; i < build->sources_len; i++)
        {
            source_free(build->sources[i]);
        }
        free(build->sources);
    }

    if (build->targets != NULL)
    {
        for (size_t i = 0; i < build->targets_len; i++)
        {
            target_free(build->targets[i]);
        }
        free(build->targets);
    }

    if (build->dependencies != NULL)
    {
        for (size_t i = 0; i < build->dependencies_len; i++)
        {
            build_dependency_free(build->dependencies[i]);
        }
        free(build->dependencies);
    }

    free(build);
}

int build_add_source(build_cfg_t *build, const char *src)
{
    if (build == NULL || build->target_dir == NULL)
    {
        log_error("Attempted to add a source file to a NULL build object.");
        return CREN_NOK;
    }

    source_t *source = source_init(src, build->target_dir->data);
    if (source == NULL)
    {
        log_error("Failed to initialize source object.");
        return CREN_NOK;
    }

    build->sources = realloc(build->sources, sizeof(source_t *) * (build->sources_len + 1));
    if (build->sources == NULL)
    {
        log_error("Failed to reallocate memory for sources.");
        source_free(source);
        return CREN_NOK;
    }

    build->sources[build->sources_len] = source;
    build->sources_len++;

    return CREN_OK;
}

int build_add_target(build_cfg_t *build, const char *target_name, const char *src_path, const char *project_dir, target_type_t type)
{
    if (build == NULL || build->target_dir == NULL || target_name == NULL || src_path == NULL || project_dir == NULL)
    {
        log_error("Attempted to add a target file to a NULL build object.");
        return CREN_NOK;
    }

    string_t *source_path = string_from_cstr(project_dir);
    string_append_path(source_path, src_path);

    target_t *target = target_init(source_path->data, build->target_dir->data, target_name, type);
    string_free(source_path);
    if (target == NULL)
    {
        log_error("Failed to initialize source object.");
        return CREN_NOK;
    }

    build->targets = realloc(build->targets, sizeof(target_t *) * (build->targets_len + 1));
    if (build->targets == NULL)
    {
        log_error("Failed to reallocate memory for targets.");
        target_free(target);
        return CREN_NOK;
    }

    build->targets[build->targets_len] = target;
    build->targets_len++;

    return CREN_OK;
}
