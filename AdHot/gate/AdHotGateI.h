#ifndef _XCE_AD_HOT_GATE_H_
#define _XCE_AD_HOT_GATE_H_

#include <time.h>
#include <vector>
#include <map>
#include "Ice/Ice.h"
#include "IceUtil/IceUtil.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "TaskManager.h"
#include "Notify/util/fcgi_request.h"
#include "AdHot.h"
#include "UserAdInfo.h"

using namespace MyUtil;
using namespace xce::notify;

namespace xce {
namespace ad {

class AdHotRequest: public Request {
public:
  AdHotRequest(FCGX_Request* r) :
    Request(r) {
  }

  virtual bool response();
};

class ExpandRequest: public Request {
public:
  ExpandRequest(FCGX_Request* r) :
    Request(r) {
  }

  virtual bool response();
};

class DeleteRequest: public Request {
public:
  DeleteRequest(FCGX_Request* r) :
    Request(r) {
  }

  virtual bool response();
};

class NotJoinRequest: public Request {
public:
  NotJoinRequest(FCGX_Request* r) :
    Request(r) {
  }

  virtual bool response();
};

class RedirectRequest: public Request {
public:
  RedirectRequest(FCGX_Request* r) :
    Request(r) {
  }

  virtual bool response();
};

class AdHotRequestFactory: public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/hot/hotnews.html") == 0 || strcmp(path, "/hot/get.html")
          == 0) {
        return new AdHotRequest(r);
      }
      if (strcmp(path, "/hot/expand.do") == 0 || strcmp(path, "/hot/expand") == 0) {
        return new ExpandRequest(r);
      }
      if (strcmp(path, "/hot/delete.do") == 0 || strcmp(path, "/hot/delete") == 0) {
        return new DeleteRequest(r);
      }
      if (strcmp(path, "/hot/notjoin.do") == 0 || strcmp(path, "/hot/notjoin") == 0) {
        return new NotJoinRequest(r);
        //return new DeleteRequest(r);
      }
      if (strcmp(path, "/hot/redirect.do") == 0 || strcmp(path, "/hot/redirect") == 0) {
        return new RedirectRequest(r);
      }
    }

    MCE_WARN("unknown request : " << path);
    return NULL;
  }
};

class AdHotGateI: public xce::ad::AdHotGate, public MyUtil::Singleton<AdHotGateI> {
public:
  AdHotGateI() {}
  virtual void Reload(const Ice::Current& = Ice::Current());
  virtual void ResetUser(Ice::Int uid, const Ice::Current& = Ice::Current());
};

}
}

#endif // _XCE_AD_HOT_GATE_H_
