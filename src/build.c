#include <build.h>
#include <cren.h>
#include <lib/log.h>

build_t *build_init(void)
{
    build_t *build = malloc(sizeof(build_t));
    if (build == NULL)
    {
        log_fatal("Failed to allocate memory for build object.");
        return NULL;
    }

    build->language = C11;
    build->defines = NULL;
    build->include_dirs = NULL;
    build->target_dir = NULL;
    build->links = NULL;
    build->sources = NULL;
    build->sources_len = 0;
    build->targets = NULL;
    build->targets_len = 0;
    build->release = false;

    return build;
}

void build_free(build_t *build)
{
    if (build == NULL)
    {
        log_debug("Attempted to free a NULL build object.");
        return;
    }

    string_list_free(build->defines);
    string_list_free(build->include_dirs);
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
            source_free(build->targets[i]);
        }
        free(build->targets);
    }

    free(build);
}

int build_add_source(build_t *build, const char *src)
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

int build_add_target(build_t *build, const char *target)
{
    if (build == NULL || build->target_dir == NULL)
    {
        log_error("Attempted to add a target file to a NULL build object.");
        return CREN_NOK;
    }

    source_t *source = source_init(target, build->target_dir->data);
    if (source == NULL)
    {
        log_error("Failed to initialize source object.");
        return CREN_NOK;
    }

    build->targets = realloc(build->targets, sizeof(source_t *) * (build->targets_len + 1));
    if (build->targets == NULL)
    {
        log_error("Failed to reallocate memory for targets.");
        source_free(source);
        return CREN_NOK;
    }

    build->targets[build->targets_len] = source;
    build->targets_len++;

    return CREN_OK;
}

int build_compile(build_t *build)
{
    if (build == NULL)
    {
        log_error("Attempted to compile a NULL build object.");
        return CREN_NOK;
    }

    log_info("Compiling project...");
    return CREN_OK;
}
