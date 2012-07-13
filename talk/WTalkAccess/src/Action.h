#ifndef __ACTION_H__
#define __ACTION_H__

#include "TaskManager.h"
#include "Session.h"
#include "Server.h"
//#include "Type.h"
#include <queue>

namespace com {
namespace xiaonei {
namespace talk {

using namespace std;
using namespace MyUtil;

class ProxyAction : public AsioAction {
public:
	virtual void execute(const AsioChannelPtr& channel,
			const Ice::ObjectPtr& data) {
		_session = SessionPtr::dynamicCast(channel);
		_doc = xml_document_ptr::dynamicCast(data);

		execute();
	}
	;
	virtual void execute() = 0;
protected:
	SessionPtr _session;
	xml_document_ptr _doc;
};
typedef IceUtil::Handle<ProxyAction> ProxyActionPtr;


//---------------------------------------------------------------------------
/*
class StartAction : public ProxyAction {
public:
	virtual void execute();
};

class StopAction : public ProxyAction {
public:
	virtual void execute();
};
*/

class LoginAction : public ProxyAction {
public:
	virtual void execute();
};

class ChatAction : public ProxyAction {
public:
	virtual void execute();
};

//-----------------------------------------------
class SessionListener : public Singleton<SessionListener> {
public:
	//virtual void started(Ice::ObjectPtr channel);
	//virtual void stopped(Ice::ObjectPtr channel);
	virtual void messaged(SharedPtr channel, xml_document_ptr& doc);
	void processPresence(const SessionPtr& session, const xml_document_ptr& doc);
};

//-----------------------------------------------
class OnlineSizeStater : public MyUtil::Timer {

public:
	OnlineSizeStater(size_t timeslot = 60000) :
		Timer(timeslot) {
	}
	;
	virtual void handle();
};

//-----------------------------------------------
class OnlineSizeMoniter : public MyUtil::Timer {

public:
	OnlineSizeMoniter(size_t timeslot = 1000) :
		Timer(timeslot) {
	}
	;
	virtual void handle();
};


}
;
}
;
}
;

#endif
