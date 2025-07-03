// Source files list

#ifndef CREN_BUILD_TARGET_H
#define CREN_BUILD_TARGET_H

#include <build/source.h>
#include <utils/string.h>

typedef enum target_type_t
{
    TARGET_TYPE_EXECUTABLE,
    TARGET_TYPE_LIBRARY,
} target_type_t;

typedef struct target_t target_t;

struct target_t
{
    string_t *src;
    string_t *obj;
    string_t *target_name;
    target_type_t type; // Type of target: executable or library
};

/// @brief Initialize the target object.
/// @param src The source file.
/// @param target_dir The directory where the target will be created.
/// @param target_name The name of the target.
/// @param type The type of target (executable or library).
/// @return The target object.
target_t *target_init(const char *src, const char *target_dir, const char *target_name, target_type_t type);

/// @brief Free the target object.
/// @param source The target object to free.
void target_free(target_t *source);

/// @brief Convert a target to a source.
/// @param target The target object.
/// @return The source object.
source_t *source_from_target(const target_t *target);

#endif // CREN_BUILD_TARGET_H
