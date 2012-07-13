#ifndef __APNSADAPTER_H__
#define __APNSADAPTER_H__

#include "AdapterI.h"
#include "GroupChat.h"

namespace xce {
namespace talk {
namespace groupchat {
using namespace MyUtil;
class GroupChatAdapter: public AdapterI, public AdapterISingleton<
		MyUtil::NotifyChannel, GroupChatAdapter> {
public:
	bool join(::Ice::Long feedId, ::Ice::Int userId,
			const ::Ice::Context* __context);
	bool kickMember(::Ice::Long feedId, ::Ice::Int userId,
			const ::Ice::Context* __context);
	bool sendMsg(::Ice::Long feedId, ::Ice::Int userId,
			const ::std::string& msg, const ::Ice::Context* __context);

	/*void addUser(Ice::Int userId, const string& token);
	 void addIphoneNotifyBySeq(const MyUtil::IntSeq& userIds, const string& msg);*/
	int getCluster() {
		return _cluster;
	}
protected:
	virtual string name() {
		return "GroupChat";
	}
	virtual string endpoints() {
		return "@GroupChat";
	}
	virtual size_t cluster() {
		return 0;
	}
	GroupChatManagerPrx getManagerOneway();
};
}
;
}
}
;

#endif
