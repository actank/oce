#include "OnlineStateFilterAdapter.h"

namespace xce {
namespace onlinestatefilter {
namespace adapter {

MyUtil::ByteSeq OnlineStateFilterAdapter::getOnlineStateMinMax(int minId,int maxId) {
	return getProxy(0)->getOnlineStateMinMax(minId, maxId);
}

xce::onlinestatefilter::Int2BoolMap OnlineStateFilterAdapter::getOnlineStateBatch(const MyUtil::IntSeq& ids){
	return getProxy(0)->getOnlineStateBatch(ids);
}

MyUtil::IntSeq OnlineStateFilterAdapter::getOnlineIdsFrom(const MyUtil::IntSeq& ids){
	return getProxy(0)->getOnlineIdsFrom(ids);
}

void OnlineStateFilterAdapter::getAllOnlineState(std::bitset<ONLINESTATE_FILTER_SIZE>& s) {
	MyUtil::ByteSeq result;
	for (int i=0;i<210;i++) {
		MyUtil::ByteSeq temp = getProxy(0)->getOnlineStateMinMax(i*10000000, (i+1)*10000000);
		result.insert(result.end(), temp.begin(), temp.end());
	}
	MyUtil::ByteSeq temp=getProxy(0)->getOnlineStateMinMax(2100000000, 2147483647);
	result.insert(result.end(), temp.begin(), temp.end());
	for (int i=0;i<ONLINESTATE_FILTER_SIZE;++i) {
		if (result[i/8]&(1<<(i%8))) {
			s.set(i);
		}
	}
}

bool OnlineStateFilterAdapter::isOnlineStateTrue(int userId) {
	return getProxy(0)->isOnlineStateTrue(userId);
}

void OnlineStateFilterAdapter::updateOnlineState(int userId,int onlinestate) {
	return getProxy(0)->updateOnlineState(userId, onlinestate);
}

void OnlineStateFilterAdapter::updateOnlineStateBatch(const MyUtil::Int2IntMap& data) {
	return getProxy(0)->updateOnlineStateBatch(data);
}

/*bool OnlineStateFilterAdapter::isValid() {
	return getProxy(0)->isValid();
}*/

}
}
}
