#ifndef _DATALOADERI_H_
#define _DATALOADERI_H_


#include "Notify.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"

namespace xce {
namespace notify {

using namespace MyUtil;
using namespace com::xiaonei::xce;

class DataLoaderRequest : public Request{
public:
  DataLoaderRequest(FCGX_Request* r) : Request(r) {}

  virtual bool response();
};

class DataLoaderI : public FeedGate, public MyUtil::Singleton<DataLoaderI> {
public:
  DataLoaderI();
};

}
}

#endif 

