// Manifest edition types

#ifndef CREN_MANIFEST_EDITION_H
#define CREN_MANIFEST_EDITION_H

/// @brief edition enum type
typedef enum edition_t edition_t;

enum edition_t
{
    /// @brief unknown edition (error)
    MANIFEST_EDITION_UNKNOWN = -1,
    /// @brief First edition
    MANIFEST_EDITION_ONE = 1,
};

/// @brief parse `edition` from a string.
/// @param str
/// @return edition
edition_t edition_from_str(const char *str);

/// @brief convert edition to string
/// @param edition edition to convert
/// @return string repr
const char *edition_to_string(edition_t edition);

#endif // CREN_MANIFEST_EDITION_H
