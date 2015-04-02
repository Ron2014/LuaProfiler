
#ifndef __GameApp_H__
#define __GameApp_H__


#include "BattleScene.h"
#include "cocos2d.h"
#include "CRefreshTriggerMgr.h"

USING_NS_CC;




class CGameApp
{
public:
	CGameApp();
	~CGameApp();

	static CGameApp* Instance();

	void OnStarUp(); //进程启动时回调，初始化游戏
	void OnEnd();	//进程退出时处理
	


	CBattleScene* GetBattleScene();

	string GetIdentifyCode();

	void UpdateServerTime(uint32 nTime);
	uint32 GetServerTime();

	void RegisterTick(CTick* pTick, uint32 nCyc, const char* szLog);
	void UnRegisterTick(CTick* pTick);

	void OnRefreshTriger(uint32 nType);
	void RegiseterRefresh(int32 nType, uint32 nFirstOffset, uint32 nInterval);
private:

	CBattleScene m_battleScene;

	uint32 m_nServerTime;	//* 服务器时间 同步下来的值 秒
	uint32 m_nStarTime;		//* 

	uint64			m_uLogicTime;
	uint64			m_uPushCount;//已推动的次数

	CTickMgr*  m_pTickMgr;
	CRefreshTriggerMgr	m_RefreshMgr;

	void PushLogicTime( uint32 nMs );
public:
	void onEventUpdate(EventCustom *event);
	void onEventDraw(EventCustom *event);
private:
	EventListenerCustom *_eventUpdate;


};

#endif
