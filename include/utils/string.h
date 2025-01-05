// String utility type

#ifndef UTILS_STRING_H
#define UTILS_STRING_H

#include <stdlib.h>

typedef struct string_t string_t;

struct string_t
{
    char *data;
    size_t length;
};

/// @brief init a new string
/// @return string
string_t *string_init();

/// @brief free a string
/// @param string
void string_free(string_t *string);

/// @brief append data to a string
/// @param string
/// @param data
void string_append(string_t *string, const char *data);

/// @brief append a char to a string
/// @param string
/// @param data
void string_append_char(string_t *string, char data);

#endif // UTILS_STRING_H
