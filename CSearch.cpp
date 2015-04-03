/** @file CSearch.cpp
    @version 1.0
    @date 2009/06/27
    @note 
    @remark 
*/

#include "CSearch.h"
#include <cstdlib>
#include <stdio.h>

CSearch::CSearch( int xWidth , int yHeight  ):
m_nWidth( xWidth ), 
m_nHeight( yHeight ),
m_nID( 0 ),
m_nHeapSize( 0 ),
m_pHeap(NULL),
m_ppMap(NULL),
m_pPoint(NULL)
{
	ReInit(xWidth, yHeight);
}

CSearch::~CSearch()
{
	Release();
}

void CSearch::AddToCloseList( uint32 nID )
{
	SPoint& Point = m_pPoint[nID];
	m_ppMap[ Point.m_nX ][ Point.m_nY ] = INVALID_32BIT - 1;
	
	if ( m_nShortFB > Point.m_nG)
	{
		m_nShortFB = Point.m_nG;
		m_nShortID = nID;
	}
}

void CSearch::AddToOpenList( uint16 x, uint16 y, uint32 nParentID)
{
	m_nID++;

	SPoint& ParentPoint = m_pPoint[nParentID]; 

	uint8 nH = 10;

	if ( x != ParentPoint.m_nX && y != ParentPoint.m_nY )
	{
		nH = 14;
	}
	SPoint Point( x, y, GetG( x, y), nH + ParentPoint.m_nH, nParentID );

	m_pPoint[m_nID] = Point ;

	InsertHeap( m_nID );

	m_ppMap[ x ][ y ] = m_nID;
	
	if (m_nShortFB > Point.m_nG)
	{
		m_nShortFB = Point.m_nG;
		m_nShortID = m_nID;
	}
}

void CSearch::InsertHeap( uint32 nID )
{
	int element = GetFByID( nID );
	int i = ++m_nHeapSize;
	for(; GetFByID( m_pHeap[ i / 2 ] ) > element; i /= 2 )
		m_pHeap[ i ] = m_pHeap[ i / 2 ];
	m_pHeap[ i ] = nID;
}

int CSearch::GetFByID( uint32 nID )
{
	return m_pPoint[nID].m_nG + m_pPoint[nID].m_nH;
}

bool CSearch::Search(int nIgnoreLv, uint16 sX, uint16 sY, uint16 dX, uint16 dY, int xRadiu, int yRadiu, SMask& Mark)
{
	m_nSrcX = GetGridByPix( sX );
	m_nSrcY = GetGridByPix( sY );
	m_nDesX = GetGridByPix( dX );
	m_nDesY = GetGridByPix( dY );
	

	Reset();

	if (m_nSrcX == m_nDesX && m_nSrcY == m_nDesY)
	{
		m_vecPath.push_back(CPos16(dX, dY));
		return true;
	}

	//* 先直线再A*
	if (MiddlePointLine(nIgnoreLv,m_nSrcX, m_nSrcY, m_nDesX, m_nDesY, xRadiu, yRadiu, Mark))
	{
		m_vecPath.push_back(CPos16( dX, dY) );
		return true;
	}


	AddSrcPoint();

	uint32 nID = GetHeapMin();

	do 
	{
		const SPoint& Point = m_pPoint[nID];


		for ( int i = -1; i < 2; i++ )
		{
			for ( int j = -1; j < 2; j++ )
			{

				if ( i == 0 && j == 0 )	//* 自身点
					continue;

				int32 nTempX = (int32)Point.m_nX + i;
				int32 nTempY = (int32)Point.m_nY + j;

				//* 剔除超出地图范围的点
				if ( nTempX < 0 || nTempY < 0 || nTempX >= m_nWidth || nTempY >= m_nHeight )
				{
					continue;
				}

				if ( nTempX == m_nDesX && nTempY == m_nDesY )
				{
					if ( Mark.IsBarrier(nTempX, nTempY, xRadiu, yRadiu, nIgnoreLv))
					{
						return false;
					}
					//* 到达目的地
					AddToOpenList(  (uint16)nTempX , (uint16)nTempY , nID  )	;

					uint32 nPathID = m_pPoint[m_nID].m_nParentID;

					uint16 nX = m_nDesX;
					uint16 nY = m_nDesY;

					m_vecPath.push_back( CPos16( dX,dY ) );
				
					while ( nPathID > 1 )
					{
 						
 						const SPoint* pLinePoint = &m_pPoint[nPathID];
						while (nPathID != 1 && MiddlePointLine(nIgnoreLv,nX, nY, m_pPoint[nPathID].m_nX, m_pPoint[nPathID].m_nY, xRadiu, yRadiu, Mark))
 						{
							pLinePoint = &m_pPoint[nPathID];
 							nPathID = m_pPoint[nPathID].m_nParentID;
							
 						}
 
						m_vecPath.push_back(CPos16((uint16)GetPixByGrid(pLinePoint->m_nX), (uint16)GetPixByGrid(pLinePoint->m_nY)));
 						nX = pLinePoint->m_nX;
 						nY = pLinePoint->m_nY;
 						nPathID = pLinePoint->m_nParentID;
						
					}



					return true;
				}

				uint32 nTempID = m_ppMap[ nTempX ][ nTempY ];
				
				if ( nTempID == INVALID_32BIT ) //* 还没加入到任何列表
				{
					//* 判断障碍点
					if ( /*CheckBarrier( nTempX, nTempY )*/Mark.IsBarrier(nTempX, nTempY, xRadiu, yRadiu,nIgnoreLv))
					{
						m_ppMap[ nTempX ][ nTempY ] = INVALID_32BIT - 1;	//加入关闭列表
						continue;
					}

					
					AddToOpenList(  (uint16)nTempX , (uint16)nTempY, nID  )	;
				}
				else if ( nTempID == INVALID_32BIT - 1)
				{
						continue;	//* 已在关闭列表里
				}
				else
				{
					SPoint& TempPoint = m_pPoint[ nTempID ];
					//* 已在开放列表，比较H值
					uint32 nNewH = Point.m_nH;

					if (  i != 0 && j != 0 )
						nNewH += 14;
					else
						nNewH += 10;

					if ( TempPoint.m_nH > nNewH )
					{
						//* 新的H值
						TempPoint.m_nH = nNewH;
						TempPoint.m_nParentID = nID;

						//* 重新排列二叉堆
						ResetHeap( nTempID );
					}

				}


			}

		}




	} while( ( nID = GetHeapMin() ) != 0 );

	return false;
}

int CSearch::GetG( uint16 x, uint16 y )
{
	return ( abs( m_nDesX - x ) + abs( m_nDesY - y ) ) * 10;
}

uint32 CSearch::GetHeapMin()
{
	int i,Child;
	uint32 MinElement,LastElement;
	if (  m_nHeapSize == 0 )
	{
		//printf( "queue is empty" );
		return m_pHeap[0];
	}

	MinElement = m_pHeap[1];
	LastElement = m_pHeap[ m_nHeapSize-- ];
	for ( i = 1; i * 2 <= m_nHeapSize; i = Child )
	{

		//* 查找最小的孩子
		Child = i * 2;	//* 假设是左子树

		if ( Child != m_nHeapSize	//* 还有Child + 1
			&& GetFByID( m_pHeap[ Child + 1] ) < GetFByID( m_pHeap[ Child ] ) )
			Child++;	//* 取右子树

		if ( GetFByID( LastElement ) > GetFByID( m_pHeap[Child] ) )
			m_pHeap[i] = m_pHeap[Child];
		else
			break;

	}
	m_pHeap[i] = LastElement;

	AddToCloseList( MinElement );

	return MinElement;
}

void CSearch::ResetHeap( uint32 nID )
{
	int32 i = 1;
	for ( ; i < m_nHeapSize; i++ )
	{
		if ( m_pHeap[i] == nID )
			break;
	}

	int32 element = GetFByID( nID );


	if ( GetFByID( m_pHeap[ i / 2 ] ) > GetFByID( m_pHeap[i] ) )
	{
		for(; GetFByID( m_pHeap[ i / 2 ] ) > element; i /= 2 )
			m_pHeap[ i ] = m_pHeap[ i / 2 ];
		m_pHeap[ i ] = nID;
	}
	else
	{
		for( ;  ( 2 * i + 1 ) <= m_nHeapSize ;  )	//* 与子节点比较
		{
			i = 2 * i + 1; //* 先比较左子树
			if ( GetFByID( m_pHeap[i] ) < element )
			{
				m_pHeap[ i / 2 ] = m_pHeap[i];
				continue;
			}
			
			//* 存在右子树
			if ( ( i + 1 ) <= m_nHeapSize )
			{
				++i; 
				if ( GetFByID( m_pHeap[i] ) < element )
				{
					m_pHeap[ i / 2 ] = m_pHeap[i];
					continue;
				}
			}

		}

		m_pHeap[i] = nID;

	}

}


void CSearch::AddSrcPoint()
{
	m_nID++;

	SPoint Point( m_nSrcX, m_nSrcY, GetG( m_nSrcX, m_nSrcY), 0, 1 );

	m_pPoint[m_nID] = Point ;

	InsertHeap( m_nID );

	m_ppMap[m_nSrcX ][ m_nSrcY ] = m_nID;	
}




void CSearch::Reset()
{
	m_nShortID = 0;
	m_nShortFB = 20000000;
	m_nID = 0;
	m_nHeapSize = 0;
	m_vecPath.clear();
	for ( int i = 0 ; i < m_nWidth; i++ )
	{
		for ( int j = 0; j < m_nHeight; j++ )
		{
			m_ppMap[i][j] = INVALID_32BIT;
		}
	}

	SPoint FirstPoint( 0,0,0,0,0 );
	m_pPoint[0] = FirstPoint;
	m_pHeap[0] = 0;

}

bool CSearch::MiddlePointLine(int nIgnoreLv, uint16 x0, uint16 y0, uint16 x1, uint16 y1, int xRadiu, int yRadiu, SMask& Mark)
{
	/*int xRadiu = xRadiuIn + GRIDPIX;
	int yRadiu = yRadiuIn + GRIDPIX;*/
	int a,b,d1,d2,d,x,y;

	a = y0 - y1;
	b = x1 - x0;


	int _x = 1;
	int _y = 1;

	if ( b < 0)
	{
		_x = -1;
	}

	if ( a > 0 )
	{
		_y = -1;
	}

	x = x0; y = y0;


	if ( a == 0 )
	{
		while ( x != x1 )
		{
			x += _x;
			if (Mark.IsBarrier(x, y, xRadiu, yRadiu, nIgnoreLv))
			{
				return false;
			}
		}

	}
	else if ( b == 0 )
	{
		while ( y != y1 )
		{
			y += _y;
			if (Mark.IsBarrier(x, y, xRadiu, yRadiu, nIgnoreLv))
			{
				return false;
			}
		}

	}
	else if(  abs( a ) < abs( b )  )		//* 斜率绝对值小于1，y随着x + 或 - 1， 而决定是否变化
	{

		if ( b > 0 )	//* 一，四 象限
		{


			if ( a < 0 )	//* 一象限
			{
				d = 2 * a + b;

				d1 = 2 * a;				//* d >= 0;
				d2 = 2 * ( a + b );		//* d < 0;

				while ( x != x1 )
				{
					x++;
					if ( d < 0 )
					{
						y++;
						d += d2;
					}
					else
						d += d1;

					if (Mark.IsBarrier(x, y, xRadiu, yRadiu, nIgnoreLv))
					{
						return false;
					}
				}

			}
			else			//* 四象限
			{
				d = 2 * a - b;

				d1 = 2 * a;				//* d >= 0;
				d2 = 2 * ( a - b );		//* d < 0;

				while ( x != x1 )
				{
					x++;
					if ( d < 0 )
					{
						d += d1;
					}
					else
					{
						y--;
						d += d2;
					}
					if (Mark.IsBarrier(x, y, xRadiu, yRadiu,nIgnoreLv))
					{
						return false;
					}
				}

			}
		}
		else			//*	二，三象限	
		{

			if ( a < 0 )	//* 二象限
			{
				d = 2 * -a + b;
				d1 = -2 * a;				//* d >= 0;
				d2 = 2 * ( -a + b );		//* d < 0;

				while ( x != x1 )
				{
					x--;
					if ( d >= 0 )
					{
						y++;
						d += d2;
					}
					else
						d += d1;
					if (Mark.IsBarrier(x, y, xRadiu, yRadiu,nIgnoreLv))
					{
						return false;
					}
				}

			}
			else			//* 三象限
			{
				d = 2 * -a + -b;
				d1 = 2 * -a;				//* d >= 0;
				d2 = 2 * ( -a - b );		//* d < 0;

				while ( x != x1 )
				{
					x--;
					if ( d >= 0 )
					{
						d += d1;
					}
					else
					{
						y--;
						d += d2;
					}
					if (Mark.IsBarrier(x, y, xRadiu, yRadiu,nIgnoreLv))
					{
						return false;
					}
				}

			}


		}


	}
	else				//* 斜率绝对值大于1，x随着y + 或 - 1， 而决定是否变化	
	{

		if ( b > 0 )	//* 一，四 象限
		{

			if ( a < 0 )	//* 一象限
			{
				d = a + 2 * b;

				d1 = 2 * b;				//* d >= 0;
				d2 = 2 * ( a + b );		//* d < 0;

				while ( y != y1 )
				{
					y++;
					if ( d >= 0 )
					{
						x++;
						d += d2;
					}
					else
						d += d1;
					if (Mark.IsBarrier(x, y, xRadiu, yRadiu,nIgnoreLv))
					{
						return false;
					}
				}

			}
			else			//* 四象限
			{
				d = a + 2 * -b;

				d1 = 2 * -b;				//* d >= 0;
				d2 = 2 * ( a - b );		//* d < 0;

				while ( y != y1 )
				{
					y--;
					if ( d >= 0 )
					{
						d += d1;
					}
					else
					{
						x++;
						d += d2;
					}
					if (Mark.IsBarrier(x, y, xRadiu, yRadiu,nIgnoreLv))
					{
						return false;
					}
				}

			}
		}
		else			//*	二，三象限	
		{

			if ( a < 0 )	//* 二象限
			{
				d =   -a + 2 * b;
				d1 = 2 * b;				//* d >= 0;
				d2 = 2 * ( -a + b );		//* d < 0;
				while ( y != y1 )
				{
					y++;
					if ( d < 0 )
					{
						x--;
						d += d2;
					}
					else
						d += d1;
					if (Mark.IsBarrier(x, y, xRadiu, yRadiu,nIgnoreLv))
					{
						return false;
					}
				}

			}
			else			//* 三象限
			{
				d =   -a + -2 * b;
				d1 = 2 * -b;				//* d >= 0;
				d2 = 2 * ( -a - b );		//* d < 0;

				while ( y != y1 )
				{
					y--;
					if ( d < 0 )
					{
						d += d1;
					}
					else
					{
						x--;
						d += d2;
					}
					if (Mark.IsBarrier(x, y, xRadiu, yRadiu,nIgnoreLv))
					{
						return false;
					}
				}

			}


		}

	}

	return true;


}



bool CSearch::SearchNearByPoint(int nIgnoreLv, int  xRadiu, int yRadiu, SMask& Mark)
{
	m_vecPath.clear();
	

	if ( m_nShortID > 0 )
	{
		m_nID = m_nShortID;
		const SPoint* nearByPoint = &m_pPoint[m_nID];
		uint32 nPathID = nearByPoint->m_nParentID;

		uint16 nX = nearByPoint->m_nX;
		uint16 nY = nearByPoint->m_nY;

		m_vecPath.push_back(CPos16((uint16)GetPixByGrid(nX), (uint16)GetPixByGrid(nY) ));

		while (nPathID > 1)
		{

			const SPoint* pLinePoint = &m_pPoint[nPathID];
			while (nPathID != 1 && MiddlePointLine(nIgnoreLv, nX, nY, m_pPoint[nPathID].m_nX, m_pPoint[nPathID].m_nY, xRadiu, yRadiu, Mark))
			{
				pLinePoint = &m_pPoint[nPathID];
				nPathID = m_pPoint[nPathID].m_nParentID;

			}

			m_vecPath.push_back(CPos16((uint16)GetPixByGrid(pLinePoint->m_nX), (uint16)GetPixByGrid(pLinePoint->m_nY) ));
			nX = pLinePoint->m_nX;
			nY = pLinePoint->m_nY;
			nPathID = pLinePoint->m_nParentID;

		}

		return true;
	}

	return false;
}

void CSearch::ReInit(int32 xWidth, int32 yHeight)
{
	//* 如果没有变化 则不需要重新初始化
	if (m_pHeap && m_nWidth == xWidth && m_nHeight == yHeight)
	{
		Reset();
		return;
	}
	Release();
	

	m_nWidth = xWidth;
	m_nHeight = yHeight;

	m_pHeap = new uint32[m_nWidth * m_nHeight];
	m_ppMap = new uint32*[m_nWidth];
	m_pPoint = new SPoint[m_nWidth * m_nHeight];

	for (int i = 0; i < m_nWidth; i++)
	{
		m_ppMap[i] = new uint32[m_nHeight];
	}
	Reset();
}

void CSearch::Release()
{
	SAFE_DELETE_ARRAY(m_pHeap);
	SAFE_DELETE_ARRAY(m_pPoint);

	if (m_ppMap)
	{
		for (int i = 0; i < m_nWidth; i++)
		{
			SAFE_DELETE_ARRAY(m_ppMap[i]);
		}

		SAFE_DELETE_ARRAY(m_ppMap);
	}

}

int GetPixByGrid( int nGrid)
{
	return nGrid * GRIDPIX ;
}

int GetGridByPix(int nPix)
{
	return nPix / GRIDPIX;
}


