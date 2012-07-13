#ifndef PRESENCEADAPTER_H_
#define PRESENCEADAPTER_H_

#include "Presence.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk;
using namespace MyUtil;

enum OnlineStat{
	//WEB_ONLINE = 1,
	WEBPAGER_ONLINE = 1,
	TALK_ONLINE = 2
};

class PresenceAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, PresenceAdapter> {
public:

	void bind(const JidPtr& jid);
	void unbind(const JidPtr& jid);
	void presence(const JidPtr& jid, const string& show, const string& ver, const string& ext);
	JidSeqMap getIMOnlineUserJidSeqMap(const MyUtil::IntSeq& ids);
	JidSeq getOnlineUserJidSeq(int id);
	OnlineInfoMap filterOnlineUsers(const MyUtil::IntSeq& ids);
	OnlineInfoPtr filterOnlineUser(const JidPtr& jid);
	OnlineInfoSeq getOnlineUsersInfo(const JidSeq& jids);

	bool has(const JidPtr& jid);
	void closeProxyServer(const string& endpoint);
	map<int,int> getOnlineStat(const MyUtil::IntSeq& ids);
protected:
	virtual void initialize();
	virtual string name() {
		return "Presence";
	}
	virtual string endpoints() {
		return "@Presence";
	}
	virtual size_t cluster() {
		return 0;
	}

	PresenceManagerPrx _manager;
	PresenceManagerPrx _managerOneway;
};

}
;
}
;
}
;
}
;

#endif
