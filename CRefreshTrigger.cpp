
#include "CRefreshTrigger.h"
#include "GameApp.h"

CRefreshTrigger::CRefreshTrigger(void) :
m_bFirst(true),
m_nType(0),
m_uCyc(0xffffffff),
m_nPreTime(0),
m_nNextTime(0)
{
}

CRefreshTrigger::~CRefreshTrigger(void)
{
}

void CRefreshTrigger::OnTick()
{
	m_nPreTime = m_nNextTime;
	m_nNextTime = CGameApp::Instance()->GetServerTime() + m_uCyc;
	CGameApp::Instance()->OnRefreshTriger(m_nType);
}

void CRefreshTrigger::Init(int32 nType, uint32 nInterval, uint32 nFirstTime )
{
	m_nType = nType;
	m_uCyc = nInterval;
	m_nNextTime = CGameApp::Instance()->GetServerTime() + nFirstTime;
}

bool CRefreshTrigger::Check()
{
	uint32 nServerTime = CGameApp::Instance()->GetServerTime();
	if (nServerTime > m_nNextTime)
	{
		OnTick();
		return true;
     }

	return false;
}
