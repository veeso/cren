// Semver types

#include <stdint.h>

#ifndef CREN_MANIFEST_SEMVER_H
#define CREN_MANIFEST_SEMVER_H

// Semver definition
typedef struct semver
{
    uint64_t major;
    uint64_t minor;
    uint64_t patch;
} semver;

struct semver
{
    uint64_t major;
    uint64_t minor;
    uint64_t patch;
};

// parse a string into `semver`. Returns 0 in case of success
int semver_from_str(const char *str, semver *semver);

#endif // CREN_MANIFEST_SEMVER_H
