#include <utils/string.h>

#include <string.h>

string_t *string_init()
{
    string_t *string = (string_t *)malloc(sizeof(string_t));
    if (string == NULL)
    {
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
        string_free(string);
        return NULL;
    }

    size_t len = strlen(data);

    strncpy(string->data, data, len);
    string->data[len] = '\0';
    string->length = len;

    return string;
}

void string_free(string_t *string)
{
    if (string == NULL)
    {
        return;
    }
    free(string->data);
    free(string);
}

void string_append(string_t *string, const char *data)
{
    if (string == NULL || data == NULL)
    {
        return;
    }
    size_t length = strlen(data);
    string->data = (char *)realloc(string->data, string->length + length + 1);
    if (string->data == NULL)
    {
        return;
    }
    memcpy(string->data + string->length, data, length);
    string->length += length;
    string->data[string->length] = '\0';
}

void string_append_char(string_t *string, char data)
{
    if (string == NULL)
    {
        return;
    }
    string->data = (char *)realloc(string->data, string->length + 2);
    if (string->data == NULL)
    {
        return;
    }
    string->data[string->length] = data;
    string->length++;
    string->data[string->length] = '\0';
}
