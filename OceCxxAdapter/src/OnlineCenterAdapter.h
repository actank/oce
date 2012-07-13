#ifndef ONLINECENTERADAPTER_H_
#define ONLINECENTERADAPTER_H_

#include "OnlineCenter.h"
#include "TalkCommon.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk {
namespace online {
namespace adapter {

using namespace com::xiaonei::talk::common;
using namespace ::talk::online;
using namespace MyUtil;

class OnlineCenterAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, OnlineCenterAdapter> {
public:
	OnlineCenterAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	bool presence(const JidPtr& jid, com::xiaonei::talk::common::OnlineStat stat, const string& ver, const string& ext, const MyUtil::Str2StrMap& paras);
	void notify(int index,const BuddyOnlineNotifySeq& notifies);
	void offline(const JidPtr& jid);

	//void updateOnlineBitmap(int serverIndex);
	void ReloadBitmapByUserPool(int pool_index, bool twoway = true);

	int getBuddyCount(int userId);
	UserOnlineTypeSeq getBuddyOnlineTypeSeq(int userId, int timeout = 260);
	UserOnlineTypeSeq getUsersOnlineTypeSeq(const MyUtil::IntSeq& ids, int timeout = 260);

	MyUtil::IntSeq getBuddyIds(int userId, int timeout = 260);
	com::xiaonei::talk::common::JidSeqMap getBuddyJids(int userId,int onlineType,
			int timeout = 260);

	com::xiaonei::talk::common::JidSeqMap getBuddyJidsAndSelf(int userId, int buddyOnlineType = 14, int selfOnlineType=14,
				int timeout = 260);

	AvatarMap getBuddyAvatars(int userId, int onlineType=14);
	AvatarMap getBuddyAvatarsAndSelf(int userId, int buddyOnlineType=14, int selfOnlineType=14);

	AvatarSeq getUserAvatars(int userId, int onlineType=14);

	AvatarPtr getAvatar(const JidPtr& jid);
	AvatarSeq getAvatars(const JidSeq& jids);
	JidSeq getUserJids(int userId, int onlineType = 14);
	JidSeqMap getUsersJids(const MyUtil::IntSeq& ids, int onlineType=14);
	AvatarMap getUsersAvatars(const MyUtil::IntSeq& ids, int onlineType=14);

	void addBuddy(int host, int guest);
	void removeBuddy(int host, int guest);
	void closeProxyServer(const string& endpoint);

	int getCluster() {
		return _cluster;
	}
	;

protected:
	virtual string name() {
		return "OnlineCenter";
	}
	virtual string endpoints() {
		return "@OnlineCenter";
	}
	virtual size_t cluster() {
		return 10;
	}

	OnlineManagerPrx getManager(int id, int timeout = 260);
	OnlineManagerPrx getManagerOneway(int id);

	vector<OnlineManagerPrx> _managersOneway;
	vector<OnlineManagerPrx> _managers;

};

}
;
}
;
}
;

#endif /*BUDDYONLINEADAPTER_H_*/
