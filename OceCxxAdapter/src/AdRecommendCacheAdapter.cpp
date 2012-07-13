#include "AdRecommendCacheAdapter.h"
#include <boost/lexical_cast.hpp>
#include "FeedMemcProxy/client/user_profile_client.h"
using namespace MyUtil;
using namespace xce::ad;
using namespace boost;

AdRecommendCachePrx AdRecommendCacheAdapter::getManager(int id) {
  return locate<AdRecommendCachePrx> (_managers, "R", id, TWO_WAY, 100);
}

AdRecommendCachePrx AdRecommendCacheAdapter::getManagerOneway(int id) {
  return locate<AdRecommendCachePrx> (_managersOneway, "R", id, ONE_WAY);
}

bool AdRecommendCacheAdapter::Insert(int server_index, int user_id, AdResult ad_result) {
  return getManager(server_index)->Insert(user_id, ad_result);
}



