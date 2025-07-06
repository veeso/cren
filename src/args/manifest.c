#include <args/manifest.h>

void args_manifest_free(args_manifest_t *args)
{
    if (args == NULL)
    {
        return;
    }

    if (args->path != NULL)
    {
        string_free(args->path);
    }
}
