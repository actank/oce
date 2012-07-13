/*
 * UserRoomAdapter.h
 *
 *  Created on: Dec 23, 2008
 *      Author: yaunfei
 */

#ifndef USERROOMADAPTER_H_
#define USERROOMADAPTER_H_

#include "TalkCommon.h"
#include "Singleton.h"
#include "AdapterI.h"
#include "UserRoom.h"


namespace talk {
namespace userroom {
namespace adapter {

using namespace com::xiaonei::talk::common;
using namespace ::talk::userroom;
using namespace MyUtil;

class UserRoomAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::TalkChannel, UserRoomAdapter> {
public:
	UserRoomAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}
	RoomIdSeq getRoomIds(int userId, const string& domain);
	void addRoomId(int userId, const RoomIdPtr& rid);
	void removeRoomId(int userId, const RoomIdPtr& rid);
	void removeRoomIds(const MyUtil::IntSeq& userIds, const RoomIdPtr& rid);
	int getCluster() {
		return _cluster;
	}

protected:
	virtual string name() {
		return "UserRoom";
	}
	virtual string endpoints() {
		return "@UserRoom";
	}
	virtual size_t cluster() {
		return 0;
	}

	UserRoomMapPrx getManager(int id);
	UserRoomMapPrx getManagerOneway(int id);

	vector<UserRoomMapPrx> _managersOneway;
	vector<UserRoomMapPrx> _managers;

}
;
}
;
}
;
}
;

#endif /* USERROOMADAPTER_H_ */
