#ifndef __MUCGATE_H__
#define __MUCGATE_H__
#include <map>
#include <string>
#include <vector>
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Thread.h>
#include "MucGate.h"
#include "Singleton.h"
#include "pugixml.hpp"
#include "MessageType.h"
#include "TalkCacheLoader.h"
#include "ServiceI.h"
#include "MucTask.h"
namespace com{
namespace xiaonei{
namespace talk{
namespace muc{
using namespace std;
using namespace MyUtil;
using namespace pugi;
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::util;
using namespace com::xiaonei::talk::cache::loader;
const string DB_INSTANCE = "minigroup";
const string PERMANENT = "group.talk.xiaonei.com";
const string TEMPORARY = "conference.talk.xiaonei.com";
const int GN = 0;
class MucGateManagerI : public MucGateManager, public Singleton<MucGateManagerI>{
public:
	
	//for memcache test
	virtual TalkUserPtr GetUserById(int userid, const Ice::Current& = Ice::Current());
	virtual TalkUserMap GetUserBySeq(const MyUtil::IntSeq& uids, const Ice::Current& = Ice::Current());	

  //------------------------RECEIVE REQUEST-------
  //FOR GROUPS
	virtual void ChatSetingChange(int userid, const MucRoomIdPtr& roomid, int set, const Ice::Current& = Ice::Current());
	virtual void CreateGroupForUgc(int groupowner, const MucRoomIdPtr& roomid, const IntSeq& inviteusers, Permisions invited_permision, const Ice::Current& = Ice::Current());
  virtual void CreateGroup(const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  virtual void RemoveGroup(const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  virtual void ReloadGroupInfo(int roomid, const string& groupname, const Ice::Current& = Ice::Current());
  //小组的实现是必须要移除所有的人以后，才能解散这个群， 所以移除群的时候，不需要关心群里面的人的房间的问题。
  virtual void AddGroupMember(int userid, const MucRoomIdPtr& roomid,  Permisions permision, const Ice::Current& = Ice::Current());
  virtual void AddGroupMemberBySeq(const MyUtil::IntSeq& userids, const MucRoomIdPtr& roomid,  Permisions permision, const Ice::Current& = Ice::Current());
  virtual void RemoveGroupMember(int userid, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  virtual void SetPermision(int operateuser, const MucRoomIdPtr& roomid, int targetuser, Permisions permision, const Ice::Current& = Ice::Current());
  //FOR IM
  virtual void EnterRoom(const MucActiveUserPtr& enteruser, const Ice::Current& = Ice::Current());
  virtual bool SetPermisionForIM(int operateuser, const MucRoomIdPtr& roomid, int targetuser, Permisions permision, const Ice::Current& = Ice::Current());
	virtual MucActiveUserSeq GetUserByPermision(const MucRoomIdPtr& roomid, Permisions permision, const Ice::Current& = Ice::Current());
	virtual void UserInviteUser(const JidPtr& opjid, const MucRoomIdPtr& roomid, int targetuser, const Ice::Current& = Ice::Current());
  virtual void SendPrivateMsg(const JidPtr& sender, const MucUserIdentityPtr& recidentity, const string& msg, const Ice::Current& = Ice::Current());
  //FOR PAGER
	virtual string GetGroupName(int groupid, const Ice::Current& = Ice::Current());
  virtual MucUserIdentitySeq GetRoomUserList(const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  //IM && PAGER(groups.renren.com)
  virtual void UserOnline(const JidPtr& onlinejid, const Ice::Current& = Ice::Current());
  virtual void UserOffline(const JidPtr& offlinejid, const Ice::Current& = Ice::Current());
  virtual MucRoomIdSeq GetUserRooms(int userid, const string& domain, const Ice::Current& = Ice::Current());  //this function may be destroy
  virtual void AwayRoom(const JidPtr& opjid, const MucRoomIdPtr& roomid, const Ice::Current& = Ice::Current());
  virtual void SendTalkMsg(const JidPtr& opjid, const MucRoomIdPtr& roomid, const string& msg, const Ice::Current& = Ice::Current());
  //------------------------RESPONSE---------
  virtual void NotifyUserEnterRoom(const MucActiveUserPtr& activeuser, const MucActiveUserSeq& presenceUsers, int mucstatus, const Ice::Current& = Ice::Current());
  virtual void NotifyUserAwayRoom(const MucActiveUserPtr& awayuser, const JidSeq& presenceUsers, int mucstatus, const Ice::Current& = Ice::Current());
  virtual void NotifyGroupChat(const MucActiveUserPtr& activeuser, const JidSeq& presenceUsers, const string& msg, const Ice::Current& = Ice::Current());
  virtual void NotifySetPermision(const MucActiveUserPtr& opuser, const JidSeq& presenceUsers, const MucActiveUserSeq& targetusers, Permisions permision, const Ice::Current& = Ice::Current());
  virtual void InviteIMUsers(const MucRoomIdPtr& roomid, const JidSeq& imusers, const Ice::Current& = Ice::Current());
	virtual void NotifyPrivateChat(const MucActiveUserPtr& sender, int targetuser, const string& msg, const Ice::Current& = Ice::Current());
 	virtual void NotifyNicknameError(const MucRoomIdPtr& roomid, const JidPtr& jid, const Ice::Current& = Ice::Current()); 
	virtual void NotifyChatSetingChange(const MucRoomIdPtr& roomid, const JidSeq& jids, int set, const Ice::Current& = Ice::Current());	
	
	string Permision2Str(Permisions permision);
  string Psermision2Role(Permisions permision);
  MucRoomIdPtr MakeRoomIdPtr(const string& roomname, const string& domain);
  MucRoomIdPtr MakeRoomIdPtr(int groupid, const string& roomname, const string& domain);
};

class GroupName : public Ice::Object{
public:
	string groupname;
};
typedef IceUtil::Handle<GroupName> GroupNamePtr;

class GroupNameFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};


class TimeStat{
	timeval _begin, _end;
	public:
	TimeStat(){
		reset();
	}
	void reset(){
		gettimeofday(&_begin, NULL);
	}
	float getTime(){
		gettimeofday(&_end, NULL);
		float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
			-_begin.tv_usec;
		timeuse/=1000;
		return timeuse;
	}
};

};
};
};
};

#endif
