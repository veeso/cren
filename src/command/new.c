#include <string.h>

#include <command/new.h>
#include <cren.h>
#include <lib/log.h>
#include <manifest.h>
#include <manifest/targets.h>
#include <utils/fs.h>
#include <utils/string.h>

string_t *get_wrkdir(void);
string_t *new_project_dir(string_t *wrkdir, string_t *package);
int init_manifest(const args_new_t *args, string_t *project_dir, string_t *main_src);
string_t *init_main_src(const args_new_t *args, string_t *project_dir);
int init_main_src_c(const args_new_t *args, FILE *file);
int init_main_src_cpp(const args_new_t *args, FILE *file);
int init_main_include(const args_new_t *args, string_t *project_dir);
int init_main_include_c(const args_new_t *args, FILE *file);
int init_main_include_cpp(const args_new_t *args, FILE *file);

int command_new(const args_new_t *args)
{
    int rc = CREN_OK;
    string_t *wrkdir = NULL;
    string_t *project_dir = NULL;
    string_t *main_src = NULL;

    // check package name
    if (cren_manifest_validate_name(args->package->data) != CREN_OK)
    {
        log_error("Invalid package name: %s", args->package->data);
        rc = CREN_NOK;
        goto cleanup;
    }

    if (args->package == NULL || args->package->length == 0)
    {
        log_error("Package name is required");
        rc = CREN_NOK;
        goto cleanup;
    }

    wrkdir = get_wrkdir();
    if (wrkdir == NULL)
    {
        log_error("Failed to get current directory");
        rc = CREN_NOK;
        goto cleanup;
    }

    // project_dir = wrkdir + args->package
    project_dir = new_project_dir(wrkdir, args->package);
    if (project_dir == NULL)
    {
        log_error("Failed to clone wrkdir");
        rc = CREN_NOK;
        goto cleanup;
    }
    // mkdir project_dir
    if (make_dir(project_dir->data) != CREN_OK)
    {
        log_error("Failed to create project directory: %.*s", project_dir->length, project_dir->data);
        rc = CREN_NOK;
        goto cleanup;
    }

    // init main src
    main_src = init_main_src(args, project_dir);
    if (main_src == NULL)
    {
        log_error("Failed to init main src");
        rc = CREN_NOK;
        goto cleanup;
    }

    // init include
    if (init_main_include(args, project_dir) != CREN_OK)
    {
        log_error("Failed to init include");
        rc = CREN_NOK;
        goto cleanup;
    }

    // init manifest
    if (init_manifest(args, project_dir, main_src) != CREN_OK)
    {
        log_error("Failed to init manifest");
        rc = CREN_NOK;
        goto cleanup;
    }

cleanup:
    string_free(wrkdir);
    string_free(project_dir);
    string_free(main_src);

    return rc;
}

string_t *get_wrkdir(void)
{
    // get current wrkdir
    char *wrkdir_cstr = get_current_dir();
    if (wrkdir_cstr == NULL)
    {
        log_error("Failed to get current directory");
        return NULL;
    }

    string_t *wrkdir = string_from_cstr(wrkdir_cstr);
    free(wrkdir_cstr);

    return wrkdir;
}

string_t *new_project_dir(string_t *wrkdir, string_t *package)
{
    string_t *project_dir = string_clone(wrkdir);
    if (project_dir == NULL)
    {
        log_error("Failed to clone wrkdir");
        return NULL;
    }

    string_append_path(project_dir, package->data);

    return project_dir;
}

int init_manifest(const args_new_t *args, string_t *project_dir, string_t *main_src)
{
    int rc = CREN_OK;
    cren_manifest_t *manifest = NULL;
    string_t *manifest_path = NULL;
    cren_manifest_target_cfg_t *cfg = NULL;
    FILE *manifest_file = NULL;

    // make manifest path
    manifest_path = string_clone(project_dir);
    if (manifest_path == NULL)
    {
        log_error("Failed to clone project_dir");
        rc = CREN_NOK;
        goto cleanup;
    }
    string_append_path(manifest_path, CREN_MANIFEST_NAME);

    // init manifest
    manifest = cren_manifest_init();
    if (manifest == NULL)
    {
        log_error("Failed to init manifest");
        rc = CREN_NOK;
        goto cleanup;
    }

    // set arguments
    if (args->language != LANGUAGE_UNKNOWN)
    {
        manifest->package->language = args->language;
    }
    manifest->package->name = string_clone(args->package);
    if (manifest->package->name == NULL)
    {
        log_error("Failed to clone package name");
        rc = CREN_NOK;
        goto cleanup;
    }

    // init cfg
    cfg = cren_manifest_target_cfg_init();
    if (cfg == NULL)
    {
        log_error("Failed to init target cfg");
        rc = CREN_NOK;
        goto cleanup;
    }
    cfg->name = string_clone(args->package);
    if (cfg->name == NULL)
    {
        log_error("Failed to clone target cfg name");
        rc = CREN_NOK;
        goto cleanup;
    }
    cfg->path = string_from_cstr("src/");
    if (cfg->path == NULL)
    {
        log_error("Failed to clone target cfg path");
        rc = CREN_NOK;
        goto cleanup;
    }
    // append package name and .c/.cpp
    string_append(cfg->path, args->package->data);
    if (args->language == LANGUAGE_UNKNOWN || language_is_c(args->language))
    {
        string_append(cfg->path, ".c");
    }
    else
    {
        string_append(cfg->path, ".cpp");
    }
    // set target
    if (args->package_type == INIT_PACKAGE_TYPE_BIN)
    {
        if (cren_manifest_targets_add_cfg(&manifest->targets->bin, &manifest->targets->bin_len, cfg) != CREN_OK)
        {
            log_error("Failed to add bin target cfg");
            rc = CREN_NOK;
            goto cleanup;
        }
    }
    else
    {
        manifest->targets->lib = cfg;
    }
    // nullify cfg to prevent double free
    cfg = NULL;

    // open file
    manifest_file = fopen(manifest_path->data, "w");
    if (manifest_file == NULL)
    {
        log_error("Failed to open manifest file: %.*s", manifest_path->length, manifest_path->data);
        rc = CREN_NOK;
        goto cleanup;
    }
    // write manifest
    if (cren_manifest_write(manifest, manifest_file) != CREN_OK)
    {
        log_error("Failed to write manifest");
        rc = CREN_NOK;
        goto cleanup;
    }

cleanup:
    string_free(manifest_path);
    cren_manifest_free(manifest);
    cren_manifest_target_cfg_free(cfg);
    if (manifest_file != NULL)
    {
        fclose(manifest_file);
    }
    return rc;
}

string_t *init_main_src(const args_new_t *args, string_t *project_dir)
{
    // make src path
    int rc = CREN_OK;
    FILE *main_src_file = NULL;
    string_t *main_src = string_clone(project_dir);
    if (main_src == NULL)
    {
        log_error("Failed to clone project_dir");
        rc = CREN_NOK;
        goto cleanup;
    }
    string_append_path(main_src, CREN_MANIFEST_SRC);
    // mkdir src
    if (make_dir(main_src->data) != CREN_OK)
    {
        log_error("Failed to create src directory: %.*s", main_src->length, main_src->data);
        rc = CREN_NOK;
        goto cleanup;
    }

    bool is_c = args->language == LANGUAGE_UNKNOWN || language_is_c(args->language);

    // append / and package name and .c/.cpp
    string_append_path(main_src, args->package->data);
    if (is_c)
    {
        string_append(main_src, ".c");
    }
    else
    {
        string_append(main_src, ".cpp");
    }
    log_debug("Main src: %.*s", main_src->length, main_src->data);

    // open file
    main_src_file = fopen(main_src->data, "w");
    if (main_src_file == NULL)
    {
        log_error("Failed to open main src file: %.*s", main_src->length, main_src->data);
        rc = CREN_NOK;
        goto cleanup;
    }
    if (is_c)
    {
        rc = init_main_src_c(args, main_src_file);
    }
    else
    {
        rc = init_main_src_cpp(args, main_src_file);
    }

cleanup:
    if (rc != CREN_OK)
    {
        string_free(main_src);
        main_src = NULL;
    }
    if (main_src_file != NULL)
    {
        fclose(main_src_file);
    }

    return main_src;
}

int init_main_src_c(const args_new_t *args, FILE *file)
{
    log_debug("Init main src c");

    // write hello world if bin
    if (args->package_type == INIT_PACKAGE_TYPE_BIN)
    {
        fprintf(file, "#include <stdio.h>\n\n");
        fprintf(file, "int main(int argc, char **argv)\n");
        fprintf(file, "{\n");
        fprintf(file, "    printf(\"Hello, World!\\n\");\n");
        fprintf(file, "    return 0;\n");
        fprintf(file, "}\n");
    }
    else
    {
        // write sum function, include header
        fprintf(file, "#include <%s.h>\n\n", args->package->data);
        fprintf(file, "int sum(int a, int b)\n");
        fprintf(file, "{\n");
        fprintf(file, "    return a + b;\n");
        fprintf(file, "}\n");
    }

    return CREN_OK;
}

int init_main_src_cpp(const args_new_t *args, FILE *file)
{
    log_debug("Init main src cpp");

    // write hello world if bin
    if (args->package_type == INIT_PACKAGE_TYPE_BIN)
    {
        fprintf(file, "#include <iostream>\n\n");
        fprintf(file, "int main(int argc, char *argv[])\n");
        fprintf(file, "{\n");
        fprintf(file, "    std::cout << \"Hello, World!\" << std::endl;\n");
        fprintf(file, "    return 0;\n");
        fprintf(file, "}\n");
    }
    else
    {
        // write sum function, include header
        fprintf(file, "#include <%s.hpp>\n\n", args->package->data);
        fprintf(file, "int sum(int a, int b)\n");
        fprintf(file, "{\n");
        fprintf(file, "    return a + b;\n");
        fprintf(file, "}\n");
    }

    return CREN_OK;
}

int init_main_include(const args_new_t *args, string_t *project_dir)
{
    // make include path
    int rc = CREN_OK;
    FILE *main_include_file = NULL;
    string_t *main_include = string_clone(project_dir);
    bool is_c = args->language == LANGUAGE_UNKNOWN || language_is_c(args->language);

    if (main_include == NULL)
    {
        log_error("Failed to clone project_dir");
        rc = CREN_NOK;
        goto cleanup;
    }

    // append / and include and / and package name and .h/.hpp
    string_append_path(main_include, CREN_MANIFEST_INCLUDE);
    // mkdir include
    if (make_dir(main_include->data) != CREN_OK)
    {
        log_error("Failed to create include directory: %.*s", main_include->length, main_include->data);
        rc = CREN_NOK;
        goto cleanup;
    }
    string_append_path(main_include, args->package->data);
    if (is_c)
    {
        string_append(main_include, ".h");
    }
    else
    {
        string_append(main_include, ".hpp");
    }

    // open file
    main_include_file = fopen(main_include->data, "w");
    if (main_include_file == NULL)
    {
        log_error("Failed to open main include file: %.*s", main_include->length, main_include->data);
        rc = CREN_NOK;
        goto cleanup;
    }

    if (is_c)
    {
        rc = init_main_include_c(args, main_include_file);
    }
    else
    {
        rc = init_main_include_cpp(args, main_include_file);
    }

cleanup:
    string_free(main_include);
    if (main_include_file != NULL)
    {
        fclose(main_include_file);
    }

    return rc;
}

int init_main_include_c(const args_new_t *args, FILE *file)
{
    log_debug("Init main include c");

    // write include guard
    fprintf(file, "#ifndef %s_H\n", args->package->data);
    fprintf(file, "#define %s_H\n\n", args->package->data);

    if (args->package_type == INIT_PACKAGE_TYPE_LIB)
    {
        // write sum function
        fprintf(file, "int sum(int a, int b);\n\n");
    }

    // write include guard end
    fprintf(file, "#endif // %s_H\n", args->package->data);

    return CREN_OK;
}

int init_main_include_cpp(const args_new_t *args, FILE *file)
{
    log_debug("Init main include cpp");

    // write include guard
    fprintf(file, "#ifndef %s_HPP\n", args->package->data);
    fprintf(file, "#define %s_HPP\n\n", args->package->data);

    if (args->package_type == INIT_PACKAGE_TYPE_LIB)
    {
        // write sum function
        fprintf(file, "int sum(int a, int b);\n\n");
    }

    // write include guard end
    fprintf(file, "#endif // %s_HPP\n", args->package->data);

    return CREN_OK;
}
