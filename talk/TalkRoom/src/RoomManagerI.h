#ifndef ROOMMANAGERI_
#define ROOMMANAGERI_

#include "TalkRoom.h"
#include "Singleton.h"
#include <map>
#include <set>
#include <IceUtil/RWRecMutex.h>
#include "ServiceI.h"
#include "XmppTools.h"

namespace talk {
namespace room {

using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace std;

const int MAX_OCCUPANTS_COUNT = 50;
const int ROOM_CLUSTER = 0;
const int MEMBER_LIST = 1;
const int ROOM_LIST = 2;

typedef map<RoomIdPtr,RoomPtr> RoomMap;

class RoomCluster : public Ice::Object{
	RoomMap _roomMap;
	//IceUtil::Mutex _mutex;
public:
	RoomPtr get(const RoomIdPtr& rid);
	bool add(const RoomIdPtr& rid,const RoomPtr& room);
	void remove(const RoomIdPtr& rid);
	void set(const RoomIdPtr& rid,const RoomPtr& room);

};
typedef IceUtil::Handle<RoomCluster> RoomClusterPtr;

class MemberList : public Ice::Object{
	map<RoomIdPtr, map<Affiliation, set<int> > > _members;
	//IceUtil::Mutex _mutex;
public:
//	bool addMember(const RoomIdPtr& rid, Affiliation affiliation, int userid);
	IntSeq getMemberList(const RoomIdPtr& rid,Affiliation affiliation);
	Int2IntSeqMap getMemberList(const RoomIdPtr& rid, const vector<Affiliation>& affiliations = vector<Affiliation>());
	Affiliation getAffiliation(const RoomIdPtr& rid,int userid);
	void changeMember(const RoomIdPtr& rid,Affiliation affiliation, int userid);
//	void removeMember(const RoomIdPtr& rid, int userid);
	void getMembers(const RoomIdPtr& rid, RoomMemberSeq& seq);
	void remove(const RoomIdPtr& rid);
};
typedef IceUtil::Handle<MemberList> MemberListPtr;
/*
class RoomList : public Ice::Object{
	set<RoomIdPtr> _rooms;
public:
	bool addRoom(const RoomIdPtr& rid);
	void removeRoom(const RoomIdPtr& rid);
	set<RoomIdPtr>& getRooms();
};*/
//typedef IceUtil::Handle<RoomList> RoomListPtr;

class RoomFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Long hashcode);
};

class MemeberFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Long hashcode);
};

/*class RoomListFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Long userId);
};*/

class RoomManagerI : public RoomManager, public Singleton<RoomManagerI> {
public:
	virtual RoomPtr createRoom(const RoomJidPtr& rJid,const JidPtr& jid, const Ice::Current& = Ice::Current());
	virtual void removeRoom(const RoomIdPtr& rid, const Ice::Current& = Ice::Current());
	virtual RoomPtr getRoom(const RoomIdPtr& rid, const Ice::Current& = Ice::Current());
	virtual bool setRoom(const RoomIdPtr& rid,const RoomPtr& room,
			const Ice::Current& = Ice::Current());
	virtual Affiliation getAffiliation(const RoomIdPtr& rid,int userid, const Ice::Current& = Ice::Current());
	//virtual RoomSeq getRoomSeq(Ice::Int userid, const Ice::Current& = Ice::Current());

	virtual void setRoomMember(const RoomMemberPtr & member,
			const Ice::Current& = Ice::Current());

	virtual IntSeq getRoomMembers(const RoomIdPtr& rid, Affiliation aType,
			const Ice::Current& = Ice::Current());

	//virtual RoomMemberPtr getRoomMember(const RoomIdPtr& rid,Ice::Int userId, const Ice::Current& = Ice::Current());

protected:

	//RoomMemberPtr _getRoomMember(const RoomIdPtr& rid, Ice::Int userId);
	IceUtil::RWRecMutex _mutex;
};
}
;
}
;

#endif /*ROOMMANAGER_*/
