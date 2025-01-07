#include <cren.h>
#include <lib/log.h>
#include <utils/string_list.h>

string_list_t *string_list_init()
{
    string_list_t *list = (string_list_t *)malloc(sizeof(string_list_t));
    if (list == NULL)
    {
        log_fatal("Failed to allocate memory for string list");
        return NULL;
    }
    list->items = NULL;
    list->nitems = 0;
    return list;
}

void string_list_free(string_list_t *list)
{
    if (list == NULL)
    {
        log_trace("Attempted to free a NULL string list");
        return;
    }
    for (size_t i = 0; i < list->nitems; i++)
    {
        string_free(list->items[i]);
    }
    free(list->items);
    free(list);

    log_trace("Freed string list");
}

int string_list_push(string_list_t *list, string_t *str)
{
    string_t **new_items = (string_t **)realloc(list->items, (list->nitems + 1) * sizeof(string_t *));
    if (new_items == NULL)
    {
        log_fatal("Failed to allocate memory for string list items");
        return CREN_NOK;
    }
    list->items = new_items;
    list->items[list->nitems] = str;
    list->nitems++;

    log_trace("Pushed string to list: %s, new items count %zu", str->data, list->nitems);

    return CREN_OK;
}
