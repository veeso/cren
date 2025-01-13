#include <build/source.h>
#include <cren.h>
#include <lib/log.h>
#include <utils/paths.h>

source_t *source_init(const char *src, const char *target_dir)
{
    if (src == NULL)
    {
        log_error("Attempted to initialize a source object with a NULL source file.");
        return NULL;
    }
    source_t *source = malloc(sizeof(source_t));
    if (source == NULL)
    {
        log_error("Failed to allocate memory for source object.");
        return NULL;
    }

    source->src = string_from_cstr(src);
    if (source->src == NULL)
    {
        log_error("Failed to initialize source file.");
        free(source);
        return NULL;
    }

    // get object file name
    string_t *object_filename = file_name(src);
    if (object_filename == NULL)
    {
        log_error("Failed to get object file name.");
        string_free(source->src);
        free(source);
        return NULL;
    }

    string_append(object_filename, ".o");
    source->obj = object_filename;

    log_debug("Source file: %s with object file %s", src, object_filename->data);

    return source;
}

void source_free(source_t *source)
{
    if (source == NULL)
    {
        log_error("Attempted to free a NULL source object.");
        return;
    }

    string_free(source->src);
    string_free(source->obj);
    free(source);
}
