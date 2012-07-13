#include "Session.h"
#include "Manager.h"
#include "Server.h"
#include "Action.h"
#include "XmppTools.h"
//#include "TalkLogicAdapter.h"

using namespace MyUtil;
using namespace com::xiaonei::talk;
//using namespace mop::hi::svc::adapter;

//----------------------------------------------------------------------
Session::Session() :
	_verify(false) {
	_binding = 0;

}

Session::~Session() {
	MCE_DEBUG("~session");
	Server::instance().leave();

}

void Session::started() {
	MCE_DEBUG((void*)this << "started");
	//SessionListener::instance().started(this);
	Server::instance().join();
}

bool Session::bind(const string& sessionKey, const string& ticket) {
	IceUtil::Mutex::Lock lock(_bindMutex);

	if (_binding != 0) {
		MCE_DEBUG("[Session::bind] isBind:"<<_binding<<" "<<connectionId());
		return false;
	}
	//MCE_DEBUG("[Session::bind] isBind:"<<_isBind<<" "<<connectionId());
	_binding = 1;
	_sessionKey = sessionKey;
	Server::instance().bind();

	if(_jid){
		MCE_INFO("[Session::bind] bind:" << jidToString(_jid));
	}
	return WTalkProxyAdapter::instance().bind(sessionKey, ticket, _jid);

}

void Session::stopped() {
	IceUtil::Mutex::Lock lock(_bindMutex);
	//SessionListener::instance().stopped(this);
	MCE_DEBUG((void*)this << "stop  isBind:"<<_binding);

	if (_binding == 1) {
		try{
			if(_jid){
				MCE_INFO("[Session::stopped] unbind:" << jidToString(_jid));
			}
			WTalkProxyAdapter::instance().unbind(_sessionKey,_jid);
		}catch(Ice::Exception& e){
			if(_jid){
				MCE_WARN("Session::stopped --> " << jidToString(_jid) << " unbind err:"<<e);
			}else{
				MCE_WARN("Session::stopped --> unbind err:"<<e);
			}
		}catch(...){
			MCE_WARN("Session::stopped --> unbind exception");
		}
		Server::instance().unbind();
	}
	_binding = 2;
}

bool Session::messaged(const string& message) {
	if (_buffer.length() + message.length() > MAX_MSG_SIZE) {
		return false;
	}
	string str(_buffer);
	for (string::const_iterator it = message.begin(); it != message.end(); ++it) {
		if (*it != 0) {
			str.push_back(*it);
		} else {
			//MCE_DEBUG("Session::messaged --> CONN_ID:"<<connectionId()<<" MSG:"<<str);
			xml_document_ptr doc = new xml_document();
			if (doc->load(str.c_str())) {
				SessionListener::instance().messaged(this, doc);
				str.clear();
			}
		}
	}
	return true;
}
