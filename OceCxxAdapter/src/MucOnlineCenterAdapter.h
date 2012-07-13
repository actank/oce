#ifndef _MUCONLINECENTERADAPTER_H__
#define _MUCONLINECENTERADAPTER_H__

#include "Singleton.h"
#include "AdapterI.h"
#include "MucOnlineCenter.h"

namespace com {
namespace xiaonei {
namespace talk {
namespace muc {

using namespace com::xiaonei::talk::common;
using namespace MyUtil;

class MucOnlineCenterAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::TalkChannel, MucOnlineCenterAdapter> {
public:
  MucOnlineCenterAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }
	void ChatSetingChange(int userid, const MucRoomIdPtr& roomid, int set);
  void EnterRoom(const MucActiveUserPtr& activeuser);
  void UserOnline(const MucActiveUserPtr& activeuser);
  void AwayRoom(const JidPtr& opuser, const MucRoomIdPtr& roomid);
  void SendTalkMessage(const JidPtr& opuser, const MucRoomIdPtr& roomid, const string& msg);
  int GetUserCount(const MucRoomIdPtr& roomid);
  void SetPermision(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid);
  MucUserIdentitySeq GetActiveUsers(const MucRoomIdPtr& roomid);
	JidSeq GetActiveJids(const MucRoomIdPtr& roomid);
  bool SetPermisionForIM(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid);
  MucActiveUserSeq GetUserByPermision(const MucRoomIdPtr& roomid, Permisions permision);
	void SendPrivateMsg(const JidPtr& sender, const MucUserIdentityPtr& recidentity, const string& msg);
private:
  MucOnlineCenterManagerPrx getManager(int id);
  MucOnlineCenterManagerPrx getManagerOneway(int id);

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@MucOnlineCenter";
  }
  virtual size_t cluster() {
    return 10;
  }

  vector<MucOnlineCenterManagerPrx> _managersOneway;
  vector<MucOnlineCenterManagerPrx> _managers;
};

};
};
};
};


#endif
