#ifndef _XCE_NOTIFY_FCGI_REQUEST_H_
#define _XCE_NOTIFY_FCGI_REQUEST_H_

#include <IceUtil/Shared.h>
#include <fcgi_stdio.h>
#include <string>
#include <map>

#include "ServiceI.h"

namespace xce {
namespace notify {

class Request: public IceUtil::Shared {
public:
  Request(FCGX_Request* r) : _login_uid(0), _fcgi_out(r->out) {
    Init(r);
  }

  std::string host() const {
    return GetParamFix("HTTP_HOST");
  }

  std::string path() const {
    return _path;
  }

  std::string body() const {
    return _body;
  }

  Ice::Int GetCookieInt(const std::string& key, Ice::Int dft) const;

  std::string cookie(const std::string& key) const { //返回 cookie 中 key 对应的value
    std::map<std::string, std::string>::const_iterator it = _cookies.find(
        key);
    if (it != _cookies.end()) {
      return it->second;
    }
    return "";
  }

  std::string getProperty(const std::string& key) const; // 返回 Property 中 key 对应的value
  Ice::Int getPropertyInt(const std::string& key, Ice::Int dft = -1) const;
  Ice::Long getPropertyLong(const std::string& key, Ice::Long dft = -1) const;

  std::map<std::string, std::string> cookies() const {
    return _cookies;
  }

  std::map<std::string, std::string> getProperties() const {
    return _props;
  }

  std::string GetRemoteAddr() const;

  std::string UriDecode(const std::string & sSrc) const;
  void jsonEscape(std::string& str);
  bool ParseKeyValue(const char * data, const char * key_seps, char kv_sep,
      std::map<std::string, std::string> & pairs);

  virtual bool response();
protected:
  void Init(FCGX_Request* r);

  std::string _method;
  std::string _path;
  std::string _host;
  std::string _body;
  std::string _query;
  std::string _cookie_string;
  std::map<std::string, std::string> _cookies;
  std::map<std::string, std::string> _props;
  std::string _post_body;

  int _login_uid;
  FCGX_Stream * _fcgi_out;

  std::map<std::string, std::string> param_map_;
  void InitParamMap(FCGX_Request* r);
  // FCGX_GetParam 有bug, 此处fix了
  std::string GetParamFix(const std::string & name) const;
};

typedef IceUtil::Handle<Request> RequestPtr;

class RequestFactory: public IceUtil::Shared {
public:
  virtual RequestPtr Create(FCGX_Request* r) = 0;
  virtual ~RequestFactory() {}
};

typedef IceUtil::Handle<RequestFactory> RequestFactoryPtr;

class FcgiTask: public MyUtil::Task {
public:
  FcgiTask(const RequestFactoryPtr & factory);

  bool init(int listen_socket) {
    return FCGX_InitRequest(&_request, listen_socket, 0) == 0;
  }
  virtual void handle();
private:
  FCGX_Request _request;
  RequestFactoryPtr _factory;
};

class FcgiThread : public IceUtil::Thread {
public:
  FcgiThread(RequestFactoryPtr factory, int sock_fd); 
  virtual void run();
private:
  RequestFactoryPtr factory_;
  int sock_fd_;
};

class FcgiServer : public RequestFactory {
public:
  FcgiServer(const std::string & sock, int thread_count, int stack_size = 1024 * 1024);

  int RegisterRequestFactory(RequestFactoryPtr factory);
  virtual RequestPtr Create(FCGX_Request* r);

  bool Start();
  bool Stop();
private:
  std::vector<RequestFactoryPtr> request_factories_;
  std::string socket_;

  int thread_count_;
  int stack_size_;
  std::vector<IceUtil::ThreadPtr> threads_;
private:
  FcgiServer(const FcgiServer &); 
  FcgiServer& operator= (const FcgiServer &); 
};

}
}

#endif // _XCE_NOTIFY_FCGI_REQUEST_H_
