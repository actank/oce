/*
 * AdRecommendCache.cpp
 *
 *  Created on: Dec 2, 2010
 *      Author: zhaohui.tang
 */

#include "AdRecommendCache.h"
#include "AdCache.h"

namespace xce {
namespace ad {

pair<AdResult, bool> AdRecommendCache::GetAdResult(int user_id) {
  bool is_exist = true;
  AdResult result;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  RecommendPtr recommend = evictor_.locate(user_id, NULL);
  if (recommend) {
    result = recommend->ad_result();
    if (result.empty() && (recommend->GetDuration() > EMPTY_PRESERVATION_TIME)) {
      is_exist = false;
      result = AdResult();

    } else if (!result.empty()
        && (recommend->GetDuration() > PRESERVATION_TIME)) {
      is_exist = false;
    //  result = AdResult();
      result = recommend->ad_result();//即使失效也推荐
    }
  } else {
    is_exist = false;
  }
  return pair<AdResult, bool>(result, is_exist);
}
void AdRecommendCache::insert(int user_id, RecommendPtr rp) {
  IceUtil::RWRecMutex::WLock lock(mutex_);
//  map_[user_id] = rp;
  evictor_.add(rp, user_id);
  MCE_DEBUG("AdRecommendCache::insert-->evictor_ size = " << evictor_.size());
}
vector<int> AdRecommendCache::GetInvalidList() {
  vector<int> result;
  IceUtil::RWRecMutex::RLock lock(mutex_);
  Evictoriter it = evictor_.begin();

  for(; it != evictor_.end(); ++it) {
    pair<int, RecommendPtr> recom = evictor_.getObject(it);
    if(recom.second) {
      if((recom.second)->GetDuration() > INVALID_TIME) {
        if(recom.first > 0) {
          result.push_back(recom.first);
        }

      }
    }
  }
  MCE_DEBUG("AdRecommendCache::GetInvalidList-->evict size" << evictor_.size() << "invalid size:" << result.size());
  return result;
}

} // end namespace ad
} // end namespace xce
