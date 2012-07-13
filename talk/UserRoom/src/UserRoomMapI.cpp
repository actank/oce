/*
 * UserRoomMap.cpp
 *
 *  Created on: Dec 23, 2008
 *      Author: yaunfei
 */
#include "UserRoomMapI.h"
#include "util/cpp/String.h"
#include "ServiceI.h"
#include "ConnectionQuery.h"
#include "QueryRunner.h"

using namespace std;
using namespace MyUtil;
using namespace talk::userroom;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::xce;

const int ROOMID_LIST = 0;

void MyUtil::initialize() {

	ServiceI& service = ServiceI::instance();
	service.registerObjectCache(ROOMID_LIST, "RL", new RoomIDListFactoryI);
	service.getAdapter()->add( &UserRoomMapI::instance(), service.createIdentity("M",
			""));
}

Ice::ObjectPtr RoomIDListFactoryI::create(Ice::Long userId) {
	Statement sql;
	sql << "select roomid from room_member where userid="<< userId << " and affiliation>=" << Member;
	try {
#ifndef NEWARCH
		mysqlpp::Result res = QueryRunner("im_groupchat", CDbRServer).store(sql);
#else
		mysqlpp::StoreQueryResult res = QueryRunner("im_groupchat", CDbRServer).store(sql);
#endif
		RoomIdListPtr rl = new RoomIdList();
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
			rl->addRoomId(rid);
		}
		return rl;
	} catch(Ice::Exception& e) {
		MCE_WARN(__FUNCTION__ << " --> " << e << " at " << __FILE__ << ":" << __LINE__);
	} catch (std::exception& e) {
		MCE_WARN(__FUNCTION__ << " --> " << e.what() << " at " << __FILE__ << ":" << __LINE__);
	} catch (...) {
		MCE_WARN(__FUNCTION__ << " --> " << "Unkown exception at " << __FILE__ << ":" << __LINE__);
	}
	return 0;
}

//------------------------------------------------------------------------
void RoomIdList::addRoomId(const RoomIdPtr& rid){
	_roomIds[rid->domain].insert(rid);
}

RoomIdSeq RoomIdList::getRoomIds(string domain){
	RoomIdSeq seq;
	if(!domain.empty()){
		map<string, set<RoomIdPtr> >::iterator it = _roomIds.find(domain);
		if(it==_roomIds.end()){
			return seq;
		}
		set<RoomIdPtr>::iterator roomIdIt = it->second.begin();
		for(; roomIdIt!=it->second.end(); ++roomIdIt){
			seq.push_back(*roomIdIt);
		}
	}else{
		map<string, set<RoomIdPtr> >::iterator it = _roomIds.begin();
		for(; it!=_roomIds.end(); ++it){
			set<RoomIdPtr>::iterator roomIdIt = it->second.begin();
			for(; roomIdIt!=it->second.end(); ++roomIdIt){
				seq.push_back(*roomIdIt);
			}
		}
	}
	return seq;
}

void RoomIdList::removeRoomId(const RoomIdPtr& rid){
	_roomIds[rid->domain].erase(rid);
}
			//RoomIdSeq   getRoomIds(const JidPtr& jid, const string& domain, Ice::Current& = Ice::Current());



RoomIdSeq UserRoomMapI::getRoomIds(int userId, const string& domain, const Ice::Current&){
	

	MCE_DEBUG(__FUNCTION__ << " --> userId=" << userId);
	RoomIdListPtr rl = ServiceI::instance().locateObject<RoomIdListPtr>(ROOMID_LIST, userId);
	
	IceUtil::RWRecMutex::RLock lock(_rwmutex);
	if (!rl) {
		return RoomIdSeq();
	}
	return rl->getRoomIds(domain);
}

void UserRoomMapI::addRoomId(int userId, const RoomIdPtr& rid, const Ice::Current&){
	
	MCE_DEBUG(__FUNCTION__);
	RoomIdListPtr rl = ServiceI::instance().findObject<RoomIdListPtr>(ROOMID_LIST, userId);
	
	if (rl) {
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		rl->addRoomId(rid);
	}
	//ServiceI::instance().locateObject<RoomIdListPtr>(ROOMID_LIST, userId);
}

void UserRoomMapI::removeRoomId(int userId, const RoomIdPtr& rid, const Ice::Current&){
	
	MCE_DEBUG(__FUNCTION__);
	RoomIdListPtr rl = ServiceI::instance().findObject<RoomIdListPtr>(ROOMID_LIST, userId);
	
	if (rl) {
		IceUtil::RWRecMutex::WLock lock(_rwmutex);
		rl->removeRoomId(rid);
	}
}

void UserRoomMapI::removeRoomIds(const MyUtil::IntSeq& userIds, const RoomIdPtr& rid, const Ice::Current&){
	
	MCE_DEBUG(__FUNCTION__);
	for(size_t i=0; i<userIds.size(); i++){
		RoomIdListPtr rl = ServiceI::instance().findObject<RoomIdListPtr>(ROOMID_LIST, userIds.at(i));
		if (rl) {
			IceUtil::RWRecMutex::WLock lock(_rwmutex);
			rl->removeRoomId(rid);
		}
	}
}


