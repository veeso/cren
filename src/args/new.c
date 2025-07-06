#include <args/new.h>

void args_new_free(args_new_t *args)
{
    if (args == NULL)
    {
        return;
    }

    if (args->package != NULL)
    {
        string_free(args->package);
    }
}
