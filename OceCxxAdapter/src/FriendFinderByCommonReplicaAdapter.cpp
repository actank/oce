#include "FriendFinderByCommonReplicaAdapter.h"

using namespace xce::friendfinder;
using namespace xce::friendfinder::adapter;
using namespace com::xiaonei::service;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= FriendFinderByCommonAdapter =================================

void FriendFinderByCommonAdapter::setCommonFriendSeq(Ice::Int userId,const CommonFriendSeq& cfs){
	getProxy(userId)->setCommonFriendSeq(userId,cfs);
}

CommonFriendItemSeq FriendFinderByCommonAdapter::getCommonFriendItemSeq(Ice::Int userId, Ice::Int limit){
	return getProxy(userId)->getCommonFriendItemSeq(userId,limit);
}

CommonFriendItemSeq FriendFinderByCommonAdapter::getCommonFriendItemSeqById(Ice::Int userId, Ice::Int limit){
	return getProxy(userId)->getCommonFriendItemSeqById(userId,limit);
}

Ice::Int FriendFinderByCommonAdapter::getCount(Ice::Int userId){
	return getProxy(userId)->getCount(userId);
}

void FriendFinderByCommonAdapter::removeCommonFriendItem(Ice::Int userId, Ice::Int friendId){
	getProxy(userId)->removeCommonFriendItem(userId,friendId);
}

void FriendFinderByCommonAdapter::reload(Ice::Int userId){
	getProxy(userId)->reload(userId);
}

MyUtil::IntSeq FriendFinderByCommonAdapter::getFriendsByCommon(Ice::Int guestId,Ice::Int hostId,Ice::Int limit){
	return getProxy(hostId)->getFriendsByCommon(guestId,hostId,limit);
}

