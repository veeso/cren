#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <utils/terminal.h>

#define PROGRESS_BAR_SIZE 30

void print_line_and_progress(size_t current, size_t total, const char *action, const char *line_fmt, ...)
{
    if (current > 0)
        printf("\r");
    printf("  ");
    va_list args;
    va_start(args, line_fmt);
    vprintf(line_fmt, args);
    va_end(args);

    // current : total = progress : PROGRESS_BAR_SIZE
    const size_t progress = (current * PROGRESS_BAR_SIZE) / total;

    printf("\n\r    %s%s [", CLEAR_LINE, action);
    for (size_t i = 0; i < PROGRESS_BAR_SIZE; i++)
    {
        if (progress > 0 && i < progress - 1)
        {
            printf("=");
        }
        else if ((progress == 0 && i == 0) || i == progress - 1)
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

void print_outcome(const char *action, const char *line_fmt, ...)
{
    printf("\r%s", CLEAR_LINE);
    printf("    %s%s%s ", COLOR_HEADER, action, COLOR_TEXT);
    va_list args;
    va_start(args, line_fmt);
    vprintf(line_fmt, args);
    va_end(args);
    puts("");
}
