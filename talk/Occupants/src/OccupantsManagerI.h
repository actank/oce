#ifndef OCCUPANTMANAGER_H_
#define OCCUPANTMANAGER_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include "TalkCommon.h"
#include "Occupants.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "XmppTools.h"
#include "XmppErrorCode.h"
#include <list>

namespace talk {
namespace occupants {
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk;
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::util;
using namespace talk::error;

class RoomInfo;

typedef map<string, OccupantPtr>::iterator OccupantIt;

class RoomInfo : public ::Ice::Object {

public:
	RoomInfo(const RoomIdPtr& rid, const RoomPtr& r, bool isReload = false) :
		_roomId(rid), _room(r) {
		if (isReload) {
			reload();
		}
	}
	;
	XmppErrorCode addOccupant(const RoomJidPtr& roomJid, const JidPtr& jid);
	void getOccupants(OccupantSeq& seq);
	OccupantPtr getOccupant(const RoomJidPtr& roomJid);
	OccupantPtr getOccupant(const JidPtr& jid);
	OccupantPtr removeOccupant(const JidPtr& jid);
	bool removeOccupants(int userId, OccupantSeq& seq);

	bool changeNick(const RoomJidPtr& roomJid, const JidPtr& jid);

	RoomRole getOccupantRole(const JidPtr& jid);

	OccupantSeq modifyRole(const JidPtr& opJid, const RoomJidPtr& targetRJid,
			RoomRole targetRole);
	OccupantSeq modifyAffiliation(const JidPtr& opJid, int targetId,
			Affiliation targetAffiliation);
	OccupantSeq modifyRoom(const RoomPtr& r);
	bool invite(const JidPtr& opJid, const JidPtr& jid);

	bool occupantEmpty() {
		return _occupants.empty();
	}
	RoomPtr getRoom() {
		return _room;
	}
	void reload(const OccupantSeq & seq = OccupantSeq());
	int getUserCount(bool onlyUserId=false);
protected:

	RoomRole _role(const RoomIdPtr& rid, Affiliation affiliation);
	string _checkNickName(const RoomJidPtr& roomJid, const JidPtr& jid);
	void _insertOccupant(const OccupantPtr& occupant);
	pair<OccupantIt,bool> _getOccupantIt(const JidPtr& jid);
	bool _removeOccupant(OccupantIt& it, bool isAll=false,
			OccupantSeq* seq=NULL);

	RoomIdPtr _roomId;
	RoomPtr _room;
	map<int, map<string, OccupantIt> > _userIds;
	typedef map<int, map<string, OccupantIt> >::iterator UserIdsIt;
	map<string, OccupantPtr> _occupants;
};
typedef ::IceInternal::Handle<RoomInfo> RoomInfoPtr;
typedef map<string, RoomInfoPtr> RoomsMap; //key = roomname
typedef map<string, RoomsMap> DomainsMap; //key = domain
typedef map<string, RoomInfoPtr>::iterator RoomsMapIt;
typedef map<string, RoomsMap>::iterator DomainsMapIt;

class OccupantsManagerI : public OccupantsManager,
	public Singleton<OccupantsManagerI> {
public:
	virtual OccupantSeq getOccupants(const RoomIdPtr&,
			const ::Ice::Current& = ::Ice::Current());

	virtual OccupantPtr getOccupant(const RoomJidPtr&,
			const ::Ice::Current& = ::Ice::Current());

	virtual OccupantPtr getOccupantByJid(const RoomIdPtr&, const JidPtr&,
			const ::Ice::Current& = ::Ice::Current());

	virtual int enterRoom(const RoomJidPtr&, const JidPtr&,
			const ::Ice::Current& = ::Ice::Current());

	virtual OccupantPtr exitRoom(const RoomIdPtr&, const JidPtr&,
			const ::Ice::Current& = ::Ice::Current());

	virtual bool changeNick(const RoomJidPtr&, const JidPtr&,
			const ::Ice::Current& = ::Ice::Current());

	virtual OccupantSeq setRole(const JidPtr& opJid,
			const RoomJidPtr& targetRJid, RoomRole role,
			const Ice::Current& = Ice::Current());
	virtual OccupantSeq setAffiliation(const JidPtr& opJid,
			const RoomMemberPtr& member, const Ice::Current& = Ice::Current());

	virtual OccupantSeq setRoomConfig(const JidPtr& opJid,
			const RoomIdPtr& rid, const RoomPtr& room,
			const ::Ice::Current& = ::Ice::Current());

	virtual OccupantSeq destroyRoom(const JidPtr& opJid, const RoomIdPtr&,
			const Ice::Current& = Ice::Current());

	virtual bool invite(const JidPtr& opJid, const RoomIdPtr&, const JidPtr& jid, const Ice::Current& = Ice::Current());
	//	void logRoom();
	//	void logUsers();
	virtual int	getRoomCount(const Ice::Current& = Ice::Current());
	virtual int getUserCount(const Ice::Current& = Ice::Current());
protected:
	RoomInfoPtr _getRoom(const RoomIdPtr& roomId);
private:
	IceUtil::RWRecMutex _rwmutex;
	DomainsMap _domains;
};


};
}
;
#endif
