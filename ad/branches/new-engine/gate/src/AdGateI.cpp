#include "AdGateI.h"
#include "ad_creative_cache.h"
#include "ip_area_cache.h"

#include <cstring>
#include "client/linux/handler/exception_handler.h"
//#include <boost/lexical_cast.hpp>
//#include <openssl/md5.h>

#include "FeedMemcProxy/client/user_profile_client.h"
#include "AdUserCacheLoaderAdapter.h"
#include "AdEngineSAdapter.h"
#include "AdEngineUAdapter.h"
#include "AdTwSAdapter.h"
#include "AdGateData.h"
#include "AdGateConfig.h"

using namespace std;
using namespace xce::ad;


bool MyCallback(const char *dump_path,
   const char *minidump_id,
   void *context,
   bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}


class CreativeReloadTimer: public MyUtil::Timer {
public:
  CreativeReloadTimer(time_t t) : Timer(t) {}
  virtual void handle() {
    MCE_INFO("Creative reload start.");
    if (AdCreativeCache::instance().Reload()) {
      MCE_INFO("Creative reload complete.");
    } else {
      MCE_WARN("Creative reload fail.");
    }
  }
};


class AdEngineJReloadTimer : public MyUtil::Timer {
public:
  AdEngineJReloadTimer (time_t t) :
    Timer(t) {
  }
  virtual void handle() {
    MCE_INFO("AdEngineJAdapter reload start.");
    if (AdEngineJAdapter::instance().Reload()) {
      MCE_INFO("AdEngineJAdapter reload complete.");
    } else {
      MCE_WARN("AdEngineJAdapter reload fail.");
    }
  }
};

class GateRequestFactory : public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if (path) {
      //if (strcmp(path, "/ebp/show") == 0 || strcmp(path, "/ebpt/show") == 0) {
      //  return new AdGetRequest(r, AdGetRequest::RT_JSON);
      //}
      if (strcmp(path, "/ebp/show") == 0 || strcmp(path, "/ebpt/show") == 0) {
        return new AdNewGetRequest(r, AdNewGetRequest::RT_JSON);
      }
      if (strcmp(path, "/ebp/get.html") == 0) {
        return new AdGetRequest(r, AdGetRequest::RT_HTML);
      }
      if (strcmp(path, "/ebp/click.html") == 0) {
        return new AdClickRequest(r);
      }
      if (strcmp(path, "/ebp/test.html") == 0) {
        return new AdTestRequest(r);
      }
    }
    return NULL;
  }
};

void MyUtil::initialize() {
  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback
                                         , 0, true);
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AdGateI::instance(), service.createIdentity("M", ""));
  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  xce::ad::GateConfig::instance().set_DB(
      props->getPropertyWithDefault("DbInstance", "ade_db"));

  AdCreativeCache::instance().Reload();

  TaskManager::instance().execute(new IpAreaCacheReloadTask());
  TaskManager::instance().schedule(new CreativeReloadTimer(0));
  TaskManager::instance().scheduleRepeated(new CreativeReloadTimer(30*1000));
  TaskManager::instance().scheduleRepeated(new AdEngineJReloadTimer(30*1000));
  //TaskManager::instance().schedule(new RefreshTimeTimer(0));
  string fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_INFO("Fcgi listens on : " << fcgi_socket);

  srand(time(NULL));

#if 0
  if (FCGX_Init() != 0) {
    MCE_WARN("FCGX_Init err");
    return;
  }
  
  int socket = FCGX_OpenSocket(fcgi_socket.c_str(), 100);
  if (socket < 0){
    MCE_WARN("FCGX_OpenSocket err");
    return;
  }
  
  for(int i = 0; i < 512; ++i)
  {
    FcgiTask * task = new FcgiTask(new GateRequestFactory());
    if (task->init(socket)) {
        TaskManager::instance().execute(task);
    } else {
      delete task;
      MCE_WARN("FCGX_InitRequest err " << i);
    }
  }
#else
  // fcgi_server memory managed as RequestFactory ...
  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 512);
  fcgi_server->RegisterRequestFactory(new GateRequestFactory());
  fcgi_server->Start();
#endif
}

AdGateI::AdGateI() {
}

bool AdGateI::Reload(const Ice::Current&) {
  return AdCreativeCache::instance().Reload();
}

bool AdGateI::ReloadCreativeById(Ice::Long id, const Ice::Current&) {
  return AdCreativeCache::instance().ReloadById(id);
}

bool AdNewGetRequest::response(){
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
  profile.set_ip_area(IpAreaCache::instance().GetIpArea(GetRemoteAddr()));
  profile.set_id(_login_uid);

  //profile.set_ip_area(IpAreaCache::instance().GetIpArea(GetRemoteAddr()));

  profile.SerializeToString(&serialized);

  int rotate_slot = GetCookieInt("an_slot", rand());
  rotate_slot %= 1000;
    
  AdResultMap ads;
  string referer = getProperty("ref");
  if (referer.empty() || referer.find("http://") == string::npos) {
    referer = getProperty("r");
    if (referer.empty() || referer.find("http://") == string::npos) {
      referer = GetParamFix("HTTP_REFERER");
    }
  }

  int refresh_idx = getPropertyInt("refresh_idx", 0);
  int next_load_time = getPropertyInt("next_load_time", 0);
  ostringstream append_para; // 附加参数
  append_para << referer << "^C" << next_load_time << "^C" << refresh_idx;
  MCE_DEBUG("append_para : " << append_para.str());

  if ((_login_uid%10) > 4) {
    try {
      //ads = AdEngineUAdapter::instance().GetAds(serialized, append_para.str(), rotate_slot);
        UserAndAction ua;
        ua.id = profile.id();
        ua.stage = profile.stage();
        ua.gender = profile.gender();
        ua.age = profile.age();
        ua.school = profile.school();
        ua.userMajor = profile.major();
        ua.grade = profile.grade();
        ua.homeArea = profile.home_area();
        ua.currentArea = profile.current_area();
        ua.ip = profile.ip();
        ua.ipArea = profile.ip_area();
        ua.actionType = ActionType::PAGEVIEW;
        AdRequest ad_request;
        ad_request.userAndAction = ua;
        ad_request.referer = referer;
        ad_request.zoneDict[100000000001] = 0;
        ad_request.attributes["100000000001"]="";
        MCE_DEBUG("AdNewGetRequest::response --> enter to request JavaServer");
        ads =AdEngineJAdapter::instance().Request(ad_request);
        //AdGateI::instance().translate(e_ads_,e_ads);
      /*if((_login_uid%10) == 5 || (_login_uid%10) == 6) {  
        ads = AdEngineAdapter::instance().GetAds(serialized, append_para.str(), rotate_slot);
      } else {
        ads = AdEngineUAdapter::instance().GetAds(serialized, append_para.str(), rotate_slot);
      }*/
    } catch(Ice::Exception& e) {
      MCE_WARN("AdEngineJAdapter::GetAds error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdEngineJdapter::GetAds error : " << e.what() << " uid=" << _login_uid);
    }
    MCE_INFO("engineU ad shown. uid=" << _login_uid << " count=" << ads.size()
             << " stage=" << profile.stage() << " ip_area=" << profile.ip_area());
  /*} else if ((_login_uid%10) > 4 || (_login_uid == 1244171572) || (_login_uid == 128487631)) {
    try {
      ads = AdEngineAdapter::instance().GetAds(serialized, append_para.str(), rotate_slot);
    } catch(Ice::Exception& e) {
      MCE_WARN("AdEngineAdapter::GetAds error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdEngineAdapter::GetAds error : " << e.what() << " uid=" << _login_uid);
    }
    MCE_INFO("renren ad shown. uid=" << _login_uid << " count=" << ads.size() << '/'
             << " stage=" << profile.stage() << " ip_area=" << profile.ip_area());*/
  } else {
    try {
      ads = AdTwAdapter::instance().GetAds(serialized, append_para.str(), rotate_slot);
    } catch(Ice::Exception& e) {
      MCE_WARN("AdTwAdapter::GetAds error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdTwAdapter::GetAds error : " << e.what() << " uid=" << _login_uid);
    }
    MCE_INFO("taiwan ad shown. uid=" << _login_uid << " count=" << ads.size() << '/' 
        << " stage=" << profile.stage() << " ip_area=" << profile.ip_area());
  }

  for(AdResultMap::const_iterator it = ads.begin(); it != ads.end(); ++it) {
    MCE_INFO("AdEngineUAdapter::GetAds return : " << it->first << '/' << it->second.size());
  }

  if (ads.empty()) {
    AdCreativeCache::instance().PushDefault(_login_uid, GetRemoteAddr(), referer, &ads);
  }

  for(AdResultMap::const_iterator it = ads.begin(); it != ads.end(); ++it) {
     MCE_INFO("AdEngineUAdapter::GetAds add default ad : " << it->first << '/' << it->second.size());
   }

  stringstream ad_header;
  string ad_body;

  if (render_ == RT_HTML) {
    AdCreativeCache::instance().Get(ads, &ad_body, refresh_idx);
    ad_header << "Content-type: text/html; charset=utf-8\r\nSet-Cookie: an_slot=" << ++ rotate_slot << "; path=/ebp\r\n\r\n";
  } else if (render_ == RT_JSON) {
    AdCreativeCache::instance().GetJson(referer, ads, &ad_body, refresh_idx);
    ad_header << "Content-type: application/x-javascript; charset=utf-8\r\nSet-Cookie: an_slot=" << ++ rotate_slot << "; path=/ebp\r\n\r\n";
  }

  FCGX_PutS(ad_header.str().c_str(), _fcgi_out);

  FCGX_PutS(ad_body.c_str(), _fcgi_out);
  MCE_DEBUG("AdGetRequest::response --> return uid=" << _login_uid);
  return true;
}

bool AdTestRequest::response(){
  MCE_DEBUG("AdTestRequest::response");
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

  /*
  string gender = getProperty("gender");
  string age = getProperty("age");
  string stage = getProperty("stage");
  string area = getProperty("area");
  string school = getProperty("school");
  string school_area = getProperty("school_area");
  string work_area = getProperty("work_area");
  string home_area = getProperty("home_area");
  string grade = getProperty("grade");*/
  string ip = getProperty("ip");
  string referer = getProperty("ref");
  /*MCE_DEBUG("[USER_DEBUG] : AdTestRequest --> referer= " << referer
            << ", uid:" << uid << ", ip:" << ip
            << ", gender = " << gender << ", age = " << age
            << ", stage = " << stage << ", area = " << area
            << ", school = " << school << ", grade = " << grade
            << ", school_area = " << school_area
            << ", work_area = " << work_area << ", home_area = " << home_area);
  */
  MCE_DEBUG("[USER_DEBUG] : AdTestRequest --> referer= " << referer
            << ", profile.id() = " << profile.id() << ", ip = " << ip);
  //UserID|Gender|Birthday|Age|Stage|Area|School_SchoolArea|Grade|IPArea

  //UserProfile profile;
  unsigned int intip =  ntohl(inet_addr(ip.c_str()));
  profile.set_ip(intip);
  profile.set_ip_area(IpAreaCache::instance().GetIpArea(ip));
  MCE_DEBUG("[USER_DEBUG] : profile.ip_area() = " << profile.ip_area() << ", intip = " << intip << " , profile.ip() = " << (unsigned int)profile.ip());

  /*try {
    profile.set_id(boost::lexical_cast<int>(uid));
  } catch (...) {}

  try {
    profile.set_stage(boost::lexical_cast<int>(stage));
  } catch (...) {}

  try {
    profile.set_home_area(home_area);
  } catch (...) {}

  try {
    profile.set_gender(boost::lexical_cast<int>(gender));
  } catch (...) {}

  try {
    profile.set_age(boost::lexical_cast<int>(age));
  } catch (...) {}

  if (stage == "4"){
    profile.set_current_area(work_area);
    profile.set_school(0);
    profile.set_grade(0);
  } else if (stage == "2" || stage == "1"){
    profile.set_current_area(school_area);
    try {
      profile.set_school(boost::lexical_cast<Ice::Long>(school));
      profile.set_grade(boost::lexical_cast<int>(grade));
    } catch (...) {}
  }*/

  profile.SerializeToString(&serialized);

  int rotate_slot = GetCookieInt("an_slot", rand());
  rotate_slot %= 1000;

  AdResultMap ads;
  try {
    ads = AdEngineUAdapter::instance().GetAds(serialized, referer, rotate_slot);
    /*if((_login_uid%10) == 5 || (_login_uid%10) == 6) {  
      ads = AdEngineAdapter::instance().GetAds(serialized, referer, rotate_slot);
    } else {
      ads = AdEngineUAdapter::instance().GetAds(serialized, referer, rotate_slot);
    }*/
  } catch(Ice::Exception& e) {
    MCE_WARN("AdEngineUAdapter::GetAds error : " << e << " uid=" << uid);
  } catch(std::exception& e) {
    MCE_WARN("AdEngineUAdapter::GetAds error : " << e.what() << " uid=" << uid);
  }
  for(AdResultMap::iterator it = ads.begin(); it != ads.end(); ++it){
    for(size_t i = 0; i < it->second.size(); ++i){
      MCE_DEBUG("[sort_test] AdTestRequest::response --> creative_id:" << it->second.at(i).creativeid);
      if(it->second.at(i).creativeid == 0){
        MCE_WARN("test, creativeid is 0");
      }
    }
  }

  MCE_INFO("AdEngineUAdapter::GetAds return size : " << ads.size() << " uid=" << uid
      << " stage=" << profile.stage() << " ip_area=" << profile.ip_area());

  string ad_json;
  string ad_body;

  AdCreativeCache::instance().Get(ads, &ad_body);
  ad_body = ad_body + "<p/>" + ad_json;
  MCE_DEBUG("AdTestRequest::response --> uid:" << uid << " ,body:" << ad_body );
  ostringstream ad_header;
  ad_header << "Content-type: text/html; charset=utf-8\r\nSet-Cookie: an_slot=" << ++ rotate_slot << "; path=/ebp\r\n\r\n";
  FCGX_PutS(ad_header.str().c_str(), _fcgi_out);
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
  profile.set_ip_area(IpAreaCache::instance().GetIpArea(GetRemoteAddr()));
  profile.set_id(_login_uid);

  //profile.set_ip_area(IpAreaCache::instance().GetIpArea(GetRemoteAddr()));

  profile.SerializeToString(&serialized);

  int rotate_slot = GetCookieInt("an_slot", rand());
  rotate_slot %= 1000;
    
  AdResultMap ads;
  string referer = getProperty("ref");
  if (referer.empty() || referer.find("http://") == string::npos) {
    referer = getProperty("r");
    if (referer.empty() || referer.find("http://") == string::npos) {
      referer = GetParamFix("HTTP_REFERER");
    }
  }

  int refresh_idx = getPropertyInt("refresh_idx", 0);
  int next_load_time = getPropertyInt("next_load_time", 0);
  ostringstream append_para; // 附加参数
  append_para << referer << "^C" << next_load_time << "^C" << refresh_idx;
  MCE_DEBUG("append_para : " << append_para.str());

  if ((_login_uid%10) > 4) {
    try {
      ads = AdEngineUAdapter::instance().GetAds(serialized, append_para.str(), rotate_slot);
      /*if((_login_uid%10) == 5 || (_login_uid%10) == 6) {  
        ads = AdEngineAdapter::instance().GetAds(serialized, append_para.str(), rotate_slot);
      } else {
        ads = AdEngineUAdapter::instance().GetAds(serialized, append_para.str(), rotate_slot);
      }*/
    } catch(Ice::Exception& e) {
      MCE_WARN("AdEngineUAdapter::GetAds error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdEngineUAdapter::GetAds error : " << e.what() << " uid=" << _login_uid);
    }
    MCE_INFO("engineU ad shown. uid=" << _login_uid << " count=" << ads.size()
             << " stage=" << profile.stage() << " ip_area=" << profile.ip_area());
  /*} else if ((_login_uid%10) > 4 || (_login_uid == 1244171572) || (_login_uid == 128487631)) {
    try {
      ads = AdEngineAdapter::instance().GetAds(serialized, append_para.str(), rotate_slot);
    } catch(Ice::Exception& e) {
      MCE_WARN("AdEngineAdapter::GetAds error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdEngineAdapter::GetAds error : " << e.what() << " uid=" << _login_uid);
    }
    MCE_INFO("renren ad shown. uid=" << _login_uid << " count=" << ads.size() << '/'
             << " stage=" << profile.stage() << " ip_area=" << profile.ip_area());*/
  } else {
    try {
      ads = AdTwAdapter::instance().GetAds(serialized, append_para.str(), rotate_slot);
    } catch(Ice::Exception& e) {
      MCE_WARN("AdTwAdapter::GetAds error : " << e << " uid=" << _login_uid);
    } catch(std::exception& e) {
      MCE_WARN("AdTwAdapter::GetAds error : " << e.what() << " uid=" << _login_uid);
    }
    MCE_INFO("taiwan ad shown. uid=" << _login_uid << " count=" << ads.size() << '/' 
        << " stage=" << profile.stage() << " ip_area=" << profile.ip_area());
  }

  for(AdResultMap::const_iterator it = ads.begin(); it != ads.end(); ++it) {
    MCE_INFO("AdEngineUAdapter::GetAds return : " << it->first << '/' << it->second.size());
  }

  if (ads.empty()) {
    AdCreativeCache::instance().PushDefault(_login_uid, GetRemoteAddr(), referer, &ads);
  }

  for(AdResultMap::const_iterator it = ads.begin(); it != ads.end(); ++it) {
     MCE_INFO("AdEngineUAdapter::GetAds add default ad : " << it->first << '/' << it->second.size());
   }

  stringstream ad_header;
  string ad_body;

  if (render_ == RT_HTML) {
    AdCreativeCache::instance().Get(ads, &ad_body, refresh_idx);
    ad_header << "Content-type: text/html; charset=utf-8\r\nSet-Cookie: an_slot=" << ++ rotate_slot << "; path=/ebp\r\n\r\n";
  } else if (render_ == RT_JSON) {
    AdCreativeCache::instance().GetJson(referer, ads, &ad_body, refresh_idx);
    ad_header << "Content-type: application/x-javascript; charset=utf-8\r\nSet-Cookie: an_slot=" << ++ rotate_slot << "; path=/ebp\r\n\r\n";
  }

  FCGX_PutS(ad_header.str().c_str(), _fcgi_out);

  FCGX_PutS(ad_body.c_str(), _fcgi_out);
  MCE_DEBUG("AdGetRequest::response --> return uid=" << _login_uid);
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
 
  string referer = getProperty("ref");
  if (referer.empty() || referer.find("http://") == string::npos) {
    referer = getProperty("r");
    if (referer.empty() || referer.find("http://") == string::npos) {
      referer = GetParamFix("HTTP_REFERER");
    }
  }

  if (matcher_cookie.empty() || matcher_cookie[0] != 't') {
    try {
      AdEngineUAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
      /*if((_login_uid%10) == 5 || (_login_uid%10) == 6) {  
        AdEngineAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
      } else {
        AdEngineUAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
      }*/
    } catch (Ice::Exception& e) {
      MCE_WARN("AdEngineUAdapter Click error : " << e << " uid=" << _login_uid);
    } catch (std::exception& e) {
      MCE_WARN("AdEngineUAdapter Click error : " << e.what() << " uid="
          << _login_uid);
    }
    MCE_INFO("renren ad clicked. uid=" << _login_uid << " ip=" << GetRemoteAddr());
  } else {
    try {
      AdTwAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
    } catch (Ice::Exception& e) {
      MCE_WARN("AdTwAdapter Click error : " << e << " uid=" << _login_uid);
    } catch (std::exception& e) {
      MCE_WARN("AdTwAdapter Click error : " << e.what() << " uid="
          << _login_uid);
    }
    MCE_INFO("taiwan ad clicked. uid=" << _login_uid << " ip=" << GetRemoteAddr());
  }

  string output = "Location: " + url + "\r\n\r\n";
  FCGX_PutS(output.c_str(), _fcgi_out);
  return true;
}


bool AdNewClickRequest::response() {
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
 
  string referer = getProperty("ref");
  if (referer.empty() || referer.find("http://") == string::npos) {
    referer = getProperty("r");
    if (referer.empty() || referer.find("http://") == string::npos) {
      referer = GetParamFix("HTTP_REFERER");
    }
  }

  if (matcher_cookie.empty() || matcher_cookie[0] != 't') {
    try {
      AdEngineJAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
      /*if((_login_uid%10) == 5 || (_login_uid%10) == 6) {  
        AdEngineAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
      } else {
        AdEngineUAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
      }*/
    } catch (Ice::Exception& e) {
      MCE_WARN("AdEngineJAdapter Click error : " << e << " uid=" << _login_uid);
    } catch (std::exception& e) {
      MCE_WARN("AdEngineJAdapter Click error : " << e.what() << " uid="
          << _login_uid);
    }
    MCE_INFO("renren ad clicked. uid=" << _login_uid << " ip=" << GetRemoteAddr());
  } else {
    try {
      AdTwAdapter::instance().Click(_login_uid, GetRemoteAddr(), matcher_cookie, referer, url);
    } catch (Ice::Exception& e) {
      MCE_WARN("AdTwAdapter Click error : " << e << " uid=" << _login_uid);
    } catch (std::exception& e) {
      MCE_WARN("AdTwAdapter Click error : " << e.what() << " uid="
          << _login_uid);
    }
    MCE_INFO("taiwan ad clicked. uid=" << _login_uid << " ip=" << GetRemoteAddr());
  }

  string output = "Location: " + url + "\r\n\r\n";
  FCGX_PutS(output.c_str(), _fcgi_out);
  return true;
}


