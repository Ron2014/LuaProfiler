#ifndef __LUABASIC_H__
#define __LUABASIC_H__
#include "ArkMath.h"

extern "C" {
#include "lua.h"
}

void CPos_to_luaval(lua_State* L, const CPos16& vec2);
void CPos_array_to_luaval(lua_State* L, const CPos16* points, int count);

#endif
