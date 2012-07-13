#include "BirthdayRecommendCacheAdapter.h"
using namespace xce::socialgraph;

void BirthdayRecommendCacheAdapter::Set(int hostId, const MyUtil::IntSeq& birthdayRecommendList) {
	getProxy(hostId)->Set(hostId, birthdayRecommendList);
}

int BirthdayRecommendCacheAdapter::Get(int hostId) {
	return getProxy(hostId)->Get(hostId);
}

int BirthdayRecommendCacheAdapter::GetAndRemove(int hostId) {
	return getProxy(hostId)->GetAndRemove(hostId);
}

MyUtil::IntSeq BirthdayRecommendCacheAdapter::GetList(int hostId, int limit) {
	return getProxy(hostId)->GetList(hostId, limit);
}

void BirthdayRecommendCacheAdapter::Remove(int hostId, int friendId) {
	getProxy(hostId)->Remove(hostId, friendId);
}
