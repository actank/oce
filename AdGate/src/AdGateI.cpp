#include "AdGateI.h"
#include "ad_creative_cache.h"
#include "ip_area_cache.h"

#include <cstring>

//#include <boost/lexical_cast.hpp>
//#include <openssl/md5.h>

#include "FeedMemcProxy/client/user_profile_client.h"
#include "AdUserCacheLoaderAdapter.h"
#include "AdMatchAdapter.h"
#include "AdTwAdapter.h"
#include "AdMatch.h"

using namespace std;
using namespace xce::ad;

class CreativeReloadTimer: public MyUtil::Timer {
public:
  CreativeReloadTimer() : Timer(30 * 1000) {}
  virtual void handle() {
    MCE_INFO("Creative reload start.");
    if (AdCreativeCache::instance().Reload()) {
      MCE_INFO("Creative reload complete.");
    } else {
      MCE_WARN("Creative reload fail.");
    }
  }
};


class GateRequestFactory : public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      if (strcmp(path, "/ebp/show") == 0) {
        return new AdShowRequest(r);
      }
      if (strcmp(path, "/ebp/get.html") == 0) {
        return new AdGetRequest(r);
      }
      if (strcmp(path, "/ebp/click.html") == 0) {
        return new AdClickRequest(r);
      }
      if (strcmp(path, "/ebp/report.html") == 0) {
        return new AdReportRequest(r);
      }
      if (strcmp(path, "/ebp/test.html") == 0) {
        return new AdTestRequest(r);
      }
    }
    return NULL;
  }
};

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AdGateI::instance(), service.createIdentity("M", ""));

  AdCreativeCache::instance().Reload();

  TaskManager::instance().execute(new IpAreaCacheReloadTask());
  TaskManager::instance().scheduleRepeated(new CreativeReloadTimer());

  if (FCGX_Init() != 0) {
    MCE_WARN("FCGX_Init err");
    return;
  }

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_DEBUG("Fcgi listens on : " << fcgi_socket);

  int socket = FCGX_OpenSocket(fcgi_socket.c_str(), 100);
  if (socket < 0) {
    MCE_WARN("FCGX_OpenSocket err");
    return;
  }

  for(int i = 0; i < 512; ++i) {
    FastcgiTask * task = new FastcgiTask();
    if (task->init(socket)) {
      TaskManager::instance().execute(task);
    } else {
      delete task;
      MCE_WARN("FCGX_InitRequest err " << i);
    }
  }
}

AdGateI::AdGateI() {
}

bool AdGateI::Reload(const Ice::Current&) {
  return AdCreativeCache::instance().Reload();
}

bool AdGateI::ReloadCreativeById(Ice::Long id, const Ice::Current&) {
  return AdCreativeCache::instance().ReloadById(id);
}

FastcgiTask::FastcgiTask() : _factory(new GateRequestFactory()) {
}

void FastcgiTask::handle(){
  while(FCGX_Accept_r(&_request) >= 0) {
    RequestPtr req = _factory->Create(&_request);
    if (req) {
      req->response();
    } else {
      MCE_WARN("unhandled request.");
    }
    FCGX_Finish_r(&_request);
  }
}

bool AdShowRequest::response(){
  if (!_fcgi_out) {
    MCE_WARN("remove notify fcgi_out null");
    return false;
  }

  string uid = getProperty("uid");
  if (_cookies.find("id") != _cookies.end()) {
    uid = _cookies["id"];
  }
  if (!uid.empty()) {
    try {
      _login_uid = boost::lexical_cast<int>(uid);
    } catch (...) {
      MCE_WARN("lexical_cast cookie id error : " << uid);
    }
  }

  string serialized;
  if (_login_uid > 0) {
    bool b = UserProfileClient::instance().GetSerialized(_login_uid, &serialized);

    if (!b) {
      MCE_INFO("profile not found in memcache. id=" << _login_uid); 
      try {
        serialized = AdUserCacheLoaderAdapter::instance().GetById(_login_uid);
      } catch(Ice::Exception& e) {
        MCE_WARN("AdUserCacheLoaderAdapter GetById error : " << e << " uid=" << _login_uid);
      } catch(std::exception& e) {
        MCE_WARN("AdUserCacheLoaderAdapter GetById error : " << e.what() << " uid=" << _login_uid);
      }
    }
  }

  UserProfile profile;
  profile.ParseFromString(serialized);

  unsigned int ip = ntohl(inet_addr(GetRemoteAddr().c_str()));

  MCE_DEBUG("addr " << GetRemoteAddr() << " as unsigned int : " << ip);
  profile.set_ip(ip);
  profile.set_ip_area(IpAreaCache::instance().GetIpArea(ip));
  profile.set_id(_login_uid);

  //profile.set_ip_area(IpAreaCache::instance().GetIpArea(GetRemoteAddr()));

  profile.SerializeToString(&serialized);

  int count = getPropertyInt("ac", 3);

  AdInfoSeq ads;
  string referer = GetParamFix("HTTP_REFERER");
  for (size_t i = 0; i < referer.size(); ++ i) {
    if (referer[i] == ',' || referer[i] == '|') {
      referer[i] = '-';
    }
  }

  // if (true) { // _login_uid % 2 == 0) {
  CHECK 需要检查的地方 : 单双号调用
  if (_login_uid % 2 == 0) {
    try {
      ads = AdMatchAdapter::instance().GetAds(serialized, count, referer);
    } catch(Ice::Exception& e) {
      MCE_WARN("AdMatchAdapter::GetAds error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdMatchAdapter::GetAds error : " << e.what() << " uid=" << _login_uid);
    }
    MCE_INFO("renren ad shown. uid=" << _login_uid << " count=" << ads.size() << '/' << count
        << " stage=" << profile.stage() << " ip_area=" << profile.ip_area());
  } else {
    try {
      ads = AdTwAdapter::instance().GetAds(serialized, count, referer);
    } catch(Ice::Exception& e) {
      MCE_WARN("AdTwAdapter::GetAds error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdTwAdapter::GetAds error : " << e.what() << " uid=" << _login_uid);
    }
    MCE_INFO("taiwan ad shown. uid=" << _login_uid << " count=" << ads.size() << '/' << count
        << " stage=" << profile.stage() << " ip_area=" << profile.ip_area());
  }

  if (ads.size() < count) {
    AdCreativeCache::instance().PushDefault(_login_uid, GetRemoteAddr(), referer, &ads, count);
  }

  string ad_body;
  AdCreativeCache::instance().GetJson(ads, &ad_body);

  FCGX_PutS("Content-type: application/x-javascript; charset=utf-8\r\n\r\n", _fcgi_out);
  FCGX_PutS(ad_body.c_str(), _fcgi_out);

  CHECK 需要检查的地方 : check 是否需要调 test
  // if (_login_uid % 2 == 1) {
  //   try {
  //     ads = AdTwAdapter::instance().GetAds(serialized, count, referer);
  //   } catch(Ice::Exception& e) {
  //     MCE_WARN("AdTwAdapter::GetAds error : " << e << " uid=" << _login_uid);
  //   } catch(std::exception& e) {
  //     MCE_WARN("AdTwAdapter::GetAds error : " << e.what() << " uid=" << _login_uid);
  //   }
  // }

  return true;
}

bool AdTestRequest::response(){
  if (!_fcgi_out) {
    MCE_WARN("AdTestRequest::response fcgi_out null");
    return false;
  }

  string uid = getProperty("uid");
  if (_cookies.find("id") != _cookies.end()) {
    uid = _cookies["id"];
  }
  if (!uid.empty()) {
    try {
      _login_uid = boost::lexical_cast<int>(uid);
    } catch (...) {
      MCE_WARN("lexical_cast cookie id error : " << uid);
    }
  }

//  string props = getProperty("props");
  uid = getProperty("uid");
  string gender = getProperty("gender");
  string age = getProperty("age");
  string stage = getProperty("stage");
  string area = getProperty("area");
  string school = getProperty("school");
  string school_area = getProperty("school_area");
  string work_area = getProperty("work_area");
  string home_area = getProperty("home_area");
  string grade = getProperty("grade");
  string ip = getProperty("ip");

  //UserID|Gender|Birthday|Age|Stage|Area|School_SchoolArea|Grade|IPArea


  UserProfile profile;
  string serialized;
//  profile.ParseFromString(serialized);
  profile.set_ip_area(IpAreaCache::instance().GetIpArea(ip));
  profile.set_id(boost::lexical_cast<int>(uid));
  profile.set_stage(boost::lexical_cast<int>(stage));
  profile.set_home_area(home_area);
  profile.set_gender(boost::lexical_cast<int>(gender));
  profile.set_age(boost::lexical_cast<int>(age));

  if (stage == "4"){
    profile.set_current_area(work_area);
    profile.set_school(0);
    profile.set_grade(0);
  } else if (stage == "2" || stage == "1"){
    profile.set_current_area(school_area);
    profile.set_school(boost::lexical_cast<Ice::Long>(school));
    profile.set_grade(boost::lexical_cast<int>(grade));
  }


  profile.SerializeToString(&serialized);

  int count = getPropertyInt("ac", 3);

  AdInfoSeq ads;
  string referer = GetParamFix("HTTP_REFERER");
  try {
    ads = AdMatchAdapter::instance().GetAds(serialized, count, referer);
  } catch(Ice::Exception& e) {
    MCE_WARN("AdMatchAdapter::GetAds error : " << e << " uid=" << uid);
  } catch(std::exception& e) {
    MCE_WARN("AdMatchAdapter::GetAds error : " << e.what() << " uid=" << uid);
  }

  MCE_INFO("AdMatchAdapter::GetAds return size : " << ads.size() << " uid=" << uid
      << " stage=" << profile.stage() << " ip_area=" << profile.ip_area());

  string ad_json;
  string ad_body;

  //AdCreativeCache::instance().GetJson(ads, count, &ad_json);
  AdCreativeCache::instance().Get(ads, &ad_body);
  ad_body = ad_body + "<p/>" + ad_json;

  FCGX_PutS("Content-type: text/html; charset=utf-8\r\n\r\n", _fcgi_out);
  FCGX_PutS(ad_body.c_str(), _fcgi_out);

  return true;
}


bool AdGetRequest::response(){
  if (!_fcgi_out) {
    MCE_WARN("remove notify fcgi_out null");
    return false;
  }

  string uid = getProperty("uid");
  if (_cookies.find("id") != _cookies.end()) {
    uid = _cookies["id"];
  }
  if (!uid.empty()) {
    try {
      _login_uid = boost::lexical_cast<int>(uid);
    } catch (...) {
      MCE_WARN("lexical_cast cookie id error : " << uid);
    }
  }

  string serialized;
  if (_login_uid > 0) {
    bool b = UserProfileClient::instance().GetSerialized(_login_uid, &serialized);

    if (!b) {
      MCE_INFO("profile not found in memcache. id=" << _login_uid); 
      try {
        serialized = AdUserCacheLoaderAdapter::instance().GetById(_login_uid);
      } catch(Ice::Exception& e) {
        MCE_INFO("AdUserCacheLoaderAdapter GetById error : " << e << " uid=" << _login_uid);
      } catch(std::exception& e) {
        MCE_INFO("AdUserCacheLoaderAdapter GetById error : " << e.what() << " uid=" << _login_uid);
      }
    } else {
      MCE_DEBUG("profile found in memcache. id=" << _login_uid); 
    }
  }

  UserProfile profile;
  profile.ParseFromString(serialized);
  unsigned int ip = ntohl(inet_addr(GetRemoteAddr().c_str()));

  MCE_DEBUG("addr " << GetRemoteAddr() << " as unsigned int : " << ip);
  profile.set_ip(ip);
  profile.set_ip_area(IpAreaCache::instance().GetIpArea(ip));
  profile.set_id(_login_uid);

  profile.SerializeToString(&serialized);

  int count = getPropertyInt("ac", 3);

  AdInfoSeq ads;

  string referer = GetParamFix("HTTP_REFERER");
  for (size_t i = 0; i < referer.size(); ++ i) {
    if (referer[i] == ',' || referer[i] == '|') {
      referer[i] = '-';
    }
  }

  try {
    // ads = AdMatchAdapter::instance().GetAds(serialized, count, referer);
    ads = AdTwAdapter::instance().GetAds(serialized, count, referer);
  } catch(Ice::Exception& e) {
    MCE_WARN("AdMatchAdapter::GetAds error : " << e << " uid=" << _login_uid);
  } catch(std::exception& e) {
    MCE_WARN("AdMatchAdapter::GetAds error : " << e.what() << " uid=" << _login_uid);
  }
  MCE_INFO("AdMatchAdapter::GetAds return size : " << ads.size() << " uid=" << _login_uid 
      << " stage=" << profile.stage() << " ip_area=" << profile.ip_area());

  if (ads.size() < count) {
    AdCreativeCache::instance().PushDefault(_login_uid, GetRemoteAddr(), referer, &ads, count);
  }
  string ad_body;
  AdCreativeCache::instance().Get(ads, &ad_body);

  FCGX_PutS("Content-type: text/html; charset=utf-8\r\n\r\n", _fcgi_out);
  FCGX_PutS(ad_body.c_str(), _fcgi_out);

  return true;
}

bool AdReportRequest::response() {
  if (!_fcgi_out) {
    MCE_WARN("fcgi_out null");
    return false;
  }

  string uid = getProperty("uid");
  if (_cookies.find("id") != _cookies.end()) {
    uid = _cookies["id"];
  }
  if (!uid.empty()) {
    try {
      _login_uid = boost::lexical_cast<int>(uid);
    } catch (...) {
      MCE_WARN("lexical_cast cookie id error : " << uid);
    }
  }

  string matcher_cookie = getProperty("mc");
  string url = getProperty("url");
 
  string referer = GetParamFix("HTTP_REFERER");
  for (size_t i = 0; i < referer.size(); ++ i) {
    if (referer[i] == ',' || referer[i] == '|') {
      referer[i] = '-';
    }
  }

  if (matcher_cookie.empty() || matcher_cookie[0] != 't') {
    MCE_INFO("renren ad clicked. uid :" << _login_uid << " " << url);
    try {
      AdMatchAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
    } catch(Ice::Exception& e) {
      MCE_WARN("AdMatchAdapter::Click error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdMatchAdapter::Click error : " << e.what() << " uid=" << _login_uid);
    }
  } else {
    MCE_INFO("taiwan ad clicked. uid :" << _login_uid << " " << url);
    try {
      AdTwAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
    } catch(Ice::Exception& e) {
      MCE_WARN("AdTwAdapter::Click error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdTwAdapter::Click error : " << e.what() << " uid=" << _login_uid);
    }
  }

  FCGX_PutS("Content-type: text/html; charset=utf-8\r\n\r\n0", _fcgi_out);
  return true;
}

bool AdClickRequest::response() {
  if (!_fcgi_out) {
    MCE_WARN("fcgi_out null");
    return false;
  }

  string uid = getProperty("uid");
  if (_cookies.find("id") != _cookies.end()) {
    uid = _cookies["id"];
  }
  if (!uid.empty()) {
    try {
      _login_uid = boost::lexical_cast<int>(uid);
    } catch (...) {
      MCE_WARN("lexical_cast cookie id error : " << uid);
    }
  }

  string matcher_cookie = getProperty("mc");
  string url = getProperty("url");
 
  string referer = GetParamFix("HTTP_REFERER");
  for (size_t i = 0; i < referer.size(); ++ i) {
    if (referer[i] == ',' || referer[i] == '|') {
      referer[i] = '-';
    }
  }

  if (matcher_cookie.empty() || matcher_cookie[0] != 't') {
    MCE_INFO("renren ad clicked. uid :" << _login_uid << " " << url);
    try {
      AdMatchAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
    } catch(Ice::Exception& e) {
      MCE_WARN("AdMatchAdapter::Click error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdMatchAdapter::Click error : " << e.what() << " uid=" << _login_uid);
    }
  } else {
    MCE_INFO("taiwan ad clicked. uid :" << _login_uid << " " << url);
    try {
      AdTwAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
    } catch(Ice::Exception& e) {
      MCE_WARN("AdTwAdapter::Click error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdTwAdapter::Click error : " << e.what() << " uid=" << _login_uid);
    }
  }

  string output = "Location: " + url + "\r\n\r\n";
  FCGX_PutS(output.c_str(), _fcgi_out);
  return true;
}

