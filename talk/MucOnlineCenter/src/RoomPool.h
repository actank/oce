#ifndef __ROOMPOOL_H__
#define __ROOMPOOL_H__
#include <map>
#include <string>
#include <set>
#include <Ice/Ice.h>
#include <IceUtil/RWRecMutex.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>
#include "MucOnlineCenter.h"
#include "XmppTools.h"
#include "Singleton.h"
#include "TaskManager.h"


namespace com{
namespace xiaonei{
namespace talk{
namespace muc{
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;

const Ice::Short STATUSCODE_ONLINE = 1;
const Ice::Short STATUSCODE_OFFLINE = -1;
const int IGNORE_TIMESTAMP = 1;

const string PERMANENT = "group.talk.xiaonei.com";
const string TEMPORARY = "conference.talk.xiaonei.com";

typedef map<JidPtr, MucActiveUserPtr> Jid2ActiveUserMap;




//TODO 可以用Mut_index 来搞个多列索引，看下面的函数很适合这么做


class RoomInfo : public Ice::Object{//房间  在线信息
public:
  bool AddActiveUser(const MucActiveUserPtr& activeuser);//给房间内加一个人
  bool NicknameCheck(const string& checkNickname, int checkUser);//检查昵称是否冲突
  MucActiveUserPtr GetActiveUser(const JidPtr& jid);//按照jid 获取 MucActiveUser(群内在线副本)
  MucActiveUserPtr GetActiveUser(int userid);//按照userid 获取 MucActiveUser(群内在线副本)
  MucActiveUserPtr GetActiveUser(const string& nickname);//按照昵称 获取 MucActiveUser(群内在线副本)
  MucActiveUserSeq GetActiveUserSeq(int userid, bool filter = true);//按照 userid 获取  MucActiveUser(群内在线副本) 添加黑名单过滤
  MucActiveUserSeq GetActiveUserSeq(Permisions permision);//按照 权限 获取 MucActiveUser(群内在线副本)
  JidSeq GetAllActiveJids();// 获取所有的在线活跃群聊副本的 Jid
  MucActiveUserSeq GetAllActiveUsers(); //获取所有的在线活跃群聊副本
  MucUserIdentitySeq GetAllUserIdentity();//获取所有的在线活跃群聊副本的 身份
	int GetUserMucStatus(int userid);//获取一个用户的群聊在线 状态码 2:pager | 4:im .....   
  bool AwayRoom(const JidPtr& opuser);//离开房间
  bool AwayRoom(int userid);//userid 副本全部离开房间
  int GetUserCount();//获取这个房间内的副本的个数
  bool SetPermisionForIM(int opuser, Permisions permision, int targetuserid);//IM对临时群的权限设置
  void SetPermision(int targetuser, Permisions permision);//设置权限
  string _roomname;//房间的名字
  string _domain;//房间的domain
private:
  Jid2ActiveUserMap _jid2activeuser;//cache
};








typedef IceUtil::Handle<RoomInfo> RoomInfoPtr;
typedef map<string, RoomInfoPtr> RoomMap;// roomname 2 RoomInfoPtr
typedef map<string, RoomMap > Cache;//domain to RoomMap




class RoomPool : public Ice::Object{
protected:
	RoomInfoPtr GetRoom(const MucRoomIdPtr& roomId);//获取房间在线信息
	bool KickRoom(const MucRoomIdPtr& roomId);//踢掉一个房间(没有人在线，不存在cache 里面)
	RoomInfoPtr CreateRoom(const MucRoomIdPtr& roomId);//在cache里面创建一个房间
	RoomInfoPtr JoinRoom(const MucActiveUserPtr& activeuser);//
public:
	void ChatSetingChange(int userid, const MucRoomIdPtr& roomid, int set);
	void EnterRoom(const MucActiveUserPtr& activeuser);
	void UserOnline(const MucActiveUserPtr& activeuser);
	void AwayRoom(const JidPtr& opuser, const MucRoomIdPtr& roomid);
	void SendTalkMessage(const JidPtr& opuser, const MucRoomIdPtr& roomid, const string& msg);
	int GetUserCount(const MucRoomIdPtr& roomid);
	bool SetPermisionForIM(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid);
	void SetPermision(int opuser, Permisions permision, int targetuser, const MucRoomIdPtr& roomid);
	MucUserIdentitySeq GetActiveUsers(const MucRoomIdPtr& roomid);
	JidSeq GetActiveJids(const MucRoomIdPtr& roomid);
	MucActiveUserSeq GetUserByPermision(const MucRoomIdPtr& roomid, Permisions permision);
	void SendPrivateMsg(const JidPtr& sender, const MucUserIdentityPtr& recidentity, const string& msg);
private:
	IceUtil::RWRecMutex _rwmutex;//每个桶内一把读写锁
	Cache _cache;
};
typedef IceUtil::Handle<RoomPool> RoomPoolPtr;

class PageCacheNotifyTask : virtual public MyUtil::Task{
public:
	PageCacheNotifyTask(const string& action, int activeuser, const MucRoomIdPtr& roomid) : _action(action), _activeuser(activeuser), _roomid(roomid){
	};
	virtual void handle();
private:
	string _action;
	int _activeuser;
	MucRoomIdPtr _roomid;
};

class PullLostUserTask : virtual public MyUtil::Task{
public:
	PullLostUserTask(const JidPtr& jid) : _jid(jid){
	};
	virtual void handle();
private:
	JidPtr _jid;
};


};
};
};
};

#endif
