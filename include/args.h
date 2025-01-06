// Cli args types

#ifndef CREN_ARGS_H
#define CREN_ARGS_H

/// Cli args command
typedef enum args_cmd args_cmd;

enum args_cmd
{
    ARGS_CMD_BUILD,
    ARGS_CMD_MANIFEST,
    ARGS_CMD_NEW,
    ARGS_CMD_RUN,
    ARGS_CMD_TEST,
    ARGS_CMD_UNKNOWN,
    ARGS_CMD_VERSION,
};

/// @brief get command from arguments
/// @param argc argument count
/// @param argv argument values
/// @return args command - `ARGS_CMD_UNKNOWN` if it failed
args_cmd args_cmd_parse_cmd(int argc, const char **argv);

#endif // CREN_ARGS_H
