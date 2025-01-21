// Command executor

#ifndef UTILS_CMD_H
#define UTILS_CMD_H

#include <stddef.h>

/// @brief Execute a command.
/// @param cmd Command to execute.
/// @return Exit code of the command. -1 if failed to execute.
int cmd_exec(const char *cmd);

#endif // UTILS_CMD_H
