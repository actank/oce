#include "PageCacheManagerI.h"

#include "Cache.h"
#include "Notify/util/fcgi_request.h"
#include "PageCacheAdapter.h"
using namespace talk::http;

// const int kUserPageCacheIndex = 0;

class PageCacheRequestFactory: public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/wpi/getonlinecount.do") == 0
          || strcmp(path, "/wpi/getgrouphistory.do") == 0
          || strcmp(path, "/wpi/getuserinfo.do") == 0
          || strcmp(path, "/wpi/getroominfo.do") == 0
          || strcmp(path, "/wpi/getonlinefriends.do") == 0
          || strcmp(path, "/wpi/getfullroomuser.do") == 0
          || strcmp(path, "/wpi/getfullfriends.do") == 0
          || strcmp(path, "/wpi/getallfullfriends.do") == 0) {
        return new PageCacheRequest(r);
      }
    }
    return NULL;
  }
};

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  // service.getObjectCache()->registerCategory(kUserPageCacheIndex, "USER_PAGE", new UserPageFactoryI, props, true);

  service.getAdapter()->add(&PageCacheManagerI::instance(), service.createIdentity("M", ""));
	InitMonitorLogger(ServiceI::instance().getName(), "monitor", "../log/" + ServiceI::instance().getName() + "/monitor/monitor_log", "INFO");
  string fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_INFO("Fcgi listens on : " << fcgi_socket);
  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 128);
  fcgi_server->RegisterRequestFactory(new PageCacheRequestFactory());
  fcgi_server->Start();
}


static std::string GmtTime(time_t t) {
  static const string week[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
  static const string month[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", 
                                    "Aug", "Sep", "Oct", "Nov", "Dec" };
  struct tm tms;
  tms = *gmtime(&t);
  ostringstream res;
  // Expires: Mon, 26 Jul 1997 05:00:00 GMT
  res << month[tms.tm_wday] << ", ";
  if(tms.tm_mday < 10){
    res << 0;
  }
  res << tms.tm_mday << " "
      << month[tms.tm_mon] << " " << tms.tm_year + 1900 << " ";
  if (tms.tm_hour < 10) {
    res << 0;
  }
  res << tms.tm_hour << ":";

  if (tms.tm_min < 10) {
    res << 0;
  }
  res << tms.tm_min << ":";

  if (tms.tm_sec < 10) {
    res << 0;
  }
  res << tms.tm_sec << " GMT";
  return res.str();
}

static const char * GetReasonPhrase(int code) {
  switch(code) {
  case 200:
    return "OK";
  case 404:
    return "Not Found";
  case 403:
    return "Forbidden";
  case 408:
    return "Request Timeout";
  default:
    return "Unknown";
  }
}

bool PageCacheRequest::response() {
  if (!_fcgi_out) {
    MCE_WARN("_fcgi_out null");
    return false;
  }
  
  if(!Request::response()) {
    FCGX_FPrintF(_fcgi_out, "Content-type: text/html\r\n\r\nverify failure");
    return false;
  }
  if (strncmp(_path.c_str(), "/wpi/", 5) == 0) {
    _path = _path.substr(4);
  }

  // UserPagePtr user_pages = ServiceI::instance().locateObject<UserPagePtr>(kUserPageCacheIndex, _login_uid);
  // ContentPtr body = user_pages->body(_login_uid, _path, _cookies, _props);
  ContentPtr body;
	TimeStat ts;
  try {
    body = PageCacheAdapter::instance().GetContent(_login_uid, _path, _cookies, _props);
  } catch(Ice::Exception& e) {
    MCE_WARN("PageCacheRequest::response-->user " << _login_uid << " get PageCacheAdapter err:" << e);
  }
	MONITOR("SignForTimeuse::PageCacheRequest::response-->PageCache::GetContent-->" << ts.getTime());
	FunStatManager::instance().Stat("PageCache.GetContent", ts.getTime(), (ts.getTime() >= 250.0));	
  stringstream res;

  if (body) {
    MCE_DEBUG("status : " << body->statusCode);
    MCE_DEBUG("content type : " << body->contentType);
    MCE_DEBUG("body : " << body->data);
    MCE_DEBUG("timeout : " << body->timeout);

    res << "HTTP/1.1 "<< body->statusCode << " " << GetReasonPhrase(body->statusCode) << "\r\n"
        << "Content-type: " << body->contentType << "\r\n";

    if (body->timeout > 0 && body->statusCode == 200) {
      res << "Cache-Control: max-age="<< body->timeout << "\r\n"
          << "Expires: "<< GmtTime(time(NULL) + body->timeout) << "\r\n";
    } else {
      res << "Cache-Control: no-cache"<< "\r\n"
          << "Expires: Thu, 01 Jan 1970 00:00:00 GMT"<< "\r\n";
    }
    res << "\r\n" << body->data;
  } else {
    res << "HTTP/1.1 500 UpstreamTimeout\r\n" << "Content-type: text/html\r\n\r\n";
  }

  FCGX_PutS(res.str().c_str(), _fcgi_out);
  return true;
}

