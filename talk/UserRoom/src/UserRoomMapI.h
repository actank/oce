/*
 * UserRoomMap.h
 *
 *  Created on: Dec 23, 2008
 *      Author: yaunfei
 */

#ifndef USERROOMMAP_H_
#define USERROOMMAP_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/RWRecMutex.h>
#include "UserRoom.h"
#include "Occupants.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "XmppTools.h"
#include <map>



namespace talk{
namespace userroom{
	using namespace com::xiaonei::talk::common;
	using namespace com::xiaonei::talk;
	using namespace std;
	using namespace MyUtil;
	using namespace com::xiaonei::talk::util;


	class RoomIDListFactoryI : public ServantFactory {
	public:
		virtual Ice::ObjectPtr create(Ice::Long userId);
	};

	class RoomIdList : public Ice::Object{
		map<string, set<RoomIdPtr> > _roomIds;
	public:
		void addRoomId(const RoomIdPtr& rid);
		RoomIdSeq getRoomIds(string domain);
		void removeRoomId(const RoomIdPtr& rid);

	};
	typedef IceUtil::Handle<RoomIdList> RoomIdListPtr;



	class UserRoomMapI: public UserRoomMap, public Singleton<UserRoomMapI>{
	public:
		virtual RoomIdSeq getRoomIds(int userId, const string& domain, const Ice::Current& = Ice::Current());
		virtual void addRoomId(int userId, const RoomIdPtr& rid, const Ice::Current& = Ice::Current());
		virtual void removeRoomId(int userId, const RoomIdPtr& rid, const Ice::Current& = Ice::Current());
		virtual void removeRoomIds(const MyUtil::IntSeq& userIds, const RoomIdPtr& rid, const Ice::Current& = Ice::Current());
	//private:
		//map<int, map<JidPtr, set<RoomIdPtr> > > _userRoomIds;
		IceUtil::RWRecMutex _rwmutex;
	};
}
}
#endif /* USERROOMMAP_H_ */
