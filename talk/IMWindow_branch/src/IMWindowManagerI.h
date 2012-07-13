#ifndef IMWINDOWMANAGER_H_
#define IMWINDOWMANAGER_H_
#include <ctemplate/template.h>
#include <ctemplate/template_dictionary.h>
#include "TemplateDataCollector.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "TaskManager.h"
#include "Notify/util/fcgi_request.h"
#include "IMWindow.h"
#include <map>
#include "PhotoUrlTools.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>



using namespace com::xiaonei::talk::util;
using namespace xce::notify;
using namespace ctemplate;
using namespace std;
using namespace com::xiaonei::talk::common;
using namespace MyUtil;

namespace talk {
namespace window{

const string LOGINREMINDTYPE = "1";
const string ADREMINDTYPE = "2";
const int KICKTIMESTAMP = 1;

class PathConfig : public IceUtil::Shared{
public:
	string template_path;
	int timeout;
	string type;
	string content_type;
	string reminder_type;
	string width;
	string height;
	string url;
	string showtime;
	string topmost;
	string title;
};
typedef IceUtil::Handle<PathConfig> PathConfigPtr;


class IMWindowRequestFactory: public RequestFactory{
public:
	virtual RequestPtr Create(FCGX_Request * r);
};


class IMWindowRequest : public Request{
public:
  IMWindowRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
	void ErrorResponse(int status_code);
};


class IMWindowManagerI : public IMWindowManager, virtual public Singleton<IMWindowManagerI> {
public:
	virtual void ReloadStaticPages(const Ice::Current& = Ice::Current());
	virtual void ReloadPathConfig(const Ice::Current& = Ice::Current());
	virtual void UserLoginReminder(const JidPtr& jid, const Ice::Current& = Ice::Current());
	virtual void UserAdReminder(const string& path, int userid,  const Ice::Current& = Ice::Current());
	virtual void UserAdReminderOnly(const string& path, const JidPtr& jid,  const Ice::Current& = Ice::Current());
	virtual void UsersAdReminder(const string& path, const MyUtil::IntSeq& userids, const Ice::Current& = Ice::Current());
  virtual void SetNotifyData(const string& notifydata,  const Ice::Current& = Ice::Current());
  virtual void NeedNotify(int userid, float ver,  const Ice::Current& = Ice::Current());

	PathConfigPtr GetConfigByPath(const string& path);
	MessageSeq GetXmppMessage(const PathConfigPtr& config, const MyUtil::IntSeq& userids);
	MessagePtr GetXmppMessage(const PathConfigPtr& config, const JidPtr& jid);
	TemplateDataCollectorPtr GetTempCollector(const string& path);

	IntSeq UserSendCheck(const MyUtil::IntSeq& userids);
	void KickTimeoutUser();

	void SetPath(const string& path){
		_path = path;
	}
private:
	IceUtil::RWRecMutex _rwmutex;
	//IceUtil::RWRecMutex::WLock lock(_rwmutex);
	map<string, PathConfigPtr> _config;
	map<int, time_t> _usersendcache;
	string _path;
};
	

class CheckUserSendTimer : public MyUtil::Timer{
public:
	CheckUserSendTimer(int delay) : Timer(delay){;
	}
	virtual void handle();
};



};
};

#endif //IMWindow 
