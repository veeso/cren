#include <stdlib.h>

#include <cren.h>
#include <lib/log.h>
#include <utils/fs.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

char *get_current_dir()
{
    char *buffer = (char *)malloc(sizeof(char) * 4096);
    if (buffer == NULL)
    {
        log_error("Failed to allocate memory for current directory");
        return NULL;
    }

#if defined(_WIN32) || defined(_WIN64)
    if (GetCurrentDirectoryA(4096, buffer) == 0)
    {
        log_error("Failed to get current directory");
        free(buffer);
        return NULL;
    }
#else
    if (getcwd(buffer, 4096) == NULL)
    {
        log_error("Failed to get current directory");
        free(buffer);
        return NULL;
    }

#endif

    return buffer;
}

int make_dir(const char *path)
{
#if defined(_WIN32) || defined(_WIN64)
    if (CreateDirectoryA(path, NULL) == 0)
    {
        log_error("Failed to create directory %s", path);
        return CREN_NOK;
    }
#else
    if (mkdir(path, 0755) == -1)
    {
        log_error("Failed to create directory %s", path);
        return CREN_NOK;
    }
#endif

    log_debug("Created directory %s", path);

    return CREN_OK;
}
