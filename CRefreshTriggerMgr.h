#pragma once
#include "CRefreshTrigger.h"

#include <vector>
using namespace std;

class CRefreshTriggerMgr
{
	std::vector< CRefreshTrigger* >	m_vecTrigger;
public:
	CRefreshTriggerMgr(void);
	~CRefreshTriggerMgr(void);

	void RegisterRefresh( int32 nType, uint32 nFirstOffset, uint32 nInterval );
	void Clear();

	void CheckTrigger();
};
