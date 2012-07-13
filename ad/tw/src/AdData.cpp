/*
 * AdData.cpp
 *
 *  Created on: 2010-12-13
 *      Author: ark
 */

//#include <stringstream>
#include "IceLogger.h"
#include "QueryRunner.h"
#include "AdUrlUtil.h"
#include "AdData.h"

namespace xce {

namespace ad {

TwZone::TwZone() {
  // TODO Auto-generated constructor stub

}

TwZone::TwZone(Ice::Long id, int count): id_(id), count_(count) {

}

TwZone::~TwZone() {
  // TODO Auto-generated destructor stub
}

TwZonePtr TwZonePool::GetAdZone(const Ice::Long adzone_id) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<Ice::Long, TwZonePtr>::iterator mit = zone_pool_.find(adzone_id);
  if(mit != zone_pool_.end()) {
    return mit->second;
  }
  return NULL;
}

void TwZonePool::GetAdZone(const set<Ice::Long>& adzone_ids, set<TwZonePtr>& adzones) {
  IceUtil::RWRecMutex::RLock lock(mutex_);
  ostringstream oss;
  oss << "all found available adzones: ";
  for(set<Ice::Long>::const_iterator it = adzone_ids.begin(); it != adzone_ids.end(); ++it) {
    map<Ice::Long, TwZonePtr>::iterator mit = zone_pool_.find(*it);
    if(mit != zone_pool_.end()) {
      adzones.insert(mit->second);
      oss << "adzone_id = " << mit->first << ", count = " << mit->second->count();
    }
  }
  MCE_DEBUG("TwZonePool::GetAdZone() --> " << oss.str());
}

bool TwZonePool::Load() {
  string db = AdTwConfig::instance().DB();
  MCE_INFO("TwZone Loading ..., db = " << db);
  Statement sql;
  mysqlpp::StoreQueryResult res;
  sql << "select adzone_id,bp_count from adzone "
      "where (member_type=0 or member_type=2) and member_online_status=1 and am_online_status=1 "
      " and audit_status=1 and delete_flag=1";
  res = QueryRunner(db, CDbRServer).store(sql);
  map<Ice::Long, TwZonePtr> temp_pool;
  for (size_t i = 0; i < res.num_rows(); i++) {
    mysqlpp::Row & row = res.at(i);
    Ice::Long adzone_id = boost::lexical_cast<Ice::Long>(row["adzone_id"]);
    int count = boost::lexical_cast<int>(row["bp_count"]);
    TwZonePtr p = new TwZone(adzone_id, count);
    MCE_INFO("TwZonePool::Load() --> adzone_id = " << adzone_id << ", count = " << count);
    //直接赋值，如果有广告位重复就覆盖
    temp_pool[adzone_id] = p;
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    zone_pool_.swap(temp_pool);
  }
  MCE_INFO("TwZone Loading done!!! db = " << db);
  return true;
}

}

}
