#include <string.h>

#include <lib/log.h>
#include <manifest/dependencies/platform.h>

platform_t *platform_init();
bool parse_alias(const char *str, platform_t *platform);
bool parse_triplet(const char *str, platform_t *platform);
bool match_component(string_t *local, string_t *target);

#define ALIAS_LINUX "linux"
#define ALIAS_MACOS "macos"
#define ALIAS_WINDOWS "windows"
#define ALIAS_FREEBSD "freebsd"
#define ALIAS_ANDROID "android"
#define ALIAS_IOS "ios"

#define UNKNOWN "unknown"

platform_t *platform_init()
{
    // init platform
    platform_t *platform = malloc(sizeof(platform_t));
    if (platform == NULL)
    {
        log_error("Error allocating platform");
        return NULL;
    }

    // set null
    platform->arch = NULL;
    platform->vendor = NULL;
    platform->os = NULL;
    platform->abi = NULL;
    platform->alias = NULL;

    return platform;
}

platform_t *platform_parse(const char *str)
{
    platform_t *platform = platform_init();
    if (platform == NULL)
    {
        log_fatal("Error initializing platform");
        return NULL;
    }

    // count separators
    int separators = 0;
    size_t str_len = strlen(str);
    for (size_t i = 0; i < str_len; i++)
    {
        if (str[i] == '-')
        {
            separators++;
        }
    }

    // if no separators; parse alias
    if (separators == 0 && !parse_alias(str, platform))
    {
        log_error("Error parsing alias");
        platform_free(platform);
        return NULL;
    }
    else if ((separators == 2 || separators == 3) && !parse_triplet(str, platform))
    {
        log_error("Error parsing triplet");
        platform_free(platform);
        return NULL;
    }

    // if is all NULL; return syntax error
    if (
        platform->arch == NULL &&
        platform->vendor == NULL &&
        platform->os == NULL &&
        platform->abi == NULL)
    {
        log_error("Syntax error '%s'", str);
        platform_free(platform);
        return NULL;
    }

    return platform;
}

platform_t *platform_clone(const platform_t *platform)
{
    platform_t *clone = platform_init();
    if (clone == NULL)
    {
        log_error("Error initializing platform");
        return NULL;
    }

    if (platform->arch != NULL)
    {
        clone->arch = string_clone(platform->arch);
        if (clone->arch == NULL)
        {
            log_error("Error cloning arch");
            platform_free(clone);
            return NULL;
        }
    }
    if (platform->vendor != NULL)
    {
        clone->vendor = string_clone(platform->vendor);
        if (clone->vendor == NULL)
        {
            log_error("Error cloning vendor");
            platform_free(clone);
            return NULL;
        }
    }
    if (platform->os != NULL)
    {
        clone->os = string_clone(platform->os);
        if (clone->os == NULL)
        {
            log_error("Error cloning os");
            platform_free(clone);
            return NULL;
        }
    }
    if (platform->abi != NULL)
    {
        clone->abi = string_clone(platform->abi);
        if (clone->abi == NULL)
        {
            log_error("Error cloning abi");
            platform_free(clone);
            return NULL;
        }
    }

    return clone;
}

void platform_free(platform_t *platform)
{
    if (platform == NULL)
    {
        log_debug("tried to free a NULL platform");
        return;
    }

    string_free(platform->arch);
    string_free(platform->vendor);
    string_free(platform->os);
    string_free(platform->abi);
    string_free(platform->alias);

    free(platform);
}

bool platform_matches(const platform_t *local, const platform_t *target)
{
    return match_component(local->arch, target->arch) && match_component(local->vendor, target->vendor) && match_component(local->os, target->os) && match_component(local->abi, target->abi);
}

bool match_component(string_t *local, string_t *remote)
{
    // local: x86_64-unknown-linux-gnu; remote: x86_64-unknown-linux-* => true
    if (local == NULL || remote == NULL)
    {
        return true;
    }

    return strcmp(local->data, remote->data) == 0;
}

bool parse_alias(const char *str, platform_t *platform)
{

    platform->alias = string_from_cstr(str);
    if (platform->alias == NULL)
    {
        log_error("Error parsing alias");
        return false;
    }

    // linux
    if (strcmp(str, ALIAS_LINUX) == 0)
    {
        platform->os = string_from_cstr(ALIAS_LINUX);
        return true;
    }

    // macos
    if (strcmp(str, ALIAS_MACOS) == 0)
    {
        platform->vendor = string_from_cstr("apple");
        platform->os = string_from_cstr("darwin");
        return true;
    }

    // windows
    if (strcmp(str, ALIAS_WINDOWS) == 0)
    {
        platform->os = string_from_cstr("windows");
        platform->abi = string_from_cstr("msvc");
        return true;
    }

    // freebsd
    if (strcmp(str, ALIAS_FREEBSD) == 0)
    {
        platform->os = string_from_cstr("freebsd");
        return true;
    }

    // android
    if (strcmp(str, ALIAS_ANDROID) == 0)
    {
        platform->os = string_from_cstr("android");
        return true;
    }

    // ios
    if (strcmp(str, ALIAS_IOS) == 0)
    {
        platform->vendor = string_from_cstr("apple");
        platform->os = string_from_cstr("darwin");
        return true;
    }

    log_error("Unknown alias '%s'", str);
    string_free(platform->alias);
    platform->alias = NULL;

    return false;
}

bool parse_triplet(const char *str, platform_t *platform)
{
    // separate str by '-'
    string_t *components[4] = {NULL, NULL, NULL, NULL};
    size_t components_len = 0;

    // parse components
    size_t str_len = strlen(str);
    size_t start = 0;
    for (size_t i = 0; i < str_len; i++)
    {
        if (str[i] == '-')
        {
            // check if component is `unknown`
            if (i - start == 7 && strncmp(str + start, UNKNOWN, 7) == 0)
            {
                start = i + 1;
                continue;
            }

            components[components_len] = string_from_substr(str, start, i - start);
            if (components[components_len] == NULL)
            {
                log_error("Error parsing component");
                return false;
            }
            components_len++;
            start = i + 1;
        }
    }

    // parse last component
    if (str_len - start > 0)
    {
        // check if component is `*`
        if (str_len - start == 7 && strncmp(str + start, UNKNOWN, 7) == 0)
        {
            components[components_len] = NULL;
        }
        else
        {

            components[components_len] = string_from_substr(str, start, str_len - start);
        }
    }

    // set components
    if (components_len == 2)
    {
        platform->arch = components[0];
        platform->vendor = components[1];
        platform->os = components[2];
    }
    else if (components_len == 3)
    {
        platform->arch = components[0];
        platform->vendor = components[1];
        platform->os = components[2];
        platform->abi = components[3];
    }

    return true;
}

string_t *platform_to_string(const platform_t *platform)
{
    if (platform->alias != NULL)
    {
        return string_clone(platform->alias);
    }

    string_t *str = string_init();
    if (str == NULL)
    {
        log_error("Error initializing string");
        return NULL;
    }

    if (platform->arch != NULL)
    {
        string_append(str, platform->arch->data);
    }
    else
    {
        string_append(str, UNKNOWN);
    }
    string_append(str, "-");

    if (platform->vendor != NULL)
    {
        string_append(str, platform->vendor->data);
    }
    else
    {
        string_append(str, UNKNOWN);
    }
    string_append(str, "-");

    if (platform->os != NULL)
    {
        string_append(str, platform->os->data);
    }
    else
    {
        string_append(str, UNKNOWN);
    }

    if (platform->abi != NULL)
    {
        string_append(str, "-");
        string_append(str, platform->abi->data);
    }

    return str;
}

platform_t *platform_local(void)
{
    platform_t *local = platform_init();
    if (local == NULL)
    {
        log_error("Error initializing platform");
        return NULL;
    }

    // arch
#if defined(__x86_64__) || defined(_M_X64)
    local->arch = string_from_cstr("x86_64");
#elif defined(__i386__) || defined(_M_IX86)
    local->arch = string_from_cstr("i386");
#elif defined(__aarch64__) || defined(_M_ARM64)
    local->arch = string_from_cstr("aarch64");
#elif defined(__arm__) || defined(_M_ARM)
    local->arch = string_from_cstr("arm");
#elif defined(__powerpc64__) || defined(__ppc64__)
    local->arch = string_from_cstr("powerpc64");
#elif defined(__powerpc__) || defined(__ppc__)
    local->arch = string_from_cstr("powerpc");
#elif defined(__riscv)
    local->arch = string_from_cstr("riscv");
#elif defined(__s390x__)
    local->arch = string_from_cstr("s390x");
#elif defined(__s390__)
    local->arch = string_from_cstr("s390");
#elif defined(__sparc64__) || defined(__sparc)
    local->arch = string_from_cstr("sparc64");
#else
    local->arch = NULL;
#endif

    // get os and vendor
#if defined(__linux__)
    local->os = string_from_cstr("linux");
#elif defined(__APPLE__) && defined(__MACH__)
    local->vendor = string_from_cstr("apple");
    local->os = string_from_cstr("darwin");
#elif defined(_WIN32) || defined(_WIN64)
    local->os = string_from_cstr("windows");
#elif defined(__FreeBSD__)
    local->os = string_from_cstr("freebsd");
#elif defined(__OpenBSD__)
    local->os = string_from_cstr("openbsd");
#elif defined(__NetBSD__)
    local->os = string_from_cstr("netbsd");
#elif defined(__ANDROID__)
    local->os = string_from_cstr("android");
#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
    local->vendor = string_from_cstr("apple");
    local->os = string_from_cstr("darwin");
#else
    local->os = NULL;
#endif

    // abi
#if defined(__GNUC__) || defined(__GLIBC__)
    local->abi = string_from_cstr("gnu");
#elif defined(__MACH__)
    local->abi = string_from_cstr("macho");
#elif defined(_MSC_VER)
    local->abi = string_from_cstr("msvc");
#elif defined(__MUSL__)
    local->abi = string_from_cstr("musl");
#else
    local->abi = NULL;
#endif

    return local;
}
