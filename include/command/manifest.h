// cren manifest commands

#ifndef CREN_COMMAND_MANIFEST_H
#define CREN_COMMAND_MANIFEST_H

#include <args/manifest.h>

/// @brief Execute manifest command
/// @param cmd
/// @return int `CREN_OK` on success, `CREN_ERR` on failure
int command_manifest(args_manifest_cmd_t cmd);

#endif // CREN_COMMAND_MANIFEST_H
