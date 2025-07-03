#ifndef CREN_BUILD_COMPILE_H
#define CREN_BUILD_COMPILE_H

#include <build.h>

/// @brief compile the project.
/// @param build The build object.
/// @return CREN_OK on success, CREN_NOK on failure.
int build_compile(build_cfg_t *build);

#endif // CREN_BUILD_COMPILE_H
