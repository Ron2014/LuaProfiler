
#ifndef __TICK_H__
#define __TICK_H__
#include "ArkMath.h"
#include "list.h"
#include <string>


class CTick;
class CTickMgr;

// linux的list必须要做的entry，用以实现可以让list容纳不同类型的元素
class stTickEntry
{

public:
	stTickEntry(){};
	~stTickEntry(){};

	list_head	TickListHead;		// liuxe list head
	uint64		uNextTickTime;		// 下次执行tick的时间
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
	uint32			m_uInterval;				//多少时间处理一次,毫秒做单位
	CTickMgr *		m_pTickMgr;					//挂在哪个mgr上，构造的时候为空
	stTickEntry*	m_pTickEntry;
	

public:
	CTick();
	virtual ~CTick(void);

	uint32			GetInterval()const		{ return m_uInterval; }			// 获得这个tick每隔多长时间运行一次
	CTickMgr*		GetTickMgr()const		{ return m_pTickMgr; }
	virtual void	OnTick()				{}

	bool			IsRegistered() const;
	

protected:
	void			UnRegister();
	
};


#endif


