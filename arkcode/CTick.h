
#ifndef __TICK_H__
#define __TICK_H__
#include "ArkMath.h"
#include "list.h"
#include <string>


class CTick;
class CTickMgr;

// linux��list����Ҫ����entry������ʵ�ֿ�����list���ɲ�ͬ���͵�Ԫ��
class stTickEntry
{

public:
	stTickEntry(){};
	~stTickEntry(){};

	list_head	TickListHead;		// liuxe list head
	uint64		uNextTickTime;		// �´�ִ��tick��ʱ��
	bool		bIsValid;
	CTick *		pTick;

	static stTickEntry* CreateObj();
	 
	void Init()
	{
		uNextTickTime = 0;
		bIsValid = false;
		pTick = NULL;
		INIT_LIST_HEAD(&TickListHead);
	}

};

class   CTick
{
	friend class CTickMgr;


private:
	uint32			m_uInterval;				//����ʱ�䴦��һ��,��������λ
	CTickMgr *		m_pTickMgr;					//�����ĸ�mgr�ϣ������ʱ��Ϊ��
	stTickEntry*	m_pTickEntry;
	

public:
	CTick();
	virtual ~CTick(void);

	uint32			GetInterval()const		{ return m_uInterval; }			// ������tickÿ���೤ʱ������һ��
	CTickMgr*		GetTickMgr()const		{ return m_pTickMgr; }
	virtual void	OnTick()				{}

	bool			IsRegistered() const;
	

protected:
	void			UnRegister();
	
};


#endif


