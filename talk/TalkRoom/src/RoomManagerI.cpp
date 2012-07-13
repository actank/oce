#include "RoomManagerI.h"
#include "ServiceI.h"
#include "util/cpp/String.h"
#include "XmppTools.h"
#include "ConnectionQuery.h"
#include "QueryRunner.h"
#include "XmppErrorCode.h"
#include "OccupantsAdapter.h"
#include "UserRoomAdapter.h"

using namespace talk::userroom::adapter;
using namespace talk::room;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::xce;
using namespace MyUtil;
using namespace talk::error;
using namespace talk::occupants::adapter;

RoomPtr RoomCluster::get(const RoomIdPtr& rid) {
	//IceUtil::Mutex::Lock lock(_mutex);
	RoomMap::iterator it = _roomMap.find(rid);
	if (it != _roomMap.end()) {
		return it->second;
	}
	return 0;
}
bool RoomCluster::add(const RoomIdPtr& rid, const RoomPtr& room) {
	//IceUtil::Mutex::Lock lock(_mutex);
	return _roomMap.insert(make_pair(rid,room)).second;
}
void RoomCluster::remove(const RoomIdPtr& rid) {
	//IceUtil::Mutex::Lock lock(_mutex);
	RoomMap::iterator it = _roomMap.find(rid);
	if (it != _roomMap.end()) {
		_roomMap.erase(it);
	}
}
void RoomCluster::set(const RoomIdPtr& rid, const RoomPtr& room) {
	//IceUtil::Mutex::Lock lock(_mutex);
	_roomMap[rid] = room;
}
//---------------------------------------
void MemberList::remove(const RoomIdPtr& rid) {
	map<RoomIdPtr, map<Affiliation, set<int> > >::iterator rit =
			_members.find(rid);
	if (rit != _members.end()) {
		_members.erase(rit);
	}
}

IntSeq MemberList::getMemberList(const RoomIdPtr& rid, Affiliation affiliation) {
	//IceUtil::Mutex::Lock lock(_mutex);
	map<RoomIdPtr, map<Affiliation, set<int> > >::iterator rit =
			_members.find(rid);
	if (rit == _members.end()) {
		return IntSeq();
	}

	map<Affiliation, set<int> >::iterator it = rit->second.find(affiliation);
	if (it == rit->second.end()) {
		return IntSeq();
	}
	IntSeq seq;
	for (set<int>::iterator sit = it->second.begin(); sit != it->second.end(); ++sit) {
		seq.push_back(*sit);
	}
	return seq;
}

Int2IntSeqMap MemberList::getMemberList(const RoomIdPtr& rid,
		const vector<Affiliation>& affiliations) {
	map<RoomIdPtr, map<Affiliation, set<int> > >::iterator rit =
			_members.find(rid);
	if (rit == _members.end()) {
		return Int2IntSeqMap();
	}

	Int2IntSeqMap res;
	if (affiliations.empty()) {
		for (map<Affiliation, set<int> >::iterator it = rit->second.begin(); it
				!= rit->second.end(); ++it) {
			for (set<int>::iterator sit = it->second.begin(); sit
					!= it->second.end(); ++sit) {
				res[it->first].push_back(*sit);
			}
		}
	}

	for (size_t i = 0; i < affiliations.size(); ++i) {
		map<Affiliation, set<int> >::iterator it =
				rit->second.find(affiliations.at(i));
		for (set<int>::iterator sit = it->second.begin(); sit
				!= it->second.end(); ++sit) {
			res[affiliations.at(i)].push_back(*sit);
		}
	}
	return res;
}

Affiliation MemberList::getAffiliation(const RoomIdPtr& rid, int userid) {
	//IceUtil::Mutex::Lock lock(_mutex);

	map<RoomIdPtr, map<Affiliation, set<int> > >::iterator rit =
			_members.find(rid);
	if (rit != _members.end()) {

		for (map<Affiliation, set<int> >::iterator it = rit->second.begin(); it
				!= rit->second.end(); ++it) {
			if (it->second.find(userid) != it->second.end()) {
				MCE_DEBUG("MemberList::getAffiliation --> " << it->first);
				return it->first;
			}
		}
	}
	return None;
}
void MemberList::changeMember(const RoomIdPtr& rid, Affiliation affiliation,
		int userid) {
	//IceUtil::Mutex::Lock lock(_mutex);

	map<RoomIdPtr, map<Affiliation, set<int> > >::iterator rit =
			_members.find(rid);
	if (rit != _members.end()) {
		for (map<Affiliation, set<int> >::iterator it = rit->second.begin(); it
				!= rit->second.end(); ++it) {
			set<int>::iterator sit = it->second.find(userid);
			if (sit != it->second.end()) {
				it->second.erase(sit);
				break;
			}
		}
		if (affiliation != None) {
			rit->second[affiliation].insert(userid);
			return;
		}
	}
	if (affiliation != None) {
		_members[rid][affiliation].insert(userid);
	}
}

void MemberList::getMembers(const RoomIdPtr& rid, RoomMemberSeq& seq) {
	map<RoomIdPtr, map<Affiliation, set<int> > >::iterator rit =
			_members.find(rid);
	if (rit != _members.end()) {
		for (map<Affiliation, set<int> >::iterator it = rit->second.begin(); it
				!= rit->second.end(); ++it) {
			set<int>::iterator sit = it->second.begin();
			for (; sit!=it->second.end(); ++sit) {
				RoomMemberPtr member = new RoomMember;
				member->rid = rit->first;
				member->aType = it->first;
				member->userId = *sit;
				seq.push_back(member);
			}
		}
	}
}
//---------------------------------------
/*
bool RoomList::addRoom(const RoomIdPtr& rid) {
	return _rooms.insert(rid).second;
}

void RoomList::removeRoom(const RoomIdPtr& rid) {
	set<RoomIdPtr>::iterator it = _rooms.find(rid);
	if (it != _rooms.end()) {
		_rooms.erase(it);
	}
}

set<RoomIdPtr>& RoomList::getRooms() {
	return _rooms;
}*/

//----------------------------------------
Ice::ObjectPtr RoomFactoryI::create(Ice::Long hashcode) {
	Statement sql;
	sql << "select * from room where hashcode = "<< hashcode;
//	MCE_DEBUG("RoomFactoryI::create --> " << sql);
	try {
#ifndef NEWARCH
		mysqlpp::Result res = QueryRunner("im_groupchat", CDbRServer).store(sql);
#else
		mysqlpp::StoreQueryResult res = QueryRunner("im_groupchat", CDbRServer).store(sql);
#endif
		MCE_DEBUG("RoomFactoryI::create --> load room from database, count=" << res.size());

		if(!res){
			return 0;
		}
		RoomClusterPtr rc = new RoomCluster();
#ifndef NEWARCH
		for (size_t i= 0; i<res.size(); ++i) {
#else
		for (size_t i= 0; i<res.num_rows(); ++i) {
#endif
			mysqlpp::Row row = res.at(i);
#ifndef NEWARCH
			RoomIdPtr rid = stringToRoomId(row["roomid"].get_string());
#else
			RoomIdPtr rid = stringToRoomId(row["roomid"].c_str());
#endif

			RoomPtr r = new Room();
#ifndef NEWARCH
			r->title = row["title"].get_string();
			r->descr = row["descr"].get_string();
			r->password = row["password"].get_string();
#else
			r->title = row["title"].c_str();
			r->descr = row["descr"].c_str();
			r->password = row["password"].c_str();
#endif
			r->maxOccupantsCount = (int)row["max_occupants_count"];
			r->allowUserToInvite = (bool)(int)row["allow_user_to_invite"];
			r->type = new RoomType();
			r->type->hidden = (bool)(int)row["hidden"];
			r->type->memberOnly = (bool)(int)row["member_only"];
			r->type->moderated = (bool)(int)row["moderated"];
			r->type->anonymousType = (int)row["anonymous_type"];
			r->type->secured = (bool)(int)row["secured"];
			r->type->persistent = (bool)(int)row["persistent"];

			rc->add(rid,r);
		}
		return rc;
	} catch(Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
	} catch (std::exception& e) {
		MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
	}

	return 0;
}

Ice::ObjectPtr MemeberFactoryI::create(Ice::Long hashcode) {
	Statement sql;
	sql << "select * from room_member where hashcode = "<< hashcode;
	//MCE_DEBUG("RoomFactoryI::create --> " << sql);
	try {
#ifndef NEWARCH
		mysqlpp::Result res = QueryRunner("im_groupchat", CDbRServer).store(sql);
#else
		mysqlpp::StoreQueryResult res = QueryRunner("im_groupchat", CDbRServer).store(sql);
#endif
		if(!res){
			return 0;
		}
		MemberListPtr ml = new MemberList();
#ifndef NEWARCH
		for (size_t i= 0; i < res.size(); ++i) {
#else
		for (size_t i= 0; i < res.num_rows(); ++i) {
#endif
			mysqlpp::Row row = res.at(i);
#ifndef NEWARCH
			RoomIdPtr rid = stringToRoomId(row["roomid"].get_string());
#else
			RoomIdPtr rid = stringToRoomId(row["roomid"].c_str());
#endif
			int userid = (int) row["userid"];
			Affiliation affiliation = (Affiliation)(int)row["affiliation"];
			ml->changeMember(rid,affiliation,userid);
		}

		return ml;
	} catch(Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
	} catch (std::exception& e) {
		MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
	}
	return 0;
}
/*
Ice::ObjectPtr RoomListFactoryI::create(Ice::Long userId) {
	Statement sql;
	sql << "select roomid from room_member where userid = "<< userId;
	try {
		mysqlpp::Result res = QueryRunner("im_groupchat", CDbRServer).store(sql);

		RoomListPtr rl = new RoomList();
		for (size_t i= 0; i < res.size(); ++i) {
			mysqlpp::Row row = res.at(i);
			RoomIdPtr rid = stringToRoomId(row["roomid"].get_string());
			rl->addRoom(rid);
		}
		return rl;
	} catch(Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
	} catch (std::exception& e) {
		MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
	}
	return 0;
}*/

//---------------------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(ROOM_CLUSTER, "RC", new RoomFactoryI);
	service.registerObjectCache(MEMBER_LIST, "ML", new MemeberFactoryI);
	//service.registerObjectCache(ROOM_LIST, "RL", new RoomListFactoryI);

	service.getAdapter()->add(&RoomManagerI::instance(), service.createIdentity("M", ""));
}

RoomPtr RoomManagerI::createRoom(const RoomJidPtr& rJid, const JidPtr & jid,
		const Ice::Current&) {
	IceUtil::RWRecMutex::WLock lock(_mutex);

	MCE_DEBUG("RoomManagerI::roomPresence --> " << roomJidToString(rJid) << " "
			<< jidToString(jid, true));

	if (!rJid) {
		return 0;
	}
	if (rJid->rid->name == "" || rJid->rid->domain == "" || rJid->nick == "") {
		return 0;
	}

	Ice::Long hashcode = StrUtil::hashcode(roomIdToString(rJid->rid));
	RoomClusterPtr rc = ServiceI::instance().locateObject<RoomClusterPtr>(ROOM_CLUSTER, hashcode);
	if (!rc) {
		return 0;
	}
	RoomPtr r = rc->get(rJid->rid);
	if (r) {
		return r;
	} else {

		r = new Room;
		r->maxOccupantsCount = MAX_OCCUPANTS_COUNT;
		r->allowUserToInvite = true;
		r->type = new RoomType;

		if (rJid->rid->domain == "conference.talk.xiaonei.com") {
			r->type->hidden = 0;
			r->type->memberOnly = 1;
			r->type->moderated = 1;
			r->type->anonymousType = 0;
			r->type->secured = 0;
			r->type->persistent = 0;
		} else if (rJid->rid->domain == "group.talk.xiaonei.com"){
			r->allowUserToInvite = false;
			r->type->hidden = 0;
			r->type->memberOnly = 1;
			r->type->moderated = 1;
			r->type->anonymousType = 0;
			r->type->secured = 0;
			r->type->persistent = 1;
		}else {
			return 0;
			/*r->type->hidden = 1;
			r->type->memberOnly = 0;
			r->type->moderated = 1;
			r->type->anonymousType = 0;
			r->type->secured = 0;
			r->type->persistent = 1;*/
		}
		ConnectionHolder conn("im_groupchat", CDbRServer," ");
		mysqlpp::Query query = conn.query();
		//Statement sql;
		query
				<< "insert into room (hashcode,roomid,title,descr,hidden,member_only,moderated,anonymous_type,secured,persistent,password,max_occupants_count,allow_user_to_invite) values("
				<< hashcode << "," << mysqlpp::quote << roomIdToString(rJid->rid).c_str() << "," << mysqlpp::quote << r->title.c_str()
				<< "," << mysqlpp::quote << r->descr.c_str() << ","
				<< (int)r->type->hidden << "," << (int)r->type->memberOnly
				<< "," << (int)r->type->moderated << ","
				<< r->type->anonymousType << "," << (int)r->type->secured
				<< "," << (int)r->type->persistent << "," << mysqlpp::quote
				<< r->password.c_str() << "," << r->maxOccupantsCount << ","
				<< (int)r->allowUserToInvite << ")";


		try {
			//MCE_DEBUG("RoomManagerI::roomPresence --> " << sql);

			//QueryRunner("im_groupchat", CDbWServer).store(sql);
			query.store();

			rc->add(rJid->rid, r);

			RoomMemberPtr rm = new RoomMember;
			rm-> rid = rJid->rid;
			rm-> userId = jid->userId;
			rm-> aType = Owner;
			setRoomMember(rm);

			MCE_DEBUG("RoomManagerI::roomPresence --> invoke enterRoom affiliation=" << Owner);
			MCE_DEBUG("RoomManagerI::roomPresence --> room type, hidden="
					<< r->type->hidden
					<< ", memberOnly=" << r->type->memberOnly
					<< ", moderated=" << r->type->moderated
					<< ", anonymousType=" << r->type->anonymousType
					<< ", secured=" << r->type->secured
					<< ", persistent=" << r->type->persistent
			);

			return r;

		}
		catch(Ice::Exception& e) {
			MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
		}
		catch (std::exception& e) {
			MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
		}
		catch (...) {
			MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
		}
	}
	return 0;
}

void RoomManagerI::removeRoom(const RoomIdPtr& rid, const Ice::Current&) {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	MCE_DEBUG("RoomManagerI::removeRoom --> remove room, "
			<< roomIdToString(rid));

	Ice::Long hashcode = StrUtil::hashcode(roomIdToString(rid));
	RoomClusterPtr rc = ServiceI::instance().findObject<RoomClusterPtr>(ROOM_CLUSTER, hashcode);
	if (!rc) {
		return;
	}
	rc->remove(rid);

	MemberListPtr ml = ServiceI::instance().locateObject<MemberListPtr>(MEMBER_LIST, hashcode);
	if (!ml) {
		return;
	}

	Int2IntSeqMap memMap = ml->getMemberList(rid);
/*	for (Int2IntSeqMap::iterator it = memMap.begin(); it != memMap.end(); ++it) {
		for (size_t i = 0; i < it->second.size(); ++i) {
			RoomListPtr rl = ServiceI::instance().findObject<RoomListPtr>(ROOM_LIST, it->second.at(i));
			if (rl) {
				rl->removeRoom(rid);
			}
		}
	}
*/
	ml->remove(rid);

	Statement sql;
	sql << "delete from room where roomid='" << roomIdToString(rid) << "'";
	Statement sql2;
	sql2 << "delete from room_member where roomid='" << roomIdToString(rid)
			<< "'";

	try {
		QueryRunner("im_groupchat", CDbWServer).store(sql);
		QueryRunner("im_groupchat", CDbWServer).store(sql2);
	} catch(Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
	} catch (std::exception& e) {
		MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
	}
	MyUtil::IntSeq userIds;
	for (Int2IntSeqMap::iterator it = memMap.begin(); it != memMap.end(); ++it) {
		if(it->first < Member){
			continue;
		}
		userIds.insert(userIds.end(), it->second.begin(), it->second.end());
	}
	UserRoomAdapter::instance().removeRoomIds(userIds, rid);
}

bool RoomManagerI::setRoom(const RoomIdPtr& rid, const RoomPtr& room,
		const Ice::Current&) {
	IceUtil::RWRecMutex::WLock lock(_mutex);
	MCE_DEBUG(__FUNCTION__);
	if (!rid || !room) {
		return false;
	}
	if (rid->name == "" || rid->domain == "") {
		return false;
	}

	Ice::Long hashcode = StrUtil::hashcode(roomIdToString(rid));
	RoomClusterPtr rc = ServiceI::instance().locateObject<RoomClusterPtr>(ROOM_CLUSTER, hashcode);

	if (!rc || 0 == rc->get(rid)) {
		return false;
	}
	//Statement sql;
	ConnectionHolder conn("im_groupchat", CDbRServer," ");
	mysqlpp::Query query = conn.query();
	query
		<< "update room set "
		<< "title=" << mysqlpp::quote << room->title
		<< ",descr=" << mysqlpp::quote << room->descr
		<< ",hidden=" << (int)room->type->hidden
		<< ",member_only=" << (int)room->type->memberOnly
		<< ",moderated=" << (int)room->type->moderated
		<< ",anonymous_type=" << room->type->anonymousType
		<< ",secured=" << (int)room->type->secured
		<< ",persistent=" << (int)room->type->persistent
		<< ",password=" << mysqlpp::quote << room->password
		<< ",max_occupants_count=" << room->maxOccupantsCount
		<< ",allow_user_to_invite="<< (int)room->allowUserToInvite
		<< " where roomid="  << mysqlpp::quote << roomIdToString(rid);
	//MCE_DEBUG("write to database, " << sql);
	try {
		query.store();
		//QueryRunner("im_groupchat", CDbWServer).store(sql);
	}catch (std::exception& e) {
		MCE_WARN(__FUNCTION__ << "-->" << e.what() << " at " << __FILE__ << ":" << __LINE__);
		return false;
	} catch (...) {
		MCE_WARN(__FUNCTION__ << "-->" "Unkown exception at " << __FILE__ << ":" << __LINE__);
		return false;
	}
	rc->remove(rid);
	return rc->add(rid, room);
}

RoomPtr RoomManagerI::getRoom(const RoomIdPtr& rid, const Ice::Current&) {
	IceUtil::RWRecMutex::RLock lock(_mutex);

	MCE_DEBUG("RoomManagerI::getRoom --> " << roomIdToString(rid));
	if (!rid) {
		return 0;
	}
	if (rid->name == "" || rid->domain == "") {
		return 0;
	}

	Ice::Long hashcode = StrUtil::hashcode(roomIdToString(rid));
	RoomClusterPtr rc = ServiceI::instance().locateObject<RoomClusterPtr>(ROOM_CLUSTER, hashcode);
	if (!rc) {
		MCE_DEBUG("RoomManagerI::getRoom --> locateObject error");
		return NULL;
	}
	return rc->get(rid);
}
/*
RoomSeq RoomManagerI::getRoomSeq(Ice::Int userid, const Ice::Current&) {
	IceUtil::RWRecMutex::RLock lock(_mutex);
//	RoomListPtr rl = ServiceI::instance().locateObject<RoomListPtr>(ROOM_LIST, userid);
	RoomSeq seq;
	set<RoomIdPtr>::iterator it = rl->getRooms().begin();
	for (; it!=rl->getRooms().end(); ++it) {
		RoomClusterPtr rc = ServiceI::instance().findObject<RoomClusterPtr>(ROOM_LIST, userid);
		if (!rc) {
			continue;
		}
		RoomPtr r = rc->get(*it);
		if (!r) {
			continue;
		}
		seq.push_back(r);
	}
	return seq;
}*/

void RoomManagerI::setRoomMember(const RoomMemberPtr & member,
		const Ice::Current&) {
	IceUtil::RWRecMutex::WLock lock(_mutex);

	MCE_DEBUG("RoomManagerI::addRoomMember --> affiliation=" << member->aType);

	if (!member) {
		return;
	}
	if (!member->rid || member->rid->name == "" || member->rid->domain == "") {
		MCE_DEBUG("RoomManagerI::_addMember --> parameter error");
		return;
	}
	Ice::Long hashcode = StrUtil::hashcode(roomIdToString(member->rid));

	MCE_DEBUG("RoomManagerI::addRoomMember --> write to database");
	Statement sql;
	if (None == member->aType) {
		sql << "delete from room_member where roomid='"
				<< roomIdToString(member->rid) << "' and userid="
				<< member->userId;
	} else {
		sql
				<< "insert into room_member(roomid, userid, affiliation, hashcode) values( "
				<< "'" << roomIdToString(member->rid) << "'," << member->userId
				<< "," << member->aType << "," << hashcode << ")"
				<< "on duplicate key update affiliation = values(affiliation)";
	}

	try {
#ifndef NEWARCH
		mysqlpp::Result res = QueryRunner("im_groupchat", CDbWServer).store(sql);
#else
		mysqlpp::StoreQueryResult res = QueryRunner("im_groupchat", CDbWServer).store(sql);
#endif
	} catch(Ice::Exception& e) {
		MCE_WARN(e << " at " << __FILE__ << ":" << __LINE__);
		return;
	} catch (std::exception& e) {
		MCE_WARN(e.what() << " at " << __FILE__ << ":" << __LINE__);
		return;
	} catch (...) {
		MCE_WARN("Unkown exception at " << __FILE__ << ":" << __LINE__);
		return;
	}

	MemberListPtr ml = ServiceI::instance().findObject<MemberListPtr>(MEMBER_LIST, hashcode);
	if (ml) {
		ml->changeMember(member->rid, member->aType, member->userId);
	}

/*	RoomListPtr rl = ServiceI::instance().findObject<RoomListPtr>(ROOM_LIST, member->userId);
	if (rl) {
		if (None == member->aType)
			rl->removeRoom(member->rid);
		else
			rl->addRoom(member->rid);
	}*/
	if(member->aType >= Member){
		try{
			UserRoomAdapter::instance().addRoomId(member->userId, member->rid);
		}catch(Ice::Exception& e){
			MCE_WARN(e << " at " << __FILE__ << " "<<__LINE__);
		}
	}else{
		try{
			UserRoomAdapter::instance().removeRoomId(member->userId, member->rid);
		}catch(Ice::Exception& e){
			MCE_WARN(e << " at " << __FILE__ << " "<<__LINE__);
		}
	}
	return;
}

Affiliation RoomManagerI::getAffiliation(const RoomIdPtr& rid, int userid,
		const Ice::Current&) {

	Ice::Long hashcode = StrUtil::hashcode(roomIdToString(rid));
	MCE_DEBUG(1);
	MemberListPtr ml = ServiceI::instance().locateObject<MemberListPtr>(MEMBER_LIST, hashcode);
	MCE_DEBUG(2);
	if (!ml) {
		MCE_DEBUG("RoomManagerI::getAffiliation --> locateObject error");
		return None;
	}
	MCE_DEBUG(3);
	return ml->getAffiliation(rid, userid);
}

IntSeq RoomManagerI::getRoomMembers(const RoomIdPtr& rid, Affiliation aType,
		const Ice::Current&) {
	IceUtil::RWRecMutex::RLock lock(_mutex);

	MCE_DEBUG("RoomManagerI::getRoomMembers --> " << roomIdToString(rid) << " " << aType);
	if (!rid) {
		return IntSeq();
	}
	if (rid->name == "" || rid->domain == "") {
		return IntSeq();
	}
	Ice::Long hashcode = StrUtil::hashcode(roomIdToString(rid));
	MemberListPtr ml = ServiceI::instance().locateObject<MemberListPtr>(MEMBER_LIST, hashcode);
	if (!ml) {
		return IntSeq();
	}
	return ml->getMemberList(rid, aType);
}
/*
RoomMemberPtr RoomManagerI::getRoomMember(const RoomIdPtr& rid,
		Ice::Int userId, const Ice::Current&) {
	IceUtil::RWRecMutex::RLock lock(_mutex);

	return _getRoomMember(rid, userId);
}*/
/*
RoomMemberPtr RoomManagerI::_getRoomMember(const RoomIdPtr& rid, Ice::Int userId) {

	if (!rid) {
		return 0;
	}
	if (rid->name == "" || rid->domain == "") {
		return 0;
	}
	Ice::Long hashcode = StrUtil::hashcode(roomIdToString(rid));
	MemberListPtr ml = ServiceI::instance().locateObject<MemberListPtr>(MEMBER_LIST, hashcode);
	if (!ml) {
		return 0;
	}
	RoomMemberPtr rm = new RoomMember();
	rm->rid = rid;
	rm->userId = userId;
	rm->aType = ml->getAffiliation(rid, userId);
	return rm;
}*/
