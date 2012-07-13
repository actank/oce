#ifndef REQUEST_H_
#define REQUEST_H_

#include <Ice/Ice.h>
#include <IceUtil/Mutex.h>

#include "Singleton.h"
#include "util.h"

namespace talk {
namespace http {
using namespace MyUtil;

enum RequestMethod {GET,POST};

class Request : public Ice::Object {
	RequestMethod _method;
	string _path;

	string _host;
	//string _userAgent;
	//string _accept;
	int _encoding; // 0 is not compress
	// 1 can gzip
	// 2 can zip use zlib
	bool _isKeepAlive;
	string _referer;
	string _body;

	map<string,string> _cookies;
	map<string,string> _props;

public:
	Request() :
		_method(GET), _encoding(0), _isKeepAlive(false) {

	}
	;
	bool init(const map<string,string>& props);

	RequestMethod method();
	string path();
	string host();
	int encoding();
	bool keepAlive();
	string referer();
	string body();
	string cookie(const string& key);
	string getProperty(const string& key);
	map<string,string> cookies();
	map<string,string> getProperties();

	string toString();

};

//------------------------
typedef ::IceInternal::Handle<Request> RequestPtr;

//------------------------
//class RequestHandler : public Singleton<RequestHandler> {
//
//public:
//	void loadConfig(const string& path = "/opt/XNTalk/etc/XNHttpServer.conf");
//
//	void handle(const Ice::ObjectPtr& session, const RequestPtr& request);
////	void response(int status_code,bool is_gzip,bool keep_alive, const string& body, string& page);
////	void response(int status_code,bool is_gzip,bool keep_alive, const string& extra_header, const string& body,string& page);
//private:
//	
//	void DefaultPageHandle(const SessionPtr& session, const RequestPtr& request, bool has_index);
//	void CometPushHandle(const SessionPtr& session, const RequestPtr& request, bool has_index);
//	void CometBroadcastHandle(const SessionPtr& session, const RequestPtr& request, bool has_index);
//	
//	
//	struct ActionConfig {
//		string type;
//		bool has_index;
//	};
//
//	map<string,ActionConfig> _config;
//
//};

}
;
}
;

#endif /*REQUEST_H_*/
