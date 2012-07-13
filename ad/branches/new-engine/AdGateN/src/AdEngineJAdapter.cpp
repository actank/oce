#include "AdEngineJAdapter.h"
#include "IceLogger.h"

namespace xce {
namespace ad {
using namespace com::renren::ad::engine;
using namespace MyUtil;
using namespace com::xiaonei::xce;

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
  int idx = GetServerIndex(request.userAndAction.id);
  pair<string,int> client_idx = clients_[idx];
  EngineJClient client(client_idx.first,client_idx.second);
  client.handleRequest(response, request);
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
  int idx = GetServerIndex(uid);
  pair<string,int> client_idx = clients_[idx];
  EngineJClient client(client_idx.first,client_idx.second);
  client.handleClick(ca);
  MCE_DEBUG("AdEngineJAdapter::Click() --> out");
}

int AdEngineJAdapter::GetServerIndex( int uid) {
  return uid % clients_.size();
}

bool AdEngineJAdapter::Reload( ) {
  MCE_DEBUG("AdEngineJAdapter::Reload() --> beg");
  mysqlpp::Connection conn("test", "10.3.16.159", "ad", "adjb###");
 // mysqlpp::Connection conn("ad_strategy", EngineConfig::instance().style_db_host().c_str(), "ad", "rrad");
  string s("select * from engine_j;");
  mysqlpp::Query q(&conn,true,s.c_str());
  mysqlpp::StoreQueryResult res = q.store();
  vector< pair<string, int > > tmp_clients;
  if (res && res.num_rows() > 0) {
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      string tmp_host = (string) row["host"];
      int tmp_port = (int) row["port"];
      ostringstream oss;
      oss << "host = " << tmp_host << " port = " << tmp_port;
      MCE_DEBUG(oss.str());
      tmp_clients.push_back(make_pair(tmp_host,tmp_port));
    }
  }
  if(tmp_clients.size() > 0) {
      clients_.swap(tmp_clients);
  }
  MCE_DEBUG("AdEngineJAdapter::Reload() --> end");
}
}
}
