#include "FriendFinderByActiveUserAdapter.h"

using namespace xce::friendfinder;
using namespace xce::friendfinder::adapter;

void FriendFinderByActiveUserAdapter::setBitMap(int cluster, const MyUtil::Int2IntMap& loginMap) {
	getProxy(cluster)->setBitMap( loginMap );
}

void FriendFinderByActiveUserAdapter::setNegtiveList(int cluster, const MyUtil::IntSeq& userList) {
	getProxy(cluster)->setNegtiveList( userList );
}

void FriendFinderByActiveUserAdapter::clearUserLoginCache(int cluster) {
	getProxy(cluster)->clearUserLoginCache();
}

MyUtil::Int2IntSeqMap FriendFinderByActiveUserAdapter::getRecommend(int userId, int limit) {
	return getProxy(userId)->getRecommend( userId, limit );
}

MyUtil::Int2IntSeqMap FriendFinderByActiveUserAdapter::getRecommendRandom(int userId, int limit) {
	return getProxy(userId)->getRecommendRandom( userId, limit );
}

void FriendFinderByActiveUserAdapter::removeRecommendFriends(int userId, int friendId, const MyUtil::IntSeq& ids) {
	getProxy(userId)->removeRecommendFriends( userId, friendId, ids );
}

MyUtil::IntSeq FriendFinderByActiveUserAdapter::getActiveUser(int userId, const MyUtil::IntSeq& userList) {
	return getProxy(userId)->getActiveUser(userId, userList);
}

MyUtil::IntSeq FriendFinderByActiveUserAdapter::getWriteRecommendRandom(int userId, int limit) {
	return getProxy(userId)->getWriteRecommendRandom(userId, limit);
}
