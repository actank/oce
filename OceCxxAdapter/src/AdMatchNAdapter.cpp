#include "AdMatchNAdapter.h"
#include <boost/lexical_cast.hpp>
#include "FeedMemcProxy/client/user_profile_client.h"
using namespace MyUtil;
using namespace xce::ad;
using namespace boost;

AdMatchManagerPrx AdMatchNAdapter::getManager(int id) {
  return locate<AdMatchManagerPrx> (_managers, "M", id, TWO_WAY);
}

AdMatchManagerPrx AdMatchNAdapter::getManagerOneway(int id) {
  return locate<AdMatchManagerPrx> (_managersOneway, "M", id, ONE_WAY);
}

AdResultMap AdMatchNAdapter::GetAds(const string & user_profile, const string & referer, int rotate_index, bool hide) {
  Ice::Context ctx;
  if(hide) {
    ctx["hide"] = "true";
  } else {
    ctx["hide"] = "false";
  }
  UserProfile profile;
  UserProfileClient::instance().Deserialize(user_profile, &profile);
  return getManager(profile.age() % 10)->GetAds(user_profile, referer, rotate_index, ctx);
}

void AdMatchNAdapter::Click(int uid, const string & ip, const string & text, const string & referer,const string & click_url) {
  int i = lexical_cast<int> (text.substr(0, 1));
  getManagerOneway(i)->Click(uid, ip, text, referer, click_url);
}

void AdMatchNAdapter::BindUser(int userid, Ice::Long creativeid, Ice::Long zoneid){
  getManagerOneway(0)->BindUser(userid, creativeid, zoneid);
}
void AdMatchNAdapter::UnbindUser(int userid, Ice::Long zoneid){
  getManagerOneway(0)->UnbindUser(userid, zoneid);
}
void AdMatchNAdapter::NotifyBindUser(int userid, Ice::Long creativeid, Ice::Long zoneid){
  size_t i, s;
  s = cluster();
  for(i = 0; i < s; ++i){
    getManagerOneway(i)->NotifyBindUser(userid, creativeid, zoneid);
  }
}
void AdMatchNAdapter::NotifyUnbindUser(int userid, Ice::Long zoneid){
  size_t i, s;
  s = cluster();
  for(i = 0; i < s; ++i){
    getManagerOneway(i)->NotifyUnbindUser(userid, zoneid);
  }
}


