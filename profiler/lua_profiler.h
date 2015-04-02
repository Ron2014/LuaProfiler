#ifndef LUA_PROFILE_H
#define LUA_PROFILE_H

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

TOLUA_API int luaopen_profiler(lua_State* tolua_S);

#endif

