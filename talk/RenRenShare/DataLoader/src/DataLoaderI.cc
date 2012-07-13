#include "DataLoaderI.h"
#include "HotShareInfo.h"
#include "DataLoaderController.h"
//#include "client/linux/handler/exception_handler.h"

using namespace std;
using namespace xce::notify;
using namespace xce::dataloader;
using namespace MyUtil;

/*
bool MyCallback(const char *dump_path, const char *minidump_id, void *context, bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}
*/

class DataLoaderFactory: public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/dataloader") == 0) {
        return NULL;
 //       return new DataLoaderRequest(r);
      }
    }
    return NULL;
  }
};

void MyUtil::initialize() {
//  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&DataLoaderI::instance(), service.createIdentity("M", ""));

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service."
      + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_DEBUG("Fcgi listens on : " << fcgi_socket);

  TaskManager::instance().scheduleRepeated(&DataLoaderTimer::instance());

  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 64);
  fcgi_server->RegisterRequestFactory(new DataLoaderFactory());
  fcgi_server->Start();
}


DataLoaderI::DataLoaderI() {
}


bool DataLoaderRequest::response() {
  if (!_fcgi_out) {
    MCE_WARN("get notify fcgi_out null");
    return false;
  }

  ostringstream ss;
  string emptyRet = "Content-type: text/html;charset=UTF-8\r\n\r\n";
  ss << emptyRet.c_str();
//  DataLoaderController::instance().DeleteHotShare();
  FCGX_PutS(ss.str().c_str(), _fcgi_out);
  return true;
}
