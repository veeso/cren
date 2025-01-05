// Init command args

#ifndef CREN_ARGS_INIT_H
#define CREN_ARGS_INIT_H

#include <stdbool.h>
#include <stdlib.h>

#include <manifest/package/language.h>

/// Args for new command
typedef struct args_new_t args_new_t;

struct args_new_t
{
    char *package;
    size_t package_len;
    args_new_package_type_t package_type;
    language_t language;
};

/// @brief New package type (bin or lib)
typedef enum args_new_package_type_t args_new_package_type_t;

enum args_new_package_type_t
{
    INIT_PACKAGE_TYPE_BIN,
    INIT_PACKAGE_TYPE_LIB,
};

/// @brief parse init command arguments
/// @param argc
/// @param argv
/// @return args for init
args_new_t *args_new_parse(int argc, char **argv);

/// @brief cleanup init command
/// @param args
void args_new_free(args_new_t *args);

#endif // CREN_ARGS_INIT_H
