
#include <build/target.h>
#include <lib/log.h>

target_t *target_init(const char *src, const char *target_dir, const char *target_name, target_type_t type)
{
    source_t *source = source_init(src, target_dir);
    if (source == NULL)
    {
        log_error("Failed to initialize source object.");
        return NULL;
    }

    target_t *target = malloc(sizeof(target_t));
    if (target == NULL)
    {
        log_error("Failed to allocate memory for target object.");
        source_free(source);
        return NULL;
    }

    target->src = source->src;
    target->obj = source->obj;
    target->target_name = NULL; // Will be set later
    target->type = type;

    // Prevent source_free from freeing the src and obj strings
    source->src = NULL; // Prevent source_free from freeing the src string
    source->obj = NULL; // Prevent source_free from freeing the obj string
    source_free(source);

    target->target_name = string_from_cstr(target_name);
    if (target->target_name == NULL)
    {
        log_error("Failed to initialize target name.");
        target_free(target);
        return NULL;
    }

    return target;
}

void target_free(target_t *target)
{
    if (target == NULL)
    {
        log_error("Attempted to free a NULL target object.");
        return;
    }

    string_free(target->src);
    string_free(target->obj);
    string_free(target->target_name);
    free(target);
}

source_t *source_from_target(const target_t *target)
{
    return source_init(target->src->data, target->obj->data);
}
