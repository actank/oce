#ifndef _AD_AD_GATE_H_
#define _AD_AD_GATE_H_

#include <ctemplate/template.h>
#include <boost/lexical_cast.hpp>

#include "AdGate.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace ctemplate;
using namespace xce::notify;

class AdGetRequest : public Request {
public:
  AdGetRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class AdClickRequest : public Request {
public:
  AdClickRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class AdReportRequest : public Request {
public:
  AdReportRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class AdShowRequest : public Request {
public:
  AdShowRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class AdTestRequest : public Request {
public:
  AdTestRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class AdGateI : public AdGate, public MyUtil::Singleton<AdGateI> {
public:
  AdGateI();
  virtual bool Reload(const Ice::Current& =  Ice::Current());
  virtual bool ReloadCreativeById(Ice::Long id, const Ice::Current& = Ice::Current());

private:
  //std::string _fcgi_socket;
  //void BodySeq2StrSeq(const NotifyBodySeq & n, Ice::Int view, StrSeq & s);
};

class FastcgiTask: public Task {
public:
  FastcgiTask();

  bool init(int listen_socket) {
    return FCGX_InitRequest(&_request, listen_socket, 0) == 0;
  }
  virtual void handle();
private:
  FCGX_Request _request;
  RequestFactoryPtr _factory;
};

}
}

#endif // _AD_AD_GATE_H_

