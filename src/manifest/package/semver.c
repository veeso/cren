#include <manifest/package/semver.h>
#include <cren.h>

#include <stdio.h>

int semver_from_str(const char *str, semver_t *semver)
{
    // Parse the string
    if (sscanf(str, "%lu.%lu.%lu", &semver->major, &semver->minor, &semver->patch) != 3)
    {
        return CREN_NOK;
    }

    return CREN_OK;
}

char *semver_to_str(const semver_t *semver)
{
    // Allocate the string
    char *str = (char *)malloc(sizeof(char) * 16);
    if (str == NULL)
    {
        return NULL;
    }

    // Format the string
    sprintf(str, "%lu.%lu.%lu", semver->major, semver->minor, semver->patch);

    return str;
}
