#ifndef __OnlineEvictor_h__
#define __OnlineEvictor_h__

#include "EvictorTemplate.h"
#include "UserOnline.h"
#include <Ice/Ice.h>
#include "IceLogger.h"
#include <IceUtil/AbstractMutex.h>

namespace mop
{
namespace hi
{
namespace svc
{
namespace online
{
    
using namespace MyUtil;
using namespace mop::hi::svc::online;

const int OFF_LINE=0;
const int OT_IM=1;
const int OT_WEB=2;
const int OT_ALL=3;

class OnlineEvictor : 
	public EvictorTemplate<Ice::Int, OnlineUserPtr>,
	virtual public IceUtil::AbstractMutexI<IceUtil::Mutex>
{
	public:
		OnlineEvictor(int expStep);
		
		int size(::Ice::Int);	
			
		void setStat(::Ice::Int, ::Ice::Int, bool);		
		int getStat(::Ice::Int);
			
    	IntSeq getStats(::Ice::Int, ::Ice::Int, ::Ice::Int);	
    	Int2IntMap checkStats(const MyUtil::IntSeq& userIds);
    	
		void timedEvictEntries();		
		void swapOnlineChange(OnlineUserMap& tmp);
private:
	OnlineUserMap _onlineChange;
	IceUtil::Mutex _onlineChangeMutex;

	time_t _expStep;
};

typedef IceUtil::Handle<OnlineEvictor> OnlineEvictorPtr;


};
};
};
};

#endif
