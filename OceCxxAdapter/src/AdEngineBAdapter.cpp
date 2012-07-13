#include <boost/lexical_cast.hpp>

#include "AdEngineBAdapter.h"
#include "FeedMemcProxy/client/user_profile_client.h"
using namespace MyUtil;
using namespace xce::ad;
using namespace boost;

AdMatchManagerPrx AdEngineBAdapter::getManager(int id) {
  return getProxy(id);
  //return locate<AdMatchManagerPrx> (_managers, "M", id, TWO_WAY);
}

AdMatchManagerPrx AdEngineBAdapter::getManagerOneway(int id) {
  return getProxyOneway(id);
  //return locate<AdMatchManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}

AdResultMap AdEngineBAdapter::GetAds(const string & user_profile, const string & referer, int rotate_index, bool hide) {
  Ice::Context ctx;
  if(hide) {
    ctx["hide"] = "true";
  } else {
    ctx["hide"] = "false";
  }
  UserProfile profile;
  UserProfileClient::instance().Deserialize(user_profile, &profile);
  return getManager(GetServerIndex(profile.id()))->GetAds(user_profile, referer, rotate_index, ctx);
  // return getManager(profile.id() / 10 % 10)->GetAds(user_profile, referer, rotate_index);
}

void AdEngineBAdapter::Click(int uid, const string & ip, const string & text, const string & referer,const string & click_url) {
  try {
    int server_index = GetServerIndex(uid); // boost::lexical_cast<int>(text[0]);
    getManagerOneway(server_index)->Click(uid, ip, text, referer, click_url);
  } catch (Ice::Exception& e) {
    MCE_WARN("AdEngineBAdapter::Click 1 err:" << e << ", line:" << __LINE__);
    getManagerOneway(0)->Click(uid, ip, text, referer, click_url);
  } catch (...) {
    MCE_WARN("AdEngineBAdapter::Click 2 err, line:" << __LINE__);
    getManagerOneway(0)->Click(uid, ip, text, referer, click_url);
  }
}

void AdEngineBAdapter::BindUser(int userid, Ice::Long creativeid, Ice::Long zoneid){
  getManagerOneway(GetServerIndex(userid))->BindUser(userid, creativeid, zoneid);
}
void AdEngineBAdapter::UnbindUser(int userid, Ice::Long zoneid){
  getManagerOneway(GetServerIndex(userid))->UnbindUser(userid, zoneid);
}
void AdEngineBAdapter::NotifyBindUser(int userid, Ice::Long creativeid, Ice::Long zoneid){
  size_t i, s;
  s = getCluster();
  for(i = 0; i < s; ++i){
    getManagerOneway(i)->NotifyBindUser(userid, creativeid, zoneid);
  }
}
void AdEngineBAdapter::NotifyUnbindUser(int userid, Ice::Long zoneid){
  size_t i, s;
  s = getCluster();
  for(i = 0; i < s; ++i){
    getManagerOneway(i)->NotifyUnbindUser(userid, zoneid);
  }
}

int AdEngineBAdapter::GetServerIndex(int uid) {
    int index_uid = uid;
    if (uid < 10) {
      index_uid = rand() % 100000;
      MCE_DEBUG("AdEngineBAdapter::GetServerIndex: set index_uid use rand id = " << index_uid);
    }
      MCE_DEBUG("AdEngineBAdapter::GetServerIndex:  uid = " << index_uid << ",cluster:"<< getCluster()<<",index:" <<(index_uid / 10) % getCluster() );
    return (index_uid / 10) % getCluster();
    //return 0;
}
