#ifndef __TICKUSER_H__
#define __TICKUSER_H__

#include "CTickMgr.h"


template<class ImpClass>
class TMyTick:public CTick 
{
public:
	typedef void (ImpClass::*ONTICKFUNC)(void);
	TMyTick(ImpClass* pImpObj, ONTICKFUNC pFunc):
	m_pImpObj(pImpObj),
	m_pFunc(pFunc)
	{
	}
	
	virtual void	OnTick()//重载父函数
	{
		(m_pImpObj->*m_pFunc)();
	}

protected:
	ImpClass* m_pImpObj;
	ONTICKFUNC m_pFunc;
};

template<class ImpClass>
class TTickUser
{
public:
	typedef void (ImpClass::*ONTICKFUNC)(void);
	TTickUser():m_pTickMgr(NULL)
	{
	}

	virtual ~TTickUser()
	{
		//析构的时候把所有的tick释放掉
		FreeSelfTicks();
	}

	void SetTickMgr( CTickMgr* pMgr) { m_pTickMgr = pMgr; }

protected:
	//管理已经注册ticks
	std::map<string, CTick*> m_ticksMap;//每个名字(函数)的tick只能有一个。
	CTickMgr* m_pTickMgr;

	//注册一个tick。首先判段是否已经有一样名字的tick,有的话且interval一样，表示已存在。
	void RegTick(ONTICKFUNC pFunc, uint32 uInterval, const char * szName)
	{
		CTick* pTick = this->FindTick(szName);
		if(pTick)
		{
			if(pTick->GetInterval() == uInterval)
			{
				return;//完全一样的,不理会,直接返回
			}
			else
			{
				RemoveTick(pTick);//名字一样，时间不同，则删除。
			}
		}
		AddNewTick(pFunc, uInterval, szName);
	}

	void AddNewTick(ONTICKFUNC pFunc, uint32 uInterval, const char * szName)
	{
		if(m_pTickMgr == NULL)
		{
			GenExp("AddNewTick Error. m_pTickMgr is NULL and get it failed!");
		}
		TMyTick<ImpClass> * pTick = _ArkNewCtor( TMyTick<ImpClass>, TMyTick<ImpClass>((ImpClass*)this, pFunc), "ArkCommon/CTickUser" );
		m_pTickMgr->Register(pTick, uInterval, szName);
		std::string strName = szName;
		m_ticksMap.insert(make_pair(strName, pTick));
	}

public:

	void UnRegister(CTick* pTick)
	{
		m_pTickMgr->UnRegister(pTick);
	}

	void UnRegister(const char *szName)
	{
		std::map<string, CTick*>::iterator it = m_ticksMap.find(szName);
		if(it != m_ticksMap.end())
		{
			m_pTickMgr->UnRegister(it->second);
		}
	}

	void RemoveTick(CTick* pTick)
	{
		RemoveTick( pTick->GetTickName() );
	}

	void RemoveTick(const char* szName)
	{
		std::map<string, CTick*>::iterator it = m_ticksMap.find(szName);
		if(it != m_ticksMap.end())
		{
			SAFE_DELETE(it->second)
			m_ticksMap.erase(it);
		}
	}


	//删除map中所有无效的tick
	void RemoveUnRegisterTick()
	{
		for(std::map<string, CTick*>::iterator it = m_ticksMap.begin(); it != m_ticksMap.end();)
		{
			CTick* pTick = it->second;
			if( pTick->IsRegistered() )
			{
				it++;
			}
			else
			{
				m_ticksMap.erase(it++);
				SAFE_DELETE(pTick);
			}
		}
	}

protected:

	CTick* FindTick(std::string szName)
	{
		std::map<string, CTick*>::iterator it = m_ticksMap.find(szName);
		if(it != m_ticksMap.end())
		{
			return it->second;
		}
		else
		{
			return NULL;
		}
	}

	void FreeSelfTicks()
	{
		for(std::map<string, CTick*>::iterator it = m_ticksMap.begin(); it != m_ticksMap.end(); ++it)
		{
			CTick* pTick = it->second;
			SAFE_DELETE( pTick );
		}
		m_ticksMap.clear();
	}
};



#endif