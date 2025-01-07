#include <string.h>

#include <lib/log.h>
#include <utils/string.h>

string_t *string_init()
{
    string_t *string = (string_t *)malloc(sizeof(string_t));
    if (string == NULL)
    {
        log_fatal("Failed to allocate memory for string");
        return NULL;
    }
    string->data = NULL;
    string->length = 0;
    return string;
}

string_t *string_from_cstr(const char *data)
{
    string_t *string = string_init();
    if (string == NULL)
    {
        return NULL;
    }

    string->data = (char *)malloc(strlen(data) + 1);
    if (string->data == NULL)
    {
        log_fatal("Failed to allocate memory for string");
        string_free(string);
        return NULL;
    }

    size_t len = strlen(data);

    strncpy(string->data, data, len);
    string->data[len] = '\0';
    string->length = len;

    log_trace("Created string from c string: %s, len %zu", string->data, string->length);

    return string;
}

void string_free(string_t *string)
{
    if (string == NULL)
    {
        log_trace("Attempted to free a NULL string");
        return;
    }
    free(string->data);
    free(string);

    log_trace("Freed string");
}

void string_append(string_t *string, const char *data)
{
    if (string == NULL || data == NULL)
    {
        log_error("Attempted to append to a NULL string");
        return;
    }
    size_t length = strlen(data);
    string->data = (char *)realloc(string->data, string->length + length + 1);
    if (string->data == NULL)
    {
        log_fatal("Failed to allocate memory for string");
        return;
    }
    memcpy(string->data + string->length, data, length);
    string->length += length;
    string->data[string->length] = '\0';

    log_trace("Appended data to string: %s, len %zu", string->data, string->length);
}

void string_append_char(string_t *string, char data)
{
    if (string == NULL)
    {
        log_error("Attempted to append to a NULL string");
        return;
    }
    string->data = (char *)realloc(string->data, string->length + 2);
    if (string->data == NULL)
    {
        log_fatal("Failed to allocate memory for string");
        return;
    }
    string->data[string->length] = data;
    string->length++;
    string->data[string->length] = '\0';

    log_trace("Appended char to string: %s, len %zu", string->data, string->length);
}
