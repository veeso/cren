#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <args.h>
#include <cren.h>
#include <manifest/package/language.h>
#include <lib/optparse.h>

#define COLOR_RESET "\033[0m"
#define COLOR_BOLD "\033[1m"
#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_WHITE "\033[37m"

#define COLOR_HEADER COLOR_BOLD COLOR_GREEN
#define COLOR_OPT COLOR_BOLD COLOR_CYAN
#define COLOR_ARG COLOR_RESET COLOR_CYAN
#define COLOR_TEXT COLOR_RESET

args_verbose_t get_verbosity(const char *optarg);
static int args_parse_manifest(args_t *args, char **argv);
static int args_parse_new(args_t *args, char **argv);
void usage_manifest();
void usage_new();
void usage_default();

args_t *args_parse_cmd(int argc, char **argv)
{
    struct optparse options;

    static const struct
    {
        char name[16];
        args_cmd_t cmd;
        int (*parse)(args_t *args, char **);
    } cmds[] = {
        {"manifest", ARGS_CMD_MANIFEST, args_parse_manifest},
        {"new", ARGS_CMD_NEW, args_parse_new},
    };
    const int ncmds = sizeof(cmds) / sizeof(*cmds);

    const struct optparse_long longopts[] =
        {
            {"help", 'h', OPTPARSE_NONE},
            {"quiet", 'q', OPTPARSE_NONE},
            {"verbose", 'v', OPTPARSE_OPTIONAL},
            {"version", 'V', OPTPARSE_NONE},
            {0}};

    // init args
    args_t *args = (args_t *)malloc(sizeof(args_t));
    if (args == NULL)
    {
        return NULL;
    }
    args->cmd = ARGS_CMD_UNKNOWN;
    args->help = false;
    args->quiet = false;
    args->version = false;
    args->verbose = VERBOSE_DEFAULT;
    args->manifest_cmd.cmd = MANIFEST_CMD_UNKNOWN;
    args->manifest_cmd.path = NULL;
    args->new_cmd.package = NULL;
    args->new_cmd.package_type = INIT_PACKAGE_TYPE_LIB;
    args->new_cmd.language = C11;

    // parse command
    optparse_init(&options, argv);
    options.permute = 0;

    int option;
    while ((option = optparse_long(&options, longopts, NULL)) != -1)
    {
        switch (option)
        {
        case 'h':
            args->help = true;
            break;
        case 'q':
            args->quiet = true;
            break;
        case 'v':
            args->verbose = get_verbosity(options.optarg);
            break;
        case 'V':
            args->version = true;
            break;
        default:
            printf("Unknown option: %c\n", option);
            goto opt_parse_end;
        }
    }
opt_parse_end:

    // if help or version is requested, return
    if (args->help || args->version)
    {
        return args;
    }

    // otherwise, parse command
    char **subargv = argv + options.optind;
    if (!subargv[0])
    {
        return args;
    }

    for (int i = 0; i < ncmds; i++)
    {
        size_t cmdlen = strlen(subargv[0]);
        size_t max_len = cmdlen > 16 ? 16 : cmdlen;
        if (strncmp(subargv[0], cmds[i].name, max_len) == 0)
        {
            if (cmds[i].parse(args, subargv) != CREN_OK)
            {
                args->cmd = ARGS_CMD_UNKNOWN;
                return args;
            }
            args->cmd = cmds[i].cmd;

            return args;
        }
    }

    // unknown command
    args->cmd = ARGS_CMD_UNKNOWN;
    return args;
}

void args_free(args_t *args)
{
    string_free(args->manifest_cmd.path);
    string_free(args->new_cmd.package);
    free(args);
}

static int args_parse_manifest(args_t *args, char **argv)
{
    // get command
    static const struct
    {
        char name[16];
        args_manifest_cmd_t cmd;
    } cmds[] = {
        {"check", MANIFEST_CMD_CHECK},
    };

    // parse options
    const struct optparse_long longopts[] =
        {
            {"help", 'h', OPTPARSE_NONE},
            {"path", 'p', OPTPARSE_REQUIRED},
            {0}};

    struct optparse options;
    optparse_init(&options, argv);
    options.permute = 0;

    int option;
    while ((option = optparse_long(&options, longopts, NULL)) != -1)
    {
        switch (option)
        {
        case 'h':
            args->help = true;
            return CREN_OK;
        case 'p':
            args->manifest_cmd.path = string_init(options.optarg);
            break;
        default:
            printf("Unknown option: %c\n", option);
            return CREN_NOK;
        }
    }

    // parse command
    char **subargv = argv + options.optind;
    if (!subargv[0])
    {
        return CREN_NOK;
    }

    // parse command
    size_t cmd_len = strlen(subargv[0]);
    size_t max_len = cmd_len > 16 ? 16 : cmd_len;
    if (strncmp(subargv[0], cmds[0].name, max_len) == 0)
    {
        args->manifest_cmd.cmd = cmds[0].cmd;
    }
    else
    {
        return CREN_NOK;
    }

    return CREN_OK;
}

static int args_parse_new(args_t *args, char **argv)
{
    const struct optparse_long longopts[] =
        {
            {"lib", 'l', OPTPARSE_NONE},
            {"bin", 'b', OPTPARSE_NONE},
            {"language", 'L', OPTPARSE_REQUIRED},
            {"help", 'h', OPTPARSE_NONE},
            {0}};

    struct optparse options;
    optparse_init(&options, argv);
    options.permute = 0;

    int option;
    while ((option = optparse_long(&options, longopts, NULL)) != -1)
    {
        switch (option)
        {
        case 'l':
            args->new_cmd.package_type = INIT_PACKAGE_TYPE_LIB;
            break;
        case 'b':
            args->new_cmd.package_type = INIT_PACKAGE_TYPE_BIN;
            break;
        case 'L':
            args->new_cmd.language = language_from_str(options.optarg);
            break;
        case 'h':
            args->help = true;
            return CREN_OK;
        default:
            printf("Unknown option: %c\n", option);
            return CREN_NOK;
        }
    }

    if (args->new_cmd.language == LANGUAGE_UNKNOWN)
    {
        printf("Unknown language\n");
        return CREN_NOK;
    }

    char **subargv = argv + options.optind;

    // get package name
    if (!subargv[0])
    {
        printf("Package name is required\n");
        return CREN_NOK;
    }

    args->new_cmd.package = string_init(subargv[0]);

    return CREN_OK;
}

args_verbose_t get_verbosity(const char *optarg)
{
    if (optarg == NULL)
    {
        return VERBOSE_INFO;
    }

    int v = 0;
    for (int i = 0; optarg[i] == 'v'; i++)
    {
        v++;
    }

    switch (v)
    {
    case 0:
        return VERBOSE_INFO;
    case 1:
        return VERBOSE_DEBUG;
    case 2:
        return VERBOSE_TRACE;
    default:
        return VERBOSE_TRACE;
    }
}

void usage(const args_t *args)
{
    switch (args->cmd)
    {
    case ARGS_CMD_MANIFEST:
        usage_manifest();
        break;
    case ARGS_CMD_NEW:
        usage_new();
        break;
    default:
        usage_default();
        break;
    }
}

void usage_default()
{
    puts("C package manager");
    puts("");

    printf("%sUsage: %scren %s[OPTIONS] [COMMAND]\n", COLOR_HEADER, COLOR_OPT, COLOR_ARG);
    puts("");

    printf("%sOptions:%s\n", COLOR_HEADER, COLOR_RESET);
    printf("  %s-V, --version\t\t\t\t%sPrint version\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-v, --verbose\t\t\t\t%sVerbose output (-v, -vv, -vvv for more verbose output)\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-h, --help\t\t\t\t%sPrint help\n", COLOR_OPT, COLOR_TEXT);
    puts("");

    printf("%sCommands:%s\n", COLOR_HEADER, COLOR_RESET);
    printf("  %snew\t\t\t\t\t%sCreate a new Cren package\n", COLOR_OPT, COLOR_TEXT);
    printf("  %smanifest\t\t\t\t%sManage package manifest\n", COLOR_OPT, COLOR_TEXT);
    puts("");
}

void usage_manifest()
{
    puts("Manage package manifest");
    puts("");

    printf("%sUsage: %scren manifest %s[OPTIONS] [COMMAND]\n", COLOR_HEADER, COLOR_OPT, COLOR_ARG);
    puts("");

    printf("%sOptions:%s\n", COLOR_HEADER, COLOR_RESET);
    printf("  %s-p, --path %s<PATH>\t\t\t%sPath to manifest file\n", COLOR_OPT, COLOR_ARG, COLOR_TEXT);
    printf("  %s-h, --help\t\t\t\t%sPrint help\n", COLOR_OPT, COLOR_TEXT);
    puts("");

    printf("%sCommands:%s\n", COLOR_HEADER, COLOR_RESET);
    printf("  %scheck\t\t\t\t\t%sCheck package manifest\n", COLOR_OPT, COLOR_TEXT);
    puts("");
}

void usage_new()
{
    puts("Create a new Cren package");
    puts("");

    printf("%sUsage: %scren new %s[OPTIONS] %sPACKAGE_NAME\n", COLOR_HEADER, COLOR_OPT, COLOR_ARG, COLOR_ARG);
    puts("");

    printf("%sOptions:%s\n", COLOR_HEADER, COLOR_RESET);
    printf("  %s-l, --lib\t\t\t\t%sCreate a new library package\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-b, --bin\t\t\t\t%sCreate a new binary package\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-L, --language %s<LANGUAGE>\t%sSet package language\n", COLOR_OPT, COLOR_ARG, COLOR_TEXT);
    printf("  %s-h, --help\t\t\t\t%sPrint help\n", COLOR_OPT, COLOR_TEXT);
    puts("");

    printf("%sLanguages:%s\n", COLOR_HEADER, COLOR_RESET);
    printf("  %sc89\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc90\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc99\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc11\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc17\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc23\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc++99\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc++98\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc++03\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc++11\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc++17\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc++20\n%s", COLOR_OPT, COLOR_RESET);
    printf("  %sc++26\n%s", COLOR_OPT, COLOR_RESET);
    puts("");
}
