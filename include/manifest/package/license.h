// Manifest license

#ifndef CREN_MANIFEST_LICENSE_H
#define CREN_MANIFEST_LICENSE_H

/// @brief License type for manifest SPDX 3.20
typedef enum license license;

enum license
{
    /// @brief Unknown license
    UNKNOWN = -1,
    /// @brief Unset
    NONE = 0,
};

/// @brief get license from str
/// @param str
/// @return license
license license_from_str(const char *str);

/// @brief convert license to string
/// @param license
/// @return str
const char *license_to_string(license license);

#endif // CREN_MANIFEST_LICENSE_H
