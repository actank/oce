#ifndef CACHE_H_
#define CACHE_H_

#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ServiceI.h"

#include "PageCache.h"

namespace talk {
namespace http {

const int UPC = 0;
const int UN = 1;

class Page : public IceUtil::Shared {
public:
  Page() : _loadTime(0) {}

  ContentPtr body(int id, const string & path,
      const map<string,string>& cookies, const map<string,string>& props);
private:
  size_t _loadTime;
  ContentPtr _content;
  IceUtil::Mutex _mutex;
};
typedef IceUtil::Handle<Page> PagePtr;

class UserPages : public Ice::Object {
  map<string,PagePtr> _pages;
public:
  ContentPtr body(int id, const string & path, const map<string,string>& cookies, const map<string,string>& props);
};
typedef IceUtil::Handle<UserPages> UserPagesPtr;

/*
class UserPagesFactoryI : public MyUtil::ServantFactory {
public:
  virtual Ice::ObjectPtr create(Ice::Int id) {
    MCE_DEBUG("UserPageFactoryI::create --> line:"<<__LINE__);
    return new UserPages();
  }
};
*/

}
}
#endif // CACHE_H_
