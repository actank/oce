/*
 * AdQualityCache.h
 *
 *  Created on: 2010-12-6
 *      Author: zhaohui.tang
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
#include "AdCache.h"

namespace xce {
namespace ad {

class AdQualityBasic : public IceUtil::Shared {
public:
  AdQualityBasic(Ice::Long group_id, Ice::Long zone_id) : group_id_(group_id), zone_id_(zone_id), quality_(-1.0), ecpm_(-1.0) {}

  void AddPvClick(int pv, int click) {
    if (!section_pv_click_.empty() && 
        (section_pv_click_.back().first < 12000 || pv < 12000)) {
      section_pv_click_.back().first += pv;
      section_pv_click_.back().second += click;
    } else {
      section_pv_click_.push_back(make_pair(pv, click));
    }
  }

  void Add(int pv, int click, int cost) {
    if (!section_pv_click_.empty() && 
        (section_pv_click_.back().first < 12000 || pv < 12000)) {
      section_pv_click_.back().first += pv;
      section_pv_click_.back().second += click;

      section_pv_cost_.back().first += pv;
      section_pv_cost_.back().second += cost;
    } else {
      section_pv_click_.push_back(make_pair(pv, click));
      section_pv_cost_.push_back(make_pair(pv, cost));
    }
  }

  double quality();
  double ecpm();
  Ice::Long group_id() const { return group_id_; }
  bool IsFresh() const;

  static size_t SectionCount();
  static int SectionInterval();
private:
  void InitWeights();
  Ice::Long group_id_;
  Ice::Long zone_id_;

  // static double section_weights_[];
  static vector<double> section_weights_;
  vector<pair<int, int> > section_pv_click_; // 每个section的 pv, click
  vector<pair<int, int> > section_pv_cost_; // 每个section的 pv, cost
  double quality_;
  double ecpm_;
};

typedef IceUtil::Handle<AdQualityBasic> AdQualityBasicPtr;
typedef map<Ice::Long, AdQualityBasicPtr> AdQualityBasicCluster;

class AdQualityCache : public MyUtil::Singleton<AdQualityCache> {
public:
  AdQualityCache() : is_updating_(false) {
    srand(time(NULL));
  }
  bool load();
  bool update(const AdStatMap & stat_map);

  AdQualityMap GetAllQualities();
  AdQualityMap GetQualityByZone(Ice::Long zone_id);
  AdQualityUMap GetAllQualitieUs();
  AdQualityUMap GetQualityUByZone(Ice::Long zone_id);
private:
  bool LoadByTime(const string & begin_time, const string & end_time,
    map<Ice::Long, AdQualityBasicCluster> & pool);

  bool is_updating_;

   map<Ice::Long, AdQualityBasicCluster> pool_; // merge_key --> user_key --> AdQuality
  IceUtil::RWRecMutex mutex_;

  AdQualityMap all_quality_;
  AdQualityUMap all_quality_u_;
};

class QualityCacheReloadTimer : public MyUtil::Timer {
public:
  QualityCacheReloadTimer() :
    Timer(1 * 60 * 1000) {
  }
  virtual void handle();
};
#endif /* ADQUALITYCACHE_H_ */

}
}
