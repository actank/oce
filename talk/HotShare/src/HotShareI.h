#ifndef _HOTSHAREI_H_
#define _HOTSHAREI_H_


#include "Notify.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"

namespace xce {
namespace notify {

using namespace MyUtil;
using namespace com::xiaonei::xce;

class HotShareRequest : public Request{
public:
  HotShareRequest(FCGX_Request* r) : Request(r) {}

  virtual bool response();
};

class TestRequest : public Request{
public:
  TestRequest(FCGX_Request* r) : Request(r) {}

  virtual bool response();

};

class HotShareI : public FeedGate, public MyUtil::Singleton<HotShareI> {
public:
  HotShareI();
};

}
}

#endif 

