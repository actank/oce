#ifndef __PRESENCEI_H__
#define __PRESENCEI_H__

#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/RWRecMutex.h>
#include <set>
#include "Presence.h"
#include "ServiceI.h"
#include "Singleton.h"
#include <IceUtil/Shared.h>
#include "MessageDispatcher.h"
#include "Evictor.h"

namespace com {
namespace xiaonei {
namespace talk {

using namespace std;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;
using namespace com::xiaonei::talk::util;

//----------------------------------------------------------------------------

class Avatar1 : public IceUtil::Shared {
public:
	string endpoint;
	string show;
//	int timestamp;
	string ver;
	string ext;
};
typedef IceUtil::Handle<Avatar1> Avatar1Ptr;

//---------------------------------------------------------------------------
class User : public IceUtil::Shared {
public:
	typedef map<Ice::Long, Avatar1Ptr>::iterator Iterator;

	User(int id) :
		_id(id), _duration(0){
		;
	}

	int id() {
		return _id;
	}


	int size() {
		return _avatars_web.size()+_avatars_pager.size()+_avatars_IM.size();
	}

	bool add(const JidPtr& jid, const string& show, const string& ver, const string& ext);
	bool remove(const JidPtr& jid); // if _map is empty return true; else return false


	//--------------------------------------------------------------
	//added by yuanfei
	bool remove(const string& endpoint, JidSeq &seq);
	void getJidSeq(JidSeq &seq);
	void getAsOnlineInfoSeq(OnlineInfoSeq &seq);
	OnlineInfoPtr getAsOnlineInfo(const JidPtr& jid);
	bool IMAvatarEmpty();
	//--------------------------------------------------------------

	void set(const JidPtr& jid, const string& show, const string& ver, const string& ext);
	bool has(const JidPtr& jid);

	//added by yuanfei
	int getOnlineDuration();
protected:
	//--------------------------------------------------------------
	//added by yuanfei
	bool _remove(map<Ice::Long, Avatar1Ptr> &map, const string& endpoint, JidSeq &seq);
	void _getJidSeq(map<Ice::Long, Avatar1Ptr> &map, JidSeq &seq);
	void _getAsOnlineInfoSeq(map<Ice::Long, Avatar1Ptr> &map, OnlineInfoSeq &seq);
	//--------------------------------------------------------------

private:
	//int next();

	map<Ice::Long, Avatar1Ptr> _avatars_web;	//amended by yuanfei
	map<Ice::Long, Avatar1Ptr> _avatars_pager;	//amended by yuanfei
	map<Ice::Long, Avatar1Ptr> _avatars_IM;	//amended by yuanfei
	int _id;
	//index = 0 is old front or hid ,
	//index = 1 is web online,
	//index >1 is proxy online
	//int _index;
	int _duration;
};
typedef IceUtil::Handle<User> UserPtr;

//---------------------------------------------------------------------------
class UserPool : public IceUtil::Shared {
public:
	typedef map<int, UserPtr>::iterator Iterator;

	bool add(const JidPtr& jid, const string& show , const string& ver, const string& ext);
	void remove(const JidPtr& jid);
	JidSeq remove(const string& endpoint);

	void set(const JidPtr& jid, const string& show, const string& ver, const string& ext);
	bool has(const JidPtr& jid);

	JidSeq get(int userId);
	JidSeq get(MyUtil::IntSeq ids);

	OnlineInfoSeq getAsOnlineInfoSeq(int userId);
	OnlineInfoPtr getAsOnlineInfo(const JidPtr& jid);
	int size(bool dedup = true);

	//added by yuanfei
	void getOnlineDurationInfo(map<int, int> &onlineDurationMap);
private:
	IceUtil::RWRecMutex _mutex;
	map<int, UserPtr> _users;

	//the online-duration of the offline user
	map<int, int> _offlineUsersMap;
};
typedef IceUtil::Handle<UserPool> UserPoolPtr;

//---------------------------------------------------------------------------
class WUserEvictor : public TimedEvictor<int, int>,
	public Singleton<WUserEvictor>, public IceUtil::Thread {
public:
	WUserEvictor();
	virtual ~WUserEvictor();
	void push(int userId);
	void pop(MyUtil::IntSeq& userIds);
private:
	virtual void run();
};

//---------------------------------------------------------------------------
class PresenceManagerI : public PresenceManager,
	public Singleton<PresenceManagerI> {

public:
	PresenceManagerI();
	virtual ~PresenceManagerI();

	virtual void bind(const JidPtr& jid, const Ice::Current& = Ice::Current());
	virtual void unbind(const JidPtr& jid, const Ice::Current& = Ice::Current());

	virtual void presence(const JidPtr& jid, const string& show,const string& ver, const string& ext,
			const Ice::Current& = Ice::Current());

	virtual JidSeqMap getIMOnlineUserJidSeqMap(const MyUtil::IntSeq& ids,
			const Ice::Current& = Ice::Current());
	virtual JidSeq getOnlineUserJidSeq(Ice::Int id,
			const Ice::Current& = Ice::Current());
	virtual OnlineInfoMap filterOnlineUsers(const MyUtil::IntSeq& ids,
			const Ice::Current& = Ice::Current());
	virtual OnlineInfoPtr filterOnlineUser(const JidPtr& jid,
				const Ice::Current& = Ice::Current());
	virtual OnlineInfoSeq getOnlineUsersInfo(const JidSeq& jids, const Ice::Current& = Ice::Current());

	virtual bool has(const JidPtr& jid, const Ice::Current& = Ice::Current());

	virtual void closeProxyServer(const string& endpoint,
			const Ice::Current& = Ice::Current());

	UserPoolPtr getPool(int uid);

	//added by yuanfei
	void		getOnlineDurationInfo(map<int, int> &OlDurMap);

private:
	const static size_t USER_POOL_SIZ = 10000;
	vector<UserPoolPtr> _pool;

	class ResetTalkProxyTask : public MyUtil::Task {
public:
		ResetTalkProxyTask(const string& endpoint) : _endpoint(endpoint) {
			;
		}
		virtual void handle();
private:
		string _endpoint;
	};

	class StatOnlineSizeTask : public MyUtil::Timer {
public:
		StatOnlineSizeTask() :
			Timer(60*1000) {
			;
		}
		virtual void handle();
	};

	//
	//write a record of online-duration of each user into a database every 30 minutes
	//
	class OnlineDurationStatTask : public MyUtil::Timer {
	public:
			OnlineDurationStatTask() :
				Timer(1000*60*30) {
				;
			}
			virtual void handle();
		};
};

}
}
}
#endif

