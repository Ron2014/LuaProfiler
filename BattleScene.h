#ifndef __BATTLESCENE_H__
#define __BATTLESCENE_H__
#include "arkcode/ArkBaseTypes.h"
#include "CSearch.h"
#include "arkcode/CTickMgr.h"
#include "arkcode/TRandomMersenne.h"
#include <assert.h>


extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

#include <map>
using namespace std;


#define TICKCYC 16

class CBattleScene
{
public:
	CBattleScene(  );
	~CBattleScene();
	void InitScene(lua_State* L);
	void SetValid(bool bValid);
	bool IsValid() { return m_bValid; };
	void Update( uint32 nMs );	//毫秒

	void Enter();
	void Leave();

	void ReInitMask( uint32 nWidthPix, uint32 nHeightPix );

	void AddMaskObj(int x, int y, int xRadiu, int yRadiu, EBarrierType eType );
	void RemoveMaskObj(int x, int y, int xRadiu, int yRadiu);
	vector< CPos16 >* Search(int nID, int nIgnoreLv, int srcX, int srcY, int destX, int destY, bool bNearBy, int& nTargetOK);
	vector< CPos16 >* FollowSearch(int nID, int nIgnoreLv, int destID, bool bNearBy = true);
	vector< CPos16 >* FollowBestPos(int nID, int nIgnoreLv, int destID);

	bool IsBarrier( int x, int y, int nID, int nIgnoreLv, bool bInMove );	//* pix
	void AddObjPos(int nObjID, int x, int y, int wRadiu, int hRadiu, EBarrierType eBarrierType);
	void UpdateObjPos(int nObjID, int x, int y);
	void UpdateObjBarrier(int nObjID, EBarrierType eBarrierType);
	void RemoveObject( int nID);

	void UpdateObjBarrierBox(int nObjID, int wRadiu, int hRadiu);
	

	int RegObjTick(int objID, uint32 interval, int nCount );
	void DelObjTick(int objID, int idx);
	void PauseObjTick( int objID, bool bPause );

	void EndBattle() { m_bEnd = true; };
	void AddSpecialAction() { m_bNeedCall = true; };

	bool GetValidPos(int objID, int destID, int nIgnoreLv, CPos16& out);

	//*获取最佳攻击点
	bool GetAttackPos(int objID, int destID, int nIgnoreLv, CPos16& out);

	TRandomMersenne& GetRandom() { return m_Random; };

	uint64 GetBattleTickMark() { return m_nCurTickMark; };

	uint32 GetMaxGridX();

	uint32 GetMaxGridY();

	int ModifyX(int nPix);
	int ModifyY(int nPix);

private:
	void IgnoreObjBarrier(int nID);
	void RestoreObjBarrier(int nID);
	void CallLuaTick(int nID, bool bDel);
	void CallLuaSpecial( int nTickMark );
	
	void CleanTick();
private:
	
	
	bool m_bValid;

//#ifndef KC_GENLUA
public:

private:
	
	SMask	m_mask;
	CSearch m_search;

	struct CObjPos
	{
		CPos center;
		int xRadiu;	
		int yRadiu;
		bool bBarrier;	//* 是否飞行对象
		EBarrierType eType;
		CObjPos()
		{
			center.x = 0;
			center.y = 0;
			xRadiu = 1;
			yRadiu = 1;
			bBarrier = true;
			eType = eBarrierType_Low;
		}
		CObjPos(int x, int y, int wRaidu, int hRadiu, EBarrierType eBarrierType)
		{
			center = CPos(x, y);
			xRadiu = wRaidu;
			yRadiu = hRadiu;
			bBarrier = ( eBarrierType != eBarrierType_None );
			eType = eBarrierType;
		};

		void UpdateBarrierType(EBarrierType eBarrierType)
		{
			bBarrier = (eBarrierType != eBarrierType_None);
			eType = eBarrierType;
		}
	};

	map< int, CObjPos > m_mapObjPos;
	
	void _MarkObjBarrier(CObjPos& objPos);
	void _RemoveObjBarrer(CObjPos& objPos);
	
	class CMyTick:public CTick
	{
	public:
		CMyTick(int funID, CBattleScene* pScene, int nCount ) :
			m_funID(funID),
			m_pBattleScene( pScene ),
			m_nMaxCount( nCount ),
			m_nCur( 0 )
		{

		}
		 
		virtual void	OnTick()
		{
			bool bDel = false;
			if ( m_nMaxCount > 0 )
			{
				m_nCur++;
				if ( m_nCur >= m_nMaxCount )
				{
					bDel = true;
				}
				
			}
			if (bDel)
			{
				UnRegister();
			}

			m_pBattleScene->CallLuaTick(m_funID, bDel);
		}
	private:
		int m_funID;
		int m_nMaxCount;
		int m_nCur;
		CBattleScene* m_pBattleScene;
	};

	struct SObjTickMgr
	{
		CTickMgr*  pTickMgr;
		map< int, CMyTick*>  mapTick;
		bool bPause;

		SObjTickMgr()
		{
			pTickMgr = new CTickMgr( TICKCYC );
			bPause = false;
		}
		~SObjTickMgr()
		{
			SAFE_DELETE(pTickMgr);
			for (auto iterTick = mapTick.begin(); iterTick != mapTick.end(); iterTick++)
			{
				SAFE_DELETE(iterTick->second);
			}
		}

	};

	lua_State* m_L;
	map< int, SObjTickMgr* >	m_mapObjTick;	// 0 的为logic的tick
	vector< int >	m_vecObjTick;			//* 记录 m_mapObjTick里的key
	vector< int >	m_vecWillDelTcik;		//* 记录要删除的

	int		m_nCurTickMark;					//* 战斗进行到的刻度
	bool	m_bNeedCall;					//* 需要回调脚本
	int m_tickIdx;
	bool m_bEnd;

	uint64			m_uLogicTime;
	uint64			m_uPushCount;//已推动的次数
	
	TRandomMersenne m_Random;

	
};

#endif
