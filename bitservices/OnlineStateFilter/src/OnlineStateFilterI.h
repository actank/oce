#ifndef __ONLINESTATE_FILTERI_H__
#define __ONLINESTATE_FILTERI_H__

#include <OnlineStateFilter.h>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <bitset>
#include <Singleton.h>
#include <AtomicSTL.h>
#include <ServiceI.h>

namespace xce {
namespace onlinestatefilter {

const int ONLINESTATE_FILTER_SIZE = 2147483647;

class OnlineStateFilterManagerI: virtual public OnlineStateFilterManager,
        virtual public MyUtil::Singleton<OnlineStateFilterManagerI>/*, virtual public IceUtil::Thread */{
public:
	OnlineStateFilterManagerI();
        void initialize();

	virtual MyUtil::ByteSeq getOnlineStateMinMax(::Ice::Int,::Ice::Int,
			const Ice::Current&);
	virtual Int2BoolMap getOnlineStateBatch(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual MyUtil::IntSeq getOnlineIdsFrom(const MyUtil::IntSeq&,
			const Ice::Current&);
	virtual bool isOnlineStateTrue(::Ice::Int,
			const Ice::Current&);
	virtual void updateOnlineState(::Ice::Int,::Ice::Int,
			const Ice::Current&);
        virtual void updateOnlineStateBatch(const ::MyUtil::Int2IntMap&,
			const Ice::Current&);
private:

	std::bitset<ONLINESTATE_FILTER_SIZE> _online_state;

//	virtual void run();
//	xce::atomicstl::BlockingQueue<std::pair<int, int> > queue_;
};

}
}

#endif
