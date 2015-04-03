#include "BattleScene.h"
#include "CCLuaEngine.h"
#include "2d/CCDrawingPrimitives.h"
USING_NS_CC;
using namespace DrawPrimitives;



#define TICKKEY( nObjID, tickIdx )  ((int64)nObjID ) << 32 & tickIdx

CBattleScene::CBattleScene( ) :
m_bValid(false),
m_tickIdx( 1 ),
m_bEnd( true ),
m_nCurTickMark(0),
m_bNeedCall( false ),
m_uLogicTime( 0 ),
m_uPushCount( 0 )
{
	
}


CBattleScene::~CBattleScene()
{
	CleanTick();
}

void CBattleScene::SetValid(bool bValid)
{
	m_bValid = bValid;
}

void CBattleScene::Update(uint32 nMs)
{
	if ( m_bEnd )
	{
		return;
	}

	for (uint32 i = 0; i < m_vecWillDelTcik.size(); i++)
	{
		int nID = m_vecWillDelTcik[i];
		SObjTickMgr* objTick = m_mapObjTick[nID];
		SAFE_DELETE(objTick);
		m_mapObjTick.erase(nID);
		auto iter = find(m_vecObjTick.begin(), m_vecObjTick.end(), nID);
		if (iter != m_vecObjTick.end())
		{
			m_vecObjTick.erase(iter);
		}
	}

	if (m_vecWillDelTcik.size() > 0)
	{
		sort(m_vecObjTick.begin(), m_vecObjTick.end());
	}

	m_vecWillDelTcik.clear();

	m_uLogicTime += nMs;

	uint64 uMustPushCount = m_uLogicTime / TICKCYC + 1;		//计算一共需推动多少次. +1 注册后确保会被推一次
	uint64 uEnd = uMustPushCount - m_uPushCount;		//计算当前需要推动多少次
	for (uint64 n = 0; n < uEnd && !m_bEnd; ++n)
	{
		for (uint32 j = 0; j < m_vecObjTick.size(); j++)
		{
			int nID = m_vecObjTick[j];

			auto iter = m_mapObjTick.find(nID);
			if (iter == m_mapObjTick.end())
				continue;

			SObjTickMgr* objTick = iter->second;
			if (objTick->bPause)
			{
				continue;
			}
			objTick->pTickMgr->OnTick();
			if (m_bEnd)
			{
				break;
			}
		}
		m_nCurTickMark++;
		++m_uPushCount;
	}

	if (m_bEnd)
	{
		CleanTick();
		return;
	}
		//if (m_bNeedCall)
		//{
		//	m_bNeedCall = false;
		//	CallLuaSpecial(m_nCurTickMark);
		//}



}



void CBattleScene::RemoveObject( int nID)
{
	IgnoreObjBarrier(nID);
	if (m_mapObjPos.find(nID) != m_mapObjPos.end())
	{
		m_mapObjPos.erase(nID);
	}

	if ( m_mapObjTick.find( nID ) != m_mapObjTick.end() )
	{
		m_vecWillDelTcik.push_back(nID);
	}

}

void CBattleScene::Enter()
{
	SetValid(true);
	m_bEnd = false;
	m_nCurTickMark = 0;
	m_uLogicTime = 0;
	m_uPushCount = 0;
	m_tickIdx = 1;
	CleanTick();
	printf("CBattleScene::Enter" );
}

void CBattleScene::Leave()
{ 
	m_bEnd = true;
	SetValid(false);
	
	printf("CBattleScene::Leave");
}

void CBattleScene::ReInitMask(uint32 nWidthPix, uint32 nHeightPix)
{
	m_mask.Init( nWidthPix, nHeightPix );
	m_search.ReInit( GetGridByPix(nWidthPix), GetGridByPix( nHeightPix ) );
}

void CBattleScene::AddMaskObj(int x, int y, int xRadiu, int yRadiu, EBarrierType eType)
{
	m_mask.SetMaskObj(GetGridByPix(x), GetGridByPix(y), xRadiu, yRadiu, eType);
}


void CBattleScene::RemoveMaskObj(int x, int y, int xRadiu, int yRadiu)
{
	m_mask.RemoveMaskObj(GetGridByPix(x), GetGridByPix(y), xRadiu, yRadiu, eBarrierType_Low);
}

vector< CPos16 >*  CBattleScene::Search(int nID, int nIgnoreLv,int srcX, int srcY, int destXIn, int destYIn, bool bNearBy, int& nTargetOK)
{

	int destX = ModifyX(destXIn);
	int destY = ModifyY(destYIn);

	int xRadiu = 0;
	int yRadiu = 0;

	auto iter = m_mapObjPos.find(nID);
	if (iter != m_mapObjPos.end())
	{
		CObjPos& objPos = iter->second;
		xRadiu = objPos.xRadiu;
		yRadiu = objPos.yRadiu;
	}
	IgnoreObjBarrier(nID);
	bool bOK = m_search.Search(nIgnoreLv, srcX, srcY, destX, destY, xRadiu, yRadiu, m_mask);
	nTargetOK = bOK ? 1 : 0;
	if ( !bOK && bNearBy)
	{
		bOK = m_search.SearchNearByPoint(nIgnoreLv, xRadiu, yRadiu, m_mask);
	}
	RestoreObjBarrier(nID);
	if (bOK)
	{
		return  &m_search.GetPath();
	}
	return NULL;
}


bool CBattleScene::IsBarrier(int x, int y, int nID, int nIgnoreLv, bool bInMove)
{
	auto iter = m_mapObjPos.find(nID);
	if (iter == m_mapObjPos.end())
	{
		return false;
	}
	IgnoreObjBarrier(nID);
	CObjPos& objPos = iter->second;
	int gridX = GetGridByPix( x );
	int gridY = GetGridByPix( y );
	bool bRet = m_mask.IsBarrier(gridX, gridY, objPos.xRadiu, objPos.yRadiu, nIgnoreLv, bInMove);
	RestoreObjBarrier(nID);
	return bRet;
}

void CBattleScene::UpdateObjPos(int nObjID, int xIn, int yIn )
{
	int x = ModifyX(xIn);
	int y = ModifyY(yIn);

	if (x != xIn || y != yIn)
	{
		CCLOG("CBattleScene::UpdateObjPos not valid pos (%d,%d) ", xIn, yIn);
	}

	auto iter = m_mapObjPos.find( nObjID );
	if ( iter == m_mapObjPos.end() )
	{
		return;
	}
	CObjPos& objPos = iter->second;
	if (objPos.bBarrier)
		_RemoveObjBarrer(objPos);
	//注意不要搞乱顺序
	objPos.center = CPos(x, y);
	if (objPos.bBarrier)
		_MarkObjBarrier(objPos);
}

void CBattleScene::IgnoreObjBarrier(int nID)
{
	auto iter = m_mapObjPos.find(nID);
	if (iter != m_mapObjPos.end())
	{
		CObjPos& objPos = iter->second;
		if (!objPos.bBarrier)
			return;
		_RemoveObjBarrer(objPos);
	}

}

void CBattleScene::RestoreObjBarrier(int nID)
{
	auto iter = m_mapObjPos.find(nID);
	if (iter != m_mapObjPos.end())
	{
		CObjPos& objPos = iter->second;
		if (!objPos.bBarrier)
			return;
		_MarkObjBarrier(objPos);
	}
}


void CBattleScene::InitScene(lua_State* L)
{
	m_L = L;
}


void CBattleScene::CallLuaTick(int nID, bool bDel)
{
	lua_getglobal( m_L, "kc_tick");										/* L: kc_tick */
	lua_getfield( m_L, -1, "OnTick");									/* L: kc_tick, OnTick */
	if (!lua_isfunction(m_L, -1) )
	{
        lua_pop(m_L, 2);
		return;
	}	
	lua_remove(m_L, -2);												/* L: OnTick */
	lua_pushnumber(m_L, nID);											/* L: OnTick, nID*/
	lua_pushboolean(m_L, bDel ? 1 : 0);									/* L: OnTick, nID, bDel*/
	//lua_call(m_L, 2, 0);

	LuaEngine* engine = LuaEngine::getInstance();
	engine->getLuaStack()->executeFunction(2);
}

int CBattleScene::RegObjTick(int objID, uint32 interval, int nCount)
{
	//* 这里不作重复判断，因为idx是自增

	int newIdx = m_tickIdx++;
	 
	if (m_mapObjTick.find(objID) == m_mapObjTick.end())
	{
		m_mapObjTick[objID] = new SObjTickMgr();
		m_vecObjTick.push_back(objID);
		sort(m_vecObjTick.begin(), m_vecObjTick.end());
	}

	SObjTickMgr* objTick = m_mapObjTick[objID];

	CMyTick* pTick = new CMyTick(newIdx, this, nCount);
	if (objTick->pTickMgr->Register(pTick, interval))
	{
		objTick->mapTick[newIdx] = pTick;
		return newIdx;
	}
	return 0;
}

void CBattleScene::DelObjTick(int objID, int idx)
{
	if (m_mapObjTick.find(objID) != m_mapObjTick.end())
	{
		SObjTickMgr* objTick = m_mapObjTick[objID];
		if (objTick->mapTick.find(idx) != objTick->mapTick.end())
		{
			CMyTick* pTick = objTick->mapTick[idx];
			objTick->pTickMgr->UnRegister(pTick);
			
			//CCLOG("idx = %d pTick = %u", idx, (uint32)pTick);
			SAFE_DELETE(pTick);
			objTick->mapTick.erase(idx);
		}
	}
}

void CBattleScene::PauseObjTick(int objID, bool bPause)
{
	if (m_mapObjTick.find(objID) != m_mapObjTick.end())
	{
		SObjTickMgr* objTick = m_mapObjTick[objID];
		objTick->bPause = bPause;
	}
}

void CBattleScene::CleanTick()
{
	m_vecWillDelTcik.clear();
	m_vecObjTick.clear();
	for (auto iter = m_mapObjTick.begin(); iter != m_mapObjTick.end(); iter++)
	{
		SObjTickMgr* objTick = iter->second;
		SAFE_DELETE(objTick);
	}
	m_mapObjTick.clear();
}

void CBattleScene::CallLuaSpecial(int nTickMark)
{
	lua_getglobal(m_L, "kc_tick");										/* L: kc_tick */
	lua_getfield(m_L, -1, "SpecialMark");								/* L: kc_tick, SpecialMark */
	if (!lua_isfunction(m_L, -1))
	{
        lua_pop(m_L, 2);
		return;
	}
	lua_remove(m_L, -2);												/* L: SpecialMark */
	lua_pushnumber(m_L, nTickMark);										/* L: SpecialMark, nTickMark*/
	
	LuaEngine* engine = LuaEngine::getInstance();
	engine->getLuaStack()->executeFunction(1);
}

vector< CPos16 >* CBattleScene::FollowSearch(int nID, int nIgnoreLv, int destID, bool bNearBy)
{
	auto iter = m_mapObjPos.find(nID);
	if (iter == m_mapObjPos.end())
	{
		return NULL;
	}

	auto iterDest = m_mapObjPos.find(destID);
	if (iterDest == m_mapObjPos.end())
	{
		return NULL;
	}

	CObjPos& objPos = iter->second;
	CObjPos& objDest = iterDest->second;

	
	
	int srcX = objPos.center.x;
	int srcY = objPos.center.y;
	int destX = objDest.center.x;
	int destY = objDest.center.y;
	int xRadiu = objPos.xRadiu;
	int yRadiu = objPos.yRadiu;


	IgnoreObjBarrier(nID);


	bool bOk = m_search.Search(nIgnoreLv, srcX, srcY, destX, destY, xRadiu, yRadiu, m_mask);
	if (!bOk && bNearBy)
	{
		bOk = m_search.SearchNearByPoint(nIgnoreLv,xRadiu, yRadiu, m_mask);
	}
	RestoreObjBarrier(nID);
	
	if (bOk)
	{
		return  &m_search.GetPath();
	}
	return NULL;
}

void CBattleScene::AddObjPos(int nObjID, int xIn, int yIn, int wRadiu, int hRadiu, EBarrierType eBarrierType)
{
	int x = ModifyX(xIn);
	int y = ModifyY(yIn);

	if ( x != xIn || y != yIn )
	{
		CCLOG("CBattleScene::AddObjPos not valid pos (%d,%d) ", xIn, yIn );
	}

	auto iter = m_mapObjPos.find(nObjID);
	if (iter == m_mapObjPos.end())
	{
		CObjPos obj = CObjPos(x, y, wRadiu, hRadiu, eBarrierType);
		m_mapObjPos[nObjID] = obj;
		if (obj.bBarrier)
			m_mask.SetMaskObj(GetGridByPix(x), GetGridByPix(y), wRadiu, hRadiu, eBarrierType);
		return;
	}
}

void CBattleScene::UpdateObjBarrier(int nObjID, EBarrierType eBarrierType)
{
	auto iter = m_mapObjPos.find(nObjID);
	if (iter == m_mapObjPos.end())
	{
		return;
	}
	CObjPos& objPos = iter->second;
	if (objPos.eType ==  eBarrierType)
		return;

	if (objPos.bBarrier)
		_RemoveObjBarrer(objPos);

	//注意不要搞乱顺序
	objPos.UpdateBarrierType(eBarrierType);
	if (objPos.bBarrier)
		_MarkObjBarrier(objPos);
}

void CBattleScene::_MarkObjBarrier(CObjPos& objPos)
{
	m_mask.SetMaskObj(GetGridByPix(objPos.center.x), GetGridByPix( objPos.center.y ), objPos.xRadiu, objPos.yRadiu, objPos.eType);
}

void CBattleScene::_RemoveObjBarrer(CObjPos& objPos)
{
	m_mask.RemoveMaskObj(GetGridByPix(objPos.center.x), GetGridByPix(objPos.center.y), objPos.xRadiu, objPos.yRadiu, objPos.eType);
}

bool CBattleScene::GetValidPos(int objID, int destID, int nIgnoreLv, CPos16& out)
{
	auto iter = m_mapObjPos.find(objID);
	if (iter == m_mapObjPos.end())
	{
		return false;
	}

	CObjPos& objPos = iter->second;
	IgnoreObjBarrier(objID);
	int srcX = GetGridByPix( objPos.center.x );
	int srcY = GetGridByPix( objPos.center.y );
	int xRadiu = objPos.xRadiu;
	int yRadiu = objPos.yRadiu;


	int n = 1;
	do 
	{
		for (int x = -n; x <= n; x++ )
		{
			int gridX = srcX + x;
			if ( gridX < 0 )
			{
				continue;
			}
			if (gridX >= GetMaxGridX())
			{
				break;
			}
			for (int y = -n; y <= n; y++)
			{
				int gridY = srcY + y;
				if ( gridY < 0 )
				{
					continue;
				}
				if ( gridY >= GetMaxGridY() )
				{
					break;
				}

				if (!m_mask.IsBarrier(gridX, gridY, objPos.xRadiu, objPos.yRadiu, nIgnoreLv))
				{
					out.x = GetPixByGrid(gridX);
					out.y = GetPixByGrid(gridY);
					RestoreObjBarrier(objID);
					return true;
				}
			}
		}
		n++;

	} while ( n < GetMaxGridX());
	RestoreObjBarrier(objID);
	return false;
}

bool CBattleScene::GetAttackPos(int objID, int destID, int nIgnoreLv, CPos16& out)
{
	auto iter = m_mapObjPos.find(objID);
	if (iter == m_mapObjPos.end())
	{
		return false;
	}

	auto iterDest = m_mapObjPos.find(destID);
	if (iterDest == m_mapObjPos.end())
	{
		return false;
	}

	CObjPos& objPos = iter->second;
	IgnoreObjBarrier(objID);

	CObjPos& destPos = iterDest->second;

	int srcX = GetGridByPix(objPos.center.x);
	int srcY = GetGridByPix(objPos.center.y);

	int destX = GetGridByPix(destPos.center.x);
	int destY = GetGridByPix(destPos.center.y);

	int xRadiu = destPos.xRadiu + objPos.xRadiu;

	/*
	1		 4
	2 target 5
	3		 6
	grid
	*/
	vector< CPos16 > vecAll;

	CPos16 p1;
	int leftX = ModifyX( destPos.center.x - xRadiu - GRIDPIX );
	int leftX_Grid = GetGridByPix(leftX);
	
	int rightX = ModifyX( destPos.center.x + xRadiu + GRIDPIX );
	int rightX_Grid = GetGridByPix(rightX);
	
	int upY = ModifyY(destPos.center.y + objPos.yRadiu * 2 + GRIDPIX);
	int upY_Grid = GetGridByPix(upY);

	int downY = ModifyY(destPos.center.y - objPos.yRadiu * 2 - GRIDPIX);
	int downY_Grid = GetGridByPix(downY);


	//1 号位
	vecAll.push_back(CPos16(leftX_Grid, upY_Grid));
	
	//2 号位
	vecAll.push_back(CPos16(leftX_Grid, destY));

	//3 号位
	vecAll.push_back(CPos16(leftX_Grid, downY_Grid));

	//4 号位
	vecAll.push_back(CPos16(rightX_Grid, upY_Grid));
	
	//5 号位
	vecAll.push_back(CPos16(rightX_Grid, destY));

	//6 号位
	vecAll.push_back(CPos16(rightX_Grid, downY_Grid));

	int nDeltaY = objPos.center.y - destPos.center.y;

	vector< int > vecIdx;

	if (objPos.center.x < destPos.center.x )
	{
		if (nDeltaY > 0)
		{
			if (abs(nDeltaY) < objPos.yRadiu)
			{
				//* 2,1,3,4,5,6
				vecIdx.push_back(2);
				vecIdx.push_back(1);
				vecIdx.push_back(3);
				vecIdx.push_back(4);
				vecIdx.push_back(5);
				vecIdx.push_back(6);
			}
			else
			{
				//* 1,2,3,4,5,6
				vecIdx.push_back(1);
				vecIdx.push_back(2);
				vecIdx.push_back(3);
				vecIdx.push_back(4);
				vecIdx.push_back(5);
				vecIdx.push_back(6);
			}
		}
		else
		{
			if (abs(nDeltaY) < objPos.yRadiu)
			{
				//* 2,3,1,6,5,4
				vecIdx.push_back(2);
				vecIdx.push_back(3);
				vecIdx.push_back(1);
				vecIdx.push_back(6);
				vecIdx.push_back(5);
				vecIdx.push_back(4);
			}
			else
			{
				//* 3,2,1,6,5,4
				vecIdx.push_back(3);
				vecIdx.push_back(2);
				vecIdx.push_back(1);
				vecIdx.push_back(6);
				vecIdx.push_back(5);
				vecIdx.push_back(4);
			}
		}

	}
	else
	{
		if (nDeltaY > 0)
		{
			if (abs(nDeltaY) < objPos.yRadiu)
			{
				//*5,4,6,1,2,3
				vecIdx.push_back(5);
				vecIdx.push_back(4);
				vecIdx.push_back(6);
				vecIdx.push_back(1);
				vecIdx.push_back(2);
				vecIdx.push_back(3);
			}
			else
			{
				//*4,5,6,1,2,3
				vecIdx.push_back(4);
				vecIdx.push_back(5);
				vecIdx.push_back(6);
				vecIdx.push_back(1);
				vecIdx.push_back(2);
				vecIdx.push_back(3);
			}
		}
		else
		{
			if (abs(nDeltaY) < objPos.yRadiu)
			{
				//* 5,6,4,3,2,1
				vecIdx.push_back(5);
				vecIdx.push_back(6);
				vecIdx.push_back(4);
				vecIdx.push_back(3);
				vecIdx.push_back(2);
				vecIdx.push_back(1);
			}
			else
			{
				//* 6,5,4,3,2,1
				vecIdx.push_back(6);
				vecIdx.push_back(5);
				vecIdx.push_back(4);
				vecIdx.push_back(3);
				vecIdx.push_back(2);
				vecIdx.push_back(1);
			}
		}
	}

	if (vecIdx.size() == 0)
	{
		CCLOG("CBattleScene::GetAttackPos vecIdx null" );
		RestoreObjBarrier(objID);
		return false;
	}

	for (uint32 i = 0; i < vecIdx.size(); i++)
	{
		int idx = vecIdx[i] - 1;
		CPos16 grid = vecAll[idx];
		if ( grid.x == srcX && grid.y == srcY )
		{
			break;
		}
		if (!m_mask.IsBarrier(grid.x, grid.y, objPos.xRadiu, objPos.yRadiu,nIgnoreLv))
		{
			out.x = GetPixByGrid(grid.x);
			out.y = GetPixByGrid(grid.y);
			RestoreObjBarrier(objID);
			return true;
		}
	}

	RestoreObjBarrier(objID);
	return false;
}

vector< CPos16 >* CBattleScene::FollowBestPos(int nIgnoreLv, int nID, int destID)
{
	auto iter = m_mapObjPos.find(nID);
	if (iter == m_mapObjPos.end())
	{
		return NULL;
	}

	auto iterDest = m_mapObjPos.find(destID);
	if (iterDest == m_mapObjPos.end())
	{
		return NULL;
	}

	CObjPos& objPos = iter->second;
	CObjPos& objDest = iterDest->second;

	int srcX = objPos.center.x;
	int srcY = objPos.center.y;
	int destX = objDest.center.x;
	int destY = objDest.center.y;
	int xRadiu = objPos.xRadiu;
	int yRadiu = objPos.yRadiu;

	CPos16 bestPos;
	if (GetAttackPos(nID, destID, nIgnoreLv, bestPos))
	{
		IgnoreObjBarrier(nID);
		bool bOk = m_search.Search(nIgnoreLv, srcX, srcY, bestPos.x, bestPos.y, xRadiu, yRadiu, m_mask);
		if (bOk)
		{
			RestoreObjBarrier(nID);
			return  &m_search.GetPath();
		}
	}

	return NULL;
}

void CBattleScene::UpdateObjBarrierBox(int nObjID, int wRadiu, int hRadiu)
{
	map< int, CObjPos >::iterator iter = m_mapObjPos.find(nObjID);
	if (iter != m_mapObjPos.end())
	{
		IgnoreObjBarrier(nObjID);
		CObjPos& objPos = iter->second;
		objPos.xRadiu = wRadiu;
		objPos.yRadiu = hRadiu;
		RestoreObjBarrier(nObjID);
	}
}

uint32 CBattleScene::GetMaxGridX()
{
	return m_mask.m_nMaxGridX;
}

uint32 CBattleScene::GetMaxGridY()
{
	return m_mask.m_nMaxGridY;
}

int CBattleScene::ModifyX(int nPix)
{
	return m_mask.ModifyX(nPix);
}

int CBattleScene::ModifyY(int nPix)
{
	return m_mask.ModifyY(nPix);
}
