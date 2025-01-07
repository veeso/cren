#include <string.h>

#include <lib/log.h>
#include <manifest/package/language.h>

const char *LANGUAGE_C89 = "c89";
const char *LANGUAGE_C90 = "c90";
const char *LANGUAGE_CANSI = "ansic";
const char *LANGUAGE_C99 = "c99";
const char *LANGUAGE_C11 = "c11";
const char *LANGUAGE_C17 = "c17";
const char *LANGUAGE_C23 = "c23";
const char *LANGUAGE_CPP_ANSI = "ansicpp";
const char *LANGUAGE_CPP99 = "cpp99";
const char *LANGUAGE_CPP98 = "cpp98";
const char *LANGUAGE_CPP03 = "cpp03";
const char *LANGUAGE_CPP11 = "cpp11";
const char *LANGUAGE_CPP17 = "cpp17";
const char *LANGUAGE_CPP20 = "cpp20";
const char *LANGUAGE_CPP26 = "cpp26";

language_t language_from_str(const char *str)
{
    size_t len = strlen(str);

    if (strncmp(str, LANGUAGE_C89, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_C89);
        return C89;
    }

    if (strncmp(str, LANGUAGE_C90, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_C90);
        return C90;
    }

    if (strncmp(str, LANGUAGE_CANSI, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_CANSI);
        return C90;
    }

    if (strncmp(str, LANGUAGE_C99, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_C99);
        return C99;
    }

    if (strncmp(str, LANGUAGE_C11, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_C11);
        return C11;
    }

    if (strncmp(str, LANGUAGE_C17, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_C17);
        return C17;
    }

    if (strncmp(str, LANGUAGE_C23, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_C23);
        return C23;
    }

    if (strncmp(str, LANGUAGE_CPP_ANSI, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_CPP_ANSI);
        return CPP98;
    }

    if (strncmp(str, LANGUAGE_CPP99, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_CPP99);
        return CPP99;
    }

    if (strncmp(str, LANGUAGE_CPP98, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_CPP98);
        return CPP98;
    }

    if (strncmp(str, LANGUAGE_CPP03, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_CPP03);
        return CPP03;
    }

    if (strncmp(str, LANGUAGE_CPP11, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_CPP11);
        return CPP11;
    }

    if (strncmp(str, LANGUAGE_CPP17, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_CPP17);
        return CPP17;
    }

    if (strncmp(str, LANGUAGE_CPP20, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_CPP20);
        return CPP20;
    }

    if (strncmp(str, LANGUAGE_CPP26, len) == 0)
    {
        log_trace("Language: %s", LANGUAGE_CPP26);
        return CPP26;
    }

    log_error("Unknown language: %s", str);

    return LANGUAGE_UNKNOWN;
}

const char *language_to_string(language_t language)
{
    switch (language)
    {
    case C89:
        return LANGUAGE_C89;
    case C90:
        return LANGUAGE_C90;
    case C99:
        return LANGUAGE_C99;
    case C11:
        return LANGUAGE_C11;
    case C17:
        return LANGUAGE_C17;
    case C23:
        return LANGUAGE_C23;
    case CPP98:
        return LANGUAGE_CPP98;
    case CPP99:
        return LANGUAGE_CPP99;
    case CPP03:
        return LANGUAGE_CPP03;
    case CPP11:
        return LANGUAGE_CPP11;
    case CPP17:
        return LANGUAGE_CPP17;
    case CPP20:
        return LANGUAGE_CPP20;
    case CPP26:
        return LANGUAGE_CPP26;
    default:
        return NULL;
    }
}
