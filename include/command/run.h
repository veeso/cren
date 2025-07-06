// Run command

#ifndef CREN_COMMAND_RUN_H
#define CREN_COMMAND_RUN_H

#include <args/run.h>

/// @brief Execute the `run` command.
/// @param args The arguments for the run command.
/// @return CREN_OK on success, CREN_NOK on failure.
int command_run(const args_run_t *args);

#endif // CREN_COMMAND_RUN_H
