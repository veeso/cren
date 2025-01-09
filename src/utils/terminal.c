#include <stdarg.h>
#include <stdio.h>

#include <utils/terminal.h>

#define PROGRESS_BAR_SIZE 20

void print_line_and_progress(size_t current, size_t total, const char *action, const char *line_fmt, ...)
{
    va_list args;
    va_start(args, line_fmt);
    vprintf(line_fmt, args);
    va_end(args);

    const size_t progress = (current * PROGRESS_BAR_SIZE) / total;

    printf("\r%s [", action);
    for (size_t i = 0; i < PROGRESS_BAR_SIZE; i++)
    {
        if (i < progress - 1)
        {
            printf("=");
        }
        else if (i == progress - 1)
        {
            printf(">");
        }
        else
        {
            printf(" ");
        }
    }

    printf("] %zu/%zu", current, total);
    fflush(stdout);
}
