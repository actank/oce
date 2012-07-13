#ifndef _MUCGATEADAPTER_H__
#define _MUCGATEADAPTER_H__

#include "Singleton.h"
#include "AdapterI.h"
#include "MucGate.h"
#include "XmppTools.h"
namespace com {
namespace xiaonei {
namespace talk {
namespace muc {
using namespace com::xiaonei::talk::cache::loader;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace com::xiaonei::talk::util;


const string GROUPDOMAIN = "group.renren.com";

class MucGateAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::TalkChannel, MucGateAdapter> {
public:
  MucGateAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

	TalkUserPtr GetUserById(int userid);
	TalkUserMap GetUserBySeq(const MyUtil::IntSeq& uids);
	//for memcache test
	void ChatSetingChange(int userid, int groupid, int set);
	void CreateGroupForUgc(int groupowner, int groupid, const IntSeq& inviteusers, int permision);
  void CreateGroup(int groupid);
  void RemoveGroup(int groupid);
	void ReloadGroupInfo(int groupid, const string& groupname);
  //小组的实现是必须要移除所有的人以后，才能解散这个群， 所以移除群的时候，不需要关心群里面的人的房间的问题。
  void AddGroupMember(int userid, int groupid, int permision);
  void AddGroupMemberBySeq(const MyUtil::IntSeq& userids, int groupid, int permision);
  void RemoveGroupMember(int userid, int groupid);
  void SetPermision(int operateuser, int groupid, int targetuser, int permision);
  //FOR IM
  void EnterRoom(const MucActiveUserPtr& enteruser);
  bool SetPermisionForIM(int operateuser, const MucRoomIdPtr& roomid, int targetuser, Permisions permision);
	MucActiveUserSeq GetUserByPermision(const MucRoomIdPtr& roomid, Permisions permision);
	void UserInviteUser(const JidPtr& opjid, const MucRoomIdPtr& roomid, int targetuser);
	void SendPrivateMsg(const JidPtr& sender, const MucUserIdentityPtr& recidentity, const string& msg);
  //FOR PAGER
  MucUserIdentitySeq GetRoomUserList(const MucRoomIdPtr& roomid);
	string GetGroupName(int groupid);
  //IM && PAGER(groups.renren.com)
  void UserOnline(const JidPtr& onlinejid);
  void UserOffline(const JidPtr& offlinejid);
  MucRoomIdSeq GetUserRooms(int userid, const string& domain);
  void AwayRoom(const JidPtr& opjid, const MucRoomIdPtr& roomid);
  void SendTalkMsg(const JidPtr& opjid, const MucRoomIdPtr& roomid, const string& msg);
  //------------------------RESPONSE---------
  void NotifyUserEnterRoom(const MucActiveUserPtr& activeuser, const MucActiveUserSeq& presenceUsers, int mucstatus);
  void NotifyUserAwayRoom(const MucActiveUserPtr& awayuser, const JidSeq& presenceUsers, int mucstatus);
  void NotifyGroupChat(const MucActiveUserPtr& activeuser, const JidSeq& presenceUsers, const string& msg);
  void NotifySetPermision(const MucActiveUserPtr& opuser, const JidSeq& presenceUsers, const MucActiveUserSeq& targetusers, Permisions permision);
  void InviteIMusers(const MucRoomIdPtr& roomid, const JidSeq& imusers);
	void NotifyPrivateChat(const MucActiveUserPtr& sender, int targetuser, const string& msg);
	void NotifyNicknameError(const MucRoomIdPtr& roomid, const JidPtr& jid);
	void NotifyChatSetingChange(const MucRoomIdPtr& roomid, const JidSeq& jids, int set);

private:
	Permisions Int2Permision(int p);
	MucRoomIdPtr MakeRoomIdPtr(int groupid, const string& domain);
  //int GetRoomId(const string& roomname);
  MucGateManagerPrx getManager(int id);
  MucGateManagerPrx getManagerOneway(int id);

  virtual string name() {
    return "M";
  }
  virtual string endpoints() {
    return "@MucGate";
  }
  virtual size_t cluster() {
    return 10;
  }

  vector<MucGateManagerPrx> _managersOneway;
  vector<MucGateManagerPrx> _managers;
};

};
};
};
};


#endif
