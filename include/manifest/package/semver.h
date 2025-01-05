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

// parse a string into `semver`. Returns 0 in case of success
int semver_from_str(const char *str, size_t strlen, semver_t *semver);

#endif // CREN_MANIFEST_SEMVER_H
