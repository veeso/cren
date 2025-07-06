#include <args/run.h>

void args_run_free(args_run_t *args)
{
    if (args == NULL)
    {
        return;
    }

    if (args->bin != NULL)
        string_free(args->bin);
    if (args->example != NULL)
        string_free(args->example);
    if (args->target_dir != NULL)
        string_free(args->target_dir);
    if (args->manifest_path != NULL)
        string_free(args->manifest_path);
    if (args->features != NULL)
        string_list_free(args->features);
    if (args->args != NULL)
        string_list_free(args->args);
}
