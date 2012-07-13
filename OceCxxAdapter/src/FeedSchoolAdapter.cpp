#include <cmath>
#include "FeedSchoolAdapter.h"

using namespace MyUtil;
using namespace xce::feed;

FeedItemManagerPrx FeedSchoolAdapter::getManager(int id) {
  return locate<FeedItemManagerPrx> (_managers, "FIM", id, TWO_WAY);
}

FeedItemManagerPrx FeedSchoolAdapter::getManagerOneway(int id) {
  return locate<FeedItemManagerPrx> (_managers, "FIM", id, ONE_WAY);
}

void FeedSchoolAdapter::addFeed(const FeedItem& item,
    const FeedConfigPtr& config, const MyUtil::Int2IntMap& uid2weight) {
//  map<int, MyUtil::Int2IntMap> tmap;
//  for (MyUtil::Int2IntMap::const_iterator it = uid2weight.begin(); it
//      != uid2weight.end(); ++it) {
//    tmap[abs(it->first) % _cluster][it->first] = it->second;
//  }
//  for (map<int, MyUtil::Int2IntMap>::iterator it = tmap.begin(); it
//      != tmap.end(); ++it) {
//    getManagerOneway(it->first)->addFeed(item, config, it->second);
//  }
  for (MyUtil::Int2IntMap::const_iterator it = uid2weight.begin(); it
      != uid2weight.end(); ++it) {
    getManagerOneway(it->first)->addFeed(item, config, uid2weight);
  }

}


