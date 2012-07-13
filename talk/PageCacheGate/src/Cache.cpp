#include "Cache.h"
#include "PageCacheAdapter.h"

using namespace talk::http;
using namespace MyUtil;

ContentPtr Page::body(int uid, const string & path, const map<string,string>& cookies, const map<string,string>& props) {
  ContentPtr content;
  {
    IceUtil::Mutex::Lock lock(_mutex);
    if (_content 
        && _content->timeout >= 0 
        && (time(NULL) - _loadTime > _content->timeout)
        && _content->statusCode == 200) {
      content = _content;
    }
  }
  if (!content) {
    try {
      content = PageCacheAdapter::instance().GetContent(uid, path, cookies, props);
    } catch(Ice::Exception& e) {
      MCE_WARN("Page::body --> get PageCacheAdapter err:"<<e);
    }
    {
      IceUtil::Mutex::Lock lock(_mutex);
      _content = content;
      _loadTime = time(NULL);
    }
  }
  return content;
}

ContentPtr UserPages::body(int uid, const string & path, const map<string,string>& cookies, const map<string,string>& props) {
  map<string,PagePtr>::iterator it = _pages.find(path);
  if (it != _pages.end()) {
    return it->second->body(uid, path, cookies, props);
  }
  PagePtr p = new Page();
  _pages[path] = p;
  return p->body(uid, path, cookies, props);
}

