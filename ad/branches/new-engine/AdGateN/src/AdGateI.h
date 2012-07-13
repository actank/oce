#ifndef _AD_GATE_H_
#define _AD_GATE_H_

#include <ctemplate/template.h>
#include <boost/lexical_cast.hpp>

#include "AdGate.h"
#include "AdEngine.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"
#include "ad/ping_client/src/PingSender.h"

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace ctemplate;
using namespace xce::notify;

enum EngineTypes {
  MATCHN = 0, ENGINE = 1, TW = 2, JAVA = 3
};

extern std::string db_instance;

class AdGetRequest : public Request {
public:
  enum RenderType {RT_JSON, RT_HTML};
  AdGetRequest(FCGX_Request* r, RenderType render) : Request(r), render_(render) {}
  virtual bool response();
private:
  RenderType render_;
};

class AdNewGetRequest : public Request {
public:
  enum RenderType {RT_JSON, RT_HTML};
  AdNewGetRequest(FCGX_Request* r, RenderType render) : Request(r), render_(render) {}
  virtual bool response();
private:
  RenderType render_;
};

class AdNewClickRequest : public Request {
public:
  AdNewClickRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};
class AdClickRequest : public Request {
public:
  AdClickRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class AdTestRequest : public Request {
public:
  AdTestRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class AdMonitorRequest : public Request {
public:
  AdMonitorRequest(FCGX_Request* r) : Request(r) {}
  virtual bool response();
};

class AdGateI : public AdGate, public MyUtil::Singleton<AdGateI> {
public:
  AdGateI();
  void translate( xce::ad::AdResultMap & map, xce::ad::AdZone2Creatives & adzone2c );
  virtual bool Reload(const Ice::Current& =  Ice::Current());
  virtual bool ReloadCreativeById(Ice::Long id, const Ice::Current& = Ice::Current());

  void CreatePingSender(const string& server_name, int threshold = 50) {
    ping_sender_.SetServerInfo(server_name, threshold);
  }
  PingSender ping_sender_;
private:
};

}
}

#endif // _AD_GATE_H_

