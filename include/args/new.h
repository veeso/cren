// Init command args

#ifndef CREN_ARGS_INIT_H
#define CREN_ARGS_INIT_H

#include <stdbool.h>
#include <stdlib.h>

#include <manifest/package/language.h>

/// Args for new command
typedef struct args_new args_new;

struct args_new
{
    char *package;
    size_t package_len;
    args_new_package_type package_type;
    language language;
};

/// @brief New package type (bin or lib)
typedef enum args_new_package_type args_new_package_type;

enum args_new_package_type
{
    INIT_PACKAGE_TYPE_BIN,
    INIT_PACKAGE_TYPE_LIB,
};

/// @brief parse init command arguments
/// @param argc
/// @param argv
/// @return args for init
args_new *args_new_parse(int argc, char **argv);

/// @brief cleanup init command
/// @param args
void args_new_free(args_new *args);

#endif // CREN_ARGS_INIT_H
