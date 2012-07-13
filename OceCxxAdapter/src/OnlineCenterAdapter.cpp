#include "OnlineCenterAdapter.h"
#include "PresenceAdapter.h"
using namespace com::xiaonei::talk::common;
using namespace talk::online;
using namespace talk::online::adapter;
using namespace mop::hi::svc::adapter;

OnlineManagerPrx OnlineCenterAdapter::getManager(int id, int timeout) {
	if (timeout != 260) {
		OnlineManagerPrx prx = locate<OnlineManagerPrx>(_managers, "M", id,
				TWO_WAY);
		return OnlineManagerPrx::uncheckedCast(prx->ice_timeout(timeout));
	}
	return locate<OnlineManagerPrx>(_managers, "M", id, TWO_WAY);

}

OnlineManagerPrx OnlineCenterAdapter::getManagerOneway(int id) {
	return locate<OnlineManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}
bool OnlineCenterAdapter::presence(const JidPtr& jid, com::xiaonei::talk::common::OnlineStat stat, const string& ver,
		const string& ext, const MyUtil::Str2StrMap& paras){
	if(!jid){
		return false;
	}
	return getManager(jid->userId)->presence(jid, stat, ver, ext, paras);
}
//void notify1(const BuddyOnlineNotifySeq& notifies);
void OnlineCenterAdapter::notify(int index,const BuddyOnlineNotifySeq& notifies) {
	return getManager(index)->notify(notifies,index);
}
void OnlineCenterAdapter::offline(const JidPtr& jid) {
	return getManager(jid->userId)->offline(jid);
}

void OnlineCenterAdapter::ReloadBitmapByUserPool(int pool_index, bool twoway) {
  if (twoway) {
	  getManager(pool_index)->ReloadBitmapByUserPool(pool_index);
  } else {
	  getManagerOneway(pool_index)->ReloadBitmapByUserPool(pool_index);
  }
}

//void OnlineCenterAdapter::updateOnlineBitmap(int serverIndex){
//	getManagerOneway(serverIndex)->updateOnlineBitmap(serverIndex);
//}

int OnlineCenterAdapter::getBuddyCount(int userId) {
	return getManager(userId)->getBuddyCount(userId);
}
UserOnlineTypeSeq OnlineCenterAdapter::getBuddyOnlineTypeSeq(int userId, int timeout) {
	return getManager(userId, timeout)->getBuddyOnlineTypeSeq(userId);
}
UserOnlineTypeSeq OnlineCenterAdapter::getUsersOnlineTypeSeq(const MyUtil::IntSeq& ids, int timeout){
	if(ids.empty()){
		return UserOnlineTypeSeq();
	}
	return getManager(ids.at(0), timeout)->getUsersOnlineTypeSeq(ids);
}
AvatarMap OnlineCenterAdapter::getBuddyAvatars(int userId,int onlineType) {
	return getManager(userId)->getBuddyAvatars(userId, onlineType);
}

AvatarMap OnlineCenterAdapter::getBuddyAvatarsAndSelf(int userId, int buddyOnlineType, int selfOnlineType) {
	return getManager(userId)->getBuddyAvatarsAndSelf(userId,buddyOnlineType,selfOnlineType);
}

AvatarSeq OnlineCenterAdapter::getUserAvatars(int userId, int onlineType) {
	return getManager(userId)->getUserAvatars(userId, onlineType);
}
AvatarPtr OnlineCenterAdapter::getAvatar(const JidPtr& jid) {
	return getManager(jid->userId)->getAvatar(jid);
}
AvatarSeq OnlineCenterAdapter::getAvatars(const JidSeq& jids) {
	if(jids.empty()){
		return AvatarSeq();
	}
	return getManager(jids.at(0)->userId)->getAvatars(jids);
}
JidSeq OnlineCenterAdapter::getUserJids(int userId, int onlineType) {
	return getManager(userId)->getUserJids(userId, onlineType);
}
JidSeqMap OnlineCenterAdapter::getUsersJids(const MyUtil::IntSeq& ids, int onlineType) {
	if(ids.empty()){
		return JidSeqMap();
	}
	return getManager(ids.at(0))->getUsersJids(ids,onlineType);
}
AvatarMap OnlineCenterAdapter::getUsersAvatars(const MyUtil::IntSeq& ids, int onlineType) {
	if(ids.empty()){
		return AvatarMap();
	}
	return getManager(ids.at(0))->getUsersAvatars(ids, onlineType);
}
void OnlineCenterAdapter::addBuddy(int host, int guest) {
	getManagerOneway(host)->addBuddy(host, guest);
	getManagerOneway(guest)->addBuddy(guest,host);
}

void OnlineCenterAdapter::removeBuddy(int host, int guest) {
	getManagerOneway(host)->removeBuddy(host, guest);
	getManagerOneway(guest)->removeBuddy(guest, host);
}

MyUtil::IntSeq OnlineCenterAdapter::getBuddyIds(int userId, int timeout) {
	return getManager(userId, timeout)->getBuddyIds(userId);
}

com::xiaonei::talk::common::JidSeqMap OnlineCenterAdapter::getBuddyJids(
		int userId, int onlineType,int timeout) {
	return getManager(userId, timeout)->getBuddyJids(userId,onlineType);
}

com::xiaonei::talk::common::JidSeqMap OnlineCenterAdapter::getBuddyJidsAndSelf(
		int userId, int buddyOnlineType, int selfOnlineType, int timeout) {
	return getManager(userId, timeout)->getBuddyJidsAndSelf(userId,buddyOnlineType,selfOnlineType);
}

void OnlineCenterAdapter::closeProxyServer(const string& endpoint) {
	int count = getCluster();
	int tmp = time(NULL)%count;
	for (int i=0; i<count; i++) {
		try{
			if (tmp == i) {
				getManagerOneway(i)->closeProxyServer(endpoint,true);
			} else {
				getManagerOneway(i)->closeProxyServer(endpoint,false);
			}
		}catch(Ice::Exception& e){
			MCE_WARN("OnlineCenterAdapter::closeProxyServer --> invoke closeProxyServer error, " << e);
		}
	}
}

