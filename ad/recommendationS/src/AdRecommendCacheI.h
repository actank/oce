/*
 * AdRecommendCacheI.h
 *
 *  Created on: Dec 2, 2010
 *      Author: zhaohui.tang
 */

#ifndef AD_RECOMMEND_CACHE_I_H_
#define AD_RECOMMEND_CACHE_I_H_
//#include <map>
#include <IceUtil/RWRecMutex.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include "Singleton.h"
#include "AdReConfig.h"
#include "../../util/src/AdCommonUtil.h"
#include <map>
#include <string>
#include "AdRecommendationHandlerI.h"
#include "TaskManager.h"

#include <boost/lexical_cast.hpp>

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace boost;

class Recommend : public IceUtil::Shared {
public:
  Recommend(const AdResultE & ad_result) :
    ad_result_(ad_result), last_update_time_(time(NULL)), 
    last_access_time_(time(NULL)) {
  }
  
//  void Merge(int uid, const Recommend & r);

  AdResultE ad_result() const {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    return ad_result_;
  }
  void set_ad_result(AdResultE ad_result) {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    this->ad_result_ = ad_result;
  }

  bool IsTimeout() {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    time_t duration = time(NULL) - last_update_time_;
    if (ad_result_.empty()) {
      return duration > GetETimeout();
    }
    return duration > GetTimeout();
  }
  int GetTimeout() {
    int to = TIMEOUT;
    string s = RecommendConfig::instance().GetValue("RE_TIMEOUT");
    if (!s.empty()) {
      to = lexical_cast<int> (s);
    }
    MCE_DEBUG("Recommend::--> GetTimeout = " << to);
    return to;
  }
  int GetETimeout() {
    int eto = EMPTY_TIMEOUT;
    string s = RecommendConfig::instance().GetValue("EMPTY_RE_TIMEOUT");
    if (!s.empty()) {
      eto = lexical_cast<int> (s);
    }
    MCE_DEBUG("Recommend::--> GetETimeout = " << eto);
    return eto;
  }
  bool IsJustAccessed() {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    time_t duration = time(NULL) - last_access_time_;
    return duration < JUST_ACCESSED_DURATION;
  }
  void Accessed() {  // update the last_access_time
//    IceUtil::RWRecMutex::WLock lock(mutex_);
    last_access_time_ = time(NULL);
  }
  void UpdateResult(int uid, const AdResultE& r);
//  void IncPv(Ice::Long group_id);
  
//  int GetPv(Ice::Long group_id);
private:
  AdResultE ad_result_;
//  map<Ice::Long, int> pv_map_; // group's pv
  time_t last_update_time_;
  time_t last_access_time_;

  static const int TIMEOUT = 8 * 60;
  static const int EMPTY_TIMEOUT = 8 * 60;

  static const int JUST_ACCESSED_DURATION = 5 * 60;
  IceUtil::RWRecMutex mutex_;
};

typedef IceUtil::Handle<Recommend> RecommendPtr;

class AdRecommendCacheI : public MyUtil::Singleton<AdRecommendCacheI> {
public:
  AdRecommendCacheI() {
    for(int i = 0; i < POOLSIZE; i++) {
      evictor_[i].size(EVICTOR_SIZE);
    }
  //  evictor_.size(EVICTOR_SIZE);
    TaskManager::instance().scheduleRepeated(new ReloadTimer());
  }
  bool Insert(int user_id, const AdResultE & ad_result);
  bool ReCalculate(int user_id);
  AdResultE GetAdResult(int user_id);
  void DisposeClick(int userid, long groupid);
//  void IncPv(int userid, long groupid);


private:
  static const int POOLSIZE = 1000;
  static const int EVICTOR_SIZE = 500;
  int GetPoolIndex(int userid) {
    return ((userid / 100) % POOLSIZE);
  }
  class ReloadTimer : public MyUtil::Timer {
  public:
    ReloadTimer() :
      Timer(2 * 60 * 1000) {
    }
    virtual void handle() {
      try {
        AdRecommendCacheI::instance().Reload();
      } catch (Ice::Exception& e) {
        MCE_WARN("AdRecommendCacheI --> reload err:" << e << ", line:"
            << __LINE__);
      } catch (...) {
        MCE_WARN("AdRecommendCacheI --> reload error, line:" << __LINE__);
      }
    }
  };

  Evictor<int, RecommendPtr> evictor_[POOLSIZE];
  void Reload();

//  IceUtil::RWRecMutex mutex_[POOLSIZE];


};

} // end namespace ad
} // end namespace xce

#endif /* AD_RECOMMEND_CACHE_I_H_ */
