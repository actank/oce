#ifndef ONLINEMANAGERI_H_
#define ONLINEMANAGERI_H_

#include <map>
#include <sys/time.h>
#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Cond.h>
#include "OnlineCenter.h"
#include "TalkCommon.h"
#include "TaskManager.h"
#include "ServiceI.h"
#include "Singleton.h"
#include "XmppTools.h"
#include "User.h"
#include "Date.h"

namespace talk {
namespace online {

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::talk::common;

const static int OBS = 0;
const float FUNTIMEOUT = 250.0;

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


class StatFunc {
	string _name;
	timeval m_tpstart, m_tpend;
	//static int count;
public:
	StatFunc(string name) :
		_name(name) {
		gettimeofday(&m_tpstart, NULL);
		//MCE_WARN("begin at -->"<<name);
	}
	;

	~StatFunc() {
		gettimeofday(&m_tpend, NULL);
		float timeuse=1000000*(m_tpend.tv_sec-m_tpstart.tv_sec)+m_tpend.tv_usec
				-m_tpstart.tv_usec;
		timeuse/=1000;
		if(timeuse >= 20){
			MCE_WARN("end at -->"<<_name <<"  time:"<<timeuse<<" ms");
		}
	}
	;
};


class SyncBitmapThread : virtual public IceUtil::Thread {
public:
  void ReloadUserPool(int pool_index);
  //void ReloadBitmap(int server_index);
  bool Push(const SyncBitmapDataPtr sync_data);

protected:
  virtual void run();

private:
  std::list<SyncBitmapDataPtr> sync_queue_;

	IceUtil::Mutex mutex_;
	IceUtil::Cond cond_;
};

class BuddyListFactory : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
	virtual ObjectResultPtr create(const MyUtil::IntSeq& ids);
};

class OnlineManagerI : public OnlineManager,
		virtual public Singleton<OnlineManagerI> {
public:
	OnlineManagerI();
	bool presence(const JidPtr& jid, OnlineStat stat, const string& ver, const string& ext, const Str2StrMap& paras, const Ice::Current& = Ice::Current());
	void notify(const BuddyOnlineNotifySeq& notifies, Ice::Int index,const Ice::Current& = Ice::Current());

	void offline(const JidPtr& jid, const Ice::Current& = Ice::Current());
	Ice::Int getBuddyCount(Ice::Int userId, const Ice::Current& = Ice::Current());

  void ReloadBitmapByUserPool(Ice::Int pool_index, const Ice::Current& = Ice::Current());
	//void updateOnlineBitmap(Ice::Int serverIndex, const Ice::Current& = Ice::Current());
	//void setOnlineBitmapStat(int serverIndex);
	
	void scoreCheck(int index);
	
	UserOnlineTypeSeq getBuddyStatSeq(Ice::Int userId, const Ice::Current& = Ice::Current());
	UserOnlineTypeSeq getUsersStatSeq(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());
	UserOnlineTypeSeq getBuddyOnlineTypeSeq(Ice::Int userId, const Ice::Current& = Ice::Current()){
		return getBuddyStatSeq(userId);
	};
	UserOnlineTypeSeq getUsersOnlineTypeSeq(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current()){
		return getUsersStatSeq(ids);
	};

	UserOnlineTypeSeq getRandomOnlineUsers(int count, const Ice::Current& = Ice::Current());
	MyUtil::IntSeq getBuddyIds(Ice::Int userId, const Ice::Current& = Ice::Current());
	com::xiaonei::talk::common::JidSeqMap getBuddyJids(Ice::Int userId, Ice::Int onlineType, const Ice::Current& = Ice::Current());
	com::xiaonei::talk::common::JidSeqMap getBuddyJidsAndSelf(Ice::Int userId, Ice::Int buddyOnlineType, Ice::Int selfOnlineType,const Ice::Current& = Ice::Current());

	AvatarMap getBuddyAvatars(Ice::Int userId,Ice::Int onlineType, const Ice::Current& = Ice::Current());
	AvatarMap getBuddyAvatarsAndSelf(Ice::Int userId,Ice::Int buddyOnlineType, Ice::Int selfOnlineType, const Ice::Current& = Ice::Current());

	AvatarSeq getUserAvatars(Ice::Int userId, Ice::Int onlineType, const Ice::Current& = Ice::Current());
	AvatarPtr getAvatar(const JidPtr& jid, const Ice::Current& = Ice::Current());
	AvatarSeq getAvatars(const JidSeq& jids, const Ice::Current& = Ice::Current());

	JidSeq getUserJids(Ice::Int userId, Ice::Int onlineType,const Ice::Current& = Ice::Current());
	JidSeqMap getUsersJids(const MyUtil::IntSeq& ids,Ice::Int onlineType, const Ice::Current& = Ice::Current());
	AvatarMap getUsersAvatars(const MyUtil::IntSeq& ids, Ice::Int onlineType, const Ice::Current& = Ice::Current());

	void addBuddy(Ice::Int host, Ice::Int guest, const Ice::Current& = Ice::Current());
	void removeBuddy(Ice::Int host, Ice::Int guest, const Ice::Current& = Ice::Current());
	void closeProxyServer(const string& endpoint,bool isBroadcasst, const Ice::Current& = Ice::Current());

	AvatarSeq getOldestAvatars(int index, int beg, int limit, int type, const Ice::Current& = Ice::Current());
	bool has(int userId);
	pair<int, int> 	getSize();
	pair<int, int> 	getLockCount(pair<int, int>& max, pair<int, int>& min);
	//int				getReqCount();
	UserPoolPtr getPool(int userId);
	void remove(const string& endpoint,bool isBroadcast);
private:
	vector<UserPoolPtr> _pool;

  //bool bitmap_running_;
  SyncBitmapThread  sync_bitmap_thread_;
};


class ResetTalkProxyTask : public MyUtil::Task {
public:
		ResetTalkProxyTask(const string& endpoint,bool isBroadcast) : _endpoint(endpoint) ,_isBroadcast(isBroadcast){
			;
		}
		virtual void handle();
private:
		string _endpoint;
		bool _isBroadcast;
};

class WebOnlineEvictorTask : public MyUtil::Timer,
							virtual public Singleton<WebOnlineEvictorTask> {
public:
	WebOnlineEvictorTask() :
		Timer(60*1000) {
		;
	}
	virtual void handle();
	void push(int userId);
private:
	vector<int> _users;
	IceUtil::Mutex _mutex;
};

class OnlineSizeStatTask : public MyUtil::Timer {
public:
	OnlineSizeStatTask() :
		Timer(10*1000) {
		;
	}
	virtual void handle();

};

class OnlineDurationStatTask : public MyUtil::Timer,
								virtual public Singleton<OnlineDurationStatTask>{
public:
	void push(int userId, int duration);
	OnlineDurationStatTask() :
		Timer(1000*60*60) {
		;
	}
	virtual void handle();
private:
	map<int, int> _durations;
	IceUtil::Mutex _mutex;

};

class ScoreAddTask : public MyUtil::Timer{
	int _taskDay;
	int _index;
public:
	ScoreAddTask(int index):Timer(30 * 1000), _taskDay(Date(time(NULL)).day()), _index(index){
	}
	virtual void handle();
};


}
;
}
;

#endif /*ONLINEMANAGERI_H_*/
