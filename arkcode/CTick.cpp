
#include "CTick.h"
#include "CTickMgr.h"


stTickEntry* stTickEntry::CreateObj()
{
	stTickEntry* pTickEntry = new stTickEntry();
	pTickEntry->Init();
	return pTickEntry;
}

	CTick::CTick()
		:m_uInterval(0)
		,m_pTickMgr(NULL)
		,m_pTickEntry(NULL)
	{
		
	}

	CTick::~CTick(void)
	{
		UnRegister();
		
	}


	void CTick::UnRegister()
	{
		if( m_pTickEntry != NULL )
		{
			m_pTickMgr->UnRegister(this);
			m_pTickEntry = NULL;
		}
	}

	bool CTick::IsRegistered() const
	{
		if( m_pTickEntry )
			return m_pTickEntry->bIsValid;
		return false;
	}




