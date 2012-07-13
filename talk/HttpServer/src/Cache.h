#ifndef CACHE_H_
#define CACHE_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ServiceI.h"

#include "PageCache.h"
#include "Request.h"

namespace talk {
namespace http {

const int UPC = 0;
const int UN = 1;

class Page : public Ice::Object {
public:
	Page() :
		_loadTime(0), _loadTimeGzip(0) {
	}
	;

	ContentPtr body(int id, const string & path,
			const map<string,string>& cookies, const map<string,string>& props);
	ContentPtr bodyGzip(int id, const string& path,
			const map<string,string>& cookies, const map<string,string>& props);

private:
	size_t _loadTime;
	size_t _loadTimeGzip;

	ContentPtr _content;
	ContentPtr _content_gzip;
	IceUtil::Mutex _mutex;
};

typedef IceUtil::Handle<Page> PagePtr;

class UserPage : public Ice::Object {
	map<string,PagePtr> _pages;
public:

	ContentPtr body(const RequestPtr & r, int index = 0);
};

typedef IceUtil::Handle<UserPage> UserPagePtr;

class UserPageFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id) {
		MCE_DEBUG("UserPageFactoryI::create --> line:"<<__LINE__);
		return new UserPage;
	}
};

//------------------------------

class UserName : public Ice::Object {
public:
	string name;
};
typedef IceUtil::Handle<UserName> UserNamePtr;

class UserNameFactoryI : public ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

}
}
#endif /*CACHE_H_*/
