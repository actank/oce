#ifndef MANAGERI_H_
#define MANAGERI_H_

#include <curl/curl.h>
#include "TaskManager.h"
#include "pugixml.hpp"
#include "RestMediator.h"
#include "Singleton.h"

namespace talk {
namespace rest {
using namespace MyUtil;
using namespace pugi;

// Escapes characters not in [0-9a-zA-Z.,_:*/~!()-] as %-prefixed hex.
// Space is encoded as a +.
struct UrlQueryEscape { std::string operator()(const std::string&) const; };

class TimeStat{
	timeval _begin, _end;
	public:
	TimeStat(){
		reset();
	}
	void reset(){
		gettimeofday(&_begin, NULL);
	}
	float getTime(){
		gettimeofday(&_end, NULL);
		float timeuse=1000000*(_end.tv_sec-_begin.tv_sec)+_end.tv_usec
			-_begin.tv_usec;
		timeuse/=1000;
		return timeuse;
	}
};




class HttpRequest : public MyUtil::Task
{
public:
	HttpRequest(Ice::Long reqId, const string& path, const map<string, string>& paras, int userid)
				:Task() , _reqId(reqId), _path(path), _parameters(paras), _urlHandle(NULL), _userid(userid) {
		//_parameters["api_key"] = "ffa4add0b30141aa9eb9a8f1af34a8c3";
		//_parameters["v"] = "1.0";
		//_parameters["format"] = "XML";
		_urlHandle = curl_easy_init();
	};
	~HttpRequest() {
		if(_urlHandle) {
			curl_easy_cleanup(_urlHandle);
		}
	};
	void handle();
protected:
	//virtual bool _handle();
	//virtual bool isSuccess(xml_document& doc) = 0;

	Ice::Long			_reqId;
	string				_path;
	map<string,string> 	_parameters;
	CURL 				*_urlHandle;
	xml_document 		_doc;
	int _userid;
};
/*
class NotificationsRequest : public HttpRequest {
	int _userid;
public:
	NotificationsRequest(int userid):HttpRequest(),_userid(userid) {};
protected:
	virtual bool _handle() ;
	virtual bool isSuccess(xml_document& doc);
};


class UpdateUserDoingRequest : public MyUtil::Task {
	int _userid;
	string _doing;
public:
	UpdateUserDoingRequest(int userid, const string& doing):
		_userid(userid), _doing(doing) {};
	virtual void handle();
};
*/
class RestMediatorManagerI : public RestMediatorManager,
							public Singleton<RestMediatorManagerI>{
public:
	//virtual void useImNotify(Ice::Int userid, const Ice::Current& = Ice::Current());
	//virtual void updateUserDoing(Ice::Int userid, const string& doing, const Ice::Current& = Ice::Current());
	virtual void restRequest(Ice::Long reqId, int userid, const string& path, const map<string, string>& paras, const Ice::Current& = Ice::Current());
};

};
}
;
#endif /*MANAGERI_H_*/
