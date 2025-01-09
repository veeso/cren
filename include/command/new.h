// Types for `cren new` command

#ifndef CREN_ARGS_NEW_H
#define CREN_ARGS_NEW_H

#include <args/new.h>

/// @brief Execute new command
/// @param args
/// @return int `CREN_OK` on success, `CREN_ERR` on failure
int command_new(const args_new_t *args);

#endif // CREN_ARGS_NEW_H
