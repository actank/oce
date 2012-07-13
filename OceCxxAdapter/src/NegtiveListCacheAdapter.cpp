#include "NegtiveListCacheAdapter.h"

using namespace xce::socialgraph;

void NegtiveListCacheAdapter::Set(int hostId, MyUtil::IntSeq negtiveList) {
	getProxy(hostId)->Set(hostId, negtiveList);
}

MyUtil::IntSeq NegtiveListCacheAdapter::Get(int hostId, int limit) {
	getProxy(hostId)->Get(hostId, limit);
}

void NegtiveListCacheAdapter::Remove(int hostId, MyUtil::IntSeq negtiveList) {
	getProxy(hostId)->Remove(hostId, negtiveList);
}
