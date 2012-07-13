#include "FriendFinderAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace com::xiaonei::service;
using namespace std;

//---------------------------------------------------------------------------
CommonFriendSeq FriendFinderAdapter::getCommonFriend(int userId,int begin,int limit) {
	return getManager(userId)->getCommonFriend(userId,begin,limit);
}

void FriendFinderAdapter::reloadCommonFriend(int userId) {
	getOnewayManager(userId)->reloadCommonFriend(userId);
}

void FriendFinderAdapter::rebuildCommonFriend(int userId) {
	getOnewayManager(userId)->rebuildCommonFriend(userId);
}

IntSeq FriendFinderAdapter::getSharedFriend(int userId, int friendId, int begin, int limit){
	return getManager(userId)->getSharedFriend(userId, friendId, begin, limit);
}

int FriendFinderAdapter::getSharedFriendCount(int userId, int friendId){
	return getManager(userId)->getSharedFriendCount(userId, friendId);
}

Int2ShareFriendMap FriendFinderAdapter::getSharedFriends(int fromId, MyUtil::IntSeq toIds, int limit){
	return getManager(fromId)->getSharedFriends(fromId, toIds, limit);
}

void FriendFinderAdapter::removeCommonFriend(int userId, int friendId) {
	getOnewayManager(userId)->removeCommonFriend(userId, friendId);
}

FriendFinderManagerPrx FriendFinderAdapter::getManager(int id) {
	return locate<FriendFinderManagerPrx>(_managers, "FFM", id, TWO_WAY);
}

FriendFinderManagerPrx FriendFinderAdapter::getOnewayManager(int id) {
	return locate<FriendFinderManagerPrx>(_managersOneway, "FFM", id, ONE_WAY);
}

