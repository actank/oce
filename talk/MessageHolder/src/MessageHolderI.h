#ifndef MESSAGEHOLDERI_H_
#define MESSAGEHOLDERI_H_

#include "MessageHolder.h"
#include "Singleton.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <IceUtil/Mutex.h>
#include "TaskManager.h"
#include "TalkStatAdapter.h"
#include "ServiceI.h"

namespace talk {
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace talk::stat;


const int LC = 0;

class LeavewordCount : public Ice::Object {
	int _count;
public:
	LeavewordCount() :
		_count(0) {
	}
	;
	void inc(int count=1) {
		_count += count;
	}
	void clean() {
		_count = 0;
	}
	int count(){
		return _count;
	}

};

typedef IceUtil::Handle<LeavewordCount> LeavewordCountPtr;


class LeavewordCountFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
	virtual Ice::ObjectPtr delegate(Ice::Int id);
};


class MsgTimerInfo : public Ice::Object {
public:
	MessagePtr msg;
	time_t time;
	bool resent;
};
typedef IceUtil::Handle<MsgTimerInfo> MsgTimerInfoPtr;

class MessageTub {
	typedef map<int, MessagePtr>::iterator MsgPos;
	static const time_t MSG_TIMEOUT = 10;
	static const time_t RESENT_TIMEOUT = 5;
	map<int, map<Ice::Long, MessagePtr> > _userMsgs;
	list<MsgTimerInfoPtr> _msgTimes;
	IceUtil::Mutex _mutex;
public:
	void insert(const MessagePtr& msg);
	void send(int userId, Ice::Long msgKey);
	void getTimeoutMsgs(vector<MessagePtr>& timeoutSeq, vector<MessagePtr>& resentSeq);
	vector<MessagePtr> getUserMsgs(int userId);
};


//----------------------------------------------------
class OfflineMessageQuery : public MyUtil::Timer,
							public MyUtil::Singleton<OfflineMessageQuery> {
	map<int,int> _offlineMsgCount;
	IceUtil::Mutex _mutexUsers;

public:
	OfflineMessageQuery() :
		Timer(5*1000) {
	}

	void incOfflineMsgCount(int userid);
	void cleanOfflineMsgCount(int usrId);
	int  getOfflineMsgCount(int usrId);
	virtual void handle();

protected:
	void _pop(vector<string>& sqltoken);
	void _popOfflineMsgCountMap(map<int,int>& countMap);
	void _processOfflineMsgCount();
	void _processOfflineMsg();
};


//--------------------------------------------------------
class HolderManagerI : public HolderManager,
	public MyUtil::Singleton<HolderManagerI> {
	static const int POOL_SIZE = 10000;
	MessageTub _tubs[POOL_SIZE];
	int _chatingMsgCount;
	int _totalMsgCount;
	IceUtil::Mutex _mutex;

public:
	HolderManagerI() :
		_chatingMsgCount(0), _totalMsgCount(0) {
	}
	;

	virtual void message(const MessagePtr& msg,
			const Ice::Current& = Ice::Current());
	virtual void send(int userId, Ice::Long messageKey, const Ice::Current& = Ice::Current());
	virtual void cleanOfflineMessageCount(int userid, const Ice::Current& = Ice::Current());
	virtual int  getOfflineMessageCount(int userid, const Ice::Current& = Ice::Current());

	virtual void loadOfflineMessage(const JidPtr& jid, const Ice::Current& = Ice::Current());
	
	virtual void LoadUnreadNotifyInfoCount(const JidPtr& jid, const Ice::Current& = Ice::Current());//pager用户登录的时候，加载未读的Notify系统的消息(不在新留言以及回复提示那显示的)的数量.


	void getTimeoutMsgs(vector<MessagePtr>& seq, vector<MessagePtr>& resentSeq);
	vector<MessagePtr> getUserMsgs(int userId);

	void incChatingMsgCount(int count=1) {
		IceUtil::Mutex::Lock lock(_mutex);
		_chatingMsgCount += count;
	}
	void incTotalMsgCount(int count=1) {
		IceUtil::Mutex::Lock lock(_mutex);
		_totalMsgCount += count;
	}
	int getChatingMsgCount() {
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _chatingMsgCount;
		_chatingMsgCount = 0;
		return c;
	}
	int getTotalMsgCount() {
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _totalMsgCount;
		_totalMsgCount = 0;
		return c;
	}
};

class MsgStatTimer : public MyUtil::Timer {
public:
	MsgStatTimer() :
		Timer(10*1000) {
	}
	;

	virtual void handle() {
		TaskManager::instance().schedule(new MsgStatTimer);

		int totalMsg = HolderManagerI::instance().getTotalMsgCount();
		int chatingMsg = HolderManagerI::instance().getChatingMsgCount();
		if (totalMsg) {
			TalkStatAdapter::instance().totalMsgCount(totalMsg);
		}
		if (chatingMsg) {
			TalkStatAdapter::instance().chatingMsgCount(chatingMsg);
		}
	}
};

class LoadOfflineMessageTask: public Task{
	JidPtr _jid;
public:
	LoadOfflineMessageTask(const JidPtr& jid):_jid(jid){};
	virtual void handle();
};

class LoadUnreadNotifyInfoCountTask: public Task{
	JidPtr _jid;
public:
	LoadUnreadNotifyInfoCountTask(const JidPtr& jid):_jid(jid){};
	virtual void handle();
};

}
;

#endif /*MESSAGEHOLDERI_H_*/
