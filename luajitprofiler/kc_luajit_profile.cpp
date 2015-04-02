#include "lua_kc_manual.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#include "BattleScene.h"
#include "LuaBasic.h"
#include "GameApp.h"
#include "MediaMgr.h"
#include "base/CCDirector.h"
#include "kc_luajit_profile.h"
#include "LuaJitProfile.h"

TOLUA_API int register_luajit_profile(lua_State* L)
{
	script::lua::LuaProfile_openLib(L);
	return 1;
}
