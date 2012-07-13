#include "Request.h"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "pugixml.hpp"
#include "Cache.h"
#include "Session.h"
//#include "TalkMessageAdapter.h"

using namespace talk::http;
using namespace boost::algorithm;
using namespace pugi;


bool Request::init(const map<string,string>& props) {
//	MCE_DEBUG("Request::init --> line:"<<__LINE__);

	map<string, string>::const_iterator it = props.find("method");
	if (it != props.end()) {
		if (it->second == "get") {
			_method = GET;
		} else if (it->second == "post") {
			_method = POST;
		} else {
			return false;
		}
	} else {
		return false;
	}
	//	MCE_DEBUG("Request::init --> line:"<<__LINE__);
	it = props.find("path");
	if (it != props.end() && (!it->second.empty())) {

		size_t pos = it->second.find('?');
		if (pos == string::npos) {
			_path = UriDecode(it->second);
		} else {
			_path = UriDecode(it->second.substr(0, pos));
			vector<string> splitVec;
			string tmp = it->second.substr(pos+1);
			split(splitVec, tmp, is_any_of("&"), token_compress_on);
			for (size_t i =0; i < splitVec.size(); ++i) {
				if (splitVec.at(i) != "") {
					size_t p = splitVec.at(i).find('=');
					if (p != string::npos) {
						if (p+1 < splitVec.at(i).length()) {
							_props[splitVec.at(i).substr(0,p)] = UriDecode(splitVec.at(i).substr(p+1));
						} else {
							_props[splitVec.at(i).substr(0,p)] = "";
						}
					} else {
						_props[splitVec.at(i)]="";
					}
				}
			}
		}
	} else {
		return false;
	}
	it = props.find("host");
	if (it != props.end()) {
		_host = it->second;
	} else {
		return false;
	}
	it = props.find("referer");
	if (it != props.end()) {
		_referer = it->second;
	}
	it = props.find("accept-encoding");
	if (it != props.end()) {
		string encode = to_lower_copy(it->second);
		size_t p = encode.find("gzip");
		if (p != string::npos) {
			_encoding = 1;
		} else {
			//			p = encode.find("deflate");
			//			if (p != string::npos) {
			//				_encoding = 2;
			//			}
		}

	}
	it = props.find("connection");
	if (it != props.end()) {
		if (to_lower_copy(it->second) == "keep-alive") {
			_isKeepAlive = true;
		}
	}
	it = props.find("body");
	if (it != props.end()) {
		_body = it->second;
	}
	it = props.find("cookie");
	if (it != props.end()) {
		vector<string> splitVec;
		split(splitVec, it->second, is_any_of(";"), token_compress_on);
		for (size_t i = 0; i < splitVec.size(); ++i) {
			string line = trim_copy(splitVec.at(i));
			if (line.empty()) {
				continue;
			}
			size_t p = line.find('=');
			if (p != string::npos && (p+1 < line.length())) {
				_cookies[line.substr(0,p)] = line.substr(p+1);
			}
		}
	}
	//MCE_DEBUG("Request::init --> toString:\r\n" << toString());
	return true;
}

RequestMethod Request::method() {
	return _method;
}
string Request::path() {
	return _path;
}

string Request::host() {
	return _host;
}
int Request::encoding() {
	return _encoding;
}

bool Request::keepAlive() {
	return _isKeepAlive;
}
string Request::referer() {
	return _referer;
}
string Request::body() {
	return _body;
}

string Request::cookie(const string& key) {
	map<string,string>::iterator it = _cookies.find(key);
	if (it != _cookies.end()) {
		return it->second;
	} else {
		return "";
	}
}

map<string,string> Request::cookies() {
	return _cookies;
}

string Request::getProperty(const string& key) {
	map<string,string>::iterator it = _props.find(key);
	if (it != _props.end()) {
		return it->second;
	} else {
		return "";
	}
}

map<string,string> Request::getProperties() {
	return _props;
}

string Request::toString() {
	ostringstream str;
	str << "--------to string------\r\n" << "method:"<<_method << "\r\n"
			<< "path:"<<_path << "\r\n" << "host:"<<_host << "\r\n"
			<< "encoding:"<<_encoding << "\r\n" << "keepalive:"<<_isKeepAlive
			<< "\r\n" << "referer:"<<_referer << "\r\n" << "body:"<<_body
			<< "\r\n" << "--------cookies--------\r\n";
	for (map<string,string>::iterator it = _cookies.begin(); it
			!= _cookies.end(); ++it) {
		str << it->first << " --> " << it ->second << "\r\n";
	}
	str << "--------get props------\r\n";
	for (map<string,string>::iterator it = _props.begin(); it != _props.end(); ++it) {
		str << it->first << " --> " << it ->second << "\r\n";
	}
	str << "-----------------------";

	return str.str();
}
//----------------------------------------

//void RequestHandler::loadConfig(const string& path) {
//	xml_document doc;
//	if (doc.load_file(path.c_str())) {
//
//		xml_node configNode = doc.child("config");
//		for (xml_node n = configNode.child("action"); n; n
//				= n.next_sibling("action")) {
//			if (n.attribute("url")) {
//				_config[n.attribute("url").value()].type = n.attribute("type").value();
//				string index = n.attribute("index_by_id").value();
//				if (index == "true") {
//					_config[n.attribute("url").value()].has_index = true;
//				} else {
//					_config[n.attribute("url").value()].has_index = false;
//				}
//			}
//		}
//
//	} else {
//		MCE_WARN("RequestAllocatee::loadConfig --> config file not find !!!");
//	}
//
//}
//
//void RequestHandler::handle(const Ice::ObjectPtr& session,
//		const RequestPtr& request) {
//
//	SessionPtr s = SessionPtr::dynamicCast(session);
//	s->requestWaitingType(1);
//	MCE_DEBUG("======= RequestAllocatee::handle [connid:"<<s->connectionId()
//			<<"]--> Request:"<<request->toString()<<" ============");
//
//	map<string,ActionConfig>::iterator it = _config.find(request->path());
//
//	if (it != _config.end()) {
//		if (it->second.type == "comet_get") {
//			MCE_DEBUG("RequestAllocatee::handle --> line:"<<__LINE__);
//
//			CometPushHandle(s, request, it->second.has_index);
//			return;
//		} else {
//			if (s->verify()) {
//				//用户Push的Socket被用作别的请求，所以需要先取消Bind
//				s->stopped();
//				s->verify(false);
//			}
//			if (it->second.type == "comet_broadcast") {
//
//				MCE_DEBUG("RequestAllocatee::handle --> line:"<<__LINE__);
//				CometBroadcastHandle(s, request, it->second.has_index);
//				//				if (!page.empty()) {
//				//					s->deliver(page);
//				//				} else {
//				//					string page;
//				//					response(404, false, false, "comet broadcast err");
//				//					s->deliver(page);
//				//				}
//				return;
//			} else {
//				MCE_DEBUG("RequestAllocatee::handle --> line:"<<__LINE__);
//				string page;
//				DefaultPageHandle(s, request, it->second.has_index);
//
//				return;
//			}
//		}
//
//	} else {
//		//		s->waitingDeliver(true);
//		MCE_WARN("ERROR[404]: unknowen url:"<<request->path());
//
//		s->response(404, false, true, "ERROR[404]:unknowen url");
//
//	}
//}
//
//void RequestHandler::DefaultPageHandle(const SessionPtr& session,
//		const RequestPtr& request, bool has_index) {
//
//	int id = 0;
//	if (has_index) {
//		string kl = request->cookie("kl");
//		if (kl != "") {
//			size_t p = kl.find('_');
//			if (p != string::npos && kl.length() > p+1) {
//				string hostid = kl.substr(p+1);
//				id = boost::lexical_cast<int>(hostid);
//			}
//		}
//	}
//
//	UserPagePtr up = ServiceI::instance().locateObject<UserPagePtr>(UPC, id);
//	pair<string,int> body = up->body(request, id);
//
//	session->response(body.second,(bool)request->encoding(),request->keepAlive(), body.first,60);
//
//}
//
//void RequestHandler::CometPushHandle(const SessionPtr& session,
//		const RequestPtr& request, bool has_index) {
//
//	MCE_DEBUG("RequestHandler::CometPushHandle --> connection_id:"
//			<< session->connectionId());
//	string kl = request->cookie("kl");
//	int hostid = 0;
//	string ticket = request->cookie("societyguester");
//	string xntalk_session = request->cookie("xntalk_session");
//	string header;
//	if (kl != "") {
//		size_t p = kl.find("_");
//		if (p != string::npos) {
//			try {
//				if(p+1 < kl.length()) {
//					hostid = boost::lexical_cast<int>(kl.substr(p+1));
//				}
//			} catch(std::exception& e) {
//				MCE_WARN("RequestHandler::CometPushHandle --> hostid  not a int  err:"<<e.what());
//			}
//
//		}
//	}
//	if (hostid==0 || ticket == "") {
//		session->response(403, false, false, header,
//				"useid or ticket is not define , please login first.");
//		return;
//	}
//
//	bool success = false;
//	if (!session->verify()) {
//		try {
//			if(xntalk_session == "") {
//				MCE_DEBUG("CometGetAction::handle --> line:"<<__LINE__);
//				ostringstream os;
//
//				//				os<< session->connectionId();
//				//				xntalk_session = os.str();
//
//				//				os.str("");
//				os << "Set-Cookie: xntalk_session=" << session->connectionId() <<"; domain=.xiaonei.com; path=/\r\n";
//				header = os.str();
//				session->jid(hostid,session->connectionId());
//			} else {
//				MCE_DEBUG("CometGetAction::handle --> line:"<<__LINE__);
//				session->jid(hostid,boost::lexical_cast<Ice::Long>(xntalk_session));
//			}
//
//			success = session->bind( ticket);
//			MCE_DEBUG("CometGetAction::handle --> success:"<<success<< " session_key:"<<request->getProperty("session_key") << " ticket:"<<ticket);
//			if (success) {
//				MCE_DEBUG("CometGetAction::handle --> line:"<<__LINE__);
//				session->requestWaitingType(2);
//				session->verify(true);
//				session->isGzip(request->encoding());
//
//				MCE_DEBUG("CometGetAction::handle --> line:"<< __LINE__);
//				session->response(200, false,true,header,"");
//				MCE_DEBUG("CometGetAction::handle --> line:"<< __LINE__);
//
//				return;
//
//			}
//		} catch(Ice::Exception& e) {
//			MCE_WARN("CometGetAction::handle -> failure binding, userid: " << hostid << ", " << e);
//			session->response(403, false,false,"userid bind err");
//			return;
//		}
//	}
//	if (!session->verify()) {
//		MCE_WARN("CometGetAction::handle -> verify is false, userid: "
//				<< hostid);
//		session->response(403, false, false, "verify is false");
//		return;
//	}
//	// hold request
//	MCE_DEBUG("Comet hold  --> connid:"<< session->connectionId());
//	success = session->bind( ticket);
//	session->requestWaitingType(2);
//	session->response();
//
//}
//
//void RequestHandler::CometBroadcastHandle(const SessionPtr& session,
//		const RequestPtr& request, bool has_index) {
//
//	MCE_DEBUG("CometBroadcastHandle::handle()  --> " << __LINE__);
//	if (request->cookie("xntalk_session") == "") {
//		session->response(403, false, false,
//				"do comet broadcast , but not login");
//		return;
//	}
//	MCE_DEBUG("CometBroadcastHandle::handle()  --> " << __LINE__);
//	string cmd = request->getProperty("cmd");
//	if (cmd == "onlinebuddylist") {
//
//	} else if (cmd == "msg") {
//		string toid = request->getProperty("toid");
//		string m = request->getProperty("msg");
//		MCE_DEBUG("toid:"<<toid << " msg:"<<m);
//
//		string hostid = request->cookie("hostid");
//
//		if (hostid != "") {
//			ostringstream msg;
//			msg << "<message to='"<<toid<<"@talk.xiaonei.com' from='" << hostid
//					<<"@talk.xiaonei.com' type='chat'><body>"<<m
//					<<"</body></message>";
//			MCE_DEBUG("send msg:"<<msg.str());
//			JidPtr j = new Jid();
//			j -> userId = boost::lexical_cast<int>(hostid);
//			j -> endpoint = ServiceI::instance().getName();
//			j -> index = 0;
//			TalkMessageAdapter::instance().message(j, msg.str());
//		}
//	}
//	session->response(200, false, true,"");
//}
//
