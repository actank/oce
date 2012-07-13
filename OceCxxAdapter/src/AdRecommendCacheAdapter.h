#ifndef AD_RECOMMEND_CACHE_ADAPTER_H_
#define AD_RECOMMEND_CACHE_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "AdEngine.h"

namespace xce {
namespace ad {

using namespace MyUtil;

class AdRecommendCacheAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::AdChannel, AdRecommendCacheAdapter> {
public:
  AdRecommendCacheAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }
  bool Insert(int server_index, int user_id, AdResult ad_result);

private:
  AdRecommendCachePrx getManager(int id);
  AdRecommendCachePrx getManagerOneway(int id);

  virtual string name() {
    return "R";
  }
  virtual string endpoints() {
    return "@AdEngineS";
  }
  virtual size_t cluster() {
    return 10; // 10
  }

  vector<AdRecommendCachePrx> _managersOneway;
  vector<AdRecommendCachePrx> _managers;
};

}
}


#endif // AD_RECOMMEND_CACHE_ADAPTER_H_
