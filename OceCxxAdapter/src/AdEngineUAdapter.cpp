#include <boost/lexical_cast.hpp>

#include "AdEngineUAdapter.h"
#include "FeedMemcProxy/client/user_profile_client.h"

using namespace MyUtil;
using namespace xce::ad;
using namespace boost;

AdMatchManagerPrx AdEngineUAdapter::getManager(int id) {
  //return locate<AdMatchManagerPrx> (_managers, "M", id, TWO_WAY, 500);
  return getProxy(id);
}

AdMatchManagerPrx AdEngineUAdapter::getManagerOneway(int id) {
  //return locate<AdMatchManagerPrx> (_managersOneway, "M", id, ONE_WAY, 500);
  return getProxyOneway(id);
}

AdResultMap AdEngineUAdapter::GetAds(const string & user_profile, const string & referer, int rotate_index, bool env_test) {
  UserProfile profile;
  UserProfileClient::instance().Deserialize(user_profile, &profile);
  return getManager(GetServerIndex(profile.id(), env_test))->GetAds(user_profile, referer, rotate_index);
}

AdResultMap AdEngineUAdapter::GetAds(const string & user_profile, const string & referer, int rotate_index, int login_uid, const string& tag, const string& photo, bool env_test) {
  Ice::Context ctx;
  if(!tag.empty()) {
    ctx["tag"] = tag;
  }
  if(!photo.empty()) {
    ctx["photo"] = photo;
  }

  int index = GetServerIndex(login_uid, env_test);
  try {
    return getManager(index)->GetAds(user_profile, referer, rotate_index, ctx);
  } catch (Ice::TimeoutException& e) {
    MCE_WARN("AdEngineUAdapter(" << index << ")::GetAds error : " << e << " uid=" << login_uid);
    return AdResultMap();
  } catch (Ice::Exception& e) {
    MCE_WARN("AdEngineUAdapter(" << index << ")::GetAds error : " << e << " uid=" << login_uid);
    return AdResultMap();
  } catch (std::exception& e) {
    MCE_WARN("AdEngineUAdapter(" << index << ")::GetAds error : " << e.what() << " uid=" << login_uid);
    return AdResultMap();
  }
}

AdZone2Creatives AdEngineUAdapter::GetAdsByZone(const string & user_profile, const string & referer, int rotate_index, int login_uid, const string& tag, const string& photo, const string& count, bool env_test) {
  Ice::Context ctx;
  if(!tag.empty()) {
    ctx["tag"] = tag;
  }
  if(!photo.empty()) {
    ctx["photo"] = photo;
  }
  if(!count.empty()) {
    ctx["count"] = count;
  }
  int index = GetServerIndex(login_uid, env_test);
  try {
    return getManager(index)->GetAdsByZone(user_profile, referer, rotate_index, ctx);
  } catch (Ice::TimeoutException& e) {
    MCE_WARN("AdEngineUAdapter(" << index << ")::GetAds error : " << e << " uid=" << login_uid);
    return AdZone2Creatives();
  } catch (Ice::Exception& e) {
    MCE_WARN("AdEngineUAdapter(" << index << ")::GetAds error : " << e << " uid=" << login_uid);
    return AdZone2Creatives();
  } catch (std::exception& e) {
    MCE_WARN("AdEngineUAdapter(" << index << ")::GetAds error : " << e.what() << " uid=" << login_uid);
    return AdZone2Creatives();
  }
}

AdZone2Creatives AdEngineUAdapter::GetAdsByZoneInfo(const ::std::string& user_profile, const ::std::string& referer, int rotate_index, const ZoneInfoMap& zonemap, int login_uid, const string& photo, const string& count, bool env_test) {
  Ice::Context ctx;
  if(!photo.empty()) {
    ctx["photo"] = photo;
  }
  if(!count.empty()) {
    ctx["count"] = count;
  }
  int index = GetServerIndex(login_uid, env_test);
  try {
    return getManager(index)->GetAdsByZoneInfo(user_profile, referer, rotate_index, zonemap, ctx);
  } catch (Ice::TimeoutException& e) {
    MCE_WARN("AdEngineUAdapter(" << index << ")::GetAdsByZoneInfo error : " << e << " uid=" << login_uid);
    return AdZone2Creatives();
  } catch (Ice::Exception& e) {
    MCE_WARN("AdEngineUAdapter(" << index << ")::GetAdsByZoneInfo error : " << e << " uid=" << login_uid);
    return AdZone2Creatives();
  } catch (std::exception& e) {
    MCE_WARN("AdEngineUAdapter(" << index << ")::GetAdsByZoneInfo error : " << e.what() << " uid=" << login_uid);
    return AdZone2Creatives();
  }
}

void AdEngineUAdapter::Click(int uid, const string & ip, const string & text, const string & referer,const string & click_url, bool env_test) {
  try {
    getManagerOneway(GetServerIndex(uid, env_test))->Click(uid, ip, text, referer, click_url);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdEngineUAdapter::Click 1 err:" << e << ", line:" << __LINE__);
    getManagerOneway(GetServerIndex(uid, env_test))->Click(uid, ip, text, referer, click_url);
  } catch (...) {
    MCE_WARN("AdEngineUAdapter::Click 2 err, line:" << __LINE__);
    getManagerOneway(GetServerIndex(uid, env_test))->Click(uid, ip, text, referer, click_url);
  }
}

void AdEngineUAdapter::Close(int uid, Ice::Long creative, bool env_test) {
  try {
    getManagerOneway(GetServerIndex(uid, env_test))->Close(uid, creative);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdEngineUAdapter::Close 1 err:" << e << ", line:" << __LINE__);
    getManagerOneway(GetServerIndex(uid, env_test))->Close(uid, creative);
  } catch (...) {
    MCE_WARN("AdEngineUAdapter::Close 2 err, line:" << __LINE__);
    getManagerOneway(GetServerIndex(uid, env_test))->Close(uid, creative);
  }
}

void AdEngineUAdapter::BindUser(int userid, Ice::Long creativeid, Ice::Long zoneid, bool env_test){
  getManagerOneway(GetServerIndex(userid, env_test))->BindUser(userid, creativeid, zoneid);
}
void AdEngineUAdapter::UnbindUser(int userid, Ice::Long zoneid, bool env_test){
  getManagerOneway(GetServerIndex(userid, env_test))->UnbindUser(userid, zoneid);
}
void AdEngineUAdapter::NotifyBindUser(int userid, Ice::Long creativeid, Ice::Long zoneid){
  size_t i, s;
  s = getCluster();
  for(i = 0; i < s; ++i){
    getManagerOneway(i)->NotifyBindUser(userid, creativeid, zoneid);
  }
}
void AdEngineUAdapter::NotifyUnbindUser(int userid, Ice::Long zoneid){
  size_t i, s;
  s = getCluster();
  for(i = 0; i < s; ++i){
    getManagerOneway(i)->NotifyUnbindUser(userid, zoneid);
  }
}

int AdEngineUAdapter::GetServerIndex(int uid, bool env_test) {
  int index_uid = uid;
  if (uid < 10) {  // uid 没有的时候，随机散到RR服务
    index_uid = rand() % 100000;
    MCE_DEBUG("AdEngineUAdapter::GetServerIndex: set index_uid use rand id = " << index_uid);
  }
  if(!env_test) {
    /*
    int index = (index_uid) % 15;
    if(index <= 8) {
      index = (index_uid / 1000) % 18;
    } else if (index == 9) {
      index = 18 + (index_uid / 1000) % 8;
    } else {
      index = 26 + (index_uid / 1000) % 20;
    }
    return index % cluster();
    */
    return index_uid % getCluster();
  } else {
    return 0;
  }
  //return index%cluster();
  //return 0;
}


