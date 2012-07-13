#ifndef TALKROOMADAPTER_H_
#define TALKROOMADAPTER_H_

#include "TalkCommon.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "TalkRoom.h"
#include "XmppErrorCode.h"


namespace talk {
namespace room {
namespace adapter {

using namespace com::xiaonei::talk::common;
using namespace ::talk::room;
using namespace MyUtil;
using namespace ::talk::error;

class TalkRoomAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, TalkRoomAdapter> {
public:

	TalkRoomAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	RoomPtr createRoom(const RoomJidPtr& rJid, const JidPtr& jid);
	void removeRoom(const RoomIdPtr& rid);
	bool setRoom(const RoomIdPtr& rid,const RoomPtr& room);
	RoomPtr getRoom(const RoomIdPtr& rid);
	RoomIdPtr createUniqRoomName(const JidPtr& jid, const string& domain);

	Affiliation getAffiliation(const RoomIdPtr& rid, int userid);
	void setRoomMember(const RoomMemberPtr & member);
	IntSeq getRoomMembers(const RoomIdPtr& rid, const Affiliation aType);

	int getCluster() {
		return _cluster;
	}
	;

protected:
	virtual string name() {
		return "TalkRoom";
	}
	virtual string endpoints() {
		return "@TalkRoom";
	}
	virtual size_t cluster() {
		return 10;
	}

	RoomManagerPrx getManager(const RoomIdPtr& rid, int timeout = 1000);
	RoomManagerPrx getManagerOneway(const RoomIdPtr& rid);

	vector<RoomManagerPrx> _managersOneway;
	vector<RoomManagerPrx> _managers;

}
;
}
;
}
;
}
;

#endif /*BUDDYONLINEADAPTER_H_*/
