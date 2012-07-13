#include "UserStateFilterAdapter.h"

namespace xce {
namespace userstatefilter {
namespace adapter {

MyUtil::ByteSeq UserStateFilterAdapter::getStateStarMinMax(int minId,int maxId) {
	return getProxy(0)->getStateStarMinMax(minId, maxId);
}

xce::userstatefilter::Int2BoolMap UserStateFilterAdapter::getStateStarBatch(const MyUtil::IntSeq& ids){
	return getProxy(0)->getStateStarBatch(ids);
}

MyUtil::IntSeq UserStateFilterAdapter::getStarIdsFrom(const MyUtil::IntSeq& ids){
	return getProxy(0)->getStarIdsFrom(ids);
}

void UserStateFilterAdapter::getAllStateStar(std::bitset<USERSTATE_FILTER_SIZE>& s) {
	MyUtil::ByteSeq result;
	for (int i=0;i<210;i++) {
		MyUtil::ByteSeq temp = getProxy(0)->getStateStarMinMax(i*10000000, (i+1)*10000000);
		result.insert(result.end(), temp.begin(), temp.end());
	}
	MyUtil::ByteSeq temp=getProxy(0)->getStateStarMinMax(2100000000, 2147483647);
	result.insert(result.end(), temp.begin(), temp.end());
	for (int i=0;i<USERSTATE_FILTER_SIZE;++i) {
		if (result[i/8]&(1<<(i%8))) {
			s.set(i);
		}
	}
}

bool UserStateFilterAdapter::isStateStarTrue(int userId) {
	return getProxy(0)->isStateStarTrue(userId);
}

MyUtil::ByteSeq UserStateFilterAdapter::getStateGuideMinMax(int minId,int maxId) {
	return getProxy(0)->getStateGuideMinMax(minId, maxId);
}

xce::userstatefilter::Int2BoolMap UserStateFilterAdapter::getStateGuideBatch(const MyUtil::IntSeq& ids){
	return getProxy(0)->getStateGuideBatch(ids);
}

MyUtil::IntSeq UserStateFilterAdapter::getGuideIdsFrom(const MyUtil::IntSeq& ids){
	return getProxy(0)->getGuideIdsFrom(ids);
}

void UserStateFilterAdapter::getAllStateGuide(std::bitset<USERSTATE_FILTER_SIZE>& s) {
	MyUtil::ByteSeq result;
	for (int i=0;i<210;i++) {
		MyUtil::ByteSeq temp = getProxy(0)->getStateGuideMinMax(i*10000000, (i+1)*10000000);
		result.insert(result.end(), temp.begin(), temp.end());
	}
	MyUtil::ByteSeq temp=getProxy(0)->getStateGuideMinMax(2100000000, 2147483647);
	result.insert(result.end(), temp.begin(), temp.end());
	for (int i=0;i<USERSTATE_FILTER_SIZE;++i) {
		if (result[i/8]&(1<<(i%8))) {
			s.set(i);
		}
	}
}

bool UserStateFilterAdapter::isStateGuideTrue(int userId) {
	return getProxy(0)->isStateGuideTrue(userId);
}

void UserStateFilterAdapter::updateUserState(int userId,int state) {
	return getProxy(0)->updateUserState(userId, state);
}

void UserStateFilterAdapter::updateUserStateBatch(const MyUtil::Int2IntMap& statemap) {
	return getProxy(0)->updateUserStateBatch(statemap);
}

/*bool UserStateFilterAdapter::isValid() {
	return getProxy(0)->isValid();
}*/

}
}
}
