#include "Session.h"
#include "Server.h"

#include "IceLogger.h"
#include <boost/lexical_cast.hpp>

using namespace MyUtil;
using namespace com::xiaonei::talk;


//void HttpParser::feed(const Ice::ObjectPtr& channel, const string& data) {
//	MCE_DEBUG("call HttpParser::feed");
//	if (data == "") {
//		return;
//	}
//	string str(_str + data);
//	if (str.length() > 4) {
//		int len = str.length();
//		if ((str[len - 4] == '\r') && (str[len - 3] == '\n') && (str[len - 2]
//				== '\r') && (str[len - 1] == '\n')) {
//			MCE_DEBUG("HttpParser::feed --> is end");
//			//size_t pos = str.find("getonlinefriends.do");
//			size_t pos = str.find("test");
//			if (pos != string::npos) {
//				MCE_DEBUG("HttpParser::feed --> find .do");
//				pos = str.find("hostid=");
//				if (pos != string::npos) {
//
//					size_t pos_end = str.find(";", pos+7);
//					MCE_DEBUG("HttpParser::feed --> find hostid");
//					if (pos_end != string::npos) {
//						string hostid = str.substr(pos+7, pos_end-7-pos);
//						MCE_DEBUG("HttpParser::feed --> hostid:"<<hostid);
//						pos = str.find("societyguester=");
//						if (pos != string::npos) {
//							pos_end = str.find(";", pos+15);
//							if (pos_end != string::npos) {
//								string ticket = str.substr(pos+15, pos_end - 15
//										- pos);
//								RequestHandler::instance().handle(channel, boost::lexical_cast<int>(hostid),ticket,OnlineList);
//
//								_str.clear();
//								return;
//							}
//						}
//					}
//
//				}
//			}
//
//			//_str.clear();
//			SessionPtr::dynamicCast(channel)->shutdown();
//			return;
//		}
//	}
//	_str.swap(str);
//
//	/*for (string::const_iterator it = data.begin(); it != data.end(); ++it) {
//	 if (*it != 0) {
//	 str.push_back(*it);
//	 } else {
//	 xml_document_ptr doc = new xml_document();
//	 if (doc->load(str.c_str())) {
//	 _handler->handle(channel, doc);
//	 str.clear();
//	 }
//	 }
//	 }
//	 _str.swap(str);*/
//}

//----------------------------------------------------------------------
Session::Session() {
	//_parser = new HttpParser();
	//_state = UNVERIFY;
	_counter = 0;
	_isClose = false;
}

Session::~Session() {

}

void Session::started() {
	//Server::instance().apply(this);
}

void Session::stopped() {

	//Server::instance().leave(this);
}

bool Session::messaged(const string& message) {
	//MCE_DEBUG("Session::message -->"<< message);
	
	if(message[message.length()-1] == '\0'){
		//xiaonei
		string msg = "<?xml version=\"1.0\"?><!DOCTYPE cross-domain-policy SYSTEM \"/xml/dtds/cross-domain-policy.dtd\"><cross-domain-policy><site-control permitted-cross-domain-policies=\"master-only\"/><allow-access-from domain=\"*.xiaonei.com\" to-ports=\"39000\" /><allow-access-from domain=\"xiaonei.com\" to-ports=\"39000\" /></cross-domain-policy>";
		
		//kaixin
		//string msg = "<?xml version=\"1.0\"?><!DOCTYPE cross-domain-policy SYSTEM \"/xml/dtds/cross-domain-policy.dtd\"><cross-domain-policy><site-control permitted-cross-domain-policies=\"master-only\"/><allow-access-from domain=\"*.kaixin.com\" to-ports=\"39000\" /><allow-access-from domain=\"kaixin.com\" to-ports=\"39000\" /></cross-domain-policy>";
				
		msg.push_back('\0');
		deliver(msg);
		setCloseStat();
	}
	return true;
//	if (_counter + message.length() > MAX_MSG_SIZE) {
//		return false;
//	} else {
//		_counter += message.length();
//		if (_parser) {
//			_parser->feed(this, message);
//		}
//		return true;
//	}
}

void Session::finishWrite(){
	if(_isClose){
		MCE_DEBUG("Session::finishWrite --> do shutdown");
		shutdown();
	}
}
