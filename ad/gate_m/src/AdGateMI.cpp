#include <boost/regex.hpp>
#include <cstring>
#include "client/linux/handler/exception_handler.h"

#include "ad_creative_cache.h"
#include "ad_update_interval.h"
#include "ip_area_cache.h"

#include "FeedMemcProxy/client/user_profile_client.h"
#include "AdUserCacheLoaderAdapter.h"
#include "AdEngineBAdapter.h"
#include "AdEngineSAdapter.h"
#include "AdEngineUAdapter.h"
#include "AdTwSAdapter.h"
#include "AdGateMI.h"

using namespace std;
using namespace xce::ad;
using namespace boost;

string xce::ad::db_instance;

class StatFunc {
  string name_;
  timeval start_, end_;
public:
  StatFunc(string name) :
    name_(name) {
    gettimeofday(&start_, NULL);
  }

  ~StatFunc() {
    gettimeofday(&end_, NULL);
    float timeuse = 1000000 * (end_.tv_sec - start_.tv_sec) + end_.tv_usec
        - start_.tv_usec;
    timeuse /= 1000;
    if (timeuse >= 5) {
      MCE_INFO(name_ << " time usage : " << timeuse << " ms");
    } else {
      MCE_DEBUG(name_ << " time usage : " << timeuse << " ms");
    }
  }
};

bool MyCallback(const char *dump_path, const char *minidump_id, void *context,
    bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}

class CreativeReloadTimer : public MyUtil::Timer {
public:
  CreativeReloadTimer(time_t t) :
    Timer(t) {
  }
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
      if (strcmp(path, "/ebpm/show") == 0 || strcmp(path, "/ebpmt/show") == 0) {
        return new AdGetRequest(r, AdGetRequest::RT_JSON);
      }
      if (strcmp(path, "/ebpm/get.html") == 0) {
        return new AdGetRequest(r, AdGetRequest::RT_HTML);
      }
      if (strcmp(path, "/ebpm/click.html") == 0) {
        return new AdClickRequest(r);
      }
      if (strcmp(path, "/ebpm/test.html") == 0) {
        return new AdTestRequest(r);
      }
    }
    return NULL;
  }
};

void MyUtil::initialize() {
  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  ServiceI& service = ServiceI::instance();
  xce::ad::db_instance = service.getCommunicator()->getProperties()->getPropertyWithDefault("DbInstance", "ade_db");
  MCE_INFO("db_instance = " << xce::ad::db_instance);
  service.getAdapter()->add(&AdGateMI::instance(), service.createIdentity("M",
      ""));

  MCE_INFO("creatives load start.");
  AdCreativeCache::instance().Reload();
  MCE_INFO("creatives load complete.");

  TaskManager::instance().execute(new AdUpdateIntervalReloadTask());
  TaskManager::instance().execute(new IpAreaCacheReloadTask());
  TaskManager::instance().schedule(new CreativeReloadTimer(0));
  TaskManager::instance().scheduleRepeated(new CreativeReloadTimer(30 * 1000));

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service."
      + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_INFO("Fcgi listens on : " << fcgi_socket);

  srand( time(NULL));

  // fcgi_server memory managed as RequestFactory ...
  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 256, 256 * 1024);
  fcgi_server->RegisterRequestFactory(new GateRequestFactory());
  fcgi_server->Start();
}

bool AdGateMI::Reload(const Ice::Current&) {
  return AdCreativeCache::instance().Reload();
}

/*bool AdGateMI::ReloadCreativeById(Ice::Long id, const Ice::Current&) {
  return AdCreativeCache::instance().ReloadById(id);
}*/

bool AdGetRequest::response() {
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
    bool b = false;
    try {
      StatFunc stat("memc GetSerialized");
      b = UserProfileClient::instance().GetSerialized(_login_uid, &serialized);
    } catch (std::exception& e) {
      MCE_WARN("UserProfileClient error : " << e.what() << " uid="
          << _login_uid);
    }

    if (!b) {
      MCE_INFO("profile not found in memcache. id=" << _login_uid);
      try {
        StatFunc stat("memc AdUserCacheLoader");
        serialized = AdUserCacheLoaderAdapter::instance().GetById(_login_uid);
      } catch (Ice::Exception& e) {
        MCE_WARN("AdUserCacheLoaderAdapter GetById error : " << e << " uid="
            << _login_uid);
      } catch (std::exception& e) {
        MCE_WARN("AdUserCacheLoaderAdapter GetById error : " << e.what()
            << " uid=" << _login_uid);
      }
    }
  }
  // FCGX_PutS("Content-type: application/x-javascript; charset=utf-8\r\n\r\n{}", _fcgi_out);
  // return true;

  UserProfile profile;
  if (serialized.empty()) {
    profile.set_stage(4);
    profile.set_gender(0);
    profile.set_age(25);
    profile.set_school(0);
    profile.set_major(0);
    profile.set_grade(0);
    profile.set_home_area("");
    profile.set_current_area("");
  } else {
    profile.ParseFromString(serialized);
  }

  unsigned int ip = ntohl(inet_addr(GetRemoteAddr().c_str()));

  profile.set_id(_login_uid);
  profile.set_ip(ip);
  profile.set_ip_area(IpAreaCache::instance().GetIpArea(ip));
  profile.SerializeToString(&serialized);

  int rotate_slot = GetCookieInt("an_slot", rand());
  rotate_slot %= 10000;

  AdResultMap ads;
  string referer = getProperty("ref");
  //MCE_DEBUG("[UrlMaping] ref = " << referer);
  if (referer.empty() || referer.find("http://") == string::npos) {
    referer = getProperty("r");
    if (referer.empty() || referer.find("http://") == string::npos) {
    referer = GetParamFix("HTTP_REFERER");
      //MCE_DEBUG("[UrlMaping] HTTP_REFERER = " << referer);
    }
  }

  bool err_flag = false;

  bool hide = false;
  string isvip = getProperty("isvip");
  string hideads = getProperty("hideads");
  if(isvip.compare("true")==0 && hideads.compare("true")==0) {
    hide = true;
  }

  stringstream log_url;
  try {
    char sep = '&';
    if (referer.find_first_of('?') == string::npos) {
      sep = '?';
    }
    int refresh_source = getPropertyInt("refresh_source", 0);
    int refresh_idx = getPropertyInt("refresh_idx", 0);
    log_url << referer << sep << "refresh_source=" << refresh_source
        << "&refresh_idx=" << refresh_idx;
    
    stringstream stat_desc;
    stat_desc << _login_uid << " GetAds";
    StatFunc stat(stat_desc.str());
  } catch (Ice::Exception& e) {
    MCE_WARN("AdGetRequest::response error : " << e << " uid=" << _login_uid);
    err_flag = true;
  } catch (std::exception& e) {
    MCE_WARN("AdGetRequest::response : " << e.what() << " uid=" << _login_uid);
    err_flag = true;
  }

//  string emtag = "M";

  try {
    ads = AdEngineBAdapter::instance().GetAds(serialized, log_url.str(),
        rotate_slot, hide);
//    emtag = "M";
  } catch (Ice::Exception& e) {
    MCE_WARN("AdEngineBAdapter::GetAds error : " << e << " uid=" << _login_uid);
    err_flag = true;
  } catch (std::exception& e) {
    MCE_WARN("AdEngineBAdapter::GetAds error : " << e.what() << " uid="
        << _login_uid);
    err_flag = true;
  }

  map<long, EngineTypes> self_zones; // 需要中小自助广告广告位id
  for(AdResultMap::iterator it =  ads.begin(); it != ads.end(); it++) {
    if(!it->second.empty() && (it->second.at(0).groupid == -1)) {
      //台湾分尾号流量切分
      MCE_INFO("Get Ads from AdEngine/AdTw");
      if(_login_uid%10>4) {
        self_zones[it->first] = ENGINE;
      } else {
        self_zones[it->first] = TW;
      }
    }
  }

  if (!self_zones.empty()) {
    try {
      AdResultMap e_ads;
      //台湾分尾号流量切分
      if(_login_uid%10>4) {
        MCE_DEBUG("AdGetRequest::response --> from AdEngine rotate_slot = "
            << rotate_slot);
        if (_login_uid % 10 == 8) {
          e_ads = AdEngineUAdapter::instance().GetAds(serialized, log_url.str(), rotate_slot);
        } else {
          e_ads = AdEngineAdapter::instance().GetAds(serialized, log_url.str(), rotate_slot);
        }
      } else {
        MCE_DEBUG("[TW_DEBUG] AdGetRequest::response --> enter to request Tw");
        e_ads = AdTwAdapter::instance().GetAds(serialized, log_url.str(), rotate_slot);
      }
      if (e_ads.empty()) {
        MCE_DEBUG("AdGetRequest::response --> from AdEngine/Tw empty ");
      } else {
        for(AdResultMap::iterator xxit =  e_ads.begin(); xxit != e_ads.end(); xxit++) {
          MCE_DEBUG("AdGetRequest::response --> from AdEngine/Tw " << xxit->second.size());
        }
      }
      for (map<long, EngineTypes>::iterator zit = self_zones.begin(); zit
          != self_zones.end(); zit++) {
        AdResultMap::iterator eit = e_ads.find(zit->first);
        if (eit != e_ads.end()) {
          ads[zit->first] = eit->second;
        } else {
          ads[zit->first] = AdInfoSeq();
        }
      }
    } catch (Ice::Exception& e) {
      MCE_WARN("AdEngineAdapter::GetAds error : " << e << " uid=" << _login_uid);
      err_flag = true;
    } catch (std::exception& e) {
      MCE_WARN("AdEngineAdapter::GetAds error : " << e.what() << " uid="
          << _login_uid);
      err_flag = true;
    }
  }


//  AdResultMap eads;
//  AdResultMap::iterator tagit = ads.find(9999963);
//  if (tagit != ads.end() && ads.find(100000000063) != ads.end()) {
//    ads.earse(tagit);
//    try {
//      MCE_INFO("AdGetRequest::response --> from AdEngine rotate_slot = "
//          << rotate_slot);
//      eads = AdEngineAdapter::instance().GetAds(serialized, log_url.str(),
//          rotate_slot);
//      AdResultMap::iterator it = eads.find(100000000063);
//      if (it != eads.end()) {
//        ads[100000000063] = it->second;
//      }
//      emtag = "E";
//    } catch (Ice::Exception& e) {
//      MCE_WARN("AdEngineAdapter::GetAds error : " << e << " uid=" << _login_uid);
//      err_flag = true;
//    } catch (std::exception& e) {
//      MCE_WARN("AdEngineAdapter::GetAds error : " << e.what() << " uid="
//          << _login_uid);
//      err_flag = true;
//    }
//  }

  //MCE_INFO("GetAds size=" << ads.size() << " uid=" << _login_uid << " stage="
  //    << profile.stage() << " ip_area=" << profile.ip_area());
  
  if (err_flag) {
    stringstream stat_desc;
    stat_desc << _login_uid << " PushDefault";
    StatFunc stat(stat_desc.str());
    AdCreativeCache::instance().PushDefault(_login_uid, GetRemoteAddr(),
        referer, &ads);
  }

  stringstream ad_header;
  string ad_body;
  if (render_ == RT_HTML) {
    ad_header
        << "Content-type: text/html; charset=utf-8\r\nSet-Cookie: an_slot="
        << ++rotate_slot << "; path=/ebpn\r\n\r\n";
    AdCreativeCache::instance().Get(ads, &ad_body);
  } else if (render_ == RT_JSON) {
    ad_header
        << "Content-type: application/x-javascript; charset=utf-8\r\nSet-Cookie: an_slot="
        << ++rotate_slot << "; path=/ebpn\r\n\r\n";
    stringstream stat_desc;
    stat_desc << _login_uid << " GetJson";
    StatFunc stat(stat_desc.str());
    AdCreativeCache::instance().GetJson(referer, getPropertyInt(
        "refresh_source", 0), getPropertyInt("refresh_idx", 0), self_zones, ads,
        &ad_body);
  }

  FCGX_PutS(ad_header.str().c_str(), _fcgi_out);
  FCGX_PutS(ad_body.c_str(), _fcgi_out);
  return true;
}

bool AdTestRequest::response() {
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
  string referer = getProperty("ref");
  MCE_DEBUG("AdTestRequest --> referer=" << referer << ",uid:" << uid
      << ", ip:" << ip);

  //UserID|Gender|Birthday|Age|Stage|Area|School_SchoolArea|Grade|IPArea

  UserProfile profile;
  profile.set_ip_area(IpAreaCache::instance().GetIpArea(ip));

  try {
    profile.set_id(boost::lexical_cast<int>(uid));
  } catch (...) {
  }

  try {
    profile.set_stage(boost::lexical_cast<int>(stage));
  } catch (...) {
  }

  try {
    profile.set_home_area(home_area);
  } catch (...) {
  }

  try {
    profile.set_gender(boost::lexical_cast<int>(gender));
  } catch (...) {
  }

  try {
    profile.set_age(boost::lexical_cast<int>(age));
  } catch (...) {
  }

  if (stage == "4") {
    profile.set_current_area(work_area);
    profile.set_school(0);
    profile.set_grade(0);
  } else if (stage == "2" || stage == "1") {
    profile.set_current_area(school_area);
    try {
      profile.set_school(boost::lexical_cast<Ice::Long>(school));
      profile.set_grade(boost::lexical_cast<int>(grade));
    } catch (...) {
    }
  }

  string serialized;
  profile.SerializeToString(&serialized);

  int rotate_slot = GetCookieInt("an_slot", rand());
  rotate_slot %= 10000;

  bool hide = false;
  string isvip = getProperty("isvip");
  string hideads = getProperty("hideads");
  if(isvip.compare("true")==0 && hideads.compare("true")==0) {
    hide = true;
  }
  MCE_DEBUG("[VIP_DEBUG] uid = " << uid << ", hide = " << hide);

  AdResultMap ads;
  // string referer = GetParamFix("HTTP_REFERER");
  try {
    ads = AdEngineBAdapter::instance().GetAds(serialized, referer, rotate_slot, hide);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdEngineBAdapter::GetAds error : " << e << " uid=" << uid);
  } catch (std::exception& e) {
    MCE_WARN("AdEngineBAdapter::GetAds error : " << e.what() << " uid=" << uid);
  }
  for (AdResultMap::iterator it = ads.begin(); it != ads.end(); ++it) {
    for (size_t i = 0; i < it->second.size(); ++i) {
      if (it->second.at(i).creativeid == 0) {
        MCE_WARN("test, creativeid is 0");
      }
    }
  }

  MCE_INFO("AdEngineBAdapter::GetAds return size : " << ads.size() << " uid="
      << uid << " stage=" << profile.stage() << " ip_area="
      << profile.ip_area());

  string ad_json;
  string ad_body;

  AdCreativeCache::instance().Get(ads, &ad_body);
  ad_body = ad_body + "<p/>" + ad_json;
  MCE_DEBUG("AdTestRequest::response --> uid:" << uid << " ,body:" << ad_body);
  ostringstream ad_header;
  ad_header << "Content-type: text/html; charset=utf-8\r\nSet-Cookie: an_slot="
      << ++rotate_slot << "; path=/ebpn\r\n\r\n";
  FCGX_PutS(ad_header.str().c_str(), _fcgi_out);
  FCGX_PutS(ad_body.c_str(), _fcgi_out);

  return true;
}

bool AdClickRequest::response() {
  MCE_INFO("Enter click");
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
  string referer = getProperty("ref");
  if (referer.empty() || referer.find("http://") == string::npos) {
    referer = getProperty("r");
    if (referer.empty() || referer.find("http://") == string::npos) {
      referer = GetParamFix("HTTP_REFERER");
    }
  }

  string matcher_cookie = getProperty("mc");
  string url = getProperty("url");
  string sid = getProperty("sid");

  MCE_DEBUG("[CLICK_URL] url = " << url << ", sid = " << sid);
  stringstream log_url;
  try {
    char sep = '&';
    if (url.find_first_of('?') == string::npos) {
      sep = '?';
    }
    int refresh_source = getPropertyInt("refresh_source", 0);
    int refresh_idx = getPropertyInt("refresh_idx", 0);
    log_url << url << sep << "refresh_source=" << refresh_source
            << "&refresh_idx=" << refresh_idx;
    //如果是站内，需要把sid参数加上，用于验票
    regex expression("http://([^\\./]*\\.){0,}renren\\.com([/?].*){0,}");
    cmatch what;
    if(regex_match(url.c_str(), what, expression)) {
      MCE_DEBUG("[CLICK_URL] url " << url << " MATCHED");
      if(!sid.empty()) {
        stringstream sid_para;
        sid_para << sep << "sid=" << sid;
        url += sid_para.str();
      } else {
        MCE_INFO("[CLICK_URL] url " << url << " MATCHED but no sid!!");
      }
    } else {
      MCE_DEBUG("[CLICK_URL] url " << url << " didn't match");
    }
    MCE_DEBUG("[CLICK_URL] final click url = " << url);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdClickRequest::response error : " << e << " uid=" << _login_uid);
  } catch (std::exception& e) {
    MCE_WARN("AdClickRequest::response error : " << e.what() << " uid="
        << _login_uid);
  }

  EngineTypes etype = (EngineTypes) getPropertyInt("engine_type");
  if (etype == ENGINE) {
    try {
      AdEngineAdapter::instance().Click(_login_uid, GetRemoteAddr(),
          matcher_cookie, referer, log_url.str());
    } catch (Ice::Exception& e) {
      MCE_WARN("AdEngineAdapter::Click error : " << e << " uid=" << _login_uid);
    } catch (std::exception& e) {
      MCE_WARN("AdEngineAdapter::Click error : " << e.what() << " uid="
          << _login_uid);
    }
  } else if (etype == TW) {
    try {
      AdTwAdapter::instance().Click(_login_uid, GetRemoteAddr(),
          matcher_cookie, referer, log_url.str());
    } catch (Ice::Exception& e) {
      MCE_WARN("AdTwSAdapter::Click error : " << e << " uid=" << _login_uid);
    } catch (std::exception& e) {
      MCE_WARN("AdTwSAdapter::Click error : " << e.what() << " uid="
          << _login_uid);
    }
  } else {
    try {
      AdEngineBAdapter::instance().Click(_login_uid, GetRemoteAddr(),
          matcher_cookie, referer, log_url.str());
    } catch (Ice::Exception& e) {
      MCE_WARN("AdEngineBAdapter::Click error : " << e << " uid=" << _login_uid);
    } catch (std::exception& e) {
      MCE_WARN("AdEngineBAdapter::Click error : " << e.what() << " uid="
          << _login_uid);
    }

  }

  MCE_INFO("click uid:" << _login_uid << " location:" << url);
  string output = "Location: " + url + "\r\n\r\n";
  FCGX_PutS(output.c_str(), _fcgi_out);
  return true;
}

string AdGateMI::GetAds(int uid, Ice::Long zoneid, const string& ip_str, const Ice::Current&) {
  MCE_INFO("AdGateMI::GetAds() --> uid = " << uid << ", zone_id = " << zoneid << ", ip = " << ip_str);
  ostringstream oss;
  oss << "http://3g.renren.com/" << zoneid;
  string referer = oss.str();
  string serialized;
  if (uid > 0) {
    bool b = false;
    try {
      StatFunc stat("memc GetSerialized");
      b = UserProfileClient::instance().GetSerialized(uid, &serialized);
    } catch (std::exception& e) {
      MCE_WARN("UserProfileClient error : " << e.what() << " uid="
          << uid);
    }   

    if (!b) {
      MCE_INFO("profile not found in memcache. id=" << uid);
      try {
        StatFunc stat("memc AdUserCacheLoader");
        serialized = AdUserCacheLoaderAdapter::instance().GetById(uid);
      } catch (Ice::Exception& e) {
        MCE_WARN("AdUserCacheLoaderAdapter GetById error : " << e << " uid="
            << uid);
      } catch (std::exception& e) {
        MCE_WARN("AdUserCacheLoaderAdapter GetById error : " << e.what()
            << " uid=" << uid);
      }
    }
  }

  UserProfile profile;
  if (serialized.empty()) {
    profile.set_stage(4);
    profile.set_gender(0);
    profile.set_age(25);
    profile.set_school(0);
    profile.set_major(0);
    profile.set_grade(0);
    profile.set_home_area("");
    profile.set_current_area("");
  } else {
    profile.ParseFromString(serialized);
  }

  unsigned int ip = ntohl(inet_addr(ip_str.c_str()));
  profile.set_id(uid);
  profile.set_ip(ip);
  profile.set_ip_area(IpAreaCache::instance().GetIpArea(ip));
  profile.SerializeToString(&serialized);

  // int rotate_slot = (an_slot == 0) ? rand(): an_slot;
  // rotate_slot %= 10000;
  int rotate_slot = GetRotateSlot(uid);

  bool err_flag = false;

  AdResultMap ads;

  stringstream log_url;
  try {
    char sep = '&';
    if (referer.find_first_of('?') == string::npos) {
      sep = '?';
    }
    int refresh_source = 0;//getPropertyInt("refresh_source", 0);
    int refresh_idx = 0;//getPropertyInt("refresh_idx", 0);
    log_url << referer << sep << "refresh_source=" << refresh_source
      << "&refresh_idx=" << refresh_idx;

    stringstream stat_desc;
    stat_desc << uid << " GetAds";
    StatFunc stat(stat_desc.str());
  } catch (Ice::Exception& e) {
    MCE_WARN("AdGetRequest::response error : " << e << " uid=" << uid);
    err_flag = true;
  } catch (std::exception& e) {
    MCE_WARN("AdGetRequest::response : " << e.what() << " uid=" << uid);
    err_flag = true;
  }

  bool hide = false;
  try {
    ads = AdEngineBAdapter::instance().GetAds(serialized, log_url.str(), rotate_slot, hide);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdEngineBAdapter::GetAds error : " << e << " uid=" << uid);
    err_flag = true;
  } catch (std::exception& e) {
    MCE_WARN("AdEngineBAdapter::GetAds error : " << e.what() << " uid="
        << uid);
    err_flag = true;
  }

  if (err_flag) {
    stringstream stat_desc;
    stat_desc << uid << " PushDefault";
    StatFunc stat(stat_desc.str());
    AdCreativeCache::instance().PushDefault(uid, ip_str,
        referer, &ads);
  }

  for(AdResultMap::const_iterator it = ads.begin(); it != ads.end(); ++it) {
    MCE_INFO("AdEngineBAdapter::GetAds add default ad : " << it->first << '/' << it->second.size());
  }

  string ad_body;
  AdCreativeCache::instance().GetHtml(referer, ads, ad_body);

  return ad_body;
}
