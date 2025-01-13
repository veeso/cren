// List of strings

#ifndef CREN_UTILS_CHAR_LIST_H
#define CREN_UTILS_CHAR_LIST_H

#include <stdbool.h>
#include <stdlib.h>

#include <utils/string.h>

typedef struct string_list_t string_list_t;

struct string_list_t
{
    string_t **items; // items
    size_t nitems;    // amount of items
};

/// @brief init a char list
string_list_t *string_list_init(void);

/// @brief clean a char list
/// @param list
void string_list_free(string_list_t *list);

/// @brief push item to list
/// @param list
/// @param str
/// @return 0 on success, 1 on failure
int string_list_push(string_list_t *list, string_t *str);

/// @brief create a char list from a c string with a delimiter
/// @param data
/// @param delim
/// @return the char list
string_list_t *string_list_from_cstr(const char *data, const char *delim);

/// @brief check if a char list contains a string
/// @param list
/// @param str
/// @return true if the string is in the list, false otherwise
bool string_list_contains(string_list_t *list, const char *str);

#endif // CREN_UTILS_CHAR_LIST_H
