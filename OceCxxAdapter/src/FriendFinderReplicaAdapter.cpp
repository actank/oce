#include "FriendFinderReplicaAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>

using namespace com::xiaonei::service;
using namespace xce::friendfinder;
using namespace xce::friendfinder::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= FriendFinderReplicaAdapter =================================

CommonFriendSeq FriendFinderReplicaAdapter::getCommonFriend(int userId,int begin,int limit) {
        return getProxy(userId)->getCommonFriend(userId,begin,limit);
}

MyUtil::IntSeq FriendFinderReplicaAdapter::getSharedFriend(int userId, int friendId, int begin, int limit) {
		return getProxy(userId)->getSharedFriend(userId, friendId, begin, limit);
}

void FriendFinderReplicaAdapter::reloadCommonFriend(int userId) {
        getProxy(userId)->reloadCommonFriend(userId);
}

void FriendFinderReplicaAdapter::rebuildCommonFriend(int userId) {
        getProxy(userId)->rebuildCommonFriend(userId);
}

void FriendFinderReplicaAdapter::removeCommonFriend(int userId, int friendId) {
        getProxy(userId)->removeCommonFriend(userId, friendId);
}

// ========= FriendFinderGuideReplicaAdapter =================================
/*
CommonFriendSeq FriendFinderGuideReplicaAdapter::getCommonFriend(int userId,int begin,int limit) {
        return getProxy(userId)->getCommonFriend(userId,begin,limit);
}

void FriendFinderGuideReplicaAdapter::reloadCommonFriend(int userId) {
        getProxy(userId)->reloadCommonFriend(userId);
}

void FriendFinderGuideReplicaAdapter::rebuildCommonFriend(int userId) {
        getProxy(userId)->rebuildCommonFriend(userId);
}

void FriendFinderGuideReplicaAdapter::removeCommonFriend(int userId, int friendId) {
        getProxy(userId)->removeCommonFriend(userId, friendId);
}

// ========= FriendFinderGuideReplicaNAdapter =================================

CommonFriendSeq FriendFinderGuideReplicaNAdapter::getCommonFriend(int userId,int begin,int limit) {
        return getProxy(userId)->getCommonFriend(userId,begin,limit);
}

void FriendFinderGuideReplicaNAdapter::reloadCommonFriend(int userId) {
        getProxy(userId)->reloadCommonFriend(userId);
}

void FriendFinderGuideReplicaNAdapter::rebuildCommonFriend(int userId) {
        getProxy(userId)->rebuildCommonFriend(userId);
}

void FriendFinderGuideReplicaNAdapter::removeCommonFriend(int userId, int friendId) {
        getProxy(userId)->removeCommonFriend(userId, friendId);
}
*/

