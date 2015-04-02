#pragma once
#include "CTick.h"

class CRefreshTrigger
{
	bool	m_bFirst;
	int32	m_nType;
	uint32	m_uCyc;
	uint32	m_nPreTime;		// �ϴδ�����ʱ��
	uint32	m_nNextTime;		// �´δ�����ʱ��
public:
	CRefreshTrigger(void);
	~CRefreshTrigger(void);

	void	Init(int32 nType, uint32 nInterval, uint32 nFirstTime);

	bool	Check();

	void	OnTick();
};
