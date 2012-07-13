#include "FriendFinderInfoCacheReplicaAdapter.h"
#include "boost/lexical_cast.hpp"
#include <iostream>
using namespace xce::friendfinder;
using namespace xce::friendfinder::adapter;
using namespace std;

//*********************************************************************************************

NetworkInfo FriendFinderInfoCacheReplicaAdapter::getNetworkInfo(int userId) {
	return getProxy(userId)->getNetworkInfo(userId);
}

NetworkInfo FriendFinderInfoCacheReplicaAdapter::getNetworkInfoWithHostage(int userId) {
	return getProxy(userId)->getNetworkInfoWithHostage(userId);
}

NetworkInfo FriendFinderInfoCacheReplicaAdapter::getNetworkInfoWithHostageFromDB(int userId) {
	return getProxy(userId)->getNetworkInfoWithHostageFromDB(userId);
}

HighSchoolInfo FriendFinderInfoCacheReplicaAdapter::getHighSchoolInfo(int userId) {
	return getProxy(userId)->getHighSchoolInfo(userId);
}

MyUtil::IntSeq FriendFinderInfoCacheReplicaAdapter::getWorkplaceData(int userId, const MyUtil::StrSeq& condition) {
	return getProxy(userId)->getWorkplaceData(condition);
}

MyUtil::IntSeq FriendFinderInfoCacheReplicaAdapter::getUnivData(int userId, const MyUtil::StrSeq& condition) {
	return getProxy(userId)->getUnivData(condition);
}

MyUtil::IntSeq FriendFinderInfoCacheReplicaAdapter::getHighschoolData(int userId, const MyUtil::StrSeq& condition) {
	return getProxy(userId)->getHighschoolData(condition);
}

MyUtil::IntSeq FriendFinderInfoCacheReplicaAdapter::getJuniorschoolData(int userId, const MyUtil::StrSeq& condition) {
	return getProxy(userId)->getJuniorschoolData(condition);
}

MyUtil::IntSeq FriendFinderInfoCacheReplicaAdapter::getElementaryschoolData(int userId, const MyUtil::StrSeq& condition) {
	return getProxy(userId)->getElementaryschoolData(condition);
}

int FriendFinderInfoCacheReplicaAdapter::getBirthdayData(int userId) {
	return getProxy(userId)->getBirthdayData(userId);
}

MyUtil::IntSeq FriendFinderInfoCacheReplicaAdapter::getFriendListByInfo(int userId, int limit) {
	return getProxy(userId)->getFriendListByInfo(userId, limit);
}

MyUtil::IntSeq FriendFinderInfoCacheReplicaAdapter::getInfoMatchFriendList(int userId, int limit) {
	return getProxy(userId)->getInfoMatchFriendList(userId, limit);
}

MyUtil::IntSeq FriendFinderInfoCacheReplicaAdapter::getFriendListByGuestInfo(int hostId,int guestId,int limit) {
	return getProxy(guestId)->getFriendListByGuestInfo(hostId, guestId, limit);
}

MyUtil::IntSeq FriendFinderInfoCacheReplicaAdapter::getGuestInfoMatchFriendList(int hostId,int guestId,int limit) {
	return getProxy(guestId)->getGuestInfoMatchFriendList(hostId, guestId, limit);
}

MyUtil::IntSeq FriendFinderInfoCacheReplicaAdapter::getGuestInfoMatchFriendListBatch(const MyUtil::IntSeq& hostIds,int guestId) {
	return getProxy(guestId)->getGuestInfoMatchFriendListBatch(hostIds, guestId );
}

MyUtil::IntSeq FriendFinderInfoCacheReplicaAdapter::getCommonFriendListByGuestInfo(int userId,int minsharecount,int limit) {
	return getProxy(userId)->getCommonFriendListByGuestInfo(userId, minsharecount, limit);
}

void FriendFinderInfoCacheReplicaAdapter::setWorkplaceDataWithMask(int cluster, const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq) {
	getProxy(cluster)->setWorkplaceDataWithMask(md5Seq, dataSeq);
}

void FriendFinderInfoCacheReplicaAdapter::setUnivDataWithMask(int cluster, const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq) {
	getProxy(cluster)->setUnivDataWithMask(md5Seq, dataSeq);
}

void FriendFinderInfoCacheReplicaAdapter::setHighschoolDataWithMask(int cluster, const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq) {
	getProxy(cluster)->setHighschoolDataWithMask(md5Seq, dataSeq);
}

void FriendFinderInfoCacheReplicaAdapter::setJuniorschoolDataWithMask(int cluster, const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq) {
	getProxy(cluster)->setJuniorschoolDataWithMask(md5Seq, dataSeq);
}

void FriendFinderInfoCacheReplicaAdapter::setElementaryschoolDataWithMask(int cluster, const MyUtil::LongSeq& md5Seq, const InfoDataSeq& dataSeq) {
	getProxy(cluster)->setElementaryschoolDataWithMask(md5Seq, dataSeq);
}

void FriendFinderInfoCacheReplicaAdapter::setBirthdayData(int cluster, int birthYear, const MyUtil::IntSeq& orderedIds) {
	getProxy(cluster)->setBirthdayData(birthYear,orderedIds);
}
