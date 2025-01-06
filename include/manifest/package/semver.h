// Semver types

#include <stdint.h>
#include <stdlib.h>

#ifndef CREN_MANIFEST_SEMVER_H
#define CREN_MANIFEST_SEMVER_H

// Semver definition
typedef struct semver_t
{
    uint64_t major;
    uint64_t minor;
    uint64_t patch;
} semver_t;

struct semver_t
{
    uint64_t major;
    uint64_t minor;
    uint64_t patch;
};

/// @brief parse a string into `semver`. Returns 0 in case of success
/// @param str the string to parse
/// @param strlen the length of the string
/// @param semver the semver to fill
/// @return CREN_OK in case of success
int semver_from_str(const char *str, semver_t *semver);

/// @brief Convert a semver to a string
/// @param semver
/// @return the string representation of the semver
char *semver_to_str(const semver_t *semver);

#endif // CREN_MANIFEST_SEMVER_H
