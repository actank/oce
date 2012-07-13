/*
 * AdQualityCache.cpp
 *
 *  Created on: 2011-04-12
 *      Author: zhizhao.sun
 */

#include "AdQualityCache.h"
//#include "QueryRunner.h"
#include "math.h"
#include <connection.h>
#include <query.h>
#include "AdConfig.h"

using namespace std;
using namespace mysqlpp;

namespace xce {
namespace ad {
using namespace MyUtil;
using namespace std;
using namespace boost;
double AdItem::quality() {
  if(IsCredible()) {
    return (double)click_count_ / (double) pv_count_;
  }
  return 0.0;
}

bool AdItem::IsFresh() const {
  if(pv_count_ > 18000 || click_count_ >2) {
    return false;
  }
  return true;
}

// 以秒为单位
int AdQualityCache::TimeInterval() { //返回单位小时
  return 1 * 60 * 60;
}
int AdQualityCache::HourCount() { //返回小时个数

  int value = HOURCOUNT;
  string s = AdConfig::instance().GetValue("HOURCOUNT");
  try{
    if(!s.empty()) {
      value = lexical_cast<int>(s);
    }
  } catch(std::exception & e) {
    MCE_WARN("AdQualityCache::HourCount-->get HOURCOUNT fail:string cast error:-" << s << "-" <<e.what());
  }
  return value;

}


bool AdQualityCache::LoadByTime(const string & begin_time, const string & end_time,
    map<Ice::Long, AdItemMap> & pool) {

//  Statement sql;
//  sql << "select group_id, zone_id, mem_id,creative_id, pv_count, click_count from ad_quality where time >= "
//      << mysqlpp::quote << begin_time << " AND time < " << mysqlpp::quote << end_time ;
  stringstream sql;
  sql << "select zone_id, sponsor_id, group_id, creative_id, impr, click from AD_CTR where time >= '"
      << begin_time << "' AND time < '" << end_time << "'";

  try {
    Connection conn("test","10.3.16.119","ad","rrad");
    string s = sql.str();
    MCE_DEBUG("AdQualityCache::LoadByTime-->sql:" << s.c_str());
    Query q(&conn,true,s.c_str());
//    Query q(&conn,true,sql.c_str());
    mysqlpp::StoreQueryResult res = q.store();
//    mysqlpp::StoreQueryResult res = QueryRunner(DB_INSTANCE, CDbRServer, "adn").store(sql);

    if (!res || res.num_rows() <= 0) {
      MCE_WARN("LoadByTime() " << begin_time << " - " << end_time<< " select ad_quality fail!");
      return false;
    }
    MCE_INFO("LoadByTime() " << begin_time << " - " << end_time<< " res size : " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long group_id = (Ice::Long) row["group_id"];
      Ice::Long zone_id = (Ice::Long) row["zone_id"];
      Ice::Long sponsor_id = (Ice::Long) row["sponsor_id"];
      Ice::Long creative_id = (Ice::Long) row["creative_id"];
      int pv_count = (int) row["impr"];
      int click_count = (int) row["click"];


      AdItemMap::iterator group_it = pool[zone_id].find(group_id);
      if (group_it == pool[zone_id].end()) {
        AdItemPtr p = new AdItem(group_id, zone_id, sponsor_id, creative_id);
        p->AddPvClick(pv_count, click_count);
        pool[zone_id].insert(make_pair(group_id, p));
      } else {
        group_it->second->AddPvClick(pv_count, click_count);
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

  MCE_INFO("AdQualityCache::load start");
  MyUtil::Date begin = MyUtil::Date::seconds(time(NULL) - HourCount() * TimeInterval());
  string begin_time = begin.str("%Y-%m-%d %H:%M:%S");
  MyUtil::Date end = MyUtil::Date::seconds(time(NULL));
  string end_time = end.str("%Y-%m-%d %H:%M:%S");


  map<Ice::Long, AdItemMap> pool;
  map<Ice::Long, AdQualityUMap> quality_pool;


  bool load_success = LoadByTime(begin_time, end_time, pool);
  if(!load_success) {
    MCE_WARN("AdQualityCache::load()-->load from DB fail");
    return false;
  }
  CalculateQuality(pool, quality_pool);
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
//    all_quality_u_.swap(all_quality_u);
//    pool_.swap(pool);
    quality_pool_.swap(quality_pool);
  }
  MCE_INFO("AdQualityCache::load and calculate stop");
  return true;
}

//完成从广告数据到最终缓存的质量结构体的转换
void AdQualityCache::CalculateQuality(map<Ice::Long, AdItemMap>& pool,map<Ice::Long, AdQualityUMap> &quality_pool){
  MCE_INFO("AdQualityCache::CalculateQuality begin-->pool size():" << pool.size() << ",quality_pool size():" <<quality_pool.size());
  for(map<Ice::Long, AdItemMap>::iterator it = pool.begin(); it != pool.end(); ++it) {
    AdQualityUMap ad_map;
    AdItemMap &item_map  = it->second;
    for(AdItemMap::iterator itm = item_map.begin(); itm != item_map.end(); ++itm) {
      AdItemPtr &ad_ptr = itm->second;
      AdQualityU ad;
      ad.groupId = ad_ptr->groupid();
      ad.quality = ad_ptr->quality();
      ad.ecpm = 0;//在AdQuality不再计算广告的ecpm，交给AdEngine计算。
      ad.isFresh = ad_ptr->IsFresh();
      ad_map[ad.groupId] = ad;
    }
    quality_pool[it->first] = ad_map;
  }

  MCE_INFO("AdQualityCache::CalculateQuality end-->pool size():" << pool.size() << ",quality_pool size():" <<quality_pool.size());
}

void QualityCacheReloadTimer::handle() {
  MCE_INFO("QualityCacheUpdateTimer::handle() begin");
  try {
    AdQualityCache::instance().load();
  } catch (...) {
    MCE_WARN("AdQualityCache.load() unknown error.");
  }

  MCE_INFO("QualityCacheUpdateTimer::handle() done");
}

AdQualityUMap AdQualityCache::GetQualityUByZone(Ice::Long zone_id) {
  TimeStat ts;
  AdQualityUMap ans;

  {
    IceUtil::RWRecMutex::RLock lock(mutex_);

    map<Ice::Long, AdQualityUMap>::iterator it = quality_pool_.find(zone_id);
    if (it != quality_pool_.end()) {
      ans = it->second;
    } else {
      MCE_WARN("AdQualityCache::GetQualityUByZone: not get zone's data:" <<zone_id);
    }
  }

  MCE_INFO("GetQualityUByZone(" << zone_id <<") used_time=" << ts.getTime() << " size=" << ans.size());
  return ans;
}

/*********************************************************************
 *
 * 一下为了兼容以前ice的接口，保留了其实现，在以后重构时可以去除。
*********************************************************************/
bool AdQualityCache::update(const AdStatMap &) {
  MCE_WARN("AdQualityCache::update not implementated.");
  return false;
}

AdQualityMap AdQualityCache::GetAllQualities() {
  TimeStat ts;
  AdQualityMap ans;
  MCE_WARN("AdQualityCache::GetQualityMap() not implementated.");
  return ans;
}

AdQualityMap AdQualityCache::GetQualityByZone(Ice::Long zone_id) {
  TimeStat ts;
  AdQualityMap ans;
  MCE_WARN("GetQualityByZone not implementated." );
  return ans;
}

AdQualityUMap AdQualityCache::GetAllQualitieUs() {
  TimeStat ts;
  AdQualityUMap ans;
  MCE_INFO("AdQualityCache::GetAllQualitieUs not implementated. ");
  return ans;
}

}
}
