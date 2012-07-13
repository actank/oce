#ifndef __GROUPCHAT_I_H__
#define __GROUPCHAT_I_H__

#include <string>
#include <vector>
#include <deque>
#include <map>
#include <IceUtil/Shared.h>
#include <ctime>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "TaskManager.h"
#include "Singleton.h"
#include "GroupChat.h"

namespace xce {
namespace talk {
namespace groupchat {

using namespace std;
using namespace MyUtil;

const int SCANTIME = 10 * 1000; // 定时扫描群的时间间隔
const int GROUPLIFE = 29;       //不活动群的生存时期
const int GROUPPOOLSIZE = 10;

class ScanGroupMapTask: public Timer {
public:
	ScanGroupMapTask() :
		Timer(SCANTIME) {
		;
	}
	virtual void handle();
};

class Msg {
public:

	Msg(const int & s, const string & c) :
		sender(s), content(c) {
		;
	}
private:
	int sender;
	string content;
};

class Group {
public:
	Group(const Ice::Long & , const int &);

	bool expired();

	void join(const int & m);
	set<int> & getMembers();
	bool existMember(const int & m);
	bool kickMember(const int & m);

	void insertMsg(const int & userId, const string & msg);

	deque<class Msg> & getMsgHistory();

private:

	//IceUtil::RWRecMutex _mutex;
	Ice::Long feedId;
	set<int> _members;
	deque<Msg> _msgHistory;
	time_t _timeLastModify;
};

class GroupChatManagerI: public GroupChatManager, public MyUtil::Singleton<
		GroupChatManagerI> {
public:

	void init();

	virtual bool
	join(Ice::Long, Ice::Int, const Ice::Current& = Ice::Current());

	virtual bool sendMsg(Ice::Long, Ice::Int, const std::string&,
			const Ice::Current& = Ice::Current());

	virtual bool kickMember(Ice::Long, Ice::Int, const Ice::Current& =
			Ice::Current());

	/*	bool join(::Ice::Long feedId, ::Ice::Int userId,
	 const ::Ice::Context* __context);

	 bool kickMember(::Ice::Long feedId, ::Ice::Int userId,
	 const ::Ice::Context* __context);

	 bool sendMsg(::Ice::Long feedId, ::Ice::Int userId,
	 const ::std::string& msg, const ::Ice::Context* __context);*/
	void scanGroupMap();

private:

	map<Ice::Long, Group> _groupPool[GROUPPOOLSIZE];
	IceUtil::Mutex _mutexPool[GROUPPOOLSIZE];
	//vector<IceUtil::RWRecMutex>  _mutexPool;
	int _groupMod(const Ice::Long &);

};

}
;
}
;
}
;

#endif
