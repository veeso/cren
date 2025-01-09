#ifndef UTILS_TERMINAL_H
#define UTILS_TERMINAL_H

#include <stdlib.h>

#define COLOR_RESET "\033[0m"
#define COLOR_BOLD "\033[1m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"

#define COLOR_HEADER COLOR_BOLD COLOR_GREEN
#define COLOR_OPT COLOR_BOLD COLOR_CYAN
#define COLOR_ARG COLOR_RESET COLOR_CYAN
#define COLOR_TEXT COLOR_RESET

#define CLEAR_LINE "\033[2K"

void print_line_and_progress(size_t current, size_t total, const char *action, const char *line_fmt, ...);

#endif // UTILS_TERMINAL_H
