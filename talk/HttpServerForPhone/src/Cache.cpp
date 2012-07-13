#include "compress.h"

#include "Cache.h"
#include "PageCacheAdapter.h"
#include "TalkCacheLoaderAdapter.h"
//#include <cryptopp/gzip.h>
#include "FeedMemcProxy/client/talk_cache_client.h"

using namespace xce::talk;

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
			if(_content){
				MCE_INFO("Page::body --> userid:" << id << " path:" << path << " size:" << _content->data.size());
			}else{
				MCE_INFO("Page::body --> userid:" << id << " path:" << path << " error");
			}
		} catch(Ice::Exception& e) {
			MCE_WARN("Page::body --> get PageCacheAdapter err, userid:" << id << " path:" << path << e);
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
			int ariginalSize = _content_gzip->data.size();
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
			if(_content_gzip){
				MCE_INFO("Page::bodyGzip --> userid:" << id << " path:" << path << " gzip size:" << _content_gzip->data.size() << " original size:" << ariginalSize);
			}else{
				MCE_INFO("Page::bodyGzip --> userid:" << id << " path:" << path << " error");
			}

		} catch(Ice::Exception& e) {
			MCE_WARN("Page::bodyGzip --> get PageCacheAdapter err, userid:" << id << " path:" << path << e);
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
	TalkUserPtr user;
	try{
		user = TalkCacheClient::instance().GetUserByIdWithLoad(id);
	}catch(Ice::Exception& e){
		MCE_WARN("UserNameFactoryI -> call TalkCacheClient err:"<<e);
	}
	if(user){
		if(user->id != id){
			try{
				user = TalkCacheLoaderAdapter::instance().LoadById(id);
			}catch(Ice::Exception& e){
				MCE_WARN("UserNameFactoryI -> call TalkCacheLoader err:"<<e);
			}
			MCE_WARN("UserNameFactoryI -> memcache get user info error id = " << id);
		}
		if(user){
			UserNamePtr un = new UserName;
			un->name = user->name;
			return un;
		}
		return 0;
	}
	return 0;
}
