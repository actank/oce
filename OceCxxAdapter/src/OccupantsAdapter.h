#ifndef OCCUPANTSADAPTER_H_
#define OCCUPANTSADAPTER_H_

#include "Occupants.h"
#include "TalkCommon.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace talk {
namespace occupants {
namespace adapter {

using namespace com::xiaonei::talk::common;
using namespace ::talk::occupants;
using namespace MyUtil;

class OccupantsAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, OccupantsAdapter> {
public:

	OccupantsAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	OccupantSeq getOccupants(const RoomIdPtr&);

	OccupantPtr getOccupant(const RoomJidPtr&);

	OccupantPtr getOccupantByJid(const RoomIdPtr& roomId, const JidPtr&);

	int enterRoom(const RoomJidPtr&, const JidPtr&);

	OccupantPtr exitRoom(const RoomIdPtr&, const JidPtr&);

	bool changeNick(const RoomJidPtr&, const JidPtr&);

	//bool kickOccupant(const RoomJidPtr&, Affiliation, const JidPtr&, Affiliation);

	OccupantSeq setRole(const JidPtr&, const RoomJidPtr&, RoomRole);
	OccupantSeq setAffiliation(const JidPtr&, const RoomMemberPtr&);

	OccupantSeq destroyRoom(const JidPtr&, const RoomIdPtr&);
	//bool revokeVoice(const RoomJidPtr&, Affiliation, const JidPtr&, Affiliation);
	OccupantSeq setRoomConfig(const JidPtr&, const RoomIdPtr&, const RoomPtr&);
	bool invite(const JidPtr&, const RoomIdPtr&, const JidPtr&);
	//	bool grant(const RoomJidPtr&, Affiliation, const JidPtr&, Affiliation);

	//	int getRoomCount(const string&);
	//
	//	RoomIdSeq getRooms(const string& domain, const int&, const int&);
	int getRoomCount();
	int getUserCount();

	int getCluster() {
		return _cluster;
	}
	;

protected:
	virtual string name() {
		return "Occupants";
	}
	virtual string endpoints() {
		return "@Occupants";
	}
	virtual size_t cluster() {
		return 10;
	}

	OccupantsManagerPrx getManager(const RoomIdPtr& rid, int timeout = 1000);
	OccupantsManagerPrx getManagerOneway(const RoomIdPtr& rid);

	vector<OccupantsManagerPrx> _managersOneway;
	vector<OccupantsManagerPrx> _managers;

};

}
;
}
;
}
;

#endif /*BUDDYONLINEADAPTER_H_*/
