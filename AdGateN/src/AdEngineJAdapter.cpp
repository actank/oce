#include "AdEngineJAdapter.h"
#include "IceLogger.h"

namespace xce {
namespace ad {
using namespace com::renren::ad::engine;

AdResultMap AdEngineJAdapter::GetAds(const string & user_profile, const string & referer, int rotate_index) {
  /*UserAndAction ua(user_profile.id(), user_profile.stage(), user_profile.gender(),
                   user_profile.age(), user_profile.school(), user_profile.major(),
                   user_profile.grade(), user_profile.home_area(), user_profile.current_area(),
                   user_profile.ip(), user_profile.ip_area(), PAGEVIEW);
  */
  return AdResultMap();
}

AdResultMap AdEngineJAdapter::Request(const AdRequest& request) {
  MCE_DEBUG("AdEngineJAdapter::Request() --> zone size = " << request.zoneDict.size());
  AdResponse response;
  engine_j_client_.handleRequest(response, request);
  AdResultMap result;
  // 生成结果
  std::map<int64_t, std::vector<Ad> >&  candidates = response.candidates;
  ostringstream oss;
  oss << "AdEngineJAdapter::Request() --> zone size = " << candidates.size() << " ";
  for(std::map<int64_t, std::vector<Ad> >::iterator zit = candidates.begin();
      zit != candidates.end(); ++zit) {
    oss << " zone_id = " << zit->first;
    vector<Ad>& ads = zit->second;
    AdInfoSeq ad_info_seq;
    for(vector<Ad>::iterator git = ads.begin(); git != ads.end(); ++git) {
        AdInfo ad_info;
        ad_info.creativeid = (*git).creativeId;
        ad_info.groupid = (*git).adGroupId;
        ad_info.updateTime = 0;
        ad_info.text = (*git).context;
        ad_info_seq.push_back(ad_info);
        oss << " group = " << ad_info.groupid;
    }
    result[zit->first] = ad_info_seq;
  }
  oss << endl;
  MCE_DEBUG(oss.str());
  MCE_DEBUG("AdEngineJAdapter::Request() --> out");
  return result;
}

void AdEngineJAdapter::Click(int uid, const string & ip, const string & text, const string & referer, const string & click_url) {
  MCE_DEBUG("AdEngineJAdapter::Click() --> enter");
  ClickAction ca;
  ca.userId = uid;
  ca.ip = ip;
  ca.referer = referer;
  ca.clickUrl = click_url;
  ca.context = text;
  engine_j_client_.handleClick(ca);
  MCE_DEBUG("AdEngineJAdapter::Click() --> out");
}

}
}
