/*
 * LowQualityAdFilterHandler.h
 *
 *  Created on: 2011-8-25
 *      Author: sunzz
 */

#ifndef LOWQUALITYADFILTERHANDLER_H_
#define LOWQUALITYADFILTERHANDLER_H_
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/mem_fun.hpp>

#include <IceUtil/Shared.h>
#include <vector>
#include <set>
#include <map>
#include "AdInvertedIndex.h"
#include "AdStruct.h"

#include "MatchHandlers.h"

using boost::multi_index_container;
using namespace boost::multi_index;

namespace xce {
namespace ad {

class AdMatchHandler;
class LowQualityAdFilterHandler : public AdMatchHandler {
public:
  virtual bool handle(MatchHandlerParamter& para, AdResultMap& result);
  AdGroupPtr GetDefaultTaobao(MatchHandlerParamter& para);
  AdGroupPtr GetDefaultAd(set<SelectedAdGroupPtr>& selected_groups,
      vector<AdGroupPtr>& default_groups);
};

class LowQualityAdCache : public MyUtil::Singleton<LowQualityAdCache> {
public:

  void LoadGidsEcpmLevelPool();
  int GetAdEcpmLevel(Ice::Long group_id);

private:
  map<Ice::Long, int> gids_ecpm_level_;
  IceUtil::RWRecMutex mutex_;
};

class LowQualityAdCacheTimer : public MyUtil::Timer {
public:
  LowQualityAdCacheTimer() :
    Timer(2 * 60 * 1000) { //2分钟重新计算一次
  }
  virtual void handle() {
    MCE_INFO("LowQualityAdCacheTimer start");
    LowQualityAdCache::instance().LoadGidsEcpmLevelPool();
    MCE_INFO("LowQualityAdCacheTimer stop");
  }
};


}
}

#endif /* LOWQUALITYADFILTERHANDLER_H_ */
