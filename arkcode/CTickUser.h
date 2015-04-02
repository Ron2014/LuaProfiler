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
	
	virtual void	OnTick()//���ظ�����
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
		//������ʱ������е�tick�ͷŵ�
		FreeSelfTicks();
	}

	void SetTickMgr( CTickMgr* pMgr) { m_pTickMgr = pMgr; }

protected:
	//�����Ѿ�ע��ticks
	std::map<string, CTick*> m_ticksMap;//ÿ������(����)��tickֻ����һ����
	CTickMgr* m_pTickMgr;

	//ע��һ��tick�������ж��Ƿ��Ѿ���һ�����ֵ�tick,�еĻ���intervalһ������ʾ�Ѵ��ڡ�
	void RegTick(ONTICKFUNC pFunc, uint32 uInterval, const char * szName)
	{
		CTick* pTick = this->FindTick(szName);
		if(pTick)
		{
			if(pTick->GetInterval() == uInterval)
			{
				return;//��ȫһ����,�����,ֱ�ӷ���
			}
			else
			{
				RemoveTick(pTick);//����һ����ʱ�䲻ͬ����ɾ����
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


	//ɾ��map��������Ч��tick
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