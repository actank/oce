#ifndef XMPPTOOLS_
#define XMPPTOOLS_
#include "IceLogger.h"
#include "TalkCommon.h"
#include "MucCommon.h"
#include <string>
#include "Singleton.h"
#include<sys/stat.h>
#include   <unistd.h>
#include "TaskManager.h"
#define MONITOR(msg) LOG_INFO("monitor", msg);
using namespace std;


inline void InitMonitorLogger(
		const std::string& servername,
		const std::string& name,
		const std::string& path,
		const std::string& loglevel = "WARN",
		const std::string& pattern = "%D{%H:%M:%S} : [INFO] : %m%n")
{
	string log_dir= "../log/" + servername + "/monitor";
	if(access(log_dir.c_str(),F_OK) != 0)
	{
		int res = mkdir(log_dir.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if(res<0)
		{
			MCE_INFO("Make monitor log directory error : "<<res);
		}else{
			MCE_INFO("Make monitor log directory OK! res: "<<res);
		}
	}else{
		MCE_INFO("monitor log directory already exists! ");
	}

	log4cplus::SharedAppenderPtr append_1(new log4cplus::DailyRollingFileAppender(path, log4cplus::HOURLY, 1, true));
	append_1->setName(name);
	append_1->setLayout( std::auto_ptr<log4cplus::Layout>(new log4cplus::PatternLayout(pattern)) );
	log4cplus::Logger::getInstance(name).addAppender(append_1);
	log4cplus::Logger::getInstance(name).setLogLevel(log4cplus::LogLevelManager().fromString(loglevel));
}

namespace std {
template<> struct less<com::xiaonei::talk::common::JidPtr> :
	public binary_function<com::xiaonei::talk::common::JidPtr, com::xiaonei::talk::common::JidPtr, bool> {
	bool operator()(const com::xiaonei::talk::common::JidPtr& j1,
			const com::xiaonei::talk::common::JidPtr& j2) const {
		if(j1->userId < j2->userId){
			return true;
		}else if(j1->userId > j2->userId){
			return false;
		}
		return j1->index < j2->index;
	}
};

template<> struct less<com::xiaonei::talk::common::RoomIdPtr> :
	public binary_function<com::xiaonei::talk::common::RoomIdPtr, com::xiaonei::talk::common::RoomIdPtr, bool> {
	bool operator()(const com::xiaonei::talk::common::RoomIdPtr& rid1,
			const com::xiaonei::talk::common::RoomIdPtr& rid2) const {
		if (rid1->name < rid2->name) {
			return true;
		} else if (rid1->name > rid2->name) {
			return false;
		}
		return rid1->domain < rid2->domain;
	}
};

template<> struct less<com::xiaonei::talk::muc::MucRoomIdPtr> :
	public binary_function<com::xiaonei::talk::muc::MucRoomIdPtr, com::xiaonei::talk::muc::MucRoomIdPtr, bool> {
	bool operator()(const com::xiaonei::talk::muc::MucRoomIdPtr& rid1,
			const com::xiaonei::talk::muc::MucRoomIdPtr& rid2) const {
		if (rid1->roomname < rid2->roomname) {
			return true;
		} else if (rid1->roomname > rid2->roomname) {
			return false;
		}
		return rid1->domain < rid2->domain;
	}
};


template<> struct less<com::xiaonei::talk::common::RoomJidPtr> :
	public binary_function<com::xiaonei::talk::common::RoomJidPtr, com::xiaonei::talk::common::RoomJidPtr, bool> {
	bool operator()(const com::xiaonei::talk::common::RoomJidPtr& rJid1,
			const com::xiaonei::talk::common::RoomJidPtr& rJid2) const {
		if (rJid1->nick < rJid2->nick) {
			return true;
		} else if (rJid1->nick> rJid2->nick) {
			return false;
		}
		if (rJid1->rid->name < rJid2->rid->name) {
			return true;
		} else if (rJid1->rid->name > rJid2->rid->name) {
			return false;
		}
		return rJid1->rid->domain < rJid2->rid->domain;
	}
};



}
;

namespace com {
namespace xiaonei {
namespace talk {
namespace util {
using namespace com::xiaonei::talk::common;
using namespace com::xiaonei::talk::muc;
using namespace std;
using namespace MyUtil;
string createJid(int userid, Ice::Long index = -1, const string& endpoint = "");

JidPtr stringToJid(const string& jid);
SysJidPtr stringToSysJid(const string& jid);
string jidToString(const JidPtr& jid, bool hasIndex = true);

bool equal(const JidPtr& jid1, const JidPtr& jid2);

string buildHeadUrl(const string& url, bool isXiaoNei = true);

string xmlEncode(const string& str);

string roomIdToString(const RoomIdPtr& rid);
string roomJidToString(const RoomJidPtr& rjid );
RoomIdPtr stringToRoomId(const string& rid);
RoomJidPtr stringToRoomJid(const string& rjid);

MucRoomIdPtr stringToMucRoomId(const string& rid);
string mucRoomIdToString(const MucRoomIdPtr& rid);
string mucUserIdentityToString(const MucUserIdentityPtr& identity);
MucUserIdentityPtr stringToMucUserIdentity(const string& identity);
string makeMucNickname(const JidPtr& jid, const string& name);
int GetIdFromMucRoomId(const MucRoomIdPtr& rid);
bool validRoomNickName(const string& rjid, int userid);
bool validRoomNickName(const RoomJidPtr& rjid, int userid);

enum IdType{UNDEFINE, JID, ROOMID, ROOMJID};
IdType idType(const string& id);

OnlineStat onlineStat(const string& show);
string onlineStatStr(OnlineStat stat);


struct Counter{
	Counter(){
		call_times_ = 0;
		total_usetime_ = 0.0;
		timeout_times_ = 0;
	};
	int call_times_;
	float total_usetime_;
	int timeout_times_;
};


const int FUNPOOLSIZE = 11;

class FunInfo : public Timer{
public:
	FunInfo() : Timer(60 * 1000, 2012){
	};
	virtual void handle();
	void Stat(const string& funname, float usetime, bool istimeout);
	IceUtil::Mutex mutex_;
	typedef map<string, Counter> CounterMap;
	CounterMap counter_;
};
typedef IceUtil::Handle<FunInfo> FunInfoPtr;

class FunStatManager : public Ice::Object, public Singleton<FunStatManager>{
public:
	FunStatManager(){
		Init();
	}
	void Init();
	void Stat(const string& funname, float usetime, bool istimeout = false);
	void ProcessStat(const string& funname, float usetime, bool istimeout = false);
	vector<FunInfoPtr> pool_;
};

class FunStatTask : public MyUtil::Task{
public:
	FunStatTask(const string& funname, float usetime, bool istimeout) : Task(2011), funname_(funname), usetime_(usetime), istimeout_(istimeout){
	}
	virtual void handle(){
		FunStatManager::instance().ProcessStat(funname_, usetime_, istimeout_);
	};
	string funname_;
	float usetime_;
	bool istimeout_;
};


}
;
}
;
}
;
}
;

#endif /*XMPPTOOLS_*/
