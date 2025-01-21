#include <build/environment.h>
#include <lib/log.h>
#include <utils/env.h>

#define ENV_CC "CC"
#define ENV_CXX "CXX"
#define ENV_LD "LD"

#define CC_GCC "gcc"       // GNU Compiler
#define CC_CLANG "clang"   // Clang Compiler
#define CC_MSVC "cl"       // Microsoft Visual C Compiler
#define CC_INTEL "icc"     // Intel C Compiler
#define CC_INTELLLVM "icx" // Intel LLVM-based Compiler
#define CC_PGI "pgcc"      // NVIDIA PGI Compiler
#define CC_ARM "armcc"     // ARM Compiler
#define CC_CRAY "cc"       // Cray Compiler
#define CC_TINYC "tcc"     // Tiny C Compiler
#define CC_BORLAND "bcc32" // Borland C++ Compiler
#define CC_SUN "cc"        // Oracle Solaris Studio Compiler
#define CC_HP "cc"         // HP-UX Compiler
#define CC_IBM "xlc"       // IBM XL C Compiler
#define CC_MINGW "gcc"     // MinGW (GNU Compiler on Windows)
#define CC_CYGGCC "gcc"    // Cygwin (GNU Compiler on Windows)
#define CC_LCC "lcc"       // Lightweight C Compiler
#define CC_WATCOM "wcc"    // Open Watcom C Compiler
#define CC_ZIG "zig"       // Zig C Compiler
#define CC_EMCC "emcc"     // Emscripten (WebAssembly Compiler)

#define CXX_GCC "g++"        // GNU C++ Compiler
#define CXX_CLANG "clang++"  // Clang C++ Compiler
#define CXX_MSVC "cl"        // Microsoft Visual C++ Compiler
#define CXX_INTEL "icpc"     // Intel C++ Compiler
#define CXX_INTELLLVM "icx"  // Intel LLVM-based C++ Compiler
#define CXX_PGI "pgc++"      // NVIDIA PGI C++ Compiler
#define CXX_ARM "armclang++" // ARM Compiler for C++
#define CXX_CRAY "CC"        // Cray C++ Compiler
#define CXX_TINYC "tcc"      // Tiny C Compiler (Supports minimal C++)
#define CXX_BORLAND "bcc32c" // Borland C++ Compiler
#define CXX_SUN "CC"         // Oracle Solaris Studio C++ Compiler
#define CXX_HP "aCC"         // HP-UX C++ Compiler
#define CXX_IBM "xlC"        // IBM XL C++ Compiler
#define CXX_MINGW "g++"      // MinGW (GNU C++ Compiler on Windows)
#define CXX_CYGGCC "g++"     // Cygwin (GNU C++ Compiler on Windows)
#define CXX_WATCOM "wpp"     // Open Watcom C++ Compiler
#define CXX_ZIG "zig"        // Zig C++ Compiler
#define CXX_EMCC "em++"      // Emscripten (WebAssembly Compiler for C++)

#define LD_GNU "ld"       // GNU Linker
#define LD_GOLD "gold"    // GNU Gold Linker (alternativa ad alte prestazioni)
#define LD_LLD "ld.lld"   // LLVM Linker
#define LD_BFD "ld.bfd"   // GNU BFD Linker
#define LD_MOLD "mold"    // Mold: Linker ad alte prestazioni
#define LD_MSVC "link"    // Microsoft Linker (parte di Visual Studio)
#define LD_INTEL "xild"   // Intel Linker (integrato nel compilatore Intel)
#define LD_ARM "armlink"  // ARM Linker (per compilatori ARM)
#define LD_CRAY "cc"      // Cray Linker
#define LD_SOLARIS "ld"   // Solaris Linker
#define LD_AIX "ld"       // IBM AIX Linker
#define LD_MINGW "ld"     // MinGW Linker (basato su GNU)
#define LD_CYGWIN "ld"    // Cygwin Linker (basato su GNU)
#define LD_WATCOM "wlink" // Open Watcom Linker
#define LD_ZIG "zig"      // Zig integrato con LLVM Linker
#define LD_EMCC "emcc"    // Emscripten Linker (WebAssembly)

build_compiler_t *compiler_init(const char *path, build_compiler_family_t family);
build_compiler_t *get_cc(void);
build_compiler_t *get_cxx(void);
string_t *get_ld(void);

build_compiler_t *compiler_init(const char *path, build_compiler_family_t family)
{
    build_compiler_t *compiler = (build_compiler_t *)malloc(sizeof(build_compiler_t));
    if (compiler == NULL)
    {
        log_fatal("Failed to allocate memory for compiler");
        return NULL;
    }

    compiler->path = string_from_cstr(path);
    compiler->family = family;

    return compiler;
}

void compiler_free(build_compiler_t *compiler)
{
    if (compiler != NULL)
    {
        string_free(compiler->path);
        free(compiler);
    }
}

void build_environment_free(build_environment_t *env)
{
    if (env != NULL)
    {
        compiler_free(env->cc);
        compiler_free(env->cxx);
        string_free(env->ld);
        free(env);
    }
}

build_environment_t *build_environment_new(void)
{
    build_environment_t *env = (build_environment_t *)malloc(sizeof(build_environment_t));
    if (env == NULL)
    {
        log_fatal("Failed to allocate memory for build environment");
        return NULL;
    }

    env->cc = NULL;
    env->cxx = NULL;
    env->ld = NULL;

    return env;
}

build_environment_t *build_environment_init(void)
{
    build_environment_t *env = build_environment_new();
    if (env == NULL)
    {
        return NULL;
    }

    // get the compiler CC
    env->cc = get_cc();
    // get the compiler CXX
    env->cxx = get_cxx();
    // get the linker LD
    env->ld = env_get(ENV_LD);
    if (env->ld == NULL)
    {
        // get from path
        env->ld = get_ld();
    }

    // check if the compiler is set
    if (env->cc == NULL && env->cxx == NULL)
    {
        log_error("No compiler found");
        build_environment_free(env);
        return NULL;
    }
    // check if the linker is set
    if (env->ld == NULL)
    {
        log_error("No linker found");
        build_environment_free(env);
        return NULL;
    }

    if (env->cc != NULL)
    {
        log_info("CC: %s", env->cc->path->data);
    }
    if (env->cxx != NULL)
    {
        log_info("CXX: %s", env->cxx->path->data);
    }
    if (env->ld != NULL)
    {
        log_info("LD: %s", env->ld->data);
    }

    return env;
}

build_compiler_t *get_cc(void)
{
    string_t *env_cc = env_get(ENV_CC);
    if (env_cc != NULL)
    {
        build_compiler_t *compiler = compiler_init(env_cc->data, COMPILER_FAMILY_GCC);
        string_free(env_cc);
        return compiler;
    }

    // all compilers array
    static const struct
    {
        const char *name;
        build_compiler_family_t family;
    } compilers[] = {
        {CC_GCC, COMPILER_FAMILY_GCC},
        {CC_CLANG, COMPILER_FAMILY_GCC},
        {CC_MSVC, COMPILER_FAMILY_MSVC},
        {CC_INTEL, COMPILER_FAMILY_GCC},
        {CC_INTELLLVM, COMPILER_FAMILY_GCC},
        {CC_PGI, COMPILER_FAMILY_GCC},
        {CC_ARM, COMPILER_FAMILY_GCC},
        {CC_CRAY, COMPILER_FAMILY_GCC},
        {CC_TINYC, COMPILER_FAMILY_GCC},
        {CC_BORLAND, COMPILER_FAMILY_GCC},
        {CC_SUN, COMPILER_FAMILY_GCC},
        {CC_HP, COMPILER_FAMILY_GCC},
        {CC_IBM, COMPILER_FAMILY_GCC},
        {CC_MINGW, COMPILER_FAMILY_GCC},
        {CC_CYGGCC, COMPILER_FAMILY_GCC},
        {CC_LCC, COMPILER_FAMILY_GCC},
        {CC_WATCOM, COMPILER_FAMILY_GCC},
        {CC_ZIG, COMPILER_FAMILY_GCC},
        {CC_EMCC, COMPILER_FAMILY_GCC}

    };

    // get the compiler from the environment
    for (size_t i = 0; i < 19; i++)
    {
        string_t *cc = env_get_from_path(compilers[i].name);
        if (cc == NULL)
        {
            continue;
        }
        log_info("Found %s in PATH", cc->data);

        build_compiler_t *compiler = compiler_init(cc->data, compilers[i].family);
        string_free(cc);
        return compiler;
    }

    return NULL;
}

build_compiler_t *get_cxx(void)
{
    string_t *env_cxx = env_get(ENV_CXX);
    if (env_cxx != NULL)
    {
        build_compiler_t *compiler = compiler_init(env_cxx->data, COMPILER_FAMILY_GCC);
        string_free(env_cxx);
        return compiler;
    }

    // all compilers array
    static const struct
    {
        const char *name;
        build_compiler_family_t family;
    } compilers[] = {
        {CXX_GCC, COMPILER_FAMILY_GCC},
        {CXX_CLANG, COMPILER_FAMILY_GCC},
        {CXX_MSVC, COMPILER_FAMILY_MSVC},
        {CXX_INTEL, COMPILER_FAMILY_GCC},
        {CXX_INTELLLVM, COMPILER_FAMILY_GCC},
        {CXX_PGI, COMPILER_FAMILY_GCC},
        {CXX_ARM, COMPILER_FAMILY_GCC},
        {CXX_CRAY, COMPILER_FAMILY_GCC},
        {CXX_TINYC, COMPILER_FAMILY_GCC},
        {CXX_BORLAND, COMPILER_FAMILY_GCC},
        {CXX_SUN, COMPILER_FAMILY_GCC},
        {CXX_HP, COMPILER_FAMILY_GCC},
        {CXX_IBM, COMPILER_FAMILY_GCC},
        {CXX_MINGW, COMPILER_FAMILY_GCC},
        {CXX_CYGGCC, COMPILER_FAMILY_GCC},
        {CXX_WATCOM, COMPILER_FAMILY_GCC},
        {CXX_ZIG, COMPILER_FAMILY_GCC},
        {CXX_EMCC, COMPILER_FAMILY_GCC},
    };

    // get the compiler from the environment
    for (size_t i = 0; i < 18; i++)
    {
        string_t *cxx = env_get_from_path(compilers[i].name);
        if (cxx == NULL)
        {
            continue;
        }

        log_info("Found %s in PATH", cxx->data);
        build_compiler_t *compiler = compiler_init(cxx->data, compilers[i].family);
        string_free(cxx);

        return compiler;
    }

    return NULL;
}

string_t *get_ld(void)
{
    // all linkers array
    const char *linkers[] = {
        LD_GNU,
        LD_GOLD,
        LD_LLD,
        LD_BFD,
        LD_MOLD,
        LD_MSVC,
        LD_INTEL,
        LD_ARM,
        LD_CRAY,
        LD_SOLARIS,
        LD_AIX,
        LD_MINGW,
        LD_CYGWIN,
        LD_WATCOM,
        LD_ZIG,
        LD_EMCC};

    // get the linker from the environment
    for (size_t i = 0; i < 16; i++)
    {
        string_t *ld = env_get_from_path(linkers[i]);
        if (ld != NULL)
        {
            return ld;
        }
    }

    return NULL;
}
