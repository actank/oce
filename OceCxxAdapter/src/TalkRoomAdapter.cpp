#include "TalkRoomAdapter.h"
#include "util/cpp/String.h"
#include <cmath>
#include "XmppTools.h"
//#include "PresenceAdapter.h"
using namespace com::xiaonei::talk::common;
//using namespace talk::room;
using namespace talk::room::adapter;
using namespace MyUtil;
using namespace com::xiaonei::talk::util;
//using namespace mop::hi::svc::adapter;


RoomManagerPrx TalkRoomAdapter::getManager(const RoomIdPtr& rid, int timeout) {
	int id = abs((int)StrUtil::hashcode(roomIdToString(rid)));
	if(timeout != 1000){
		RoomManagerPrx prx = locate<RoomManagerPrx>(_managers, "M", id, TWO_WAY, 1000);
		return RoomManagerPrx::uncheckedCast(prx->ice_timeout(timeout));
	}
	return locate<RoomManagerPrx>(_managers, "M", id, TWO_WAY,1000);

}

RoomManagerPrx TalkRoomAdapter::getManagerOneway(const RoomIdPtr& rid) {
	int id = abs((int)StrUtil::hashcode(roomIdToString(rid)));
	return locate<RoomManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

RoomPtr TalkRoomAdapter::createRoom(const RoomJidPtr& rJid, const JidPtr& jid){
	return getManager(rJid->rid)->createRoom(rJid,jid);
}

Affiliation TalkRoomAdapter::getAffiliation(const RoomIdPtr& rid, int userid){
	return getManager(rid)->getAffiliation(rid, userid);
}

void TalkRoomAdapter::removeRoom(const RoomIdPtr& rid){
	getManager(rid)->removeRoom(rid);
}

bool TalkRoomAdapter::setRoom(const RoomIdPtr& rid,const RoomPtr& room){
	return getManager(rid)->setRoom(rid, room);
}

RoomPtr TalkRoomAdapter::getRoom(const RoomIdPtr& rid){
	return getManager(rid)->getRoom(rid);
}

RoomIdPtr TalkRoomAdapter::createUniqRoomName(const JidPtr& jid, const string& domain){
	//int index = (int)StrUtil::hashcode(rid->name);
	//return getManager(index)->createUniqRoomName(rid, domain);
	return NULL;
}

void TalkRoomAdapter::setRoomMember(const RoomMemberPtr & member){

	return getManager(member->rid)->setRoomMember(member);
}

IntSeq TalkRoomAdapter::getRoomMembers(const RoomIdPtr& rid, const Affiliation aType){

	return getManager(rid)->getRoomMembers(rid, aType);
}

