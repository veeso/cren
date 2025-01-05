// Manifest parse error

#ifndef CREN_MANIFEST_ERROR_H
#define CREN_MANIFEST_ERROR_H

typedef enum manifest_parse_error manifest_parse_error;

enum manifest_parse_error
{
    MANIFEST_PARSER_OK = 0,
    MANIFEST_PARSER_FILE_NOT_FOUND,
};

/// @brief get message error for manifest parse error
/// @param err
/// @return msg
const char *manifest_parse_error_msg(manifest_parse_error err);

#endif // CREN_MANIFEST_ERROR_H
