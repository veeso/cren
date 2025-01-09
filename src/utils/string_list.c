#include <cren.h>
#include <lib/log.h>
#include <utils/string_list.h>

string_list_t *string_list_init(void)
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

string_list_t *string_list_from_cstr(const char *data, const char *delim)
{
    string_list_t *list = string_list_init();
    if (list == NULL)
    {
        return NULL;
    }

    size_t data_len = strlen(data);
    char *data_copy = (char *)malloc(data_len + 1);
    if (data_copy == NULL)
    {
        log_fatal("Failed to copy string");
        string_list_free(list);
        return NULL;
    }
    strncpy(data_copy, data, data_len);
    data_copy[data_len] = '\0';

    char *token = strtok(data_copy, delim);
    while (token != NULL)
    {
        string_t *str = string_from_cstr(token);
        if (str == NULL)
        {
            log_fatal("Failed to init string");
            string_list_free(list);
            free(data_copy);
            return NULL;
        }

        if (string_list_push(list, str) != CREN_OK)
        {
            log_fatal("Failed to push string to list");
            string_list_free(list);
            free(data_copy);
            return NULL;
        }

        token = strtok(NULL, delim);
    }

    free(data_copy);

    log_trace("Created string list from cstr");

    return list;
}
