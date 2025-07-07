#include <string.h>

#include <lib/log.h>
#include <utils/string.h>

string_t *string_init(void)
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

string_t *string_clone(const string_t *string)
{
    if (string == NULL)
    {
        log_debug("Attempted to clone a NULL string");
        return NULL;
    }
    string_t *new_string = string_init();
    if (new_string == NULL)
    {
        log_fatal("Failed to allocate memory for string");
        return NULL;
    }
    new_string->data = (char *)malloc(string->length + 1);
    if (new_string->data == NULL)
    {
        log_fatal("Failed to allocate memory for string");
        string_free(new_string);
        return NULL;
    }
    memcpy(new_string->data, string->data, string->length);
    new_string->length = string->length;
    new_string->data[new_string->length] = '\0';

    log_trace("Cloned string: %s, len %zu", new_string->data, new_string->length);

    return new_string;
}

string_t *string_from_cstr(const char *data)
{
    string_t *string = string_init();
    if (string == NULL)
    {
        return NULL;
    }

    size_t data_len = strlen(data);

    string->data = (char *)malloc(data_len + 1);
    if (string->data == NULL)
    {
        log_fatal("Failed to allocate memory for string");
        string_free(string);
        return NULL;
    }

    strncpy(string->data, data, data_len + 1);
    string->data[data_len] = '\0';
    string->length = data_len;

    log_trace("Created string from c string: %s, len %zu", string->data, string->length);

    return string;
}

string_t *string_from_substr(const char *str, size_t start, size_t len)
{
    string_t *string = string_init();
    if (string == NULL)
    {
        return NULL;
    }

    string->data = (char *)malloc(len + 1);
    if (string->data == NULL)
    {
        log_fatal("Failed to allocate memory for string");
        string_free(string);
        return NULL;
    }

    strncpy(string->data, str + start, len);
    string->data[len] = '\0';
    string->length = len;

    log_trace("Created string from substring: %s, len %zu", string->data, string->length);

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

void string_append_path(string_t *string, const char *data)
{
    if (string == NULL || data == NULL)
    {
        log_error("Attempted to append to a NULL string");
        return;
    }

#ifdef _WIN32
#define SEPARATOR '\\'
#else
#define SEPARATOR '/'
#endif

    if (string->length > 0 && string->data[string->length - 1] != SEPARATOR)
    {
        string_append_char(string, SEPARATOR);
    }
    string_append(string, data);
}

void string_to_upper(string_t *string)
{
    if (string == NULL)
    {
        log_error("Attempted to convert a NULL string to uppercase");
        return;
    }

    for (size_t i = 0; i < string->length; i++)
    {
        if (string->data[i] >= 'a' && string->data[i] <= 'z')
        {
            string->data[i] -= ('a' - 'A');
        }
    }

    log_trace("Converted string to uppercase: %s", string->data);
}

bool str_ends_with(const char *str, const char *suffix)
{
    if (!str || !suffix)
    {
        return false; // Handle NULL pointers
    }

    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);

    if (suffix_len > str_len)
    {
        return false; // Suffix is longer than the string
    }

    // Compare the end of the string with the suffix
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}
