// List of strings

#ifndef CREN_UTILS_CHAR_LIST_H
#define CREN_UTILS_CHAR_LIST_H

#include <stdlib.h>

#include <utils/string.h>

typedef struct string_list_t string_list_t;

struct string_list_t
{
    string_t **items; // items
    size_t nitems;    // amount of items
};

/// @brief init a char list
string_list_t *string_list_init();

/// @brief clean a char list
/// @param list
void string_list_free(string_list_t *list);

/// @brief push item to list
/// @param list
/// @param str
/// @return 0 on success, 1 on failure
int string_list_push(string_list_t *list, string_t *str);

#endif // CREN_UTILS_CHAR_LIST_H
