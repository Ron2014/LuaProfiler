//

#ifndef KC_LUAJIT_PROFILER_H
#define KC_LUAJIT_PROFILER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

TOLUA_API int register_luajit_profile(lua_State* tolua_S);

#endif