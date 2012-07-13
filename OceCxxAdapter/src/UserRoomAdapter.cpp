/*
 * UserRoomAdapter.cpp
 *
 *  Created on: Dec 23, 2008
 *      Author: yaunfei
 */

#include "UserRoomAdapter.h"
#include "util/cpp/String.h"
#include "XmppTools.h"

using namespace com::xiaonei::talk::common;
using namespace talk::userroom::adapter;
using namespace MyUtil;
using namespace com::xiaonei::talk::util;


UserRoomMapPrx UserRoomAdapter::getManager(int id) {
	return locate<UserRoomMapPrx>(_managers, "M", id, TWO_WAY,1000);
}

UserRoomMapPrx UserRoomAdapter::getManagerOneway(int id) {
	return locate<UserRoomMapPrx>(_managersOneway, "M", id, ONE_WAY);
}

RoomIdSeq UserRoomAdapter::getRoomIds(int userId, const string& domain){
	return getManager(userId)->getRoomIds(userId, domain);
}

void UserRoomAdapter::addRoomId(int userId, const RoomIdPtr& rid){
	getManager(userId)->addRoomId(userId, rid);
}

void UserRoomAdapter::removeRoomId(int userId, const RoomIdPtr& rid){
	getManager(userId)->removeRoomId(userId, rid);
}

void UserRoomAdapter::removeRoomIds(const MyUtil::IntSeq& userIds, const RoomIdPtr& rid){
	int size = (cluster()==0 ? 1:cluster());
	map<int, IntSeq> tmp;
	for(size_t i = 0; i < userIds.size(); ++i){
		tmp[userIds.at(i)%size] .push_back(userIds.at(i));
	}
	for(map<int, IntSeq>::iterator it = tmp.begin(); it!=tmp.end(); ++it){
		getManager(it->first)->removeRoomIds(it->second, rid);
	}
}
