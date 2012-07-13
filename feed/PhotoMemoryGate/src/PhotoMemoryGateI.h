#ifndef _PHOTO_MEMORY_GATE_H_
#define _PHOTO_MEMORY_GATE_H_

#include <ctemplate/template.h>
#include <boost/lexical_cast.hpp>

#include "PhotoMemory.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"
#include "UserCacheAdapter.h"

namespace xce {
namespace feed {

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::usercache;
using namespace xce::notify;
using namespace ctemplate;

class PhotoMemoryGetRequest : public Request{
public:
  PhotoMemoryGetRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class PhotoMemoryReadRequest : public Request{
public:
  PhotoMemoryReadRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class PhotoMemoryGateI : public PhotoMemoryGate, public MyUtil::Singleton<PhotoMemoryGateI> {
public:
  PhotoMemoryGateI();
  virtual bool ReloadTemplate(const Ice::Current& = Ice::Current());
  virtual string GetPhotos(Ice::Int uid, Ice::Int begin, Ice::Int limit, const Ice::Current& = Ice::Current());
  int BuildListDict(Ice::Int uid, Ice::Int begin, Ice::Int limit, ctemplate::TemplateDictionary * dict);
protected:
  bool GetUserCache(Ice::Int uid, const PhotoContentSeq & photos, map<Ice::Int, UserCachePtr> * user_caches);
};

}
}

#endif // _PHOTO_MEMORY_GATE_H_

