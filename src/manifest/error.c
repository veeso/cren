#include <manifest/error.h>

const char *manifest_parse_error_msg(manifest_parse_error_t err)
{
    switch (err)
    {
    case MANIFEST_PARSER_OK:
        return "No error";
    case MANIFEST_PARSER_FILE_NOT_FOUND:
        return "Manifest file not found";
    }
}
