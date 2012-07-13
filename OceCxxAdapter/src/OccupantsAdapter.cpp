#include "OccupantsAdapter.h"
#include "XmppTools.h"
#include "util/cpp/String.h"
#include <cmath>
//#include "PresenceAdapter.h"
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace talk::occupants;
using namespace talk::occupants::adapter;
using namespace MyUtil;
//using namespace mop::hi::svc::adapter;


OccupantsManagerPrx OccupantsAdapter::getManager(const RoomIdPtr& rid, int timeout) {

	int id = abs((int)StrUtil::hashcode(roomIdToString(rid)));
	if(timeout != 1000){
		OccupantsManagerPrx prx = locate<OccupantsManagerPrx>(_managers, "M", id, TWO_WAY,1000);
		return OccupantsManagerPrx::uncheckedCast(prx->ice_timeout(timeout));
	}
	MCE_DEBUG("OccupantsAdapter::getManager --> " << id);
	return locate<OccupantsManagerPrx>(_managers, "M", id, TWO_WAY, 1000);

}

OccupantsManagerPrx OccupantsAdapter::getManagerOneway(const RoomIdPtr& rid) {
	int id = abs((int)StrUtil::hashcode(roomIdToString(rid)));
	return locate<OccupantsManagerPrx>(_managersOneway, "M", id, ONE_WAY);
}

OccupantSeq OccupantsAdapter::getOccupants(const RoomIdPtr& roomId){

	return getManager(roomId)->getOccupants(roomId);
}

OccupantPtr OccupantsAdapter::getOccupant(const RoomJidPtr& roomJid){

	return getManager(roomJid->rid)->getOccupant(roomJid);
}

OccupantPtr OccupantsAdapter::getOccupantByJid(const RoomIdPtr& roomId, const JidPtr& jid){
	return getManager(roomId)->getOccupantByJid(roomId, jid);
}

int OccupantsAdapter::enterRoom(const RoomJidPtr& roomJid, const JidPtr& jid){

	return getManager(roomJid->rid,2000)->enterRoom(roomJid, jid);
}

OccupantPtr OccupantsAdapter::exitRoom(const RoomIdPtr& rid, const JidPtr& jid){

	return getManager(rid)->exitRoom(rid, jid);
}

bool OccupantsAdapter::changeNick(const RoomJidPtr& roomJid, const JidPtr& jid){

	return getManager(roomJid->rid)->changeNick(roomJid, jid);
}
//const RoomJidPtr&, Affiliation, const JidPtr&, Affiliation
//bool OccupantsAdapter::kickOccupant(const RoomJidPtr& roomJid, Affiliation affiliation1,
//		const JidPtr& jid, Affiliation affiliation2){
//
//	return getManager(roomJid->rid)->kickOccupant(roomJid, affiliation1, jid, affiliation2);
//}

OccupantSeq OccupantsAdapter::setRole(const JidPtr& opJid, const RoomJidPtr& targetRJid,
		RoomRole role){
	return getManager(targetRJid->rid)->setRole(opJid, targetRJid, role);
}

OccupantSeq OccupantsAdapter::setAffiliation(const JidPtr& opJid, const RoomMemberPtr& member){
	return getManager(member->rid)->setAffiliation(opJid, member);
}

OccupantSeq OccupantsAdapter::destroyRoom(const JidPtr& opJid, const RoomIdPtr& rid){
	return getManager(rid)->destroyRoom(opJid, rid);
}

OccupantSeq OccupantsAdapter::setRoomConfig(const JidPtr& opJid, const RoomIdPtr& rid, const RoomPtr& room){
	return getManager(rid)->setRoomConfig(opJid, rid, room);
}

bool OccupantsAdapter::invite(const JidPtr& opJid, const RoomIdPtr& rid, const JidPtr& jid){
	return getManager(rid)->invite(opJid, rid, jid);
}

int OccupantsAdapter::getRoomCount(){
	int sum = 0;
	int count = cluster();
	for(int i=0; i<count; ++i){
		OccupantsManagerPrx prx = locate<OccupantsManagerPrx>(_managers, "M", i, TWO_WAY, 1000);
		sum += prx->getRoomCount();
	}
	return sum;
}

int OccupantsAdapter::getUserCount(){
	int sum = 0;
	int count = cluster();
	for(int i=0; i<count; ++i){
		OccupantsManagerPrx prx = locate<OccupantsManagerPrx>(_managers, "M", i, TWO_WAY, 1000);
		sum += prx->getUserCount();
	}
	return sum;
}

//OccupantSeq OccupantsAdapter::affiliationChanged(const RoomMemberPtr & member, const RoomTypePtr& roomType){
//	return getManager(member->rid)->affiliationChanged(member, roomType);
//}
/*
bool OccupantsAdapter::revokeVoice(const RoomJidPtr& roomJid, Affiliation affiliation1,
		const JidPtr& jid, Affiliation affiliation2){

	return getManager(roomJid->rid)->revokeVoice(roomJid, affiliation1, jid, affiliation2);
}*/

//bool OccupantsAdapter::grant(const RoomJidPtr& roomJid, Affiliation affiliation1,
//		const JidPtr& jid, Affiliation affiliation2){
//
//	return getManager(roomJid->rid)->grant(roomJid, affiliation1, jid, affiliation2);
//}

//int OccupantsAdapter::getRoomCount(const string& domain){
//	int count = 0;
//	for(size_t i = 0; i<cluster(); i++){
//		count += locate<OccupantsManagerPrx>(_managers, "M", i, TWO_WAY)->getRoomCount(domain);
//	}
//	return count;
//}

//RoomIdSeq OccupantsAdapter::getRooms(const string& domain, const int& index, const int& count){
//	int sum = 0;
//	int current = 0;
//	//int pre = 0;
//	//int presum = 0;
//	int i = 0;
//	for(; i<(int)cluster(); i++){
//		current = locate<OccupantsManagerPrx>(_managers, "M", i, TWO_WAY)->getRoomCount(domain);
//		sum += current;
//		if(sum > (index*count))
//			break;
//		//presum = sum;
//
//	}
//	MCE_DEBUG("OccupantsAdapter::getRooms --> sum=" << sum << " current=" << current);
//	if(sum < index*count)
//		return RoomIdSeq();
//
//	int beg = current - (sum - index*count);
//	if(beg < 0){
//		MCE_DEBUG("OccupantsAdapter::getRooms --> caculate index error, " << beg);
//	}
//	MCE_DEBUG("OccupantsAdapter::getRooms --> beg=" << beg);
//	RoomIdSeq seq = locate<OccupantsManagerPrx>(_managers, "M", i, TWO_WAY)->getRooms(domain, beg, count);
//	MCE_DEBUG("OccupantsAdapter::getRooms --> size=" << seq.size());
//
//	for(size_t j = i+1; seq.size()< count && j<getCluster(); j++){
//		RoomIdSeq tSeq = locate<OccupantsManagerPrx>(_managers, "M", j, TWO_WAY)->getRooms(domain, 0, count - seq.size());
//		for(size_t k=0; k<tSeq.size(); k++){
//			seq.push_back(tSeq.at(k));
//			//if(seq.size()>=count)
//			//	return seq;
//		}
//	}
//	return seq;
//}
