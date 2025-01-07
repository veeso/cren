// Cli args types

#ifndef CREN_ARGS_H
#define CREN_ARGS_H

#include <stdbool.h>

#include <args/manifest.h>
#include <args/new.h>

typedef enum args_verbose_t
{
    VERBOSE_DEFAULT,
    VERBOSE_TRACE,
    VERBOSE_DEBUG,
    VERBOSE_INFO,
    VERBOSE_WARN,
    VERBOSE_ERROR,
    VERBOSE_FATAL,
} args_verbose_t;

/// Cli args command
typedef enum args_cmd_t
{
    ARGS_CMD_BUILD,
    ARGS_CMD_MANIFEST,
    ARGS_CMD_NEW,
    ARGS_CMD_RUN,
    ARGS_CMD_TEST,
    ARGS_CMD_UNKNOWN,
    ARGS_CMD_VERSION,
} args_cmd_t;

typedef struct args_t args_t;

struct args_t
{
    // cmd for subcommands
    args_cmd_t cmd;
    // subcommands
    args_manifest_t manifest_cmd;
    args_new_t new_cmd;

    // options
    args_verbose_t verbose;
    bool help;
    bool quiet;
    bool version;
};

/// @brief get command from arguments
/// @param argc argument count
/// @param argv argument values
/// @return args command - `ARGS_CMD_UNKNOWN` if it failed
args_t *args_parse_cmd(int argc, char **argv);

void args_free(args_t *args);

/// @brief print usage
void usage(const args_t *args);

#endif // CREN_ARGS_H
