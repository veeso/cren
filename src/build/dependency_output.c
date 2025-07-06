#include <build/dependency_output.h>
#include <cren.h>
#include <lib/log.h>

/// @brief Add an object to the dependency outputs.
/// @param outputs The dependency outputs.
/// @param include the include path to add.
/// @return CREN_OK on success, CREN_NOK on failure.
int dependency_outputs_add_include(dependency_outputs_t *outputs, const string_t *include);

/// @brief Add an object to the dependency outputs.
/// @param outputs The dependency outputs.
/// @param object the object path to add.
/// @return CREN_OK on success, CREN_NOK on failure.
int dependency_outputs_add_object(dependency_outputs_t *outputs, const string_t *object);

dependency_outputs_t *dependency_outputs_init(void)
{
    dependency_outputs_t *outputs = malloc(sizeof(dependency_outputs_t));
    if (outputs == NULL)
    {
        log_error("Failed to allocate memory for dependency outputs.");
        return NULL;
    }
    outputs->includes = NULL;
    outputs->objects = NULL;

    outputs->includes = string_list_init();
    if (outputs->includes == NULL)
    {
        log_error("Failed to initialize includes list for dependency outputs.");
        dependency_outputs_free(outputs);
        return NULL;
    }

    outputs->objects = string_list_init();
    if (outputs->objects == NULL)
    {
        log_error("Failed to initialize objects list for dependency outputs.");
        dependency_outputs_free(outputs);
        return NULL;
    }

    return outputs;
}

void dependency_outputs_free(dependency_outputs_t *outputs)
{
    if (outputs == NULL)
        return;

    if (outputs->includes)
        string_list_free(outputs->includes);

    if (outputs->objects)
        string_list_free(outputs->objects);

    free(outputs);
}

int dependency_outputs_insert(dependency_outputs_t *outputs, const build_cfg_t *build, const build_environment_t *env, const string_t *objects_dir)
{
    for (size_t i = 0; i < build->include_dirs->nitems; i++)
    {
        string_t *include_dir = build->include_dirs->items[i];
        if (dependency_outputs_add_include(outputs, include_dir) != CREN_OK)
        {
            log_error("Failed to add include dir %s to dependency outputs.", include_dir->data);
            return CREN_NOK;
        }
    }

    for (size_t i = 0; i < build->targets_len; i++)
    {
        const target_t *target = build->targets[i];
        string_t *lib_path = get_lib_path(env, build, target);
        log_debug("Adding library artifact from dependency %s: %s", build->project_dir->data, lib_path->data);
        string_list_push(outputs->objects, lib_path);
    }

    return CREN_OK;
}

int dependency_outputs_concat(dependency_outputs_t *outputs, const dependency_outputs_t *other)
{
    for (size_t i = 0; i < other->includes->nitems; i++)
    {
        string_t *include = string_clone(other->includes->items[i]);
        if (include == NULL)
        {
            log_error("Failed to clone include %s.", other->includes->items[i]->data);
            return CREN_NOK;
        }
        if (string_list_push(outputs->includes, include) != CREN_OK)
        {
            log_error("Failed to add include %s to dependency outputs.", include->data);
            return CREN_NOK;
        }
    }

    for (size_t i = 0; i < other->objects->nitems; i++)
    {
        string_t *object = string_clone(other->objects->items[i]);
        if (object == NULL)
        {
            log_error("Failed to clone object %s.", other->objects->items[i]->data);
            return CREN_NOK;
        }
        if (string_list_push(outputs->objects, object) != CREN_OK)
        {
            log_error("Failed to add object %s to dependency outputs.", object->data);
            return CREN_NOK;
        }
    }

    return CREN_OK;
}

int dependency_outputs_add_include(dependency_outputs_t *outputs, const string_t *include)
{
    if (outputs == NULL || include == NULL)
    {
        log_error("Invalid arguments for dependency_outputs_add_include.");
        return CREN_NOK;
    }

    string_t *include_clone = string_clone(include);
    if (include_clone == NULL)
    {
        log_error("Failed to clone include %s.", include->data);
        return CREN_NOK;
    }

    if (string_list_push(outputs->includes, include_clone) != CREN_OK)
    {
        log_error("Failed to add include %s to dependency outputs.", include->data);
        string_free(include_clone);
        return CREN_NOK;
    }

    return CREN_OK;
}

int dependency_outputs_add_object(dependency_outputs_t *outputs, const string_t *object)
{
    if (outputs == NULL || object == NULL)
    {
        log_error("Invalid arguments for dependency_outputs_add_object.");
        return CREN_NOK;
    }

    string_t *object_clone = string_clone(object);
    if (object_clone == NULL)
    {
        log_error("Failed to clone object %s.", object->data);
        return CREN_NOK;
    }

    if (string_list_push(outputs->objects, object_clone) != CREN_OK)
    {
        log_error("Failed to add object %s to dependency outputs.", object->data);
        string_free(object_clone);
        return CREN_NOK;
    }

    return CREN_OK;
}

string_t *get_object_path(const string_t *objects_dir, const string_t *object_name)
{
    if (objects_dir == NULL || object_name == NULL)
    {
        log_error("Invalid arguments for object_path.");
        return NULL;
    }

    string_t *object_path = string_clone(objects_dir);
    if (object_path == NULL)
    {
        log_error("Failed to clone objects directory.");
        return NULL;
    }

    string_append_path(object_path, object_name->data);
    return object_path;
}

string_t *get_lib_path(const build_environment_t *env, const build_cfg_t *build, const target_t *target)
{
    string_t *target_path = string_clone(build->target_dir);
    if (target_path == NULL)
    {
        log_error("Failed to clone target directory.");
        return NULL;
    }
    string_t *target_name = NULL;
    build_compiler_t *ar = env->ar;

    if (target->target_name == NULL)
    {
        log_error("Target name is NULL.");
        string_free(target_path);
        return NULL;
    }

    target_name = string_clone(target->target_name);
    if (target_name == NULL)
    {
        log_error("Failed to clone target name.");
        string_free(target_path);
        return NULL;
    }

    // append '.a' or '.lib' extension based on compiler family
    if (ar->family == COMPILER_FAMILY_GCC)
    {
        string_append(target_name, ".a");
    }
    else
    {
        string_append(target_name, ".lib");
    }

    string_append_path(target_path, target_name->data);
    string_free(target_name);

    return target_path;
}
