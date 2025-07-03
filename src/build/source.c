#include <build/source.h>
#include <cren.h>
#include <lib/log.h>
#include <utils/paths.h>

string_t *source_path_in_src(const char *src);

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
    string_t *object_filename = source_path_in_src(src);
    bool is_in_src_root = (object_filename == NULL);
    if (object_filename == NULL)
    {
        // it's okay, just set empty (it means that the source file is in the root of src)
        object_filename = string_from_cstr("");
    }

    string_t *src_filename = file_name(src);
    if (src_filename == NULL)
    {
        log_error("Failed to get object file name.");
        string_free(object_filename);
        string_free(source->src);
        free(source);
        return NULL;
    }

    // push object file name
    if (!is_in_src_root)
    {
        string_append(object_filename, "/");
    }
    string_append(object_filename, src_filename->data);
    string_free(src_filename);

    string_append(object_filename, ".o");
    source->obj = object_filename;

    log_debug("Source file: %s with object file %s", src, object_filename->data);

    source->target_name = NULL;

    return source;
}

source_t *target_init(const char *src, const char *target_dir, const char *target_name)
{
    source_t *source = source_init(src, target_dir);
    if (source == NULL)
    {
        log_error("Failed to initialize source object.");
        return NULL;
    }

    source->target_name = string_from_cstr(target_name);
    if (source->target_name == NULL)
    {
        log_error("Failed to initialize target name.");
        source_free(source);
        return NULL;
    }

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
    string_free(source->target_name);
    free(source);
}

string_t *source_path_in_src(const char *src)
{
    // find "src/" in src
    const char *src_dir = strstr(src, SRC_DIR);
    if (src_dir == NULL)
    {
        log_error("Source file %s is not in src directory.", src);
        return NULL;
    }

    // get source path in src
    string_t *source_path = string_from_cstr(src_dir + sizeof(SRC_DIR) - 1);
    if (source_path == NULL)
    {
        log_error("Failed to get source path in src.");
        return NULL;
    }

    log_debug("source path in src: %s", source_path->data);

    // remove file name
    string_t *source_dir = parent_dir(source_path->data);
    if (source_dir == NULL)
    {
        log_debug("Failed to get source directory.");
        string_free(source_path);
        return NULL;
    }

    string_free(source_path);

    return source_dir;
}
