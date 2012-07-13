/*
 * AdQualityCache.h
 *
 *  Created on: 2011-04-12
 *      Author: zhizhao.sun
 */

#ifndef ADQUALITYCACHE_H_
#define ADQUALITYCACHE_H_

#include "AdQualityManagerI.h"
#include <string>
#include <map>
#include <IceUtil/RWRecMutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include "Singleton.h"
#include "Date.h"
#include "IceLogger.h"



namespace xce {
namespace ad {

class AdItem : public IceUtil::Shared {
public:
  AdItem(Ice::Long group_id, Ice::Long zone_id, Ice::Long mem_id, Ice::Long creative_id) : group_id_(group_id),
  zone_id_(zone_id), sponsor_id_(mem_id),creative_id_(creative_id),pv_count_(0),click_count_(0){}

  void AddPvClick(int pv,int click) {
	  pv_count_ += pv;
	  click_count_ += click;
  }
  //是否数据可信，来计算广告质量
  bool IsCredible() {
    return pv_count_ > 0;
  }

  double quality();
  Ice::Long groupid() const { return group_id_; }
  bool IsFresh() const;


private:
  Ice::Long group_id_;
  Ice::Long zone_id_;
  Ice::Long sponsor_id_;
  Ice::Long creative_id_;
  int pv_count_;
  int click_count_;

};

typedef IceUtil::Handle<AdItem> AdItemPtr;
typedef map<Ice::Long, AdItemPtr> AdItemMap;

class AdQualityCache : public MyUtil::Singleton<AdQualityCache> {
public:
  AdQualityCache() {
   // srand(time(NULL));
  }
  virtual bool load();

  AdQualityUMap GetQualityUByZone(Ice::Long zone_id);


  /**********************************************
   * 以下接口只为了兼容老版本，没有实际实现
   **********************************************/
  bool update(const AdStatMap & stat_map);

  AdQualityMap GetAllQualities();
  AdQualityMap GetQualityByZone(Ice::Long zone_id);
  AdQualityUMap GetAllQualitieUs();

private:
  void CalculateQuality(map<Ice::Long, AdItemMap>& pool, map<Ice::Long, AdQualityUMap> &quality_pool);
  bool LoadByTime(const string & begin_time, const string & end_time,
    map<Ice::Long, AdItemMap> & pool);
  static int TimeInterval();
  static int HourCount();
  static const size_t HOURCOUNT = 24;
   map<Ice::Long, AdQualityUMap> quality_pool_;

  IceUtil::RWRecMutex mutex_;

};

class QualityCacheReloadTimer : public MyUtil::Timer {
public:
  QualityCacheReloadTimer() :
    Timer(10 * 60 * 1000) { //10分钟重新计算一次
  }
  virtual void handle();
};
#endif /* ADQUALITYCACHE_H_ */

}
}
