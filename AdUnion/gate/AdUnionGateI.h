#ifndef _AD_UNION_GATE_H_
#define _AD_UNION_GATE_H_

#include <boost/lexical_cast.hpp>

#include "AdUnion.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::notify;

class GetRequest : public Request {
public:
  GetRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class RedirectRequest : public Request {
public:
  RedirectRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
private:
  bool IsCheat();
};

class NuomiSetRequest : public Request {
public:
  NuomiSetRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
private:
  bool CheckUrl();
};

class AdUnionGateI : public AdUnionGate, public MyUtil::Singleton<AdUnionGateI> {
public:
  AdUnionGateI();
  ~AdUnionGateI();
  virtual bool Reload(const Ice::Current& =  Ice::Current());
  virtual bool ReloadById(Ice::Int id, const Ice::Current& =  Ice::Current());
};

}
}

#endif // _AD_UNION_GATE_H_

