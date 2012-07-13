#ifndef ACTIVE_USER_NOTIFY_H_
#define ACTIVE_USER_NOTIFY_H_

#include <set>
#include <set>
#include "TaskManager.h" 
#include "Singleton.h" 
using namespace std;

class ActiveUserNotify : public MyUtil::Timer,
		public MyUtil::Singleton<ActiveUserNotify>{
public:
	static const int MAX_NOTIFY = 2000000;
	ActiveUserNotify() : Timer(60*1000){
		setDay();
		_count = 0;
		_inited = false;
		_notifyCount = 0;
	}
	void notify(int userid);
	void needNotify(int userid);
	void loadUsers();
	void updateSendNotifyCount();
	void setDay();
	void handle();
	//void push(int id){
	//	_userids.push_back(pair(id, time(NULL)));
	//}
private:
	bool			_inited;
	set<int> 		_users;
	list<pair<int, int> > 	_uids;
	
	int		_day;
	int		_count;
	int		_notifyCount;
	IceUtil::Mutex	_mutex;
};

#endif
