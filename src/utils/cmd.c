#include <stdlib.h>

#include <lib/log.h>
#include <utils/cmd.h>

#ifndef _WIN32
#include <sys/wait.h>
#include <unistd.h>
#endif

int cmd_exec(const char *cmd)
{
    log_debug("Executing %s", cmd);
    const int rc = system(cmd);
    if (rc == -1)
    {
        log_error("failed to execute command");
        return -1;
    }

#ifdef _WIN32
    log_debug("Command returned %d", rc);
    return rc;
#else
    if (WIFEXITED(rc))
    {
        const int exit_status = WEXITSTATUS(rc);
        log_debug("Command returned %d", exit_status);
        return exit_status;
    }

    log_error("Command did not exit normally");
    return -1;
#endif
}
