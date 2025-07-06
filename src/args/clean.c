#include <args/clean.h>

void args_clean_free(args_clean_t *args)
{
    if (args == NULL)
    {
        return;
    }

    if (args->manifest_path != NULL)
    {
        string_free(args->manifest_path);
    }

    if (args->target_dir != NULL)
    {
        string_free(args->target_dir);
    }
}
