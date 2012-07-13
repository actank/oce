#ifndef MUCTASK_H_
#define MUCTASK_H_

#include "TaskManager.h"
#include "MucCommon.h"
#include "pugixml.hpp"
#include "XmppTools.h"
#include "TalkDeliverAdapter.h"

namespace com
{
namespace xiaonei
{
namespace talk
{
namespace muc
{

using namespace MyUtil;
using namespace pugi;
using namespace com::xiaonei::talk::common;

class NotifyGroupChatTask : virtual public MyUtil::Task{
public:
	NotifyGroupChatTask(const MucActiveUserPtr& _activeuser, const JidSeq& _presenceUsers, const string& _msg):activeuser(_activeuser), presenceUsers(_presenceUsers), msg(_msg){
	};
	virtual void handle();
private:
	MucActiveUserPtr activeuser;
	JidSeq presenceUsers;
	string msg;
};

class NotifySetPermisionTask : virtual public MyUtil::Task{
public:
	NotifySetPermisionTask(const MucActiveUserPtr& _opuser, const JidSeq& _presenceUsers, const MucActiveUserSeq& _targetusers, Permisions _permision):opuser(_opuser), presenceUsers(_presenceUsers), targetusers(_targetusers), permision(_permision){
	};
	virtual void handle();
private:
	MucActiveUserPtr opuser;
	JidSeq presenceUsers; 
	MucActiveUserSeq targetusers;
	Permisions permision;
};

class InviteIMUsersTask : virtual public MyUtil::Task{
public:
	InviteIMUsersTask(const MucRoomIdPtr& _roomid, const JidSeq& _imusers):roomid(_roomid), imusers(_imusers){
	};
	virtual void handle();
private:
	MucRoomIdPtr roomid;
	JidSeq imusers;
};

class NotifyPrivateChatTask : virtual public MyUtil::Task{
public:
	NotifyPrivateChatTask(const MucActiveUserPtr& _sender, int _targetuser, const string& _msg):sender(_sender), targetuser(_targetuser), msg(_msg){
	};
	virtual void handle();
private:
	MucActiveUserPtr sender;
	int targetuser;
	string msg;
};

class CreateMiniGroupTask : virtual public MyUtil::Task{
public:
	CreateMiniGroupTask(int _groupowner, const MucRoomIdPtr& _roomid, const IntSeq& _inviteusers, Permisions _invited_permision):groupowner(_groupowner), roomid(_roomid), inviteusers(_inviteusers), invited_permision(_invited_permision){
	};
	virtual void handle();
private:
	int groupowner;
	MucRoomIdPtr roomid;
	IntSeq inviteusers;
	Permisions invited_permision;
};


class NotifyNicknameErrorTask : virtual public MyUtil::Task{
public:
	NotifyNicknameErrorTask(const MucRoomIdPtr& _roomid, const JidPtr& _jid) : roomid(_roomid), jid(_jid){
	}
	virtual void handle();
private:
	MucRoomIdPtr roomid;
	JidPtr jid;
};


};
};
};
};
#endif 
