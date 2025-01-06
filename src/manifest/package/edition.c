#include <string.h>

#include <manifest/package/edition.h>

const char *EDITION_ONE = "1";

edition_t edition_from_str(const char *str)
{
    size_t str_len = strlen(str);

    if (strncmp(str, EDITION_ONE, str_len) == 0)
    {
        return MANIFEST_EDITION_ONE;
    }

    return MANIFEST_EDITION_UNKNOWN;
}

const char *edition_to_string(edition_t edition)
{
    switch (edition)
    {
    case MANIFEST_EDITION_ONE:
        return EDITION_ONE;
    default:
        return NULL;
    }
}
