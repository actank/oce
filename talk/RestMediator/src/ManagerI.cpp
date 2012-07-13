#include "ManagerI.h"
#include <boost/lexical_cast.hpp>
//#include "BuddyCacheAdapter.h"
//#include "PassportAdapter.h"
#include "RestLogicAdapter.h"
#include <cmath>
#include <ServiceI.h>

using namespace talk::rest;
//using namespace com::xiaonei::service::buddy;
//using namespace mop::hi::svc::adapter;
using namespace std;
//using namespace xce::buddy;

namespace talk {
namespace rest {

static size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data) {
	string *buff = (string *) data;
	buff->append((char *) ptr, size * nmemb);
	return size * nmemb;
}

//struct WriteThis {
//	const char *readptr;
//	int sizeleft;
//};

//static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *data) {
//	struct WriteThis *pooh = (struct WriteThis *) data;
//	if (size * nmemb < 1)
//		return 0;
//	size_t wrote = 0;
//	while (pooh->sizeleft && wrote < size * nmemb) {
//		*(((char*) ptr) + wrote) = pooh->readptr[0];
//		wrote++;
//		pooh->readptr++;
//		pooh->sizeleft--;
//	}
//	return wrote;
//}

}
;
}
;
//----------------------------

inline std::string UrlQueryEscape::operator()(const std::string& in) const {
	// Everything not matching [0-9a-zA-Z.,_*/~!()-] is escaped.
	static unsigned long _safe_characters[8] = {
		0x00000000L, 0x03fff702L, 0x87fffffeL, 0x47fffffeL,
		0x00000000L, 0x00000000L, 0x00000000L, 0x00000000L
	};

	int max_string_length = in.size() * 3 + 1;
	char * out = new char[max_string_length];

	int i;
	int j;

	for (i = 0, j = 0; i < in.size(); i++) {
		unsigned char c = in[i];
		if (c == ' ') {
			out[j++] = '+';
		} else if ((_safe_characters[(c)>>5] & (1 << ((c) & 31)))) {
			out[j++] = c;
		} else {
			out[j++] = '%';
			out[j++] = ((c>>4) < 10 ? ((c>>4) + '0') : (((c>>4) - 10) + 'A'));
			out[j++] = ((c&0xf) < 10 ? ((c&0xf) + '0') : (((c&0xf) - 10) + 'A'));
		}
	}
	out[j++] = '\0';
	std::string ret(out);
	delete [] out;
	return ret;
}
//---------------------------
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&RestMediatorManagerI::instance(), service.createIdentity("M", ""));
}

void HttpRequest::handle() {
	TimeStat ts;
	string method = "";
	map<string,string>::iterator it;
	it = _parameters.find("method"); 
	if(it != _parameters.end()){
		method = it->second;
	}



	string xml;
	curl_easy_setopt(_urlHandle, CURLOPT_URL, _path.c_str());
	curl_easy_setopt(_urlHandle, CURLOPT_TIMEOUT_MS, 20000L);
	curl_easy_setopt(_urlHandle, CURLOPT_POST, 1);
	curl_easy_setopt(_urlHandle, CURLOPT_VERBOSE, 1L);

	ostringstream os;
	for (map<string,string>::iterator it = _parameters.begin(); it
			!= _parameters.end(); ++it) {
		if (it != _parameters.begin()) {
			os << "&";
		}
		os << it->first << "=" << UrlQueryEscape()(it->second);
		//os << it->first << "=" << (it->second);
	}
	//MCE_DEBUG("HttpRequest::handle() -->  Post data:"<< os.str());

	string encode = os.str();

	curl_easy_setopt(_urlHandle, CURLOPT_POSTFIELDS, encode.c_str());
	curl_easy_setopt(_urlHandle, CURLOPT_POSTFIELDSIZE, (curl_off_t)encode.length());

	{
		struct curl_slist *chunk = NULL;

		chunk = curl_slist_append(chunk, "Expect:");// data lenght more than 1024
		curl_easy_setopt(_urlHandle, CURLOPT_HTTPHEADER, chunk);
		/* use curl_slist_free_all() after the *perform() call to free this
		 list again */
	}

	curl_easy_setopt(_urlHandle, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(_urlHandle, CURLOPT_WRITEDATA, &xml);

	CURLcode success = curl_easy_perform(_urlHandle);

	//curl_formfree(post);
	RestResultPtr res = new RestResult;

	if (success != CURLE_OK) {
		string msg = curl_easy_strerror(success);
		MCE_WARN("HttpRequest::handle() --> curl_easy_perform err and code:"
				<< success << "  err msg:"<< msg << "   msg:"<<encode
				<<"  and length:"<<encode.length());
		res->err = -1;
		res->errstr = msg;
		try{
			RestLogicAdapter::instance().notifyResult(_reqId, res);
		}catch(Ice::Exception& e){
			MCE_WARN("HttpRequest::handle-->RestLogicAdapter::notifyResult-->" << e);
		}
		
		MCE_INFO("HttpRequest::handle --> error1 & userid = " << _userid << " method = " << method  << " usetime = " << ts.getTime());
		return;
	}
	//MCE_DEBUG("HttpRequest::handle() -->  curl_easy_perform OK");
	long response_code = 0;
	success = curl_easy_getinfo(_urlHandle, CURLINFO_RESPONSE_CODE,
			&response_code);
	if(success != CURLE_OK){
		string msg = curl_easy_strerror(success);
		res->err = -1;
		res->errstr = msg;
		try{
			RestLogicAdapter::instance().notifyResult(_reqId, res);
		}catch(Ice::Exception& e){
			MCE_WARN("HttpRequest::handle-->RestLogicAdapter::notifyResult-->" << e);
		}
		//MCE_WARN("HttpRequest::handle --> error2 & userid = " << _userid << " method = " << method  << " usetime = " << ts.getTime());
		MCE_WARN("HttpRequest::handle-->OpenPlatformAdapter::EasyPerform-->error userid=" << _userid << " method = " << method<< " usetime = " << ts.getTime());
		return;
	}
	//MCE_DEBUG("HttpRequest::handle() --> res:" << xml);
	res->err = response_code;
	res->xml = xml;
	try{
		MCE_DEBUG("HttpRequest::handle --> invoke RestLogicAdapter, reqid " << _reqId);
		RestLogicAdapter::instance().notifyResult(_reqId, res);
	}catch(Ice::Exception& e){
		MCE_WARN("HttpRequest::handle-->RestLogicAdapter::notifyResult-->" << e);
	}
/*	if (!(success == CURLE_OK && response_code == 200L)) {
		MCE_WARN("HttpRequest::handle() --> curl_easy_getinfo , response code:"
				<< response_code);
		return;
	}*/
/*	MCE_DEBUG("HttpRequest::handle() --> response 200 OK");
	xml_document doc;
	if (!doc.load(xml.c_str())) {
		MCE_WARN("HttpRequest::handle() --> xml parse err");
		return;
	}
	MCE_DEBUG("HttpRequest::handle() --> doc:"<<doc.xml());*/
/*	if (!isSuccess(doc)) {
		MCE_WARN("HttpRequest::handle() --> rest return error");
		return;
	}
*/
	//MCE_INFO("Http request success and length:"<< encode.length());
	MCE_INFO("HttpRequest::handle --> OK & userid = " << _userid << " method = " << method << " usetime = " << ts.getTime());
}
//---------------------------

/*
bool NotificationsRequest::_handle() {
	_parameters["method"] = "xiaonei.notifications.send";
	_parameters["session_key"] = boost::lexical_cast<string>(_userid);

	MyUtil::IntSeq ids = BuddyCacheAdapter::instance().getFriendList(_userid);
	if(ids.empty()) {
		return false;
	}
	ostringstream buddyids;
	for(size_t i = 0; i < ids.size(); ++i) {
		//	int c = ids.size() < 10? ids.size(): 10;
		//	for(size_t i = 0; i < c; ++i) {
		if(i) {
			buddyids << ",";
		}
		buddyids << ids.at(i);
	}
	_parameters["to_ids"] = buddyids.str();

	ostringstream msg[3];
	msg[0] << "<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>用<a href=\"http://gadget.talk.xiaonei.com/imhome.do?c=notify_page1\">校内通软件</a>处理理留言、回复和朋友新鲜事，不用开网页，特别方便 | <a href=\"http://gadget.talk.xiaonei.com/imdownload.do?d=notify_download1\">我也安装试试</a>";
	msg[1] << "<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>在用<a href=\"http://gadget.talk.xiaonei.com/imhome.do?c=notify_page2\">校内通软件</a>挂校内，第一时间收到你的照片、日志和留言回复 | <a href=\"http://gadget.talk.xiaonei.com/imdownload.do?d=notify_download2\">我也安装试试</a>";
	msg[2] << "<xn:name uid=\""<<_userid<<"\" linked=\"true\"/>安装了<a href=\"http://gadget.talk.xiaonei.com/imhome.do?c=notify_page3\">校内通软件</a>，能第一时间看到你的照片、日志和留言回复 | <a href=\"http://gadget.talk.xiaonei.com/imdownload.do?d=notify_download3\">我也安装试试</a>";

	_parameters["notification"] = msg[abs(_userid)%3].str();

	return true;
}

bool NotificationsRequest::isSuccess(xml_document& doc) {
	if (!doc.child("notifications_send_response").first_child()) {
		return true;
	}
	return false;
}*/
//---------------------------

/*
void UpdateUserDoingRequest::handle() {
	MCE_DEBUG("UpdateUserDoingRequest::handle --> userid=" << _userid << " doing=" << _doing);
	string ticket;
	try{
		ticket = PassportAdapter::instance().createTicket(_userid);
	}catch(Ice::Exception& e){
		MCE_WARN("UpdateUserDoingRequest::handle --> get ticket error, " << e);
	}

	CURL * urlHandle = curl_easy_init();
	if(!urlHandle){
		return;
	}
//	curl_easy_setopt(urlHandle, CURLOPT_URL,
//			"http://home.xiaonei.com/doing/update.do");
	curl_easy_setopt(urlHandle, CURLOPT_URL,
				"http://status.xiaonei.com/doing/update.do");

	curl_easy_setopt(urlHandle, CURLOPT_TIMEOUT_MS, 20000L);
	curl_easy_setopt(urlHandle, CURLOPT_POST, 1);
	curl_easy_setopt(urlHandle, CURLOPT_VERBOSE, 1L);
	ostringstream cookies;
	cookies << "societyguester=" << ticket << ";hostid=" << _userid;
	curl_easy_setopt(urlHandle, CURLOPT_COOKIE, cookies.str().c_str());
	MCE_DEBUG("HttpRequest::handle() -->  cookies:"<< cookies.str());

	string referer = "http://im.xiaonei.com";
	curl_easy_setopt(urlHandle, CURLOPT_REFERER, referer.c_str());

	ostringstream content;
	content << "c=" << UrlQueryEscape()(_doing.c_str()) << "&raw=" << UrlQueryEscape()(_doing.c_str());
	MCE_DEBUG("HttpRequest::handle() -->  Post data:"<< content.str());

	string c = content.str();
	curl_easy_setopt(urlHandle, CURLOPT_POSTFIELDS, c.c_str());
	curl_easy_setopt(urlHandle, CURLOPT_POSTFIELDSIZE,
			(curl_off_t)c.length());


	{
		struct curl_slist *chunk = NULL;

		chunk = curl_slist_append(chunk, "Expect:");// data lenght more than 1024
		curl_easy_setopt(urlHandle, CURLOPT_HTTPHEADER, chunk);

	}
	string res;
	curl_easy_setopt(urlHandle, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(urlHandle, CURLOPT_WRITEDATA, &res);

	CURLcode success = curl_easy_perform(urlHandle);

	//curl_formfree(post);
	if (success != CURLE_OK) {
		string msg = curl_easy_strerror(success);
		MCE_WARN("HttpRequest::handle() --> curl_easy_perform err and code:"
				<< success << "  err msg:"<< msg);
		curl_easy_cleanup(urlHandle);
		return;
	}
	MCE_DEBUG("HttpRequest::handle() -->  curl_easy_perform OK");
	long response_code = 0;
	success = curl_easy_getinfo(urlHandle, CURLINFO_RESPONSE_CODE,
			&response_code);
	if (!(success == CURLE_OK && response_code == 200L)) {
		MCE_WARN("HttpRequest::handle() --> curl_easy_getinfo , response code:"
				<< response_code);
		curl_easy_cleanup(urlHandle);
		return;
	}
	MCE_DEBUG("HttpRequest::handle() --> response 200 OK");
	MCE_DEBUG("HttpRequest::handle() --> content: " << res);
	curl_easy_cleanup(urlHandle);
}
*/
//--------------------------
void RestMediatorManagerI::restRequest(Ice::Long reqId, int userid, const string& path, const map<string, string>& paras, const Ice::Current&){
	TaskManager::instance().execute(new HttpRequest(reqId, path, paras, userid));
}
/*
void MediatorManagerI::useImNotify(Ice::Int userid, const Ice::Current&) {
	//TaskManager::instance().execute(new NotificationsRequest(userid));
}

void MediatorManagerI::updateUserDoing(Ice::Int userid, const string& doing, const Ice::Current&){
	//TaskManager::instance().execute(new UpdateUserDoingRequest(userid, doing));
}
*/
