//

#ifndef LUA_KC_PROFILER_H
#define LUA_KC_PROFILER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "tolua++.h"
#ifdef __cplusplus
}
#endif

TOLUA_API int register_all_kc_profile(lua_State* tolua_S);

#endif