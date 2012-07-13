/*
 * AdQualityScore.h
 *
 *  Created on: 2011-9-21
 *      Author: shuang
 */

#ifndef ADQUALITYSCORE_H_
#define ADQUALITYSCORE_H_

#include "AdQualityUtil.h"
#include "AdDemoQuality.h"

namespace xce{
namespace ad{
using namespace com::xiaonei::xce;
using namespace std;
using namespace MyUtil;

class ScoreAdCache : public MyUtil::Singleton<ScoreAdCache> {
private:
  IceUtil::RWRecMutex mutex_score;
  map<Ice::Long,double> avg_cpc;  
  map<Ice::Long,Ice::Long> Map_group_cam; 
  map<Ice::Long,Ice::Long> Map_cam_spon;
  map<Ice::Long,double> Campaign_score;  
  map<Ice::Long,double> Sponsor_score; 
  map<Ice::Long,int> score_ad_status;   
  AdQualityScoreMap score_ad_pool_; 
  
  void LoadAvgCpc();
  void LoadMap_group_cam();
  void LoadMap_cam_spon();
  void LoadSponsor_score();
  void LoadScore_status(); 
  double format_score(const double score,double base_point);
  void clear(){
    avg_cpc.clear();
    Map_group_cam.clear();
    Map_cam_spon.clear();
    Campaign_score.clear();
    Sponsor_score.clear();
    score_ad_status.clear();
  }
public:
  void LoadScoreAdPool();
  void UpdateScoreAdPool();  //更新 quality_score
  void LoadScoreAdMap(AdQualityScoreMap& score_ad_pool); 
};

class ScoreAdCacheTimer : public MyUtil::Timer {
public:
  ScoreAdCacheTimer(int time =0 ) :Timer(time){
  }
  virtual void handle() {
    MCE_INFO("ScoreAdCacheTimer handle :begin");
    ScoreAdCache::instance().LoadScoreAdPool();
    MCE_INFO("ScoreAdCacheTimer handle :end");
    MyUtil::TaskManager::instance().schedule(new ScoreAdCacheTimer(6 * 60 * 60 * 1000));  //6 小时 更新一次
  }
};

}
}
#endif /* ADQUALITYSCORE_H_*/
