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

string_t *get_cc(void);
string_t *get_cxx(void);
string_t *get_ld(void);

void build_environment_free(build_environment_t *env)
{
    if (env != NULL)
    {
        string_free(env->cc);
        string_free(env->cxx);
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
    env->cc = env_get(ENV_CC);
    if (env->cc == NULL)
    {
        // get from path
        env->cc = get_cc();
    }
    // get the compiler CXX
    env->cxx = env_get(ENV_CXX);
    if (env->cxx == NULL)
    {
        // get from path
        env->cxx = get_cxx();
    }
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
        log_info("CC: %s", env->cc->data);
    }
    if (env->cxx != NULL)
    {
        log_info("CXX: %s", env->cxx->data);
    }
    if (env->ld != NULL)
    {
        log_info("LD: %s", env->ld->data);
    }

    return env;
}

string_t *get_cc()
{
    // all compilers array
    string_t *compilers[] = {
        CC_GCC,
        CC_CLANG,
        CC_MSVC,
        CC_INTEL,
        CC_INTELLLVM,
        CC_PGI,
        CC_ARM,
        CC_CRAY,
        CC_TINYC,
        CC_BORLAND,
        CC_SUN,
        CC_HP,
        CC_IBM,
        CC_MINGW,
        CC_CYGGCC,
        CC_LCC,
        CC_WATCOM,
        CC_ZIG,
        CC_EMCC};

    // get the compiler from the environment
    for (size_t i = 0; i < 19; i++)
    {
        string_t *cc = env_get(compilers[i]);
        if (cc != NULL)
        {
            return cc;
        }
    }

    return NULL;
}

string_t *get_cxx()
{
    // all compilers array
    string_t *compilers[] = {
        CXX_GCC,
        CXX_CLANG,
        CXX_MSVC,
        CXX_INTEL,
        CXX_INTELLLVM,
        CXX_PGI,
        CXX_ARM,
        CXX_CRAY,
        CXX_TINYC,
        CXX_BORLAND,
        CXX_SUN,
        CXX_HP,
        CXX_IBM,
        CXX_MINGW,
        CXX_CYGGCC,
        CXX_WATCOM,
        CXX_ZIG,
        CXX_EMCC};

    // get the compiler from the environment
    for (size_t i = 0; i < 18; i++)
    {
        string_t *cxx = env_get(compilers[i]);
        if (cxx != NULL)
        {
            return cxx;
        }
    }

    return NULL;
}

string_t *get_ld()
{
    // all linkers array
    string_t *linkers[] = {
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
        string_t *ld = env_get(linkers[i]);
        if (ld != NULL)
        {
            return ld;
        }
    }

    return NULL;
}
