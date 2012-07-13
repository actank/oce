/*
 * AdRecommendCache.h
 *
 *  Created on: Dec 2, 2010
 *      Author: zhaohui.tang
 */

#ifndef AD_RECOMMEND_CACHE_H_
#define AD_RECOMMEND_CACHE_H_
//#include <map>
#include <IceUtil/RWRecMutex.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include "Singleton.h"
#include "Evictor.h"
#include "AdRecommendationHandler.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class Recommend : public IceUtil::Shared {
public:
  Recommend() : last_update_time_(time(NULL)) {
  }

  AdResult ad_result() const {
    return ad_result_;
  }
  void set_ad_result(AdResult ad_result) {
    this->ad_result_ = ad_result;
  }

  time_t GetDuration() {
    return time(NULL) - last_update_time_; // second
  }
private:
  AdResult ad_result_;

  time_t last_update_time_;
};

typedef IceUtil::Handle<Recommend> RecommendPtr;


template<class _Key, class _Object>
class Evictor: public MyUtil::SizedEvictor<_Key, _Object> {
public:
  typedef typename MyUtil::SizedEvictor<_Key, _Object>::EvictorMap::iterator EvictorIterator;

  EvictorIterator begin() {
    return MyUtil::SizedEvictor<_Key, _Object>::_map.begin();
  }

  EvictorIterator end() {
    return MyUtil::SizedEvictor<_Key, _Object>::_map.end();
  }
  pair<_Key,_Object> getObject(const EvictorIterator& pos) {
    IceUtil::Mutex::Lock lock(MyUtil::SizedEvictor<_Key, _Object>::_mutex);
    if (pos != MyUtil::SizedEvictor<_Key, _Object>::_map.end()) {

      return pair<_Key,_Object>(pos->first, pos->second.obj);
    }
    return pair<_Key,_Object>(0,NULL);

  }
};

class AdRecommendCache : public MyUtil::Singleton<AdRecommendCache> {
public:
  AdRecommendCache() {
    evictor_.size(EVICTOR_SIZE);
  }
  pair<AdResult, bool> GetAdResult(int user_id);
  void insert(int user_id, RecommendPtr rp);
  vector<int> GetInvalidList();
  typedef Evictor<int, RecommendPtr>::EvictorIterator Evictoriter;
private:
  static const int PRESERVATION_TIME = 1 * 5 * 60; //1 * 60 * 60
  static const int EMPTY_PRESERVATION_TIME = 3 * 60; //20 * 60
  static const int INVALID_TIME = 8 * 60 * 60; //1 * 60 * 60
  static const int EVICTOR_SIZE = 50 * 10000;
//  map<int, RecommendPtr> map_;
//  SizedEvictor<int, RecommendPtr> evictor_;
  Evictor<int, RecommendPtr> evictor_;

  IceUtil::RWRecMutex mutex_;
};

} // end namespace ad
} // end namespace xce

#endif /* AD_RECOMMEND_CACHE_H_ */
