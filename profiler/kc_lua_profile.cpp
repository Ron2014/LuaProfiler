#include "lua_kc_manual.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#include "BattleScene.h"
#include "LuaBasic.h"
#include "GameApp.h"
#include "MediaMgr.h"
#include "base/CCDirector.h"
#include "kc_lua_profile.h"
#include "lua_profiler.h"

TOLUA_API int register_lua_profile(lua_State* L)
{
	luaopen_profiler(L);
	return 1;
}

