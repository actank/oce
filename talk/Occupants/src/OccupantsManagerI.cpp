#include "OccupantsManagerI.h"
#include "TalkRoomAdapter.h"
#include "UserRoomAdapter.h"
#include <stdio.h>
#include <vector>
//#include "OccupantsBackupAdapter.h"

using namespace talk::room::adapter;
using namespace talk::occupants;
using namespace talk::userroom::adapter;
//using namespace ::talk::occupants::adapter;
using namespace std;
using namespace MyUtil;

void MyUtil::initialize() {

	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add( &OccupantsManagerI::instance(), service.createIdentity("M",
			""));
}
//---------------------------------------------
void RoomInfo::reload(const OccupantSeq& seq) {
	if (seq.empty()) {
		/*try {
			OccupantSeq seq1 = OccupantsBackupAdapter::instance().get(_roomId);
			for (size_t i = 0; i < seq1.size(); ++i) {
				_insertOccupant(seq1.at(i));
			}
		} catch(Ice::Exception & e) {
			MCE_WARN("RoomInfo::RoomInfo --> do reload from OccupantsBackAdapter err:"<<e);
		} catch(...) {
			MCE_WARN("RoomInfo::RoomInfo --> do reload from OccupantsBackAdapter");
		}*/
	} else {
		for (size_t i = 0; i < seq.size(); ++i) {
			_insertOccupant(seq.at(i));
		}
	}

}

string RoomInfo::_checkNickName(const RoomJidPtr& roomJid, const JidPtr& jid) {

	MCE_DEBUG("RoomInfo::_checkNickName --> " << roomJid->nick << " "
			<< jid->userId);

	if (!validRoomNickName(roomJid, jid->userId)) {
		MCE_DEBUG("RoomInfo::_checkNickName --> Room Nick Name is not a well form");
		return "";
	}
	OccupantIt occupantIt = _occupants.find(roomJid->nick);
	if (occupantIt == _occupants.end()) {
		return roomJid->nick;
	}

	return "";
}

RoomRole RoomInfo::_role(const RoomIdPtr& rid, Affiliation affiliation) {
	//Affiliation affiliation = TalkRoomAdapter::instance().getAffiliation(rid, userId);

	switch (affiliation) {
	case Owner:
	case Admin:
		return Moderator;
	case Member:
		return Participant;
	case None:
		if (_room->type->memberOnly) {
			return NoneRole;
		} else if (_room->type->moderated) {
			return Visitor;
		} else {
			return Participant;
		}
	case Outcast:
		return NoneRole;
	default:
		MCE_DEBUG("OccupantGrouop::addOccupant --> Unkonw affiliation"
				<< affiliation);
		return NoneRole;
	}
	return NoneRole;
}
//OccupantSeq	_modifyRole(const JidPtr& opJid, const RoomJidPtr& targetRJid, RoomRole targetRole);
OccupantSeq RoomInfo::modifyRole(const JidPtr& opJid,
		const RoomJidPtr& targetRJid, RoomRole targetRole) {
	OccupantPtr op = getOccupant(opJid);
	OccupantPtr target = getOccupant(targetRJid);

	if (!(op && target)) {
		return OccupantSeq();
	}
	OccupantSeq seq;
	bool isAllowed = false;
	if (op->aType >= Admin) {
		if (target->aType < Admin) {
			isAllowed = true;
			//target->role = targetRole;
			//return target;
		}
	} else {
		if (op->role == Moderator && target->role < Moderator && target
				== NoneRole) {
			isAllowed = true;
			//target->role = targetRole;
			//return target;
		}
	}
	if (!isAllowed) {
		return OccupantSeq();
	}
	if (NoneRole == targetRole) {
		MCE_DEBUG("RoomInfo::setOccupantRole --> remove occupant");
		removeOccupants(target->fJid->userId, seq);
	} else {
		UserIdsIt userIt = _userIds.find(target->fJid->userId);
		if (userIt == _userIds.end()) {
			return OccupantSeq();
		}
		map<string, OccupantIt>::iterator itTmp = userIt->second.begin();
		for (; itTmp!=userIt->second.end(); ++itTmp) {
			itTmp->second->second->role = targetRole;
			seq.push_back(itTmp->second->second);
		}
	}
	MCE_DEBUG("RoomInfo::setOccupantRole --> success");

	return seq;
}

OccupantSeq RoomInfo::modifyAffiliation(const JidPtr& opJid, int targetId,
		Affiliation targetAffiliation) {
	MCE_DEBUG("RoomInfo::_modifyAffiliation --> to " << targetAffiliation);
	OccupantPtr op = getOccupant(opJid);
	if (!op) {
		return OccupantSeq();
	}
	Affiliation oldAffiliation;
	UserIdsIt userIt = _userIds.find(targetId);
	if (userIt == _userIds.end() || userIt->second.empty()) {
		oldAffiliation = TalkRoomAdapter::instance().getAffiliation(op->rJid->rid, targetId);
	} else {
		oldAffiliation = userIt->second.begin()->second->second->aType;
	}
	OccupantSeq res;
	if (op->aType >= Admin && targetAffiliation < op->aType && op->aType > oldAffiliation)
{		RoomMemberPtr rm = new RoomMember();
		rm -> rid = op->rJid->rid;
		rm -> userId = targetId;
		rm -> aType = targetAffiliation;
		TalkRoomAdapter::instance().setRoomMember(rm);
		//--------------
		UserIdsIt it = _userIds.find(targetId);
		if (it != _userIds.end()) {
			RoomRole role = _role(op->rJid->rid, targetAffiliation);
			map<string, OccupantIt>::iterator oit = it->second.begin();
			for (; oit != it->second.end(); ++oit) {
				oit->second->second->aType = targetAffiliation;
				oit->second->second->role = role;
				res.push_back(oit->second->second);
				if(role == NoneRole) {
					_occupants.erase(oit->second);
				}
			}
			if (role == NoneRole) {
				_userIds.erase(it);
			}
		}
	}
	return res;
}

OccupantSeq RoomInfo::modifyRoom(const RoomPtr& r) {
	if (!r) {
		return OccupantSeq();
	}
	_room = r;
	OccupantSeq res;
	for (UserIdsIt uit = _userIds.begin(); uit != _userIds.end();) {
		map<string, OccupantIt>::iterator oit = uit->second.begin();
		RoomRole role = NoneRole;//_role(_roomId, uit->first);
		for (; oit != uit->second.end(); ++oit) {
			role = _role(_roomId, oit->second->second->aType);
			if (oit->second->second->role == role) {
				continue;
			}
			oit->second->second->role = role;
			res.push_back(oit->second->second);
			if (oit->second->second->role == NoneRole) {
				_occupants.erase(oit->second);
			}
		}
		if (role == NoneRole) {
			_userIds.erase(uit++);
		} else {
			++uit;
		}
	}
	return res;
}

bool RoomInfo::invite(const JidPtr& opJid, const JidPtr& jid) {

/*	OccupantPtr o = getOccupant(jid);
	if (o) {
		return false;
	}
*/
	OccupantPtr op = getOccupant(opJid);
	if (!op) {
		return false;
	}
	if (!_room->allowUserToInvite) {
		if (op->aType < Admin) {
			return false;
		}
	}

	Affiliation affiliation = TalkRoomAdapter::instance().getAffiliation(op->rJid->rid, jid->userId);
	if (affiliation == Outcast) {
		if (op->aType < Admin) {
			return false;
		}
	}else if(affiliation >= Member){
		return true;
	}


	RoomMemberPtr rm = new RoomMember();
	rm->rid = op->rJid->rid;
	rm->userId = jid->userId;
	rm->aType = Member;

	TalkRoomAdapter::instance().setRoomMember(rm);
	return true;
}

void RoomInfo::_insertOccupant(const OccupantPtr& occupant) {

	OccupantIt occupantIt = _occupants.insert(_occupants.begin(), make_pair(
			occupant->rJid->nick, occupant));

	_userIds[occupant->fJid->userId][occupant->rJid->nick] = occupantIt;
}

bool RoomInfo::_removeOccupant(OccupantIt& it, bool isAll, OccupantSeq* seq) {


	int userId = it->second->fJid->userId;
	_userIds[userId].erase(it->second->rJid->nick);
	_occupants.erase(it);

	if (_userIds[userId].empty()) {
		_userIds.erase(userId);
	}
	return true;
}

pair<OccupantIt,bool> RoomInfo::_getOccupantIt(const JidPtr& jid) {
	if (jid) {
		UserIdsIt it = _userIds.find(jid->userId);
		if (it == _userIds.end()) {
			return pair<OccupantIt, bool>(_occupants.end(), false);
		}
		map<string, OccupantIt>::iterator itTmp = it->second.begin();
		for (; itTmp!=it->second.end(); ++itTmp) {
			//if (jidToString(itTmp->second->second->fJid) == jidToString(jid)) {
			if (equal(itTmp->second->second->fJid, jid)) {
				return pair<OccupantIt, bool>(itTmp->second, true);
			}
		}
	}
	return pair<OccupantIt, bool>(_occupants.end(), false);;
}

void RoomInfo::getOccupants(OccupantSeq& seq) {
	OccupantIt it = _occupants.begin();
	for (; it!=_occupants.end(); ++it) {
		seq.push_back(it->second);
	}
}

XmppErrorCode RoomInfo::addOccupant(const RoomJidPtr& roomJid, const JidPtr& jid) {

	Affiliation affiliation = TalkRoomAdapter::instance().getAffiliation(roomJid->rid, jid->userId);

	RoomRole role = _role(roomJid->rid, affiliation);
	if (role == NoneRole) {
		if (affiliation == Outcast) {
			return forbidden;
		}
		if (_room->type->memberOnly) {
			if(roomJid->rid->name != "xiaoneioffice_1234213412834194371"){
				return registration_required;
			}
		}
	}

	string nickName = _checkNickName(roomJid, jid);
	if (nickName.empty()) {
		MCE_DEBUG("RoomInfo::_checkNickName --> nick name conflict, "
				<< roomJid->nick.c_str());
		return conflict;
	}
	roomJid->nick = nickName;
	MCE_DEBUG("RoomInfo::addOccupant --> get a new nick, " << nickName);

	OccupantPtr occupant = new Occupant;

	occupant->rJid = roomJid;
	occupant->fJid = jid;
	occupant->role = role;
	occupant->aType = affiliation>Member?affiliation:Member;

	_insertOccupant(occupant);
	/*try{
		OccupantsBackupAdapter::instance().put(occupant);
	}catch(Ice::Exception& e){
		MCE_WARN("RoomInfo::addOccupant --> call backup err:"<< e);
	}*/
	if (affiliation < Member) {
		RoomMemberPtr member = new RoomMember;
		member->rid = roomJid->rid;
		member->userId = jid->userId;
		member->aType = Member;
		TalkRoomAdapter::instance().setRoomMember(member);
	}

	return none;
}

OccupantPtr RoomInfo::removeOccupant(const JidPtr& jid) {
	if (!jid) {
		return 0;
	}
	pair<OccupantIt,bool> p = _getOccupantIt(jid);
	if (!p.second) {
		return 0;
	}
	/*
	 if(roomJid && roomJid->nick != p->first->second->nick){
	 return false;
	 }*/
	OccupantPtr res = p.first->second;

	_removeOccupant(p.first);

	return res;
}

bool RoomInfo::removeOccupants(int userId, OccupantSeq& seq) {
	UserIdsIt userIt = _userIds.find(userId);
	if (userIt == _userIds.end()) {
		return false;
	}
	map<string, OccupantIt>::iterator itTmp = userIt->second.begin();
	for (; itTmp!=userIt->second.end(); ++itTmp) {
		seq.push_back(itTmp->second->second);
		_occupants.erase(itTmp->second);
	}
	_userIds.erase(userIt);
	return true;
}

bool RoomInfo::changeNick(const RoomJidPtr& roomJid, const JidPtr& jid) {
	pair<OccupantIt,bool> p = _getOccupantIt(jid);
	if (!p.second) {
		MCE_DEBUG("RoomInfo::changNick -> the jid does not exist, "
				<< jidToString(jid, true).c_str());
		return false;
	}

	string nickName = _checkNickName(roomJid, jid);
	if (nickName.empty()) {
		MCE_DEBUG("RoomInfo::_checkNickName --> nick name conflict, "
				<< roomJid->nick);
		return false;
	}
	OccupantPtr occupant = p.first->second;
	_removeOccupant(p.first);
	occupant->rJid->nick = roomJid->nick;
	//OccupantsBackupAdapter::instance().put(occupant);
	_insertOccupant(occupant);
	return true;
}
//bool 		setOccupantRole(const RoomJidPtr& roomJid, RoomRole role);
RoomRole RoomInfo::getOccupantRole(const JidPtr& jid) {
	pair<OccupantIt,bool> p = _getOccupantIt(jid);
	if (!p.second) {
		MCE_DEBUG("RoomInfo::getOccupantRole --> the user does not exist"
				<< jid->userId);
		return NoneRole;
	}
	return p.first->second->role;
}

OccupantPtr RoomInfo::getOccupant(const RoomJidPtr& roomJid) {
	OccupantIt it = _occupants.find(roomJid->nick);
	if (it == _occupants.end())
		return 0;
	return it->second;
}

OccupantPtr RoomInfo::getOccupant(const JidPtr& jid) {
	pair<OccupantIt,bool> p = _getOccupantIt(jid);
	if (!p.second) {
		MCE_DEBUG("RoomInfo::getOccupant --> the user does not exist"
				<< jid->userId);
		return 0;
	}
	return p.first->second;
}

int RoomInfo::getUserCount(bool onlyUserId){
	if(onlyUserId){
		return _userIds.size();
	}
	int sum = 0;
	UserIdsIt it = _userIds.begin();
	for(; it!=_userIds.end(); ++it){
		sum += it->second.size();
	}
	return sum;
}



////////////////////////////////////////////////////////////////////////////
//RoomInfo* _getRoom();
RoomInfoPtr OccupantsManagerI::_getRoom(const RoomIdPtr& roomId) {
	map<string, RoomsMap>::iterator it = _domains.find(roomId->domain);

	if (it != _domains.end()) {
		map<string, RoomInfoPtr>::iterator roomIt =
				it->second.find(roomId->name);
		if (roomIt != it->second.end()) {
			return roomIt->second;
		}
	}

	//OccupantSeq oseq = OccupantsBackupAdapter::instance().get(roomId);
	//MCE_DEBUG("------size:"<< oseq.size());
	/*if (!oseq.empty()) {
		RoomPtr r = TalkRoomAdapter::instance().getRoom(roomId);
		if (r) {
			RoomInfoPtr ri = new RoomInfo(roomId, r);
			ri->reload(oseq);
			_domains[roomId->domain][roomId->name] = ri;
			return ri;
		}
	}*/
	return NULL;
}

OccupantSeq OccupantsManagerI::getOccupants(const RoomIdPtr& roomId,
		const ::Ice::Current&) {
	IceUtil::RWRecMutex::RLock lock(_rwmutex);

	MCE_DEBUG("OccupantsManagerI::getOccupants --> " << roomIdToString(roomId));
	OccupantSeq seq;
	RoomInfoPtr room = _getRoom(roomId);
	if (0 == room)
		return seq;
	room->getOccupants(seq);
	//logRoom();
	//logUsers();
	return seq;
}

OccupantPtr OccupantsManagerI::getOccupant(const RoomJidPtr& roomJid,
		const ::Ice::Current&) {
	IceUtil::RWRecMutex::RLock lock(_rwmutex);

	RoomInfoPtr room = _getRoom(roomJid->rid);
	if (0 == room)
		return NULL;
	//logRoom();
	//logUsers();
	return room->getOccupant(roomJid);
}

OccupantPtr OccupantsManagerI::getOccupantByJid(const RoomIdPtr& rid,
		const JidPtr& jid, const ::Ice::Current&) {
	IceUtil::RWRecMutex::RLock lock(_rwmutex);
	RoomInfoPtr room = _getRoom(rid);
	if (!room)
		return 0;
	//logRoom();
	//logUsers();
	return room->getOccupant(jid);
}

int OccupantsManagerI::enterRoom(const RoomJidPtr& roomJid, const JidPtr& jid,
		const ::Ice::Current&) {

	IceUtil::RWRecMutex::WLock lock(_rwmutex);

	MCE_DEBUG("-----OccupantsManagerI::enterRoom----- --> enterRoom "
			<< roomJidToString(roomJid));

	if(roomJid->rid->domain!="conference.talk.xiaonei.com" && roomJid->rid->domain!="group.talk.xiaonei.com"){
		return not_allowed;
	}

	map<string, RoomsMap>::iterator it = _domains.find(roomJid->rid->domain);
	if (it != _domains.end()) {
		RoomsMap::iterator rit = it->second.find(roomJid->rid->name);
		if (rit != it->second.end()) {
			XmppErrorCode err = rit->second->addOccupant(roomJid, jid);
			//			if(none == err){
			//				UserRoomAdapter::instance().addRoomId(jid, roomJid->rid);
			//			}
			MCE_DEBUG("OccupantsManagerI::enterRoom --> user count, " << rit->second->getUserCount(true)
					<< " " << rit->second->getUserCount(false));
			return err;
		}
	}
	RoomPtr r = TalkRoomAdapter::instance().getRoom(roomJid->rid);
	if (r) {
		MCE_DEBUG("OccupantsManagerI::enterRoom --> room exist, enter");
		RoomInfoPtr ri = new RoomInfo(roomJid->rid, r);
		XmppErrorCode code = ri->addOccupant(roomJid, jid);
		if (code == none) {
			_domains[roomJid->rid->domain][roomJid->rid->name] = ri;
		}
		MCE_DEBUG("OccupantsManagerI::enterRoom --> user count, " << ri->getUserCount(true) << " " << ri->getUserCount(false));
		return code;

	} else {

		//不能随便创建固定群
		if(roomJid->rid->domain.find("group") == 0){
			return not_allowed;
		}

		MCE_DEBUG("OccupantsManagerI::enterRoom --> the room does not exist, create");
		RoomPtr r = TalkRoomAdapter::instance().createRoom(roomJid, jid);
		if (r) {
			RoomInfoPtr ri = new RoomInfo(roomJid->rid, r);
			XmppErrorCode code = ri->addOccupant(roomJid, jid);
			MCE_DEBUG("Enter Room error code ->" << code);
			if (code == none) {
				//UserRoomAdapter::instance().addRoomId(jid, roomJid->rid);
				_domains[roomJid->rid->domain][roomJid->rid->name] = ri;
			}
			MCE_DEBUG("OccupantsManagerI::enterRoom --> user count, " << ri->getUserCount(true) << " " << ri->getUserCount(false));
			return success;
		}
		return service_unavailable;
	}

}

OccupantPtr OccupantsManagerI::exitRoom(const RoomIdPtr& roomId,
		const JidPtr& jid, const ::Ice::Current&) {

	IceUtil::RWRecMutex::WLock lock(_rwmutex);
	MCE_DEBUG("-----OccupantsManagerI::exitRoom----- --> "
				<< roomIdToString(roomId) << ", " << jidToString(jid));
	map<string, RoomsMap>::iterator it = _domains.find(roomId->domain);
	if (it == _domains.end()) {
		return 0;
	}
	map<string, RoomInfoPtr>::iterator roomIt = it->second.find(roomId->name);
	if (roomIt == it->second.end()) {
		return 0;
	}

	OccupantPtr oPtr = roomIt->second->removeOccupant(jid);
	//OccupantsBackupAdapter::instance().remove(roomId,jid);
	if (!oPtr) {
		return 0;
	}

	if (roomIt->second->occupantEmpty() && roomIt->second->getRoom()->type->persistent == 0) {
		it->second.erase(roomIt);
		MCE_DEBUG("OccupantsManagerI::exitRoom --> remove room" << roomIdToString(roomId));
		TalkRoomAdapter::instance().removeRoom(roomId);
		//OccupantsBackupAdapter::instance().removeAll(roomId);
	}
	//logRoom();
	//logUsers();
	return oPtr;
}

bool OccupantsManagerI::changeNick(const RoomJidPtr& roomJid,
		const JidPtr& jid, const ::Ice::Current&) {

	IceUtil::RWRecMutex::WLock lock(_rwmutex);
	//logRoom();
	//logUsers();

	RoomInfoPtr room = _getRoom(roomJid->rid);
	if (0 == room)
		return false;
	bool success = room->changeNick(roomJid, jid);
	//logRoom();
	//logUsers();
	return success;
}

OccupantSeq OccupantsManagerI::setRole(const JidPtr& opJid,
		const RoomJidPtr& targetRJid, RoomRole role, const ::Ice::Current&) {
	IceUtil::RWRecMutex::WLock lock(_rwmutex);
	//logRoom();
	//logUsers();

	if (!targetRJid || !targetRJid->rid || !opJid) {
		MCE_DEBUG("OccupantsManagerI::setPrivilege --> invalid parameter");
		return OccupantSeq();
	}
	RoomInfoPtr room = _getRoom(targetRJid->rid);
	if (0 == room) {
		MCE_DEBUG("OccupantsManagerI::setPrivilege --> can not find room");
		return OccupantSeq();
	}

	OccupantSeq oseq = room->modifyRole(opJid, targetRJid, role);
	/*if (!oseq.empty()) {
		OccupantsBackupAdapter::instance().puts(oseq);
	}*/
	return oseq;
}

OccupantSeq OccupantsManagerI::setAffiliation(const JidPtr& opJid,
		const RoomMemberPtr& member, const Ice::Current&) {
	IceUtil::RWRecMutex::WLock lock(_rwmutex);
	RoomInfoPtr room = _getRoom(member->rid);
	if (0 == room) {
		return OccupantSeq();
	}

	OccupantSeq oseq = room->modifyAffiliation(opJid, member->userId,
			member->aType);
	/*if (!oseq.empty()) {
		OccupantsBackupAdapter::instance().puts(oseq);
	}*/
	return oseq;
}

OccupantSeq OccupantsManagerI::destroyRoom(const JidPtr& opJid,
		const RoomIdPtr& rid, const Ice::Current&) {
	IceUtil::RWRecMutex::WLock lock(_rwmutex);
	MCE_DEBUG("------OccupantsManagerI::destroyRoom--------");

	//	map<string, RoomsMap>::iterator it = _domains.find(rid->domain);
	//
	//	if (it == _domains.end()) {
	//		return OccupantSeq();
	//	}
	//	map<string, RoomInfoPtr>::iterator roomIt = it->second.find(rid->name);
	RoomInfoPtr rInfo = _getRoom(rid);

	if (!rInfo) {
		return OccupantSeq();
	}

	OccupantPtr occupant = rInfo->getOccupant(opJid);
	if (!occupant || occupant->aType != Owner) {
		return OccupantSeq();
	}
	try {
		TalkRoomAdapter::instance().removeRoom(rid);
	} catch(Ice::Exception& e) {
		MCE_WARN("OccupantsManagerI::destroyRoom --> invoke TalkRoom removeRoom error:"<< e);
	}
	/*try {
		OccupantsBackupAdapter::instance().removeAll(rid);
	} catch(Ice::Exception& e) {
		MCE_WARN("OccupantsManagerI::destroyRoom --> call backup remove err:"<<e);
	}*/

	OccupantSeq seq;
	rInfo->getOccupants(seq);
	//rInfo.erase(roomIt);

	map<string, RoomsMap>::iterator it = _domains.find(rid->domain);

	if (it != _domains.end()) {
		map<string, RoomInfoPtr>::iterator roomIt = it->second.find(rid->name);
		if (roomIt != it->second.end()) {
			it->second.erase(roomIt);
		}
	}

	return seq;
}

OccupantSeq OccupantsManagerI::setRoomConfig(const JidPtr& opJid,
		const RoomIdPtr& rid, const RoomPtr& room, const ::Ice::Current&) {
	IceUtil::RWRecMutex::WLock lock(_rwmutex);
	MCE_DEBUG("----OccupantsManagerI::setRoomConfig----");
	RoomInfoPtr roomInfo = _getRoom(rid);
	if (!roomInfo || !room)
		return OccupantSeq();
	OccupantPtr occupant = roomInfo->getOccupant(opJid);
	if (!occupant || occupant->aType != Owner) {
		return OccupantSeq();
	}
	bool success = false;
	try {
		success = TalkRoomAdapter::instance().setRoom(rid, room);
	} catch(Ice::Exception& e) {
		MCE_DEBUG(__FUNCTION__ << " --> invoke TalkRoom setRoom err :"<<e);
	} catch(...) {
		MCE_DEBUG(__FUNCTION__ << " --> invoke TalkRoom setRoom error");
		return OccupantSeq();
	}
	if (!success) {
		return OccupantSeq();
	}
	OccupantSeq oseq = roomInfo->modifyRoom(room);
	/*if (!oseq.empty()) {
		OccupantsBackupAdapter::instance().puts(oseq);
	}*/
	return oseq;
}
bool OccupantsManagerI::invite(const JidPtr& opJid, const RoomIdPtr& rid, const JidPtr& jid, const Ice::Current&){
	IceUtil::RWRecMutex::RLock lock(_rwmutex);
	MCE_DEBUG("----OccupantsManagerI::invite----");
	RoomInfoPtr rInfo = _getRoom(rid);
	if (!rInfo) {
		return false;
	}
	return rInfo->invite(opJid, jid);
}

int	OccupantsManagerI::getRoomCount(const Ice::Current&){
	IceUtil::RWRecMutex::RLock lock(_rwmutex);
	int sum = 0;
	DomainsMapIt it = _domains.begin();
	for(; it!=_domains.end(); ++it){
		sum += it->second.size();
	}
	return sum;
}

int OccupantsManagerI::getUserCount(const Ice::Current&){
	IceUtil::RWRecMutex::RLock lock(_rwmutex);
	int sum = 0;
	DomainsMapIt it = _domains.begin();
	for(; it!=_domains.end(); ++it){
		RoomsMapIt roomIt = it->second.begin();
		for(; roomIt!=it->second.end(); ++roomIt){
			sum += roomIt->second->getUserCount();
		}
	}
	return sum;
}






