// Clean args

#ifndef ARGS_CLEAN_H
#define ARGS_CLEAN_H

#include <stdbool.h>

#include <utils/string.h>

typedef struct args_clean_t args_clean_t;

struct args_clean_t
{
    bool release;
    string_t *target_dir;
    string_t *manifest_path;
};

#endif // ARGS_CLEAN_H
