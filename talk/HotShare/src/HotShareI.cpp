 #include "HotShareI.h"

#include "HotShareInfo.h"
#include "HotShareController.h"

//#include "client/linux/handler/exception_handler.h"

using namespace std;
using namespace xce::notify;
using namespace xce::hotshare;
using namespace MyUtil;

/*
bool MyCallback(const char *dump_path, const char *minidump_id, void *context, bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}
*/
Ice::Long count_ = 0;

class HotShareFactory: public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/hotshare/hs/show") == 0
          || strcmp(path, "/hotshare/show") == 0) {
        return new HotShareRequest(r);
      }
      else if (strcmp(path, "/test/feedview") == 0) {
        return new TestRequest(r);
      }
    }
    return NULL;
  }
};

void MyUtil::initialize() {
//  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&HotShareI::instance(), service.createIdentity("M", ""));

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service."
      + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_DEBUG("Fcgi listens on : " << fcgi_socket);

  TaskManager::instance().scheduleRepeated(&GetDataTimer::instance());

  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 64);
  fcgi_server->RegisterRequestFactory(new HotShareFactory());
  fcgi_server->Start();

}


HotShareI::HotShareI() {
}


bool HotShareRequest::response() {
  if (!_fcgi_out) {
    MCE_WARN("get notify fcgi_out null");
    return false;
  }

  ostringstream ss;
  string emptyRet = "Content-type: text/html;charset=UTF-8\r\n\r\n";
  ss << emptyRet.c_str();
  ss << HotShareController::instance().GetData(_post_body).c_str();
  FCGX_PutS(ss.str().c_str(), _fcgi_out);
  return true;
}

bool TestRequest::response() {
  if (!_fcgi_out) {
    MCE_WARN("get notify fcgi_out null");
    return false;
  }
  string temp_post_body = "maxid=0&size=50&type=1&bottom=447654572&top=49464c51393c6965e79912e6eacaf2b9&mac=ba87d1dcec4560420b95f9fd8d31d7c1";
  ostringstream ss;
  string emptyRet = "Content-type: text/html;charset=UTF-8\r\n\r\n";
  ss << emptyRet.c_str();
  //ss << HotShareController::instance().GetData(temp_post_body).c_str();
  
  ++count_;
  MCE_DEBUG("test.feedview count: "<<count_);
  FCGX_PutS(ss.str().c_str(), _fcgi_out);
  return true;
}

