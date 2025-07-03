#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <args.h>
#include <cren.h>
#include <lib/optparse.h>
#include <manifest/package/language.h>
#include <utils/terminal.h>

#define OPT_ALL_FEATURES 1
#define OPT_NO_DEFAULT_FEATURES 2
#define OPT_MANIFEST_PATH 3

args_t *args_init(void);
args_verbose_t get_verbosity(const char *optarg);
static int args_parse_build(args_t *args, char **argv);
static int args_parse_clean(args_t *args, char **argv);
static int args_parse_manifest(args_t *args, char **argv);
static int args_parse_new(args_t *args, char **argv);
void usage_build(void);
void usage_clean(void);
void usage_manifest(void);
void usage_new(void);
void usage_default(void);

args_t *args_parse_cmd(int argc, char **argv)
{
    struct optparse options;

    static const struct
    {
        char name[16];
        args_cmd_t cmd;
        int (*parse)(args_t *args, char **);
    } cmds[] = {
        {"build", ARGS_CMD_BUILD, args_parse_build},
        {"clean", ARGS_CMD_CLEAN, args_parse_clean},
        {"init", ARGS_CMD_NEW, args_parse_new},
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
    args_t *args = args_init();
    if (args == NULL)
    {
        return NULL;
    }

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

args_t *args_init(void)
{
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

    // build
    args->build_cmd.all_targets = false;
    args->build_cmd.all_features = false;
    args->build_cmd.no_default_features = false;
    args->build_cmd.bins = false;
    args->build_cmd.examples = false;
    args->build_cmd.lib = false;
    args->build_cmd.release = false;
    args->build_cmd.bin = NULL;
    args->build_cmd.example = NULL;
    args->build_cmd.target_dir = NULL;
    args->build_cmd.features = NULL;
    args->build_cmd.manifest_path = NULL;

    // clean
    args->clean_cmd.release = false;
    args->clean_cmd.manifest_path = NULL;
    args->clean_cmd.target_dir = NULL;

    // manifest
    args->manifest_cmd.cmd = MANIFEST_CMD_UNKNOWN;
    args->manifest_cmd.path = NULL;

    // new
    args->new_cmd.package = NULL;
    args->new_cmd.package_type = INIT_PACKAGE_TYPE_LIB;
    args->new_cmd.language = C11;

    return args;
}

void args_free(args_t *args)
{
    // build
    string_free(args->build_cmd.bin);
    string_free(args->build_cmd.example);
    string_free(args->build_cmd.target_dir);
    string_list_free(args->build_cmd.features);

    // clean
    string_free(args->clean_cmd.manifest_path);
    string_free(args->clean_cmd.target_dir);

    // manifest
    string_free(args->manifest_cmd.path);

    // new
    string_free(args->new_cmd.package);

    free(args);
}

static int args_parse_build(args_t *args, char **argv)
{

    // parse options
    const struct optparse_long longopts[] =
        {
            {"help", 'h', OPTPARSE_NONE},
            {"release", 'r', OPTPARSE_NONE},
            {"all-targets", 'a', OPTPARSE_NONE},
            {"bin", 'b', OPTPARSE_REQUIRED},
            {"bins", 'B', OPTPARSE_NONE},
            {"example", 'e', OPTPARSE_REQUIRED},
            {"examples", 'E', OPTPARSE_NONE},
            {"lib", 'l', OPTPARSE_NONE},
            {"features", 'F', OPTPARSE_REQUIRED},
            {"all-features", OPT_ALL_FEATURES, OPTPARSE_NONE},
            {"no-default-features", OPT_NO_DEFAULT_FEATURES, OPTPARSE_NONE},
            {"target-dir", 't', OPTPARSE_REQUIRED},
            {"manifest-path", OPT_MANIFEST_PATH, OPTPARSE_REQUIRED},
            {0}};
    ;

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
        case 'r':
            args->build_cmd.release = true;
            break;
        case 'a':
            args->build_cmd.all_targets = true;
            break;
        case 'b':
            args->build_cmd.bin = string_from_cstr(options.optarg);
            break;
        case 'B':
            args->build_cmd.bins = true;
            break;
        case 'e':
            args->build_cmd.example = string_from_cstr(options.optarg);
            break;
        case 'E':
            args->build_cmd.examples = true;
            break;
        case 'l':
            args->build_cmd.lib = true;
            break;
        case 'F':
            args->build_cmd.features = string_list_from_cstr(options.optarg, ",");
            break;
        case OPT_ALL_FEATURES:
            args->build_cmd.all_features = true;
            break;
        case OPT_NO_DEFAULT_FEATURES:
            args->build_cmd.no_default_features = true;
            break;
        case OPT_MANIFEST_PATH:
            args->build_cmd.manifest_path = string_from_cstr(options.optarg);
            break;
        case 't':
            args->build_cmd.target_dir = string_from_cstr(options.optarg);
            break;
        default:
            printf("Unknown option: %c\n", option);
            return CREN_NOK;
        }
    }

    // if no targets are specified, build all
    if (!args->build_cmd.bins && !args->build_cmd.examples && !args->build_cmd.lib && !args->build_cmd.bin && !args->build_cmd.example)
    {
        args->build_cmd.all_targets = true;
    }

    return CREN_OK;
}

static int args_parse_clean(args_t *args, char **argv)
{

    // parse options
    const struct optparse_long longopts[] =
        {
            {"help", 'h', OPTPARSE_NONE},
            {"release", 'r', OPTPARSE_NONE},
            {"target-dir", 't', OPTPARSE_REQUIRED},
            {"manifest-path", OPT_MANIFEST_PATH, OPTPARSE_REQUIRED},
            {0}};
    ;

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
        case 'r':
            args->clean_cmd.release = true;
            break;
        case OPT_MANIFEST_PATH:
            args->clean_cmd.manifest_path = string_from_cstr(options.optarg);
            break;
        case 't':
            args->clean_cmd.target_dir = string_from_cstr(options.optarg);
            break;
        default:
            printf("Unknown option: %c\n", option);
            return CREN_NOK;
        }
    }

    return CREN_OK;
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
            args->manifest_cmd.path = string_from_cstr(options.optarg);
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

    args->new_cmd.package = string_from_cstr(subargv[0]);

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
    case ARGS_CMD_BUILD:
        usage_build();
        break;
    case ARGS_CMD_CLEAN:
        usage_clean();
        break;
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

void usage_default(void)
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
    printf("  %sbuild\t\t\t\t\t%sBuild package\n", COLOR_OPT, COLOR_TEXT);
    printf("  %sclean\t\t\t\t\t%sClean target directory\n", COLOR_OPT, COLOR_TEXT);
    printf("  %sinit\t\t\t\t\t%sCreate a new Cren package\n", COLOR_OPT, COLOR_TEXT);
    printf("  %smanifest\t\t\t\t%sManage package manifest\n", COLOR_OPT, COLOR_TEXT);
    printf("  %snew\t\t\t\t\t%sCreate a new Cren package\n", COLOR_OPT, COLOR_TEXT);
    puts("");
}

void usage_build(void)
{
    puts("Build package");
    puts("");

    printf("%sUsage: %scren build %s[OPTIONS]\n", COLOR_HEADER, COLOR_OPT, COLOR_ARG);
    puts("");

    printf("%sOptions:%s\n", COLOR_HEADER, COLOR_RESET);
    printf("  %s-r, --release\t\t\t\t%sRelease build\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-a, --all-targets\t\t\t%sBuild all targets\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-b, --bin %s<TARGET>\t\t\t%sBuild binary target\n", COLOR_OPT, COLOR_ARG, COLOR_TEXT);
    printf("  %s-B, --bins\t\t\t\t%sBuild binary targets\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-e, --example %s<TARGET>\t\t%sBuild example target\n", COLOR_OPT, COLOR_ARG, COLOR_TEXT);
    printf("  %s-E, --examples\t\t\t%sBuild example targets\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-l, --lib\t\t\t\t%sBuild library target\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-F, --features %s<FEATURES>\t\t%sComma separated list of features to activate\n", COLOR_OPT, COLOR_ARG, COLOR_TEXT);
    printf("  %s--all-features\t\t\t%sBuild with all features\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s--no-default-features\t\t\t%sDo not build with default features\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-t, --target-dir %s<TARGET_DIR>\t\t%sSet target directory\n", COLOR_OPT, COLOR_ARG, COLOR_TEXT);
    printf("  %s--manifest-path %s<PATH>\t\t%sPath to manifest file\n", COLOR_OPT, COLOR_ARG, COLOR_TEXT);
    printf("  %s-h, --help\t\t\t\t%sPrint help\n", COLOR_OPT, COLOR_TEXT);

    puts("");
}

void usage_clean(void)
{
    puts("Clean target directory");
    puts("");

    printf("%sUsage: %scren clean %s[OPTIONS]\n", COLOR_HEADER, COLOR_OPT, COLOR_ARG);
    puts("");

    printf("%sOptions:%s\n", COLOR_HEADER, COLOR_RESET);
    printf("  %s-r, --release\t\t\t\t%sKeep release artifacts\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-t, --target-dir %s<TARGET_DIR>\t\t%sSet target directory\n", COLOR_OPT, COLOR_ARG, COLOR_TEXT);
    printf("  %s--manifest-path %s<PATH>\t\t%sPath to manifest file\n", COLOR_OPT, COLOR_ARG, COLOR_TEXT);
    printf("  %s-h, --help\t\t\t\t%sPrint help\n", COLOR_OPT, COLOR_TEXT);

    puts("");
}

void usage_manifest(void)
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

void usage_new(void)
{
    puts("Create a new Cren package");
    puts("");

    printf("%sUsage: %scren new %s[OPTIONS] %sPACKAGE_NAME\n", COLOR_HEADER, COLOR_OPT, COLOR_ARG, COLOR_ARG);
    puts("");

    printf("%sOptions:%s\n", COLOR_HEADER, COLOR_RESET);
    printf("  %s-l, --lib\t\t\t\t%sCreate a new library package\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-b, --bin\t\t\t\t%sCreate a new binary package\n", COLOR_OPT, COLOR_TEXT);
    printf("  %s-L, --language %s<LANGUAGE>\t\t%sSet package language\n", COLOR_OPT, COLOR_ARG, COLOR_TEXT);
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
