#include "StatusCacheAdapter.h"

namespace xce {
namespace statuscache {

MyUtil::ByteSeq StatusCacheAdapter::getStatusMinMax(int minId,int maxId) {
	return getProxy(0)->getStatusMinMax(minId, maxId);
}

xce::statuscache::Int2BoolMap StatusCacheAdapter::getStatusBatch(const MyUtil::IntSeq& ids){
	return getProxy(0)->getStatusBatch(ids);
}

MyUtil::IntSeq StatusCacheAdapter::getStatusIdsFrom(const MyUtil::IntSeq& ids){
	return getProxy(0)->getStatusIdsFrom(ids);
}

void StatusCacheAdapter::getAllStatus(std::bitset<STATUS_CACHE_SIZE>& s) {
	MyUtil::ByteSeq result;
	for (int i=0;i<210;i++) {
		MyUtil::ByteSeq temp = getProxy(0)->getStatusMinMax(i*10000000, (i+1)*10000000);
		result.insert(result.end(), temp.begin(), temp.end());
	}
	MyUtil::ByteSeq temp=getProxy(0)->getStatusMinMax(2100000000, 2147483647);
	result.insert(result.end(), temp.begin(), temp.end());
	for (int i=0;i<STATUS_CACHE_SIZE;++i) {
		if (result[i/8]&(1<<(i%8))) {
			s.set(i);
		}
	}
}

bool StatusCacheAdapter::isStatusTrue(int userId) {
	return getProxy(0)->isStatusTrue(userId);
}

bool StatusCacheAdapter::isGoodUser(int id) {
	return getProxy(0)->isGoodUser(id);
}

void StatusCacheAdapter::updateStatus(int userId,int status) {
	return getProxy(0)->updateStatus(userId, status);
}

void StatusCacheAdapter::updateStatusBatch(const MyUtil::Int2IntMap& data) {
	return getProxy(0)->updateStatusBatch(data);
}

bool StatusCacheAdapter::isValid() {
	return getProxy(0)->isValid();
}

}
}
