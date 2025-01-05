// Manifest edition types

#ifndef CREN_MANIFEST_EDITION_H
#define CREN_MANIFEST_EDITION_H

/// @brief edition enum type
typedef enum edition edition;

enum edition
{
    /// @brief unknown edition (error)
    UNKNOWN = -1,
    /// @brief First edition
    ONE = 1,
};

/// @brief parse `edition` from a string.
/// @param str
/// @return edition
edition edition_from_str(const char *str);

/// @brief convert edition to string
/// @param edition edition to convert
/// @return string repr
const char *edition_to_string(edition edition);

#endif // CREN_MANIFEST_EDITION_H
