#ifndef HTTPPARSER_H_
#define HTTPPARSER_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>

#include "Request.h"


namespace talk {
namespace http {

class Session;
typedef IceUtil::Handle<Session> SessionPtr;

class HttpParser : public IceUtil::Shared {
	map<string,string> _props;
	string _tmp;
	string _head;
	string _postData;
	int _contentLength;
public:
	HttpParser() :
		_contentLength(0) {
	}
	;
	bool feed(const SessionPtr& sesson, string& data);
};

typedef IceUtil::Handle<HttpParser> HttpParserPtr;

}
}
#endif /*HTTPPARSER_H_*/
