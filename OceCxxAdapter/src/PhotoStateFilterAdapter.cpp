#include "PhotoStateFilterAdapter.h"

namespace xce {
namespace photostatefilter {
namespace adapter {

MyUtil::ByteSeq PhotoStateFilterAdapter::getPhotoStateMinMax(int minId,int maxId) {
	return getProxy(0)->getPhotoStateMinMax(minId, maxId);
}

xce::photostatefilter::Int2BoolMap PhotoStateFilterAdapter::getPhotoStateBatch(const MyUtil::IntSeq& ids){
	return getProxy(0)->getPhotoStateBatch(ids);
}

MyUtil::IntSeq PhotoStateFilterAdapter::getPhotoIdsFrom(const MyUtil::IntSeq& ids){
	return getProxy(0)->getPhotoIdsFrom(ids);
}

void PhotoStateFilterAdapter::getAllPhotoState(std::bitset<PHOTOSTATE_FILTER_SIZE>& s) {
	MyUtil::ByteSeq result;
	for (int i=0;i<210;i++) {
		MyUtil::ByteSeq temp = getProxy(0)->getPhotoStateMinMax(i*10000000, (i+1)*10000000);
		result.insert(result.end(), temp.begin(), temp.end());
	}
	MyUtil::ByteSeq temp=getProxy(0)->getPhotoStateMinMax(2100000000, 2147483647);
	result.insert(result.end(), temp.begin(), temp.end());
	for (int i=0;i<PHOTOSTATE_FILTER_SIZE;++i) {
		if (result[i/8]&(1<<(i%8))) {
			s.set(i);
		}
	}
}

bool PhotoStateFilterAdapter::isPhotoStateTrue(int userId) {
	return getProxy(0)->isPhotoStateTrue(userId);
}

void PhotoStateFilterAdapter::updatePhotoState(int userId,const std::string& photo) {
	return getProxy(0)->updatePhotoState(userId, photo);
}

void PhotoStateFilterAdapter::updatePhotoStateBatch(const MyUtil::Int2StrMap& photomap) {
	return getProxy(0)->updatePhotoStateBatch(photomap);
}

/*bool PhotoStateFilterAdapter::isValid() {
	return getProxy(0)->isValid();
}*/

}
}
}
