#ifndef _AD_AD_GATE_H_
#define _AD_AD_GATE_H_

#include <ctemplate/template.h>
#include <boost/lexical_cast.hpp>

#include "AdGateM.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "Notify/util/fcgi_request.h"
#include "ad/util/src/AdCommonUtil.h"

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace ctemplate;
using namespace xce::notify;

enum EngineTypes {
  MATCHN = 0, ENGINE = 1, TW = 2
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

class AdGateMI : public AdGateM, public MyUtil::Singleton<AdGateMI> {
public:
  AdGateMI() {
    rotate_slots_.size(EVICTOR_SIZE);
  }
  virtual string GetAds(int uid, Ice::Long zoneid, const string& ip, const Ice::Current& = Ice::Current());
  virtual bool Reload(const Ice::Current& = Ice::Current());
private:
  int GetRotateSlot(int uid) {
    int rs = rotate_slots_.find(uid, 0);
    if (rs == 0) {
      rs = rand() % 10000;
      MCE_INFO("uid " << uid << " is not exist, rs = " << rs);
    }
    MCE_INFO("uid " << uid << " is exist, rs = " << rs);
    ++rs;
    rs = (rs) % 10000;
    rotate_slots_.add(rs, uid);
    return rs;
  }
  Evictor<int, int> rotate_slots_;
  static const int EVICTOR_SIZE = 50 * 10000;
};

}
}

#endif // _AD_AD_GATE_H_

