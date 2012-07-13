#ifndef MUCPRESENCEMSGHANDLERADAPTER_H_
#define MUCPRESENCEMSGHANDLERADAPTER_H_

#include "TalkLogic.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk{
namespace adapter {
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class MucPresenceMsgHandlerAdapter : public MyUtil::AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, MucPresenceMsgHandlerAdapter> {
public:
	MucPresenceMsgHandlerAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	void presence(const JidPtr& j, const string& show, const string& ver, const string& subver, const string& ext);
	void presenceWithSecver(const JidPtr& j, const string& show, const string& ver, const string& subver, const string& ext, const string& secver);

//	void broadcastPresence(const JidPtr& j, const string& show, const string& ver, const string& ext,
//			const string& status, const string& photo, const string& nick);


	void presenceSubscribe(const string& id, int from, int to,
			const string& status);
	void presenceSubscribeWithPostscript(const string& id, int from, int to,
			const string& status, const string& postscript);
	void presenceSubscribed(const string& id, int from, int to);
	void presenceUnsubscribed(const string& id, int from, int to);


	void webPresence(int userId, const Ice::Context& ctx);

	void message(const JidPtr& host, const string& msg);
	void offlineNotify(const JidSeq& seq);

	void notifyBuddiesOnlineStat(const AvatarSeq& activeAvatars, const AvatarSeq& selfOtherAvatars,
									const AvatarMap& buddies, NotifyType type, const MyUtil::Str2StrMap& paras);
	size_t getCluster(){
		return _cluster;
	};
	void offline(const JidPtr& j);
	void userStatusChanged(int userId, int userStatus);
protected:
	virtual string name() {
		return "MucPresence";
	}
	virtual string endpoints() {
		return "@MucPresence";
	}
	virtual size_t cluster() {
		return 10;
	}

	PresenceHandlerPrx getManager(int id);
	PresenceHandlerPrx getManagerOneway(int id);

	vector<PresenceHandlerPrx> _managersOneway;
	vector<PresenceHandlerPrx> _managers;
};


}
;
}
;

#endif /*TALKLOGICADAPTER_H_*/
