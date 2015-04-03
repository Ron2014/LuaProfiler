#include "CRefreshTriggerMgr.h"
#include "GameApp.h"


CRefreshTriggerMgr::CRefreshTriggerMgr(void)
{
}

CRefreshTriggerMgr::~CRefreshTriggerMgr(void)
{
	for ( uint32 i = 0; i < m_vecTrigger.size(); ++i )
	{
		SAFE_DELETE( m_vecTrigger[i] );
	}
}

void CRefreshTriggerMgr::RegisterRefresh( int32 nType, uint32 nFirstOffset, uint32 nInterval )
{
	time_t nNow = CGameApp::Instance()->GetServerTime();
	nNow = nNow + 8 * 3600;		//*手动调整到北京时区
	tm* tmTime = gmtime( &nNow );

	uint32 nOffset = (uint32)( tmTime->tm_wday * 24 * 3600 + tmTime->tm_hour * 3600 + tmTime->tm_min * 60 + tmTime->tm_sec );
	
	m_vecTrigger.push_back( new CRefreshTrigger() );
	CRefreshTrigger* pRefreshTick = m_vecTrigger[ m_vecTrigger.size() - 1 ];
	

	if ( nFirstOffset > nOffset )
	{
		pRefreshTick->Init(nType, nInterval , nFirstOffset - nOffset);
	}
	else
	{
		uint32 nDiff = nOffset - nFirstOffset;
		uint32 nCount = 1;
		for ( ;; ++nCount  )
		{
			uint32 nTemp = nCount * nInterval;
			if ( nTemp > nDiff )
			{
				break;
			}
		}
		pRefreshTick->Init(nType, nInterval, nCount * nInterval - nDiff);
	}
}


void CRefreshTriggerMgr::Clear()
{
	for (uint32 i = 0; i < m_vecTrigger.size(); ++i)
	{
		SAFE_DELETE(m_vecTrigger[i]);
	}
	m_vecTrigger.clear();
}

void CRefreshTriggerMgr::CheckTrigger()
{
	for (uint32 i = 0; i < m_vecTrigger.size(); ++i)
	{
		m_vecTrigger[i]->Check();
	}
}
