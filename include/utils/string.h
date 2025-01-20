// String utility type

#ifndef UTILS_STRING_H
#define UTILS_STRING_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct string_t string_t;

struct string_t
{
    char *data;
    size_t length;
};

/// @brief init a new string
/// @return string
string_t *string_init(void);

/// @brief Clone a string
/// @param string
/// @return string
string_t *string_clone(const string_t *string);

/// @brief create a string from a c string
/// @param data
/// @return string
string_t *string_from_cstr(const char *data);

/// @brief create a string from a substring
/// @param str
/// @param start
/// @param len
/// @return string
string_t *string_from_substr(const char *str, size_t start, size_t len);

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

/// @brief append a path to a string
/// @param string
/// @param data
void string_append_path(string_t *string, const char *data);

bool str_ends_with(const char *str, const char *suffix);

#endif // UTILS_STRING_H
