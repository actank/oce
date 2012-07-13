#ifndef STATMANAGERI_H_
#define STATMANAGERI_H_

#include "TalkStat.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "LogWrapper.h"
#include "TaskManager.h"
#include "Date.h"
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
namespace talk {
namespace stat {

using namespace MyUtil;
using namespace com::xiaonei::talk::common;

class Stat : public IceUtil::Shared {
public:
	int loginCount; // the copy of online user
	int msgCount; // send msg count
	int revMsgCount; // rev msg count
	//int onlinePeriod;
	//int onlineBuddyCount; //stat by hour

	Stat() :
		loginCount(0), msgCount(0), revMsgCount(0){
	}
	;
};
typedef IceUtil::Handle<Stat> StatPtr;
typedef map<int, StatPtr> StatMap;

class VersionStatData{
public:
	VersionStatData(int userId, const string& version, const string& subver, const string& date):
		_userId(userId), _version(version), _subver(subver), _date(date){}
	int _userId;
	string _version;
	string _subver;
	string _date;
};

class StatManagerI : public StatManager, public MyUtil::Singleton<StatManagerI> {
public:
	StatManagerI() :
		_totalMsgPerMin(0), _totalChatingMsgPerMin(0), _msgFromClient(0),
				_msgFromPager(0),
				_mucMsgCount(0),
				_mucMsgBroadcastCount(0){
		_msgFromPhone = 0;
		_msgFromAll = 0;
		_setStatusCount = 0;
		_replyStatusCount = 0;
		_replyPhotoCount = 0;
		_replyBlogCount = 0;
		_replyShareCount = 0;
	}
	;
	virtual void incSetStatusCount(int c, const Ice::Current& = Ice::Current());
	virtual void incReplyStatusCount(int c, const Ice::Current& = Ice::Current());
	virtual void incReplyPhotoCount(int c, const Ice::Current& = Ice::Current());
	virtual void incReplyBlogCount(int c, const Ice::Current& = Ice::Current());
	virtual void incReplyShareCount(int c, const Ice::Current& = Ice::Current());
	
	virtual void login(const MyUtil::IntSeq& ids,
			const Ice::Current& = Ice::Current());

	virtual void wLogin(const MyUtil::IntSeq& ids,
			const Ice::Current& = Ice::Current());

	virtual void totalMsgCount(Ice::Int count,
			const Ice::Current& = Ice::Current());
	virtual void chatingMsgCount(Ice::Int count,
			const Ice::Current& = Ice::Current());
	virtual void msgStat(const JidPtr& from, const JidPtr& to,
			const Ice::Current& = Ice::Current());

	virtual void versionStat(int userId, const string& ver, const string& subver, const Ice::Current& = Ice::Current());

	virtual void mucMsgCount(int msgCount, int msgBroadcastCount, const Ice::Current& = Ice::Current());

	void addMsgCount(int count) {
		IceUtil::Mutex::Lock lock(_mutex);
		_totalMsgPerMin += count;
	}
	int getMsgCount() {
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _totalMsgPerMin;
		_totalMsgPerMin = 0;
		return c;
	}
	void addChatingMsgCount(int count) {
		IceUtil::Mutex::Lock lock(_mutex);
		_totalChatingMsgPerMin += count;
	}
	int getChatingMsgCount() {
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _totalChatingMsgPerMin;
		_totalChatingMsgPerMin = 0;
		return c;
	}

	void addMsgFromClient(int count) {
		IceUtil::Mutex::Lock lock(_mutex);
		_msgFromClient += count;
	}
	int getMsgFromClient() {
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _msgFromClient;
		_msgFromClient = 0;
		return c;
	}

	void addMsgFromPager(int count) {
		IceUtil::Mutex::Lock lock(_mutex);
		_msgFromPager += count;
	}

	int getMsgFromPager() {
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _msgFromPager;
		_msgFromPager = 0;
		return c;
	}
	void addMsgFromPhone(int count) {
		IceUtil::Mutex::Lock lock(_mutex);
		_msgFromPhone += count;
	}

	int getMsgFromPhone() {
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _msgFromPhone;
		_msgFromPhone = 0;
		return c;
	}
	void addMsgFromAll(int count) {
		IceUtil::Mutex::Lock lock(_mutex);
		_msgFromAll += count;
	}
	int getMsgFromAll(){
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _msgFromAll;
		_msgFromAll = 0;
		return c;
	}

	bool addLoginUser(int id) {
		IceUtil::Mutex::Lock lock(_mutex);
		set<int>::iterator it = _loginUsers.find(id);
		if (it != _loginUsers.end()) {
			return false;
		}
		_loginUsers.insert(id);
		return true;
	}
//	bool isLoginUser(int id) {
//		IceUtil::Mutex::Lock lock(_mutex);
//		set<int>::iterator it = _loginUsers.find(id);
//		return it != _loginUsers.end();
//	}
	void popLoginUsers(set<int>& ids) {
		IceUtil::Mutex::Lock lock(_mutex);
		_loginUsers.swap(ids);
	}

	bool addWloginUser(int id) {
		IceUtil::Mutex::Lock lock(_mutex);
		set<int>::iterator it = _wloginUsers.find(id);
		if (it != _wloginUsers.end()) {
			return false;
		}
		_wloginUsers.insert(id);
		return true;
	}
	void popWloginUsers(set<int>& ids) {
		IceUtil::Mutex::Lock lock(_mutex);
		_wloginUsers.swap(ids);
	}

	void addNotifyId(int id, const string& ver, const string& subver) {
		IceUtil::Mutex::Lock lock(_mutex);
		//_notifyIds.insert(make_pair(id, ver));
		_notifyIds[id] = make_pair<string, string>(ver, subver);
	}
	void popNotifyIds(map<int, pair<string,string> >& ids) {
		IceUtil::Mutex::Lock lock(_mutex);
		_notifyIds.swap(ids);
	}

	void addVersionDistribution(int userId, const string& ver, const string& subver) {
		IceUtil::Mutex::Lock lock(_mutex);
		/*map<string,int>::iterator it = _versionDistribution.find(ver);
		if(it == _versionDistribution.end()){
			_versionDistribution[ver] = 1;
		}else{
			it->second ++;
		}*/
		ostringstream strDate;
		time_t tmp = time(NULL);
		tm* t = localtime(&tmp);
		if(!t){
			MCE_WARN("get date error");
			return;
		}
		strDate << t->tm_year+1900 << "-" << t->tm_mon+1 << "-"  << t->tm_mday;
		_userVersions.push_back(VersionStatData(userId, ver, subver, strDate.str()));
	}
	void popVersionDistribution(vector<VersionStatData>& vers) {
		IceUtil::Mutex::Lock lock(_mutex);
		_userVersions.swap(vers);
	}

	void setStat(int userid, const StatPtr& stat) {
		IceUtil::Mutex::Lock lock(_mutex);
		StatMap::iterator it = _statPerHalfHour.find(userid);
		if (it == _statPerHalfHour.end()) {
			_statPerHalfHour[userid] = stat;
		} else {
			it->second->loginCount += stat->loginCount;
			it->second->msgCount += stat->msgCount;
			it->second->revMsgCount += stat->revMsgCount;
			//it->second->onlinePeriod += stat->onlinePeriod;
			//it->second->onlineBuddyCount += stat->onlineBuddyCount;
		}
	}
	void popStatMap(StatMap& statmap) {
		IceUtil::Mutex::Lock lock(_mutex);
		_statPerHalfHour.swap(statmap);
	}

	int getMucMsgCount() {
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _mucMsgCount;
		_mucMsgCount = 0;
		return c;
	}
	int getMucMsgBroadcastCount() {
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _mucMsgBroadcastCount;
		_mucMsgBroadcastCount = 0;
		return c;
	}
	int getSetStatusCount(){
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _setStatusCount;
		_setStatusCount = 0;
		return c;	
	}
	int getReplyStatusCount(){
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _replyStatusCount;
		_replyStatusCount = 0;
		return c;	
	}
	int getReplyPhotoCount(){
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _replyPhotoCount;
		_replyPhotoCount = 0;
		return c;	
	}
	int getReplyBlogCount(){
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _replyBlogCount;
		_replyBlogCount = 0;
		return c;	
	}
	int getReplyShareCount(){
		IceUtil::Mutex::Lock lock(_mutex);
		int c = _replyShareCount;
		_replyShareCount = 0;
		return c;	
	}

private:
	IceUtil::Mutex _mutex;

	int _totalMsgPerMin;
	int _totalChatingMsgPerMin;
	int _msgFromClient;
	int _msgFromPager;
	int _msgFromPhone;
	int _msgFromAll;
	
	int _setStatusCount;
	int _replyStatusCount;
	int _replyPhotoCount;
	int _replyBlogCount;
	int _replyShareCount;

	int _mucMsgCount;			//the count of messages sent to all chat rooms.
	int _mucMsgBroadcastCount;	//the count of messages broadcasted

	set<int> _wloginUsers;
	set<int> _loginUsers;

	map<int, pair<string, string> > _notifyIds;

	StatMap _statPerHalfHour;
	//map<string,int> _versionDistribution;
	vector<VersionStatData> _userVersions;
};

class UserStatSaveTask : public Task
{
public:
	virtual void handle();
};

class MinSaveTask : public Task
{
public:
	virtual void handle();
};

class NewUserNotifyTask : public Task
{
public:
	virtual void handle();
};

class VersionDistributionTask : public Task
{
public:
	virtual void handle();
};

class MucStatTask : public Task
{
public:
	virtual void handle();
};

class OnlineUserCountTask : public Task{
public:
	virtual void handle();
};

class ControlTimer : public Timer {
	static int _times;
public:
	ControlTimer() :
		Timer(1000*60) {

	}
	;
	virtual void handle() {

		TaskManager::instance().schedule(new ControlTimer());
		_times++;
		//do task per min
		TaskManager::instance().execute(new MinSaveTask());
		TaskManager::instance().execute(new NewUserNotifyTask());
		TaskManager::instance().execute(new MucStatTask());
		TaskManager::instance().execute(new VersionDistributionTask());

		if(_times%5 == 0){
			TaskManager::instance().execute(new OnlineUserCountTask());
		}

		if (_times%30 == 0) {
			//do task per half hour
			TaskManager::instance().execute(new UserStatSaveTask());
			_times = 0;
		}
	}
};
int ControlTimer::_times = 0;
}
;
}
;

#endif /*STATMANAGERI_H_*/
