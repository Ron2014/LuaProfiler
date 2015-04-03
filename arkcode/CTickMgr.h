#ifndef __TICKMGR_H__
#define __TICKMGR_H__

#include "ArkMath.h"
#include "CTick.h"
#include "list.h"

using namespace std;

// 5,5,5,5,12
#define TVN_BITS 5					// TVN = time vector node
#define TVR_BITS 12					// TVR = time vector root 都是从linux内核抄过来的
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)

struct tvec
{
	struct list_head vec[TVN_SIZE];
};

struct tvec_root
{
	struct list_head vec[TVR_SIZE];
};

#define INDEX(N) (uint32)((m_uCurrentTime >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)


	class  CTickMgr
		:public CTick
	{
		friend class CTick;
		

	private:
		uint64			m_uCurrentTime;			//当前的时间
		uint32			m_uTickInterval;

		struct 			tvec_root tv1;
		struct 			tvec tv2;
		struct 			tvec tv3;
		struct 			tvec tv4;
		struct 			tvec tv5;

	public:
		

		CTickMgr( uint32 uTickCyc = 33 );					//uTickCyc Tick的周期=多少毫秒Tick一次
		virtual ~CTickMgr();

		uint32		GetInterval()const				{ return m_uTickInterval; }
		void		OnTick();
		void		Update(uint32 nMs);
		void		TickObjStep();

		bool		Register( CTick* pTick, uint32 uInterval  );	//uInterval Tick的周期=多少毫秒Tick一次
		void		UnRegister( CTick* pTick );
	private:
		void		ClearTickList( struct list_head* phead );		// 清除list中的所有tick
		void		InternalRegister( stTickEntry * pTickEntry );	// 将tick根据时间注册到不同的time vector中
		void		TickStep();										// 时间到了，call tick
		uint32		Cascade(struct tvec *tv, int index);			// copy linux内核的，开始遍历所有time vector，搬移tick

		
		
	};


#endif
