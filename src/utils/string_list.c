#include <utils/string_list.h>

string_list_t *string_list_init()
{
    string_list_t *list = (string_list_t *)malloc(sizeof(string_list_t));
    if (list == NULL)
    {
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
        return;
    }
    for (size_t i = 0; i < list->nitems; i++)
    {
        string_free(list->items[i]);
    }
    free(list->items);
    free(list);
}

int string_list_push(string_list_t *list, string_t *str)
{
    string_t **new_items = (string_t **)realloc(list->items, (list->nitems + 1) * sizeof(string_t *));
    if (new_items == NULL)
    {
        return 1;
    }
    list->items = new_items;
    list->items[list->nitems] = str;
    list->nitems++;
    return 0;
}