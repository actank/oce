#ifndef __ACTION_H__
#define __ACTION_H__

#include "TaskManager.h"
#include "Session.h"
#include "Server.h"
//#include "Type.h"
#include <queue>
#include "Connection.h"

namespace com {
namespace xiaonei {
namespace talk {

using namespace std;
using namespace MyUtil;
using namespace ::xce::nio;

class XmppAction : public AsioAction {
public:
	virtual void execute(const AsioChannelPtr& channel,
			const Ice::ObjectPtr& data) {
		_session = SessionPtr::dynamicCast(channel);
		_doc = xml_document_ptr::dynamicCast(data);
		execute();
	}
	virtual void execute() = 0;
protected:
	SessionPtr _session;
	xml_document_ptr _doc;
};
typedef IceUtil::Handle<XmppAction> XmppActionPtr;

//---------------------------------------------------------------------------
//class StartAction : public XmppAction {
//public:
//	virtual void execute();
//};
//
//class StopAction : public XmppAction {
//public:
//	virtual void execute();
//};

class StreamAction : public XmppAction {
public:
	virtual void execute();
private:
	bool verifyVersion(const string& version);
	string buildStreamBegin();
	string buildStreamEnd();
	string buildStreamError(const string& text);
	string buildFirstStreamFeature();
	string buildSecondStreamFeature();
	string buildBindSessionFeature();
};

class StartTlsAction : public XmppAction {
protected:
	virtual void execute();
private:
	string buildTlsProceed();
	string buildTlsFailure();
};

class AuthAction : public XmppAction {
protected:
	virtual void execute();
	virtual void exception(Ice::Exception& e);
private:
	string buildSaslSuccess();
	string buildSaslFailure(const string& tag);
};

class PresenceAction : public XmppAction {
protected:
	virtual void execute();
};

class DefultAction : public XmppAction {
protected:
	virtual void execute();
};

class IqAction : public XmppAction {
protected:
	virtual void execute();
};

//-----------------------------------------------
class SessionListener : public TagHandler, public Singleton<SessionListener> {
public:
//	virtual void started(Ice::ObjectPtr channel);
//	virtual void stopped(Ice::ObjectPtr channel);
	virtual void messaged(SharedPtr channel, xml_document_ptr& doc);
	void processPresence(const SessionPtr& session, const xml_document_ptr& doc);
};

//---------------------------------------------------------------------------
class OnlineSizeStater : public MyUtil::Timer {
	//size_t _timeslot;
public:
	OnlineSizeStater(size_t timeslot = 60000) :
		Timer(timeslot) {
	}
	;
	virtual void handle();
};
//----------------------------------------------------------------------------

//-----------------------------------------------
class OnlineSizeMoniter : public MyUtil::Timer {

public:
	OnlineSizeMoniter(size_t timeslot = 1000) :
		Timer(timeslot) {
	}
	;
	virtual void handle();
};

class LoginProcedureStat : public MyUtil::Timer,
							public Singleton<LoginProcedureStat>{
public:
	LoginProcedureStat(size_t timeslot = 10*1000) :
		Timer(timeslot) {
		clear();
	}
	void clear(){
		connect = 0;
		tls = 0;
		verify = 0;
		bind = 0;
		presence = 0;
	}
	void incConnect(){
		connect++;
	}
	void incTls(){
		tls++;
	}
	void incVerify(){
		verify++;
	}
	void incBind(){
		bind++;
	}
	void incPresence(){
		presence++;
	}
	virtual void handle(){
		MCE_INFO("~~ProcedureStat connect " << connect
				<< ", tls " << tls
				<< ", verify " << verify
				<< ", bind " << bind
				<< ", presence " << presence);
		clear();
	}
private:
	int connect;
	int tls;
	int verify;
	int bind;
	int presence;
};

}
;
}
;
}
;

#endif
