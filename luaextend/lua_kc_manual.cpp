#include "lua_kc_manual.h"
#include "tolua_fix.h"
#include "LuaBasicConversions.h"
#include "BattleScene.h"
#include "LuaBasic.h"
#include "GameApp.h"
#include "MediaMgr.h"
#include "base/CCDirector.h"

int IsBarrier4Lua(lua_State* L)
{

	int n = lua_gettop(L);
	
	if ( n == 5)
	{
		int32 x;
		int32 y;
		int32 nID;
		int nIgnoreBarrierLv;
		bool bInMove;
		bool ok = true;

		ok &= luaval_to_int32(L, 1, &x);
		ok &= luaval_to_int32(L, 2, &y);
		ok &= luaval_to_int32(L, 3, &nID);
		ok &= luaval_to_int32(L, 4, &nIgnoreBarrierLv);
		ok &= luaval_to_boolean(L, 5, &bInMove);
		if (!ok)
		{
			lua_pushnil(L);
			return 1;
		}
		
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		if (battleScene->IsBarrier(x, y, nID, nIgnoreBarrierLv, bInMove))
		{
			lua_pushboolean(L, 1);
		}
		else
		{
			lua_pushboolean(L, 0);
		}
		return 1;
	}
	
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "IsBarrier4Lua", n, 5);
	lua_pushnil(L);
	return 1;
}

int average(lua_State* L)
{

	int n = lua_gettop(L);
	double sum = 0;
	int i = 0;
	for (i = 1; i <= n; i++)
	{
		if (!lua_isnumber(L, i))
		{
			lua_pushstring(L, "not number");
			lua_error(L);
		}

		sum += lua_tonumber(L, i);
	}
	lua_pushnumber(L, sum / n);
	lua_pushnumber(L, sum);
	return 2;
}


int GetIdentifyCode(lua_State* L)
{
	std::string str = CGameApp::Instance()->GetIdentifyCode();
	lua_pushstring(L, str.c_str());
	return 1;
}

int FormatPix(lua_State* L)
{

	int n = lua_gettop(L);
	if ( n == 1 )
	{
		int nPix;
		bool ok = true;
		ok &= luaval_to_int32(L, 1, &nPix);
		if (!ok)
		{
			lua_pushnil(L);
			return 1;
		}
		int ret = GetGridByPix(nPix);
		ret = GetPixByGrid(ret);
		lua_pushnumber(L, ret);
		return 1;
	}
	lua_pushnil(L);
	return 1;
}

int BattleScene_Search(lua_State* tolua_S)
{

	int n = lua_gettop(tolua_S );

	if ( n == 7)
	{
		int32 nID;
		int32 nIgnoreLv;
		int32 srcX;
		int32 srcY;
		int32 destX;
		int32 destY;
		bool bNearBy;
		int idx = 1;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, idx++, &nID);
		ok &= luaval_to_int32(tolua_S, idx++, &nIgnoreLv);
		ok &= luaval_to_int32(tolua_S, idx++, &srcX);
		ok &= luaval_to_int32(tolua_S, idx++, &srcY);
		ok &= luaval_to_int32(tolua_S, idx++, &destX);
		ok &= luaval_to_int32(tolua_S, idx++, &destY);
		ok &= luaval_to_boolean(tolua_S, idx++, &bNearBy);

		if (!ok)
		{
			lua_pushnil(tolua_S);
			return 1;
		}

		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		int nTargetOK;
		vector< CPos16 >* pPath = battleScene->Search(nID, nIgnoreLv, srcX, srcY, destX, destY, bNearBy, nTargetOK);
		if (!pPath)
		{
			lua_pushnil(tolua_S);
			lua_pushboolean(tolua_S, 0);
			return 2;
		}

		CPos_array_to_luaval(tolua_S, &((*pPath)[0]), pPath->size());
		lua_pushboolean(tolua_S, 1);
		return 2;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "Search", n, 7);
	lua_pushnil(tolua_S);
	lua_pushboolean(tolua_S, 0);
	return 2;
}


int BattleScene_FollowSearch(lua_State* tolua_S)
{

	int n = lua_gettop(tolua_S);

	if (n == 3)
	{
		int32 nID;
		int32 nDestID;
		int32 nIgnoreLv;
		
		int idx = 1;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, idx++, &nID);
		ok &= luaval_to_int32(tolua_S, idx++, &nIgnoreLv);
		ok &= luaval_to_int32(tolua_S, idx++, &nDestID);

		if (!ok)
		{
			lua_pushnil(tolua_S);
			return 1;
		}

		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		vector< CPos16 >* pPath = battleScene->FollowSearch(nID, nIgnoreLv, nDestID);
		if (!pPath)
		{
			lua_pushnil(tolua_S);
			return 1;
		}

		CPos_array_to_luaval(tolua_S, &((*pPath)[0]), pPath->size());

		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "FollowSearch", n, 3);
	lua_pushnil(tolua_S);
	return 1;
}


int BattleScene_FollowBestPos(lua_State* tolua_S)
{

	int n = lua_gettop(tolua_S);

	if (n == 3)
	{
		int32 nID;
		int32 nDestID;
		int32 nIgnoreLv;
		
		int idx = 1;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, idx++, &nID);
		ok &= luaval_to_int32(tolua_S, idx++, &nIgnoreLv);
		ok &= luaval_to_int32(tolua_S, idx++, &nDestID);

		if (!ok)
		{
			lua_pushnil(tolua_S);
			return 1;
		}

		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		vector< CPos16 >* pPath = battleScene->FollowBestPos(nID,nIgnoreLv, nDestID);
		if (!pPath)
		{
			lua_pushnil(tolua_S);
			return 1;
		}

		CPos_array_to_luaval(tolua_S, &((*pPath)[0]), pPath->size());

		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "FollowBestPos", n, 2);
	lua_pushnil(tolua_S);
	return 1;
}

int BattleScene_EnterScene(lua_State* tolua_S)
{
	int argc = 0;

	bool ok = true;
	argc = lua_gettop(tolua_S);
	if (argc == 0)
	{
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->Enter();
		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "EnterScene", argc, 0);
	return 0;

}

int BattleScene_LeaveScene(lua_State* tolua_S)
{
	int argc = 0;

	bool ok = true;
	argc = lua_gettop(tolua_S);
	if (argc == 0)
	{
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->Leave();
		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "LeaveScene", argc, 0);
	return 0;

}

int BattleScene_ReInitMask(lua_State* tolua_S)
{
	int argc = 0;
	
	bool ok = true;
	argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int32 w , h;

		int idx = 1;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, idx++, &w);
		ok &= luaval_to_int32(tolua_S, idx++, &h);

		if (!ok)
		{
			return 0;
		}

		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->ReInitMask( w, h);
		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "ReInitMask", argc, 2);
	return 0;

}


int BattleScene_AddMaskObj(lua_State* tolua_S)
{
	int argc = 0;
	
	bool ok = true;

	argc = lua_gettop(tolua_S); 
	if (argc == 5)
	{
		int32 x;
		int32 y;
		int32 xRadiu;
		int32 yRadiu;
		int eType;

		ok &= luaval_to_int32(tolua_S, 1, &x);
		ok &= luaval_to_int32(tolua_S, 2, &y);
		ok &= luaval_to_int32(tolua_S, 3, &xRadiu);
		ok &= luaval_to_int32(tolua_S, 4, &yRadiu);
		ok &= luaval_to_int32(tolua_S, 6, &eType);

		if (!ok)
			return 0;
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->AddMaskObj(x, y, xRadiu, yRadiu,(EBarrierType)eType);
		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "AddMaskObj", argc, 0);
	return 0;

}



int BattleScene_AddObjPos(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 6)
	{
		int nID, x, y, xRadiu, yRadiu;
		int eType;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, 1, &nID);
		ok &= luaval_to_int32(tolua_S, 2, &x);
		ok &= luaval_to_int32(tolua_S, 3, &y);
		ok &= luaval_to_int32(tolua_S, 4, &xRadiu);
		ok &= luaval_to_int32(tolua_S, 5, &yRadiu);
		ok &= luaval_to_int32(tolua_S, 6, &eType);
		if (!ok)
			return 0;
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->AddObjPos(nID, x, y, xRadiu, yRadiu, (EBarrierType)eType);
		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "AddObjPos", argc, 6);
	return 0;

}


int BattleScene_UpdateObjBarrierBox(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 3)
	{
		int nID,xRadiu, yRadiu;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, 1, &nID);
		ok &= luaval_to_int32(tolua_S, 2, &xRadiu);
		ok &= luaval_to_int32(tolua_S, 3, &yRadiu);
		if (!ok)
			return 0;
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->UpdateObjBarrierBox(nID,  xRadiu, yRadiu );
		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "UpdateObjBarrierBox", argc, 3);
	return 0;

}
int BattleScene_UpdateObjPos(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 3)
	{
		int nID, x, y;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, 1, &nID);
		ok &= luaval_to_int32(tolua_S, 2, &x);
		ok &= luaval_to_int32(tolua_S, 3, &y);
		if (!ok)
			return 0;
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->UpdateObjPos(nID, x, y);
		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "UpdateObjPos", argc, 3);
	return 0;
}


int BattleScene_UpdateObjBarrier(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int nID;
		int eType;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, 1, &nID);
		ok &= luaval_to_int32(tolua_S, 2, &eType);
		if (!ok)
			return 0;
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->UpdateObjBarrier(nID, (EBarrierType)eType);
		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "UpdateObjBarrier", argc, 2);
	return 0;
}

int BattleScene_RemoveObj(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{
		int nID;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, 1, &nID);
		if (!ok)
			return 0;
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->RemoveObject( nID );
		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "RemoveObj", argc, 1);
	return 0;

}


int BattleScene_RegObjTick(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 3)
	{
		int nID;
		uint32 interval;
		int count;
		int i = 1;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, i++, &nID);
		ok &= luaval_to_uint32(tolua_S, i++, &interval);
		ok &= luaval_to_int32(tolua_S, i++, &count);

		if (!ok)
		{
			lua_pushnumber(tolua_S, 0);
			return 1;
		}
			
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		int ret = battleScene->RegObjTick(nID, interval, count);
		lua_pushnumber(tolua_S, ret);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "RegObjTick", argc, 3);
	lua_pushnumber(tolua_S, 0);
	return 1;
}


int BattleScene_DelObjTick(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int nID;
		int idx;
		int i = 1;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, i++, &nID);
		ok &= luaval_to_int32(tolua_S, i++, &idx); 

		if (!ok)
		{
			return 0;
		}

		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->DelObjTick(nID, idx);
		
		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "DelObjTick", argc, 2);
	
	return 0;
}

int BattleScene_PauseObjTick(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int nID;
		bool bPause;
		int i = 1;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, i++, &nID);
		ok &= luaval_to_boolean(tolua_S, i++, &bPause);
		
		if (!ok)
		{
			return 0;
		}

		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->PauseObjTick(nID, bPause);

		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "PauseObjTick", argc, 2);

	return 0;
}

int BattleScene_EndBattle(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 0)
	{
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->EndBattle();

		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "EndBattle", argc, 0);

	return 0;
}

int BattleScene_AddSpecialAction(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 0)
	{
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->AddSpecialAction();

		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "AddSpecialAction", argc, 0);

	return 0;
}


int BattleScene_GetValidPos(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 3)
	{
		int nID;
		int nIgnoreLv;
		int nDestID;
		int i = 1;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, i++, &nID);
		ok &= luaval_to_int32(tolua_S, i++, &nDestID);
		ok &= luaval_to_int32(tolua_S, i++, &nIgnoreLv);

		if (!ok)
		{
			lua_pushnil(tolua_S);
			return 1;
		}
		CPos16 pos;
		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		bool bRet = battleScene->GetValidPos(nID, nDestID, nIgnoreLv, pos);
		if ( bRet )
		{
			CPos_to_luaval(tolua_S, pos);
		}
		else
		{
			lua_pushnil(tolua_S);
		}
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "GetValidPos", argc, 3);
	lua_pushnil(tolua_S);
	return 1;
}

int BattleScene_GetBattleTickMark(lua_State* tolua_S)
{
	CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
	lua_pushnumber(tolua_S, battleScene->GetBattleTickMark());
	return 1;
}

int MediaMgr_LoadFevFile(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{
		int projectId;
		string szFev;
		bool ok = true;
		ok &= luaval_to_std_string(tolua_S,1,&szFev);
		if (!ok)
		{
			lua_pushinteger(tolua_S, 0);
			return 1;
		}
		
		projectId = CMediaMgr::Instance().LoadFevFile(szFev.c_str());
		lua_pushinteger(tolua_S, projectId);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:LoadFevFile", argc, 1);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_FmodUpdate(lua_State* tolua_S)
{
	int result = CMediaMgr::Instance().FmodUpdate();
	lua_pushboolean(tolua_S, result);
	return 1;
}

int MediaMgr_LoadEventGroup(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int projectId;
		string szGroup;
		int result;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, 1, &projectId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		ok = true;
		ok &= luaval_to_std_string(tolua_S,2,&szGroup);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		result = CMediaMgr::Instance().LoadEventGroup(projectId, szGroup.c_str());
		lua_pushboolean(tolua_S, result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:LoadEventGroup", argc, 1);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_FreeEventGroup(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int projectId;
		string szGroup;
		int result;
		bool ok = true;
		ok &= luaval_to_int32(tolua_S, 1, &projectId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		ok = true;
		ok &= luaval_to_std_string(tolua_S,2,&szGroup);
		
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		result = CMediaMgr::Instance().FreeEventGroup(projectId, szGroup.c_str());
		lua_pushboolean(tolua_S, result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:FreeEventGroup", argc, 1);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_Play(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int projectId;
		int eventId;
		string szEvent;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S, 1, &projectId);
		if (!ok)
		{
			lua_pushinteger(tolua_S, 0);
			return 1;
		}
		
		ok &= luaval_to_std_string(tolua_S, 2, &szEvent);
		if (!ok)
		{
			lua_pushinteger(tolua_S, 0);
			return 1;
		}
		
		eventId = CMediaMgr::Instance().Play(projectId, szEvent.c_str());
		lua_pushinteger(tolua_S, eventId);
		return 1;
	}
	
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:PlayEvent", argc, 2);
	lua_pushinteger(tolua_S, 0);
	return 1;
}

int MediaMgr_GetGroupId(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int projectId;
	    string szGroup;
		unsigned int handle;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S, 1, &projectId);
		if (!ok)
		{
			lua_pushinteger(tolua_S, 0);
			return 1;
		}
		
		ok = true;
		ok &= luaval_to_std_string(tolua_S,2, &szGroup);
		if (!ok)
		{
			lua_pushinteger(tolua_S, 0);
			return 1;
		}
		
		handle = CMediaMgr::Instance().GetGroupId(projectId, szGroup.c_str());
		lua_pushinteger(tolua_S, handle);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:GetGroupId", argc, 2);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_GetGroupEventId(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 3)
	{
		int projectId;
		string szGroup;
		string szEvent;
		unsigned int handle;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S, 1, &projectId);
		if (!ok)
		{
			lua_pushinteger(tolua_S, 0);
			return 1;
		}
		
		ok = true;
		ok &= luaval_to_std_string(tolua_S,2, &szGroup);
		if (!ok)
		{
			lua_pushinteger(tolua_S, 0);
			return 1;
		}
		
		ok = true;
		ok &= luaval_to_std_string(tolua_S,3, &szEvent);
		if (!ok)
		{
			lua_pushinteger(tolua_S, 0);
			return 1;
		}
		
		handle = CMediaMgr::Instance().GetGroupEventId(projectId, szGroup.c_str(), szEvent.c_str());
		lua_pushinteger(tolua_S, handle);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:GetGroupEventId", argc, 3);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_GetEventId(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int projectId;
		string szEvent;
		unsigned int handle;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S, 1, &projectId);
		if (!ok)
		{
			lua_pushinteger(tolua_S, 0);
			return 1;
		}
		
		ok = true;
		ok &= luaval_to_std_string(tolua_S,2, &szEvent);
		if (!ok)
		{
			lua_pushinteger(tolua_S, 0);
			return 1;
		}
		
		handle = CMediaMgr::Instance().GetEventId(projectId, szEvent.c_str());
		lua_pushinteger(tolua_S, handle);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:GetEventId", argc, 2);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_SetEventVolume(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int eventId;
		double volume;
		int result;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S,1, &eventId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		ok = true;
		ok &= luaval_to_number(tolua_S,2, &volume);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		result = CMediaMgr::Instance().SetEventVolume(eventId,volume);
		lua_pushboolean(tolua_S, result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:SetVolume", argc, 2);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_GetEventVolume(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{
		int eventId;
		bool ok = true;
		float volume;
		
		ok &= luaval_to_int32(tolua_S,1, &eventId);
		if (!ok)
		{
			lua_pushnumber(tolua_S, -1);
			return 1;
		}
		
		volume = CMediaMgr::Instance().GetEventVolume(eventId);
		lua_pushnumber(tolua_S, volume);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:GetVolume", argc, 1);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_SetEventPaused(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int eventId;
		bool paused;
		bool ok = true;
		int result;
		
		ok &= luaval_to_int32(tolua_S,1, &eventId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		ok = true;
		ok &= luaval_to_boolean(tolua_S,2, &paused);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		result = CMediaMgr::Instance().SetEventPaused(eventId,paused);
		lua_pushboolean(tolua_S,result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:SetPaused", argc, 2);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_GetEventPaused(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{
		int eventId;
		bool ok = true;
		int result;
		
		ok &= luaval_to_int32(tolua_S,1, &eventId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		result = CMediaMgr::Instance().GetEventPaused(eventId);
		lua_pushboolean(tolua_S,result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:GetPaused", argc, 1);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_SetEventMute(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int eventId;
		bool mute;
		bool ok = true;
		int result;
		
		ok &= luaval_to_int32(tolua_S,1, &eventId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		ok = true;
		ok &= luaval_to_boolean(tolua_S,2, &mute);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		result = CMediaMgr::Instance().SetEventMute(eventId,mute);
		lua_pushboolean(tolua_S, result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:SetMute", argc, 2);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_GetEventMute(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{
		int eventId;
		bool ok = true;
		int result;
		
		ok &= luaval_to_int32(tolua_S,1, &eventId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		result = CMediaMgr::Instance().GetEventMute(eventId);
		lua_pushboolean(tolua_S, result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:GetMute", argc, 1);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_EventStart(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{
		int eventId;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S,1, &eventId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		CMediaMgr::Instance().EventStart(eventId);
		lua_pushboolean(tolua_S, 1);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:Start", argc, 1);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_EventStop(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int eventId;
		bool isimmediate;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S,1, &eventId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		ok = true;
		ok &= luaval_to_boolean(tolua_S,2, &isimmediate);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		
		CMediaMgr::Instance().EventStop(eventId,isimmediate);
		lua_pushboolean(tolua_S, 1);
		return 1;
	}
	
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:Stop", argc, 2);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

//ÒôÀÖ
int MediaMgr_GetMusicId(lua_State* tolua_S)
{
	int musicId;
	musicId = CMediaMgr::Instance().GetMusicId();
	lua_pushinteger(tolua_S, musicId);
	return 1;
}

int MediaMgr_SetMusicVolume(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int musicId;
		double volume;
		int result;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S,1, &musicId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}

		ok &= luaval_to_number(tolua_S, 1, &volume);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}

		result = CMediaMgr::Instance().SetMusicVolume(musicId, volume);
		lua_pushboolean(tolua_S, result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:Stop", argc, 2);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_GetMusicVolume(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{
		int musicId;
		float volume;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S,1, &musicId);
		if (!ok)
		{
			lua_pushnumber(tolua_S, -1.0);
			return 1;
		}
		volume = CMediaMgr::Instance().GetMusicVolume(musicId);
		lua_pushnumber(tolua_S, volume);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:GetMusicVolume", argc, 1);
	lua_pushnumber(tolua_S, -1);
	return 1;
}

int MediaMgr_SetMusicPaused(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int musicId;
		bool pause;
		int result;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S,1, &musicId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}

		ok &= luaval_to_boolean(tolua_S, 2, &pause);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		result = CMediaMgr::Instance().SetMusicPaused(musicId, pause);
		lua_pushboolean(tolua_S, result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:Stop", argc, 2);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_GetMusicPaused(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int musicId;
		int pause;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S,1, &musicId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}
		pause = CMediaMgr::Instance().GetMusicPaused(musicId);

	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:Stop", argc, 2);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_SetMusicMute(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int musicId;
		int result;
		bool mute;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S,1, &musicId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}

		ok &= luaval_to_boolean(tolua_S, 2, &mute);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}

		result = CMediaMgr::Instance().SetMusicMute(musicId,mute);
		lua_pushboolean(tolua_S, result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:Stop", argc, 2);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_GetMusicMute(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{
		int musicId;
		int mute;
		bool ok = true;
		
		ok &= luaval_to_int32(tolua_S,1, &musicId);
		if (!ok)
		{
			lua_pushinteger(tolua_S, -1);
			return 1;
		}

		mute = CMediaMgr::Instance().GetMusicMute(musicId);
		lua_pushinteger(tolua_S, mute);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:Stop", argc, 1);
	lua_pushinteger(tolua_S, -1);
	return 1;
}

int MediaMgr_LoadMusicData(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 3)
	{
		int musicId;
		unsigned int res;
		unsigned int mod;
		bool ok = true;

		ok &= luaval_to_int32(tolua_S, 1, &musicId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}

		ok &= luaval_to_uint32(tolua_S, 2, &res);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}

		ok &= luaval_to_uint32(tolua_S, 3, &mod);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}

		int result = CMediaMgr::Instance().LoadMusicData(musicId, res, mod);
		lua_pushboolean(tolua_S, result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:Stop", argc,3);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int MediaMgr_FreeMusicData(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{
		int musicId;
		bool waituntilready;
		bool ok = true;

		ok &= luaval_to_int32(tolua_S, 1, &musicId);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}

		ok &= luaval_to_boolean(tolua_S, 2, &waituntilready);
		if (!ok)
		{
			lua_pushboolean(tolua_S, 0);
			return 1;
		}

		int result = CMediaMgr::Instance().FreeMusicData(musicId, waituntilready);
		lua_pushboolean(tolua_S, result);
		return 1;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "CMediaMgr:Stop", argc, 2);
	lua_pushboolean(tolua_S, 0);
	return 1;
}

int BattleScene_UpdateServerTime(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{

		int i = 1;
		uint32 nTime = (uint32)lua_tonumber(tolua_S, i++);


		CGameApp::Instance()->UpdateServerTime(nTime);
		

		return 0;
	}
	CCLOG("%s has wrong number of arguments: %d, was expecting %d \n", "SetRandomSeed", argc, 1);
	return 0;
}

int BattleScene_GetServerTime(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	
	lua_pushnumber( tolua_S, CGameApp::Instance()->GetServerTime());


	return 1;
}

int BattleScene_SetRandomSeed(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);
	if (argc == 1)
	{

		int i = 1;
		uint32 nSeed = (uint32)lua_tonumber(tolua_S, i++);


		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		battleScene->GetRandom().RandomInit(nSeed);

		return 0;
	}
	CCLOG(  "%s has wrong number of arguments: %d, was expecting %d \n", "SetRandomSeed", argc, 1);
	return 0;
}

int BattleScene_RandomDouble(lua_State* tolua_S)
{
	int argc = lua_gettop(tolua_S);

	CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
	double d = battleScene->GetRandom().Random();
	lua_pushnumber(tolua_S, d);
	return 1;
}

static int s_nRandCount = 0;
int BattleScene_Random(lua_State* tolua_S)
{
	s_nRandCount++;
	int argc = lua_gettop(tolua_S);
	if (argc == 2)
	{

		int i = 1;
		int32 nMin = (int32)lua_tonumber(tolua_S, i++);
		int32 nMax = (int32)lua_tonumber(tolua_S, i++);


		CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
		int32 nRet = battleScene->GetRandom().IRandom(nMin, nMax);
		lua_pushnumber(tolua_S, nRet);
		//CCLOG("$$$$$$$$$$$$$ rand %d, ret = %d, (%d,%d)", s_nRandCount, nRet, nMin, nMax );
		return 1;
	}
	CBattleScene* battleScene = CGameApp::Instance()->GetBattleScene();
	double d = battleScene->GetRandom().Random();
	lua_pushnumber(tolua_S, d);
	//CCLOG("$$$$$$$$$$$$$ rand %d, ret = %f", s_nRandCount, (float)d);
	return 1;
}



int RegiseterRefresh(lua_State* L)
{
	int n = lua_gettop(L);
	if (n == 3)
	{
		int i = 1;


		int32 nType = (int32)lua_tonumber(L, i++);
		uint32 nOffset = (uint32)lua_tonumber(L, i++);
		uint32 nInterval = (uint32)lua_tonumber(L, i++);

		CGameApp::Instance()->RegiseterRefresh(nType, nOffset, nInterval);

		return 0;
	}

	CCLOG(  "RegiseterRefresh require 3 param, but pass %d", n);
	return 0;
}


int SetTimeScale(lua_State* L)
{
	int n = lua_gettop(L);
	if (n == 1)
	{
		int i = 1;


		float scale = (float)lua_tonumber(L, i++); 

		CCDirector::getInstance()->SetTimeScale(scale);

		return 0;
	}

	CCLOG("RegiseterRefresh require 3 param, but pass %d", n);
	return 0;
}


void RegisterKCModule(lua_State* L)
{
	lua_newtable(L);

	lua_pushstring(L, "average");
	lua_pushcfunction(L, average);
	lua_rawset(L, -3);

	lua_pushstring(L, "Search");
	lua_pushcfunction(L, BattleScene_Search);
	lua_rawset(L, -3);

	lua_pushstring(L, "GRIDPIX");
	lua_pushnumber(L, GRIDPIX);
	lua_rawset(L, -3);


	lua_pushstring(L, "IsBarrier4Lua");
	lua_pushcfunction(L, IsBarrier4Lua);
	lua_rawset(L, -3);

	lua_pushstring(L, "AddMaskObj");
	lua_pushcfunction(L, BattleScene_AddMaskObj);
	lua_rawset(L, -3);

	lua_pushstring(L, "ReInitMask");
	lua_pushcfunction(L, BattleScene_ReInitMask);
	lua_rawset(L, -3);

	lua_pushstring(L, "AddObjPos");
	lua_pushcfunction(L, BattleScene_AddObjPos);
	lua_rawset(L, -3);

	lua_pushstring(L, "UpdateObjBarrierBox");
	lua_pushcfunction(L, BattleScene_UpdateObjBarrierBox);
	lua_rawset(L, -3);

	lua_pushstring(L, "UpdateObjPos");
	lua_pushcfunction(L, BattleScene_UpdateObjPos);
	lua_rawset(L, -3);

	lua_pushstring(L, "UpdateObjBarrier");
	lua_pushcfunction(L, BattleScene_UpdateObjBarrier);
	lua_rawset(L, -3);

	lua_pushstring(L, "RemoveObj");
	lua_pushcfunction(L, BattleScene_RemoveObj);
	lua_rawset(L, -3);

	lua_pushstring(L, "RegObjTick");
	lua_pushcfunction(L, BattleScene_RegObjTick);
	lua_rawset(L, -3);

	lua_pushstring(L, "DelObjTick");
	lua_pushcfunction(L, BattleScene_DelObjTick);
	lua_rawset(L, -3);

	lua_pushstring(L, "PauseObjTick");
	lua_pushcfunction(L, BattleScene_PauseObjTick);
	lua_rawset(L, -3);

	lua_pushstring(L, "EndBattle");
	lua_pushcfunction(L, BattleScene_EndBattle);
	lua_rawset(L, -3);

	lua_pushstring(L, "AddSpecialAction");
	lua_pushcfunction(L, BattleScene_AddSpecialAction);
	lua_rawset(L, -3);

	lua_pushstring(L, "FollowSearch");
	lua_pushcfunction(L, BattleScene_FollowSearch);
	lua_rawset(L, -3);

	lua_pushstring(L, "FollowBestPos");
	lua_pushcfunction(L, BattleScene_FollowBestPos);
	lua_rawset(L, -3);

	lua_pushstring(L, "FormatPix");
	lua_pushcfunction(L, FormatPix);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetValidPos");
	lua_pushcfunction(L, BattleScene_GetValidPos);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetBattleTickMark");
	lua_pushcfunction(L, BattleScene_GetBattleTickMark);
	lua_rawset(L, -3);
	//sound
	lua_pushstring(L, "LoadFevFile");
	lua_pushcfunction(L, MediaMgr_LoadFevFile);
	lua_rawset(L, -3);
	
	lua_pushstring(L,"FmodUpdate");
	lua_pushcfunction(L, MediaMgr_FmodUpdate);
	lua_rawset(L, -3);
	
	lua_pushstring(L, "LoadEventGroup");
	lua_pushcfunction(L, MediaMgr_LoadEventGroup);
	lua_rawset(L, -3);

	lua_pushstring(L, "FreeEventGroup");
	lua_pushcfunction(L, MediaMgr_FreeEventGroup);
	lua_rawset(L, -3);

	lua_pushstring(L, "Play");
	lua_pushcfunction(L, MediaMgr_Play);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetGroupId");
	lua_pushcfunction(L, MediaMgr_GetGroupId);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetGroupEventId");
	lua_pushcfunction(L, MediaMgr_GetGroupEventId);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetEventId");
	lua_pushcfunction(L, MediaMgr_GetEventId);
	lua_rawset(L, -3);

	lua_pushstring(L, "SetEventVolume");
	lua_pushcfunction(L, MediaMgr_SetEventVolume);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetEventVolume");
	lua_pushcfunction(L, MediaMgr_GetEventVolume);
	lua_rawset(L, -3);

	lua_pushstring(L, "SetEventPaused");
	lua_pushcfunction(L, MediaMgr_SetEventPaused);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetEventPaused");
	lua_pushcfunction(L, MediaMgr_GetEventPaused);
	lua_rawset(L, -3);

	lua_pushstring(L, "SetEventMute");
	lua_pushcfunction(L, MediaMgr_SetEventMute);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetEventMute");
	lua_pushcfunction(L, MediaMgr_GetEventMute);
	lua_rawset(L, -3);

	lua_pushstring(L, "EventStart");
	lua_pushcfunction(L, MediaMgr_EventStart);
	lua_rawset(L, -3);

	lua_pushstring(L, "EventStop");
	lua_pushcfunction(L, MediaMgr_EventStop);
	lua_rawset(L, -3);
	
	//music
	lua_pushstring(L, "GetMusicId");
	lua_pushcfunction(L, MediaMgr_GetMusicId);
	lua_rawset(L, -3);

	lua_pushstring(L, "SetMusicVolume");
	lua_pushcfunction(L, MediaMgr_SetMusicVolume);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetMusicVolume");
	lua_pushcfunction(L, MediaMgr_GetMusicVolume);
	lua_rawset(L, -3);

	lua_pushstring(L, "SetMusicPaused");
	lua_pushcfunction(L, MediaMgr_SetMusicPaused);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetMusicPaused");
	lua_pushcfunction(L, MediaMgr_GetMusicPaused);
	lua_rawset(L, -3);

	lua_pushstring(L, "SetMusicMute");
	lua_pushcfunction(L, MediaMgr_SetMusicMute);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetMusicMute");
	lua_pushcfunction(L, MediaMgr_GetMusicMute);
	lua_rawset(L, -3);

	lua_pushstring(L, "LoadMusicData");
	lua_pushcfunction(L, MediaMgr_LoadMusicData);
	lua_rawset(L, -3);

	lua_pushstring(L, "FreeMusicData");
	lua_pushcfunction(L, MediaMgr_FreeMusicData);
	lua_rawset(L, -3);


	lua_pushstring(L, "GetIdentifyCode");
	lua_pushcfunction(L, GetIdentifyCode);
	lua_rawset(L, -3);

	lua_pushstring(L, "LeaveScene");
	lua_pushcfunction(L, BattleScene_LeaveScene);
	lua_rawset(L, -3);

	lua_pushstring(L, "EnterScene");
	lua_pushcfunction(L, BattleScene_EnterScene);
	lua_rawset(L, -3);


	lua_pushstring(L, "SetBattleRandomSeed");
	lua_pushcfunction(L, BattleScene_SetRandomSeed);
	lua_rawset(L, -3);


	lua_pushstring(L, "BattleRandomDouble");
	lua_pushcfunction(L, BattleScene_RandomDouble);
	lua_rawset(L, -3);

	lua_pushstring(L, "BattleRandom");
	lua_pushcfunction(L, BattleScene_Random);
	lua_rawset(L, -3);

	lua_pushstring(L, "UpdateServerTime");
	lua_pushcfunction(L, BattleScene_UpdateServerTime);
	lua_rawset(L, -3);

	lua_pushstring(L, "GetServerTime");
	lua_pushcfunction(L, BattleScene_GetServerTime);
	lua_rawset(L, -3);

	lua_pushstring(L, "RegiseterRefresh");
	lua_pushcfunction(L, RegiseterRefresh);
	lua_rawset(L, -3);

	lua_pushstring(L, "SetTimeScale");
	lua_pushcfunction(L, SetTimeScale);
	lua_rawset(L, -3);

	lua_setglobal(L, "kc");
	lua_pop(L, 1);
}

TOLUA_API int register_all_kc_manual(lua_State* L)
{
	RegisterKCModule(L);
	return 1;
}