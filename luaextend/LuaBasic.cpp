#include "LuaBasic.h"


void CPos_to_luaval(lua_State* L, const CPos16& vec2)
{
	if (NULL == L)
		return;
	lua_newtable(L);                                    /* L: table */
	lua_pushstring(L, "x");                             /* L: table key */
	lua_pushnumber(L, (lua_Number)vec2.x);               /* L: table key value*/
	lua_rawset(L, -3);                                  /* table[key] = value, L: table */
	lua_pushstring(L, "y");                             /* L: table key */
	lua_pushnumber(L, (lua_Number)vec2.y);               /* L: table key value*/
	lua_rawset(L, -3);
}


void CPos_array_to_luaval(lua_State* L, const CPos16* points, int count)
{
	if (NULL == L)
		return;
	lua_newtable(L);
	for (int i = 1; i <= count; ++i)
	{
		lua_pushnumber(L, i);
		CPos_to_luaval(L, points[i - 1]);
		lua_rawset(L, -3);
	}
}
