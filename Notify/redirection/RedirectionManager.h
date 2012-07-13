/*
 * RedirectionManager.h
 *
 *  Created on: 2010-5-21
 *      Author: ld
 */

#ifndef __NOTIFY_REDIRECTIONMANAGER_H__
#define __NOTIFY_REDIRECTIONMANAGER_H__

#include <vector>
#include <map>
#include "Ice/Ice.h"
#include "Singleton.h"
#include "ServiceI.h"
#include "IceLogger.h"
#include "Notify/util/fcgi_request.h"
#include "Redirection.h"

using namespace MyUtil;

namespace xce {
namespace notify {

class RedirectionRequest: public Request {
public:
  RedirectionRequest(FCGX_Request* r) :
    Request(r) {
  }

  void ReplaceAll(string& src, string replaced_part, string replacing_part);
  map<string, string> ParseQueryString(string query_string);

  virtual bool response();
};


class SafeRedirectionRequest: public RedirectionRequest {
public:
  SafeRedirectionRequest(FCGX_Request* r) :
    RedirectionRequest(r) {
  }

  virtual bool response();
};

class PassportRequest: public Request {
public:
  PassportRequest(FCGX_Request* r) :
    Request(r) {
  }

  virtual bool response();
};


class RedirectionRequestFactory: public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {

    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/redirect.do") == 0) {
        return new RedirectionRequest(r);
      } else if (strcmp(path, "/redirects") == 0) {
        return new SafeRedirectionRequest(r);
      } else if (strcmp(path, "/passport/verify") == 0) {
        return new PassportRequest(r);
      }
    }
    return NULL;
  }
};

class RedirectionI: public xce::notify::Redirection, public MyUtil::Singleton<
    RedirectionI> {
public:
  RedirectionI();
  void Run();
  string fcgi_socket() {
    return _fcgi_socket;
  }
private:
  RequestFactoryPtr _factory;

  std::string _fcgi_socket;
};

}

}

#endif /* __NOTIFY_REDIRECTIONMANAGER_H__ */
