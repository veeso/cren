// List of strings

#ifndef CREN_UTILS_CHAR_LIST_H
#define CREN_UTILS_CHAR_LIST_H

#include <stdlib.h>

typedef struct char_list_t char_list_t;

struct char_list_t
{
    char_list_item_t **items; // items
    size_t nitems;            // amount of items
};

typedef struct char_list_item_t char_list_item_t;

struct char_list_item_t
{
    char *str;  // string
    size_t len; // string length
};

/// @brief init a char list
char_list_t *char_list_init();

/// @brief clean a char list
void char_list_free(char_list_t *list);

/// @brief push item to list
int char_list_push(char_list_t *list, char *str, size_t len);

#endif // CREN_UTILS_CHAR_LIST_H