#ifndef MUCUSERROOMADAPTER_H__
#define MUCUSERROOMADAPTER_H__

#include "Singleton.h"
#include "AdapterI.h"
#include "MucUserRoom.h"

namespace com {
namespace xiaonei {
namespace talk {
namespace muc {

using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class MucUserRoomAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::TalkChannel, MucUserRoomAdapter> {
public:
  MucUserRoomAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }
  MucRoomIdSeq GetUserRooms(int userid, const string& domain);
  bool AddRoom(int userid, const MucRoomIdPtr& roomid, const MucMemberPtr& member);
  bool RemoveRoom(int userid, const MucRoomIdPtr& roomid);
  MucMemberPtr GetMemberPermision(int userid, const MucRoomIdPtr& roomid);
//  MucMemberMap GetMemberPermisionBySeq(const MyUtil::IntSeq& userids, const MucRoomIdPtr& roomid);
  MucRoomMemberSeq GetUserRoomAndMember(int userid);
  void SetUserPermision(int targetuser, const MucRoomIdPtr& roomid, Permisions permision);
  void UserOnline(const JidPtr& onlinejid);
  void UserOffline(const JidPtr& offlinejid);
	void EnterRoomForTemporary(const MucActiveUserPtr& enteruser);
	void AwayRoomForTemporary(const JidPtr& awayjid, const MucRoomIdPtr& roomid);
private:
  MucUserRoomManagerPrx getManager(int id);
  MucUserRoomManagerPrx getManagerOneway(int id);

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@MucUserRoom";
  }
  virtual size_t cluster() {
    return 10;
  }

  vector<MucUserRoomManagerPrx> _managersOneway;
  vector<MucUserRoomManagerPrx> _managers;
};

};
};
};
};


#endif
