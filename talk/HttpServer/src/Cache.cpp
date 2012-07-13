#include "compress.h"
#include "UserNameAdapter.h"
#include "Cache.h"
#include "PageCacheAdapter.h"
#include "TalkCacheLoaderAdapter.h"
//#include <cryptopp/gzip.h>
#include "FeedMemcProxy/client/talk_cache_client.h"

using namespace xce::talk;
using namespace xce::adapter::username;
using namespace talk::http;
using namespace MyUtil;
//using namespace CryptoPP;
using namespace com::xiaonei::talk;
using namespace com::xiaonei::talk::util;

ContentPtr Page::body(int id, const string & path,
		const map<string,string>& cookies, const map<string,string>& props) {
	IceUtil::Mutex::Lock lock(_mutex);
	if (_content == 0 || (_content->timeout >= 0 && time(NULL) - _loadTime
			> _content->timeout) || _content->statusCode != 200) {

		try {
			_content = PageCacheAdapter::instance().GetContent(id, path, cookies, props);
			_loadTime = time(NULL);

		} catch(Ice::Exception& e) {
			MCE_WARN("HttpServer_Page::body --> PageCacheAdapter::GetContent err --> "<<e);
		}
	}
	return _content;
}

ContentPtr Page::bodyGzip(int id, const string & path,
		const map<string,string>& cookies, const map<string,string>& props) {
	IceUtil::Mutex::Lock lock(_mutex);
	if (_content_gzip == 0 || (_content_gzip->timeout >= 0 && time(NULL)
			- _loadTimeGzip > _content_gzip->timeout)
			|| _content_gzip->statusCode != 200) {
		try {
			_content_gzip = PageCacheAdapter::instance().GetContent(id, path, cookies, props);
			if(!_content_gzip->isBin) {
				//_content_gzip->data = gzip_compress(_content_gzip->data);
				try{
					_content_gzip->data = gzip_compress( _content_gzip->data);
				}catch(std::exception& e){
					MCE_WARN("Page::bodyGzip --> gzip err:"<<e.what());
					_content_gzip->data = "";
					 _content_gzip->statusCode = 404;
				}catch(...){
					MCE_WARN("Page::bodyGzip --> gzip err");
					_content_gzip->data = "";
					_content_gzip->statusCode = 404;
				}

			}
			_loadTimeGzip = time(NULL);
		} catch(Ice::Exception& e) {
			MCE_WARN("HttpServer_Page::bodyGzip --> PageCacheAdapter::GetContent --> "<<e);
		}
	}
	return _content_gzip;
}

//------------------------

ContentPtr UserPage::body(const RequestPtr & r, int index) {
	//	pair<string, int> res;

	map<string,PagePtr>::iterator it = _pages.find(r->path());
	if (it == _pages.end()) {
		PagePtr p = new Page();
		_pages[r->path()] = p;
		if (r->encoding()) {
			return p->bodyGzip(index, r->path(), r->cookies(),
					r->getProperties());
			//			res.second = p->getStatusCode();
		} else {
			return p->body(index, r->path(), r->cookies(), r->getProperties());
			//			res.second = p->getStatusCode();
		}
	} else {
		if (r->encoding()) {
			return it->second->bodyGzip(index, r->path(), r->cookies(),
					r->getProperties());
			//			res.second = it->second->getStatusCode();
		} else {
			return it->second->body(index, r->path(), r->cookies(),
					r->getProperties());
			//			res.second = it->second->getStatusCode();
		}
	}

}
;
//----------------------------------
Ice::ObjectPtr UserNameFactoryI::create(Ice::Int id) {
	UserNameInfoPtr username;
	try{
		username = UserNameAdapter::instance().getUserName(id);
	}catch(Ice::Exception& e){
		MCE_WARN("UserNameFactoryI::create --> UserNameAdapter::getUserName--> id = " << id << "  error : "<<e);
	}
	if(!username){
		return 0;
	}
	UserNamePtr un = new UserName;
	un->name = username->name();
	return un;
}
