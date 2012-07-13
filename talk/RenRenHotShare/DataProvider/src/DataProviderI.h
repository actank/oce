#ifndef _DATAPROVIDERI_H_
#define _DATAPROVIDERI_H_


#include "Notify.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"

namespace xce {
namespace notify {

using namespace MyUtil;
using namespace com::xiaonei::xce;

class DataProviderRequest : public Request{
public:
  DataProviderRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class PhotoListRequest : public Request{
public:
  PhotoListRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class DataProviderI : public FeedGate, public MyUtil::Singleton<DataProviderI> {
public:
  DataProviderI();
};

}
}

#endif 

