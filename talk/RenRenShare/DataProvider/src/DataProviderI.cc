#include "DataProviderI.h"
#include "DataProviderInfo.h"
#include "DataProviderController.h"

//#include "client/linux/handler/exception_handler.h"

using namespace std;
using namespace xce::notify;
using namespace xce::dataprovider;
using namespace MyUtil;

/*
bool MyCallback(const char *dump_path, const char *minidump_id, void *context, bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}
*/

class DataProviderFactory: public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/hotshare/hot") == 0) {
        return new DataProviderRequest(r);
      }
      else if (strcmp(path, "/hotshare/photolist") == 0) {
        return new PhotoListRequest(r);
      }
    }
    return NULL;
  }
};

void MyUtil::initialize() {
//  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&DataProviderI::instance(), service.createIdentity("M", ""));

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service."
      + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_DEBUG("Fcgi listens on : " << fcgi_socket);

  TaskManager::instance().scheduleRepeated(&DataProviderTimer::instance());

  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 64);
  fcgi_server->RegisterRequestFactory(new DataProviderFactory());
  fcgi_server->Start();
}


DataProviderI::DataProviderI() {
}

bool PhotoListRequest::response() {
  if (!_fcgi_out) {
    MCE_WARN("get notify fcgi_out null");
    return false;
  }

  ostringstream ss;
  string emptyRet = "Content-type: text/html;charset=UTF-8\r\n\r\n";
  ss << emptyRet.c_str();
  if (_post_body != "")  
    ss << DataProviderController::instance().GetPhotoListData(_post_body);
  FCGX_PutS(ss.str().c_str(), _fcgi_out);
  return true;
}

bool DataProviderRequest::response() {
  if (!_fcgi_out) {
    MCE_WARN("get notify fcgi_out null");
    return false;
  }
  MCE_DEBUG("DataProviderRequest::response post_body:"<<_post_body);
  ostringstream ss;
  string emptyRet = "Content-type: text/html;charset=UTF-8\r\n\r\n";
  ss << emptyRet.c_str();
  if (_post_body != "")
    ss << DataProviderController::instance().GetHotShareData(_post_body);
  FCGX_PutS(ss.str().c_str(), _fcgi_out);
  return true;
}
