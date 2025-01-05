// Manifest command arguments for cren

#ifndef CREN_ARGS_MANIFEST_H
#define CREN_ARGS_MANIFEST_H

/// Command for manifest command
typedef enum args_manifest_cmd args_manifest_cmd;

enum args_manifest_cmd
{
    MANIFEST_CMD_CHECK,
    MANIFEST_CMD_UNKNOWN,
};

/// @brief parse manifest cmd to get the command
/// @param argc
/// @param argv
/// @return args_manifest_cmd
args_manifest_cmd args_manifest_cmd_parse(int argc, char **argv);

#endif // CREN_ARGS_MANIFEST_H