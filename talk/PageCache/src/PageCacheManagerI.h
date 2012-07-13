#ifndef PAGECACHEMANAGERI_H_
#define PAGECACHEMANAGERI_H_

#include "Singleton.h"
#include "ServiceI.h"

#include "TemplateDataCollector.h"
#include "PageCache.h"

namespace talk {
namespace http {

using namespace MyUtil;

const static int AU = 0;
const static int AG = 1;
class Config {
public:
	string template_path;
	int timeout;
	string type;
	string content_type;
};

//---------------------------------------------

class Buddy : public IceUtil::Shared {
	int _id;
	string _name;
	string _tinyurl;
	string _doing;
	int _onlineStatus;
public:
	bool operator < (const Buddy& o) const{
		return _id < o._id;
	}
	void id(int id) {
		_id = id;
	}
	int id() {
		return _id;
	}

	void name(const string& name);

	string name() {
		return _name;
	}

	void tinyUrl(const string& url) {
		_tinyurl = url;
	}
	string tinyUrl();

	void doing(const string& doing);

	string doing() {
		return _doing;
	}

	void onlineStatus(int stat) {
		_onlineStatus = stat;
	}
	int onlineStatus() {
		return _onlineStatus;
	}
};
typedef IceUtil::Handle<Buddy> BuddyPtr;

class ActiveUser : public Ice::Object {
	int _userid;
	string _name;
	string _tiny_url;
	string _ticket;
	vector<BuddyPtr> _buddyList;

	size_t _buddyListTimestamp;

  void load();
public:
	ActiveUser(int userid) :
		_userid(userid), _buddyListTimestamp(0) {
	}
	;
	vector<BuddyPtr> getBuddyList();

	int getBuddyCount();
	bool checkTicket(const string& ticket, bool isWap=false);
	int userId();
	string name();
	string tiny_url();
};
typedef IceUtil::Handle<ActiveUser> ActiveUserPtr;
//------------------------------------------------------------------
typedef map<int, BuddyPtr> RoomMemberMap;
class ActiveGroup : public Ice::Object{
public:
	ActiveGroup(int groupid): _groupid(groupid), _groupListTimestamp(0), _groupname(""){
	};
	string GetGroupName();
	void UpdateGroupName(const string& groupname);
	void KickMember(int memberid);
	bool FindMember(int memberid);
	void AddMember(int memberid, const BuddyPtr& buddy);
	void ChangeMemberOnlineType(int memberid, int onlinetype);
	vector<BuddyPtr> getMemberList();
private:
	RoomMemberMap _groupMember;
	size_t _groupListTimestamp;
	int _groupid;
	string _groupname;
};
typedef IceUtil::Handle<ActiveGroup> ActiveGroupPtr;
//------------------------------------------------------------------
class ActiveUserFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

class ActiveGroupFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

//-------------------------------------------------
class PageCacheManagerI : public PageCacheManager,
		virtual public Singleton<PageCacheManagerI> {

public:
	virtual ContentPtr GetContent(const string& path,
			const MyUtil::Str2StrMap& cookies,
			const MyUtil::Str2StrMap& parameter,
			const Ice::Current& = Ice::Current());
	virtual void ReloadStaticPages(const Ice::Current& = Ice::Current());
	virtual void MemberStatusChange(int groupid, int memberid, int mucstatus, const Ice::Current& = Ice::Current());
	virtual void UpdateRoomName(int groupid, const string& groupname, const Ice::Current& = Ice::Current());
	virtual int GetVideoViewPower(const Ice::Current& = Ice::Current());
	virtual void SetVideoViewPower(int value, const Ice::Current& = Ice::Current());
	void loadConfig(const string& path);
	BuddyPtr MakeBuddyById(int userid, int mucstatus);
	TemplateDataCollectorPtr getTemplateCollector(const string& path);
//	string getBinData(const string& localFile,const string& url, const string& type);
private:
	int _video_view_power;
	IceUtil::Mutex _mutex;
	IceUtil::RWRecMutex _video_power_mutex;
	map<string,Config> _config;
	map<string,TemplateDataCollectorPtr> _collectors;
//	map<string,string> _binData;
};

}
}

#endif /*PAGECACHEMANAGERI_H_*/
