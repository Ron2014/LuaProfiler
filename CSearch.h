/** @file CSearch.h
    @version 1.0
	@auto wingpig
    @date 2009/06/27
    @note 一切坐标都是格子坐标
    @remark 


	F = G + H
	G 坐标点到目标点的距离
	H 父节点到当前点的距离

	F
	  (x,y) 	
	H        G

*/
#ifndef __SEARCH_H__
#define __SEARCH_H__

#include "arkcode/ArkMath.h"
#include <vector>
#include <string.h>
using namespace std;

//#define MAX_PIX_X 3072
//#define MAX_PIX_Y 720
//#define GRIDPIX 16
//
//#define	MAX_X  MAX_PIX_X / GRIDPIX
//#define	MAX_Y  MAX_PIX_Y / GRIDPIX

#define GRIDPIX 16
enum EBarrierType
{
	eBarrierType_None = 0,		//不是障碍
	eBarrierType_Low = 1 << 0,	//低
	eBarrierType_High = 1 << 1, //高
};

int GetPixByGrid( int nGrid);
int GetGridByPix(int nPix);


struct SMask
{
	

	uint32	m_nWidthPix;
	uint32	m_nHeightPix;

	uint32	m_nMaxGridX;
	uint32	m_nMaxGridY;


	uint8*	m_pMask;		//* 低障碍
	uint8*	m_pMaskHigh;	//* 高障碍

	//uint8	dwMask[MAX_Y][MAX_X];
	uint8	maskcmp[512];

	int  Width()	{ return m_nMaxGridX; }
	int  Depth()	{ return m_nMaxGridY; }

	SMask()
	{
		m_pMask = NULL;
		m_pMaskHigh = NULL;
		Init(3072, 960);

		memset(maskcmp, 0, sizeof(maskcmp));
	}

	~SMask()
	{
		SAFE_DELETE_ARRAY( m_pMask );
		SAFE_DELETE_ARRAY( m_pMaskHigh );
	}

	int ModifyX(int nPix)
	{
		if (nPix < 0)
		{
			return 0;
		}
		if (nPix >= m_nWidthPix)
		{
			return m_nWidthPix - 1;
		}

		return nPix;
	}

	int ModifyY(int nPix)
	{
		if (nPix < 0)
		{
			return 0;
		}
		if (nPix >= m_nHeightPix)
		{
			return m_nHeightPix - 1;
		}

		return nPix;
	}

	EBarrierType GetBarrier(int gridX, int gridY)
	{
		return (EBarrierType)( m_pMask[gridY * m_nMaxGridX + gridX] );
	}

	bool _IsBarrier(int gridX, int gridY, int nIgnoreLv)
	{
		return GetBarrier(gridX, gridY) > nIgnoreLv;
	}

	void _GetStartAndEndGrid(int gridX, int gridY, int xRadiu, int yRadiu, CPos& startGrid, CPos& endGrid)
	{
		int pX = GetPixByGrid(gridX);
		int pY = GetPixByGrid(gridY);

		//先判断是否超出地图范围
		int left = pX - xRadiu;
		int right = pX + xRadiu;
		int top = pY + yRadiu;
		int bottom = pY - yRadiu;


		if (left < 0)
		{
			left = 0;
		}

		if (right >= m_nWidthPix)
		{
			right = m_nWidthPix - 1;
		}

		if (bottom < 0)
		{
			bottom = 0;
		}

		if (top >= m_nHeightPix)
		{
			top = m_nHeightPix - 1;
		}

		startGrid = GetGrid(left, bottom);
		endGrid = GetGrid(right, top);
	}



	bool IsBarrier(int gridX, int gridY, int xRadiu, int yRadiu, int nIgnoreLv, bool bInMove = false)
	{
		uint8* pTestMask = m_pMask;

		//* 忽视低障碍
		if (nIgnoreLv == 1)
		{
			pTestMask = m_pMaskHigh;
		}

		if (gridX < 0 || gridX >= m_nMaxGridX || gridY < 0 || gridY >= m_nMaxGridY)
			return true;
		
		if (xRadiu == 0 || yRadiu == 0)
		{
			return _IsBarrier(gridX, gridY, nIgnoreLv);
		}

		CPos startGrid;
		CPos endGrid;
		_GetStartAndEndGrid(gridX, gridY, xRadiu, yRadiu, startGrid, endGrid );
		int width = endGrid.x - startGrid.x + 1;
		if (bInMove)
		{
			for (int i = startGrid.y; i <= endGrid.y; i++)
			{
				if (i == startGrid.y || i == endGrid.y)
				{
					if (width >= 3 )
					{
						if (memcmp(&pTestMask[i * m_nMaxGridX + startGrid.x + 1], &maskcmp[0], width - 2) != 0)
							return true;
					}
					else
					{
						continue;
					}
				}
				else
				{
					if (memcmp(&pTestMask[i * m_nMaxGridX + startGrid.x], &maskcmp[0], width) != 0)
						return true;

				}
			}
		}
		else
		{
			for (int i = startGrid.y; i <= endGrid.y; i++)
			{
				if (memcmp(&pTestMask[i * m_nMaxGridX + startGrid.x], &maskcmp[0], width) != 0)
					return true;
			}
		}
		return false;


	};

	//* true 是障碍
	bool CheckBarrierIgnoreLv(int nBegin, int nWidth, int nIgnoreLv)
	{
		uint8* pTestMask = m_pMask;

		//* 忽视低障碍
		if (nIgnoreLv == 1)
		{
			pTestMask = m_pMaskHigh;
		}

		for (int i = 0; i < nWidth; i++ )
		{
			if (pTestMask[nBegin + i] > 0)
				return true;
		}
		return false;
	}


	void Init( uint32 nWidth, uint32 nHeight )
	{
		if (m_pMask)
		{
			if (nWidth == m_nWidthPix && nHeight == m_nHeightPix)
			{
				//*如果场景一样大小，只需初始化障碍
				memset(m_pMask, 0, m_nMaxGridX * m_nMaxGridY);
				memset(m_pMaskHigh, 0, m_nMaxGridX * m_nMaxGridY);
				return;
			}

			delete []m_pMask;
			m_pMask = NULL;

			delete[]m_pMaskHigh;
			m_pMaskHigh = NULL;
		}

		m_nWidthPix = nWidth;
		m_nHeightPix = nHeight;

		m_nMaxGridX = m_nWidthPix / GRIDPIX;
		m_nMaxGridY = m_nHeightPix / GRIDPIX;


		m_pMask = new uint8[m_nMaxGridX * m_nMaxGridY];
		memset(m_pMask, 0, m_nMaxGridX * m_nMaxGridY);

		m_pMaskHigh = new uint8[m_nMaxGridX * m_nMaxGridY];
		memset(m_pMaskHigh, 0, m_nMaxGridX * m_nMaxGridY);
	}


	inline void SetMaskGrid(int gridX, int gridY, EBarrierType eType, int nValue )
	{
		if (eType == eBarrierType_Low)
		{
			int nNew = m_pMask[gridY * m_nMaxGridX + gridX] + nValue;

			if (nNew < 0)
			{
				nNew = 0;
			}
			m_pMask[gridY * m_nMaxGridX + gridX] = nNew;
		}
		else if (eType == eBarrierType_High)
		{
			int nNew = m_pMask[gridY * m_nMaxGridX + gridX] + nValue;

			if (nNew < 0)
			{
				nNew = 0;
			}
			m_pMask[gridY * m_nMaxGridX + gridX] = nNew;

			int nNewHigh = m_pMaskHigh[gridY * m_nMaxGridX + gridX] + nValue;

			if (nNewHigh < 0)
			{
				nNewHigh = 0;
			}
			m_pMaskHigh[gridY * m_nMaxGridX + gridX] = nNewHigh;
		}
	}
	
	

	//* int xRadiu, int yRadiu参数是pix
	void SetMaskObj(int gridX, int gridY, int xRadiu, int yRadiu, EBarrierType eType)
	{
		_UpdateMask(gridX, gridY, xRadiu, yRadiu, eType, 1);
	}

	//* int xRadiu, int yRadiu参数是pix
	void RemoveMaskObj(int gridX, int gridY, int xRadiu, int yRadiu, EBarrierType eType)
	{
		_UpdateMask(gridX, gridY, xRadiu, yRadiu, eType, -1);
	}

	void _UpdateMask(int gridX, int gridY, int xRadiu, int yRadiu, EBarrierType eType, int nValue )
	{
		CPos startGrid;
		CPos endGrid;
		_GetStartAndEndGrid(gridX, gridY, xRadiu, yRadiu, startGrid, endGrid);

		for (int tempY = startGrid.y; tempY <= endGrid.y; tempY++)
		{
			if (tempY < 0 || tempY >= m_nMaxGridY)
				continue;
			for (int tempX = startGrid.x; tempX <= endGrid.x; tempX++)
			{
				if (tempX < 0 || tempX >= m_nMaxGridX)
					continue;

				SetMaskGrid(tempX, tempY, eType, nValue);

			}
		}
	}

	inline CPos GetGrid(int x, int y)
	{
		int gridX = x / GRIDPIX;
		int gridY = y / GRIDPIX;
		return CPos(gridX, gridY);
	}
	



};

/*

struct SMask
{


uint32	m_nWidth;
uint32	m_nHeight;
uint8*	m_pMask;
uint8	dwMask[MAX_Y][MAX_X];
uint8	maskcmp[512];

int  Width()	{ return MAX_X; }
int  Depth()	{ return MAX_Y; }

void _GetStartAndEndGrid(int gridX, int gridY, int xRadiu, int yRadiu, CPos& startGrid, CPos& endGrid)
{
int pX = GetPixByGrid(gridX);
int pY = GetPixByGrid(gridY);

//先判断是否超出地图范围
int left = pX - xRadiu;
int right = pX + xRadiu;
int top = pY + yRadiu;
int bottom = pY - yRadiu;


if (left < 0)
{
left = 0;
}

if (right >= MAX_PIX_X)
{
right = MAX_PIX_X - 1;
}

if (bottom < 0)
{
bottom = 0;
}

if (top >= MAX_PIX_Y)
{
top = MAX_PIX_Y - 1;
}

startGrid = GetGrid(left, bottom);
endGrid = GetGrid(right, top);
}

bool IsBarrier( int gridX, int gridY, int xRadiu, int yRadiu, bool bInMove = false )
{
if (gridX < 0 || gridX >= MAX_X || gridY < 0 || gridY >= MAX_Y)
return true;

if (xRadiu == 0 || yRadiu == 0)
{
return dwMask[gridY][gridX] > 0;
}

CPos startGrid;
CPos endGrid;
_GetStartAndEndGrid(gridX, gridY, xRadiu, yRadiu, startGrid, endGrid );
int width = endGrid.x - startGrid.x + 1;
if (bInMove)
{
for (int i = startGrid.y; i <= endGrid.y; i++)
{
if (i == startGrid.y || i == endGrid.y)
{
if (width >= 3 )
{
if (memcmp(&dwMask[i][startGrid.x + 1], &maskcmp[0], width - 2) != 0)
return true;
}
else
{
continue;
}
}
else
{
if (memcmp(&dwMask[i][startGrid.x], &maskcmp[0], width) != 0)
return true;
}
}
}
else
{
for (int i = startGrid.y; i <= endGrid.y; i++)
{
if (memcmp(&dwMask[i][startGrid.x], &maskcmp[0], width) != 0)
return true;
}
}
return false;


};



void Init( uint32 nWidth, uint32 nHeight )
{
m_nWidth = nWidth;
m_nHeight = nHeight;
uint8* m_pMask = new uint8[ m_nWidth * m_nHeight ];
memset(dwMask, 0, MAX_Y * MAX_X);


}

inline void SetMaskGrid( int gridX, int gridY, uint8 nValue)
{
//maskPix[y][x] = nValue;

dwMask[gridY][gridX] = nValue;
}

inline void UpdateMaskGrid(int gridX, int gridY, int8 nValue)
{
//maskPix[y][x] = nValue;
int ret = dwMask[gridY][gridX] + nValue;
if ( ret < 0 )
{
ret = 0;
}
dwMask[gridY][gridX] = (uint8)ret;

}
//* int xRadiu, int yRadiu参数是pix
void SetMaskObj(int gridX, int gridY, int xRadiu, int yRadiu)
{
_UpdateMask(gridX, gridY, xRadiu, yRadiu, 1);
}

//* int xRadiu, int yRadiu参数是pix
void RemoveMaskObj(int gridX, int gridY, int xRadiu, int yRadiu)
{
_UpdateMask(gridX, gridY, xRadiu, yRadiu, -1);
}

void _UpdateMask(int gridX, int gridY, int xRadiu, int yRadiu, int nValue)
{
CPos startGrid;
CPos endGrid;
_GetStartAndEndGrid(gridX, gridY, xRadiu, yRadiu, startGrid, endGrid);

for (int tempY = startGrid.y; tempY <= endGrid.y; tempY++)
{
if (tempY < 0 || tempY >= MAX_Y)
continue;
for (int tempX = startGrid.x; tempX <= endGrid.x; tempX++)
{
if (tempX < 0 || tempX >= MAX_X)
continue;

UpdateMaskGrid(tempX, tempY, nValue);

}
}
}

inline CPos GetGrid(int x, int y)
{
int gridX = x / GRIDPIX;
int gridY = y / GRIDPIX;
return CPos(gridX, gridY);
}


SMask()
{
m_pMask = NULL;
memset(maskcmp, 0, sizeof(maskcmp));
}

};
*/
class CSearch
{
	struct  SPoint
	{

		SPoint(){};
		SPoint( uint16 x, uint16 y, uint32 G , uint32 H ,int32 nParentID):m_nX( x ), m_nY( y ), m_nH( H ), m_nG(  G ),m_nParentID( nParentID )
		{
		}
		uint16	m_nX;
		uint16	m_nY;
		uint32	m_nH;
		uint32	m_nG;
		uint32	m_nParentID;		
	};

	
	SPoint* m_pPoint;
	uint32* m_pHeap;
	int32 m_nWidth;
	int32 m_nHeight;
	uint32 m_nID;
	int m_nHeapSize;

	uint32**	m_ppMap;	//* INVALID_32BIT - 没有加入任何列表， INVALID_32BIT - 1 关闭列表

	vector< CPos16 > m_vecPath;
	

	int m_nShortID;
	int m_nShortFB;

	uint16 m_nSrcX;
	uint16 m_nSrcY;
	uint16 m_nDesX;
	uint16 m_nDesY;

public:
	CSearch(int32 xWidth = 3072 / GRIDPIX, int32 yHeight = 960 / GRIDPIX);
    ~CSearch();
	
	void ReInit(int32 xWidth, int32 yHeight );

protected:
	//* 检测判断是否是障碍点。
	bool CheckBarrier( int x, int y);
	void AddToCloseList( uint32 nID );
	void AddToOpenList( uint16 x, uint16 y,uint32 nParentID );
	void AddSrcPoint();

	void InsertHeap( uint32 nID );
	uint32 GetHeapMin();
	void ResetHeap( uint32 nID );

	int GetFByID( uint32 nID );
	
	int GetG( uint16 x, uint16 y );

	void Release();
public:
	bool Search(int nIgnoreLv,uint16 sX, uint16 sY, uint16 dX, uint16 dY, int xRadiu, int yRadiu, SMask& Mark);
	bool SearchNearByPoint(int nIgnoreLv, int  xRadiu, int yRadiu, SMask& Mark);	//* 获取最近点的路径,必须是先调用Search后

	bool MiddlePointLine(int nIgnoreLv, uint16 sX, uint16 sY, uint16 dX, uint16 dY, int xRadiu, int yRadiu, SMask& Mark);




	vector< CPos16 >& GetPath() { return m_vecPath; } ;

	

	//* 归位
	void Reset();

	//*test
	void DrawPoint( SMask& mask );
};

#endif