// Manifest command arguments for cren

#ifndef CREN_ARGS_MANIFEST_H
#define CREN_ARGS_MANIFEST_H

#include <utils/string.h>

/// Command for manifest command
typedef enum args_manifest_cmd_t
{
    MANIFEST_CMD_CHECK,
    MANIFEST_CMD_UNKNOWN,
} args_manifest_cmd_t;

typedef struct args_manifest_t args_manifest_t;

struct args_manifest_t
{
    args_manifest_cmd_t cmd;
    string_t *path;
};

void args_manifest_free(args_manifest_t *args);

#endif // CREN_ARGS_MANIFEST_H
