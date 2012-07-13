/*
 * AdQualityCache.cpp
 *
 *  Created on: 2010-12-6
 *      Author: zhaohui.tang
 */

#include "AdQualityCache.h"
#include "QueryRunner.h"
#include "math.h"

namespace xce {
namespace ad {
using namespace MyUtil;
using namespace std;
using namespace boost;

const string DB_INSTANCE = "ads_db";

vector<double> AdQualityBasic::section_weights_;

void AdQualityBasic::InitWeights() {
}

size_t AdQualityBasic::SectionCount() {
  if (section_weights_.empty()) {
    /* double weights[] = {
      18.0, 16.0, 15.0, 15.0, 13.0, 12.0, 
      12.0, 10.0, 10.0,  9.0,  9.0,  7.0, 
       6.0,  6.0,  5.0,  5.0,  5.0,  4.0, 
       4.0,  3.0,  2.6,  2.5,  2.4,  2.0, 
       2.0,  2.0,  2.0,  2.0,  1.6,  1.6, 
       2.0,  2.0,  2.0,  2.0,  1.6,  1.6, 
       1.6,  1.5,  1.5,  1.4,  1.3,  1.3, 
       1.3,  1.2,  1.1,  1.0,  1.0,  1.0, 
    }; */
    double weights[] = {
      256, 128,  64,  32,  16,   8,   4,   2,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
        0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,   0.1,  
    };
    size_t size = sizeof(weights) / sizeof(weights[0]);
    section_weights_.reserve(size);
    for(size_t i = 0; i < size; ++i) {
      section_weights_.push_back(weights[i]);
    }
    // InitWeights();
  }
  return section_weights_.size();
}

// 以秒为单位
int AdQualityBasic::SectionInterval() {
  return 1 * 60 * 60;
}

double AdQualityBasic::quality() {
  if (quality_ < 0.0) {
    int total_pv = 0;
    int total_click = 0;
    double total_qality = 0.0;
    double total_weight = 0.0;
    for (size_t i = 0; i < SectionCount(); ++i) {
      if (i >= section_pv_click_.size()) {
        break;
      }
      if (section_pv_click_[i].first > 0) {
        total_pv += section_pv_click_[i].first;
        total_click += section_pv_click_[i].second;
        total_weight += section_weights_[i];
        total_qality += section_weights_[i] * (double) section_pv_click_[i].second / (double) section_pv_click_[i].first;
      }
    }
    // quality_ = pow(1000000.0 * total_qality / total_weight, 0.5);
    quality_ = 10000.0 * total_qality / total_weight;

    if (total_pv < 30000 && total_click < 4) {
      quality_ *= 0.5; // pv 较少少而且click较少的广告，quality要打折, 以减少对稳定ctr广告的扰乱
    }
  }

  stringstream ss;
  ss << group_id_ << " zone="<< zone_id_ << " quality=" << quality_;
  int total_pv = 0, total_click = 0;
  for (size_t i = 0; i < section_pv_click_.size(); ++i) {
    ss << " "<< i << "-" << section_pv_click_[i].first << "-" << section_pv_click_[i].second;
    total_pv += section_pv_click_[i].first;
    total_click += section_pv_click_[i].second;
  }
  double ctr = 0.0;
  if (total_pv > 0) {
    ctr = 10000.0 * (double)total_click / (double)total_pv;
  }
  ss << " ctr=" << ctr << " fresh=" << IsFresh();
  MCE_INFO(ss.str());

  return quality_;
}

double AdQualityBasic::ecpm() {
  if (ecpm_ < 0.0) {
    int total_pv = 0;
    int total_cost = 0;
    for (size_t i = 0; i < SectionCount(); ++i) {
      if (i >= section_pv_cost_.size()) {
        break;
      }
      total_pv += section_pv_cost_[i].first;
      total_cost += section_pv_cost_[i].second;
    }
    if (total_pv > 0) {
    ecpm_ = double(total_cost) * 10 / double(total_pv);
    } 
  }

  stringstream ss;
  ss << group_id_ << " zone="<< zone_id_ << " ecpm=" << ecpm_;
  MCE_INFO(ss.str());
  return ecpm_;
}

bool AdQualityBasic::IsFresh() const {
  int total_pv = 0;
  int total_click = 0;
  for (size_t i = 0; i < section_pv_click_.size(); ++i) {
    total_click += section_pv_click_[i].second;
    total_pv += section_pv_click_[i].first;

    if (total_click > 2 || total_pv > 18000) {
      return false;
    }
  }
  return true;
}

bool AdQualityCache::LoadByTime(const string & begin_time, const string & end_time,
    map<Ice::Long, AdQualityBasicCluster> & pool) {

  Statement sql;
  sql << "select group_id, zone_id, sum(pv_count), sum(click_count), sum(cost) from ad_quality where time >= "
      << mysqlpp::quote << begin_time << " AND time < " << mysqlpp::quote << end_time  << " group by group_id";

  try {
    mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "adn").store(sql);
    if (!res || res.num_rows() <= 0) {
      MCE_WARN("LoadByTime() " << begin_time << " - " << end_time<< " select ad_quality fail!");
      return false;
    }
    MCE_INFO("LoadByTime() " << begin_time << " - " << end_time<< " res size : " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long group_id = (Ice::Long) row["group_id"];
      Ice::Long zone_id = (Ice::Long) row["zone_id"];
      int pv_count = (int) row["sum(pv_count)"];
      int click_count = (int) row["sum(click_count)"];
      int cost = (int) row["sum(cost)"];

      AdQualityBasicCluster::iterator group_it = pool[zone_id].find(group_id);
      if (group_it == pool[zone_id].end()) {
        AdQualityBasicPtr p = new AdQualityBasic(group_id, zone_id);
        // p->AddPvClick(pv_count, click_count);
        p->Add(pv_count, click_count, cost);
        pool[zone_id].insert(make_pair(group_id, p));
      } else {
        // group_it->second->AddPvClick(pv_count, click_count);
        group_it->second->Add(pv_count, click_count, cost);
      }
    }
  } catch (std::exception& e) {
    MCE_WARN("LoadByTime --> load ad_quality error: " << e.what());
    return false;
  } catch (...) {
    MCE_WARN("LoadByTime --> load ad_quality error!");
    return false;
  }
  return true;
}

bool AdQualityCache::load() {
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    if (is_updating_) {
      MCE_INFO("AdQualityCache::load return directly.");
      return false;
    }
    is_updating_ = true; // enter update
  }

  map<Ice::Long, AdQualityBasicCluster> pool;

  for (size_t section = 0; section < AdQualityBasic::SectionCount(); ++section) {
    Date begin = Date::seconds(time(NULL) - (section + 1) * AdQualityBasic::SectionInterval());
    string begin_time = begin.str("%Y-%m-%d %H:%M:%S");
    Date end = Date::seconds(time(NULL) - section * AdQualityBasic::SectionInterval());
    string end_time = end.str("%Y-%m-%d %H:%M:%S");

    LoadByTime(begin_time, end_time, pool);
  }

  AdQualityMap all_quality;
  AdQualityUMap all_quality_u;

  for(map<Ice::Long, AdQualityBasicCluster>::iterator i = pool.begin(); i != pool.end(); ++i) {
    AdQualityBasicCluster & cluster = i->second;
    MCE_INFO("zone " << i->first << " group_count=" << cluster.size());
    for(AdQualityBasicCluster::iterator j = cluster.begin(); j != cluster.end(); ++j) {
      AdQualityBasicPtr & quality_ptr = j->second;
      AdQuality adq;
      AdQualityU adqu;
      adq.groupId = quality_ptr->group_id();
      adq.quality = quality_ptr->quality();
      adq.isFresh = quality_ptr->IsFresh();
      all_quality[j->first] = adq;

      adqu.groupId = quality_ptr->group_id();
      adqu.quality = quality_ptr->quality();
      adqu.ecpm = quality_ptr->ecpm();
      adqu.isFresh = quality_ptr->IsFresh();
      all_quality_u[j->first] = adqu;
      MCE_INFO("adgroup_id " << quality_ptr->group_id() << " quality=" << adqu.quality << " ecpm=" << adqu.ecpm << " isFresh=" << adqu.isFresh);
    }
  }

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    all_quality_.swap(all_quality);
    all_quality_u_.swap(all_quality_u);
    pool_.swap(pool);
    is_updating_ = false;
  }

  return true;
}

bool AdQualityCache::update(const AdStatMap &) {
  MCE_INFO("AdQualityCache::update not implementated.");
  return false;
}

AdQualityMap AdQualityCache::GetAllQualities() {
  TimeStat ts;
  AdQualityMap ans;

  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    for(map<Ice::Long, AdQualityBasicCluster>::iterator i = pool_.begin(); i != pool_.end(); ++i) {
      AdQualityBasicCluster & cluster = i->second;
      for(AdQualityBasicCluster::iterator j = cluster.begin(); j != cluster.end(); ++j) {
        AdQualityMap::iterator qit = all_quality_.find(j->first);
        if(qit != all_quality_.end()) {
          ans.insert(*qit);
        }
      }
    }
  }

  MCE_INFO("AdQualityCache::GetQualityMap() used_time = " << ts.getTime() << " size=" << ans.size());
  return ans;
}

AdQualityMap AdQualityCache::GetQualityByZone(Ice::Long zone_id) {
  TimeStat ts;
  AdQualityMap ans;

  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, AdQualityBasicCluster>::iterator i = pool_.find(zone_id);
    if (i != pool_.end()) {
      AdQualityBasicCluster & cluster = i->second;
      for(AdQualityBasicCluster::iterator j = cluster.begin(); j != cluster.end(); ++j) {
        AdQualityMap::iterator qit = all_quality_.find(j->first);
        if(qit != all_quality_.end()) {
          ans.insert(*qit);
        }
      }
    }
  }

  MCE_INFO("GetQualityByZone(" << zone_id <<") used_time=" << ts.getTime() << " size=" << ans.size());
  return ans;
}

AdQualityUMap AdQualityCache::GetAllQualitieUs() {
  TimeStat ts;
  AdQualityUMap ans;

  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    for(map<Ice::Long, AdQualityBasicCluster>::iterator i = pool_.begin(); i != pool_.end(); ++i) {
      AdQualityBasicCluster & cluster = i->second;
      for(AdQualityBasicCluster::iterator j = cluster.begin(); j != cluster.end(); ++j) {
        AdQualityUMap::iterator qit = all_quality_u_.find(j->first);
        if(qit != all_quality_u_.end()) {
          ans.insert(*qit);
        }
      }
    }
  }

  MCE_INFO("AdQualityCache::GetAllQualitieUs() used_time = " << ts.getTime() << " size=" << ans.size());
  return ans;
}

AdQualityUMap AdQualityCache::GetQualityUByZone(Ice::Long zone_id) {
  TimeStat ts;
  AdQualityUMap ans;

  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, AdQualityBasicCluster>::iterator i = pool_.find(zone_id);
    if (i != pool_.end()) {
      AdQualityBasicCluster & cluster = i->second;
      for(AdQualityBasicCluster::iterator j = cluster.begin(); j != cluster.end(); ++j) {
        AdQualityUMap::iterator qit = all_quality_u_.find(j->first);
        if(qit != all_quality_u_.end()) {
          ans.insert(*qit);
        }
      }
    }
  }

  MCE_INFO("GetQualityUByZone(" << zone_id <<") used_time=" << ts.getTime() << " size=" << ans.size());
  return ans;
}
void QualityCacheReloadTimer::handle() {
  MCE_INFO("QualityCacheUpdateTimer::handle() begin");
  if (AdQualityManagerI::instance().init_ok()) {
    try {
      AdMemberPool::instance().Init();
      AdCampaignPool::instance().Init();
      AdGroupPool::instance().Init();

      AdQualityCache::instance().load();
    } catch (...) {
      MCE_WARN("AdQualityCache.load() unknown error.");
    }
  }
  MCE_INFO("QualityCacheUpdateTimer::handle() done");
}

}
}
