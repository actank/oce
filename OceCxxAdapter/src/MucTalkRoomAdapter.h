#ifndef MUCTALKROOMADAPTER_H__
#define MUCTALKROOMADAPTER_H__

#include "Singleton.h"
#include "AdapterI.h"
#include "MucTalkRoom.h"

namespace com {
namespace xiaonei {
namespace talk {
namespace muc {


using namespace MyUtil;

class MucTalkRoomAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::TalkChannel, MucTalkRoomAdapter> {
public:
  MucTalkRoomAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  void CreateRoom(const MucRoomIdPtr& roomid);
  void RemoveRoom(const MucRoomIdPtr& roomid);
  void AddRoomMember(int userid, const MucRoomIdPtr& roomid);
  void RemoveRoomMember(int userid, const MucRoomIdPtr& roomid);

  IntSeq GetRoomMember(const MucRoomIdPtr& roomid);
  MucActiveUserSeq GetRoomActiveUserSeq(const MucRoomIdPtr& roomid);

	//void PageNotifyOnline(int onlineuser, const MucRoomIdPtr& roomid);
	//void PageNotifyOffline(int offlineuser, const MucRoomIdPtr& roomid);
private:
  //int GetRoomId(const string& roomname);
  MucTalkRoomManagerPrx getManager(int id);
  MucTalkRoomManagerPrx getManagerOneway(int id);

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@MucTalkRoom";
  }
  virtual size_t cluster() {
    return 10;
  }

  vector<MucTalkRoomManagerPrx> _managersOneway;
  vector<MucTalkRoomManagerPrx> _managers;
};

};
};
};
};


#endif
