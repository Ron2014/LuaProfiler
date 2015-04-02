#include "GameApp.h"
#include "CCLuaEngine.h"
#include "luaextend/lua_kc_manual.h"
#include "2d/CCDrawingPrimitives.h"
#include "MediaMgr.h"
#include "base/CCConsole.h"

#ifdef USE_PROFILER
#include "kc_luajit_profile.h"
#include "kc_lua_profile.h"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
#include "platform/android/jni/JniHelper.h"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS )
#include "IosHelper.h"
#endif


#include "lpack.h"
#include "pb.h"

using namespace DrawPrimitives;

CGameApp::CGameApp():
m_nServerTime(0),
m_nStarTime(0),
m_uLogicTime(0),
m_uPushCount(0)
{
	LuaEngine* engine = LuaEngine::getInstance();
	m_battleScene.InitScene( engine->getLuaStack()->getLuaState() );

	m_pTickMgr = new CTickMgr( TICKCYC );
}


CGameApp::~CGameApp()
{
	SAFE_DELETE(m_pTickMgr);
}

CGameApp* CGameApp::Instance()
{
	static CGameApp _instance;
	return &_instance;
}

void CGameApp::OnStarUp()
{
	auto director = Director::getInstance();
	auto dispatcher = director->getEventDispatcher();

	_eventUpdate = dispatcher->addCustomEventListener(Director::EVENT_AFTER_UPDATE, std::bind(&CGameApp::onEventUpdate, this, std::placeholders::_1));

	

	LuaEngine* engine = LuaEngine::getInstance();
	
	luaopen_pack(engine->getLuaStack()->getLuaState());
	luaopen_pb(engine->getLuaStack()->getLuaState());

	register_all_kc_manual(engine->getLuaStack()->getLuaState());
	
#ifdef USE_PROFILER
	register_luajit_profile(engine->getLuaStack()->getLuaState());
	register_lua_profile(engine->getLuaStack()->getLuaState());
#endif
	
	//* test 
	//dispatcher->addCustomEventListener(Director::EVENT_AFTER_DRAW, std::bind(&CGameApp::onEventDraw, this, std::placeholders::_1));
}

void CGameApp::onEventUpdate(EventCustom *event)
{
	auto director = Director::getInstance();
	int nMs = int( director->getDeltaTime() * 1000 );

	if (nMs < 0)
		return;
	
	m_RefreshMgr.CheckTrigger();
	
	PushLogicTime(nMs);

	if ( m_battleScene.IsValid())
	{              
		m_battleScene.Update(nMs);
	}
	
	CMediaMgr::Instance().FmodUpdate();
}

void CGameApp::OnEnd()
{
	auto director = Director::getInstance();
	auto dispatcher = director->getEventDispatcher();
	dispatcher->removeEventListener(_eventUpdate);
}

CBattleScene* CGameApp::GetBattleScene()
{
	return &m_battleScene;
}

void CGameApp::onEventDraw(EventCustom *event)
{
	
}

std::string CGameApp::GetIdentifyCode()
{
	std::string strRet = "";
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID )
	{
		JniMethodInfo methInfo;
		bool isHave = JniHelper::getStaticMethodInfo(methInfo,"org/kittycraft/Helper", "getIdentifyCode", "()Ljava/lang/String;");
		jstring jstr;
		if (!isHave)
		{
			CCLog("jni:此函数不存在");
		}
		else
		{
			CCLog("jni:此函数存在");
			//调用此函数
			jstr = (jstring)methInfo.env->CallStaticObjectMethod(methInfo.classID, methInfo.methodID);
			strRet = JniHelper::jstring2string(jstr);
		}
		CCLog("jni-java函数执行完毕");
	}
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS )
	{
        strRet = IosHelper::GetIntance().GetIdentifyCode();
    }
#endif
	return strRet;
}

void CGameApp::UpdateServerTime(uint32 nTime)
{
	m_nServerTime = nTime;
	m_nStarTime = time(NULL);

	m_RefreshMgr.Clear();

	LuaEngine* engine = LuaEngine::getInstance();
	lua_State* pLS = engine->getLuaStack()->getLuaState();
	lua_getglobal(pLS, "funcall_client");								/* L: funcall_client */
	lua_getfield(pLS, -1, "OnUpdateServerTime");						/* L: funcall_client, OnUpdateServerTime */
	if (!lua_isfunction(pLS, -1))
	{
		log("CGameApp::OnRefreshTriger error");
		lua_pop(pLS, 2);
		return;
	}
	lua_remove(pLS, -2);											/* L: OnUpdateServerTime */
	
	engine->getLuaStack()->executeFunction(0);


}

uint32 CGameApp::GetServerTime()
{
	return m_nServerTime + time(NULL) - m_nStarTime;
}

void CGameApp::RegisterTick(CTick* pTick, uint32 nCyc, const char* szLog)
{
	m_pTickMgr->Register(pTick, nCyc);
}

void CGameApp::UnRegisterTick(CTick* pTick)
{
	m_pTickMgr->UnRegister(pTick);
}

void CGameApp::OnRefreshTriger(uint32 nType)
{
	LuaEngine* engine = LuaEngine::getInstance();
	lua_State* pLS= engine->getLuaStack()->getLuaState();
	lua_getglobal(pLS, "funcall_client");								/* L: funcall_client */
	lua_getfield(pLS, -1, "OnRefresh");									/* L: funcall_client, OnRefresh */
	if (!lua_isfunction(pLS, -1))
	{
		log( "CGameApp::OnRefreshTriger error");
		lua_pop(pLS, 2);
		return;
	}
	lua_remove(pLS, -2);											/* L: OnRefresh */
	lua_pushnumber(pLS, nType);										/* L: OnRefresh, nType*/

	engine->getLuaStack()->executeFunction(1);
}

void CGameApp::RegiseterRefresh(int32 nType, uint32 nFirstOffset, uint32 nInterval)
{
	m_RefreshMgr.RegisterRefresh(nType, nFirstOffset, nInterval);
}

void CGameApp::PushLogicTime(uint32 nMs)
{
	m_uLogicTime += nMs;

	uint64 uMustPushCount = m_uLogicTime / TICKCYC + 1;		//计算一共需推动多少次. +1 注册后确保会被推一次
	uint64 uEnd = uMustPushCount - m_uPushCount;		//计算当前需要推动多少次
	for (uint64 n = 0; n < uEnd; ++n)
	{
		m_pTickMgr->OnTick();
		++m_uPushCount;
	}
}
