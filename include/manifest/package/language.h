// Manifest language

#ifndef CREN_MANIFEST_LANGUAGE_H
#define CREN_MANIFEST_LANGUAGE_H

typedef enum language_t language_t;

/// @brief Language variants
enum language_t
{
    UNKNOWN = -1,
    C89,
    C90,
    C99,
    C11,
    C17,
    C23,
    CPP99,
    CPP98,
    CPP03,
    CPP11,
    CPP17,
    CPP20,
    CPP26,
};

/// @brief convert string into language. In case of error returns `UNKNOWN`
/// @param str
/// @return language
language_t language_from_str(const char *str);

/// @brief convert language into string.
/// @param language
/// @return string
const char *language_to_string(language_t language);

#endif // CREN_MANIFEST_LANGUAGE_H