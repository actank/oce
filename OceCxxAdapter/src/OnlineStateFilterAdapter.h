#ifndef ONLINESTATEFILTER_H
#define ONLINESTATEFILTER_H

#include <OnlineStateFilter.h>
#include <AdapterI.h>
#include <bitset>

namespace xce {
namespace onlinestatefilter {
namespace adapter {

const int ONLINESTATE_FILTER_SIZE = 2147483647;

class OnlineStateFilterAdapter :
	public MyUtil::ReplicatedClusterAdapterI<OnlineStateFilterManagerPrx>,
		public MyUtil::Singleton<OnlineStateFilterAdapter> {
public:
	OnlineStateFilterAdapter() : MyUtil::ReplicatedClusterAdapterI <OnlineStateFilterManagerPrx>("ControllerOnlineStateFilter", 120, 3000) {}
	MyUtil::ByteSeq getOnlineStateMinMax(int minId,int maxId);
        xce::onlinestatefilter::Int2BoolMap getOnlineStateBatch(const MyUtil::IntSeq& ids);
        MyUtil::IntSeq getOnlineIdsFrom(const MyUtil::IntSeq& ids);
        void getAllOnlineState(std::bitset<ONLINESTATE_FILTER_SIZE>& s);
        bool isOnlineStateTrue(int userId);
        void updateOnlineState(int userId,int onlinestate);
        void updateOnlineStateBatch(const MyUtil::Int2IntMap& data);
};

}
}
}

#endif
