
#ifndef __FEEDITEMCACHE_BYAPPID_ADAPTER_H_
#define __FEEDITEMCACHE_BYAPPID_ADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "UserFeeds.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedItemCacheByAppidAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedItemCacheByAppidAdapter> {
public:
  FeedItemCacheByAppidAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  void put(const FeedMeta& d);
  FeedMetaSeq get(const IntSeq& uids, int begin, int limit);
  FeedMetaSeq getByTypes(const IntSeq& uids, int begin, int limit, const IntSeq& types);
  FeedMetaSeq getWithFilter(const IntSeq& uids, int begin, int limit, const StrSeq& filter);
  FeedMetaSeq getByTypesWithFilter(const IntSeq& uids, int begin, int limit, const IntSeq& types, const StrSeq& filter);

  UserFeedsPrx getManager(int id);
  UserFeedsPrx getManagerOneway(int id);

protected:
  virtual string name() {
    return "FeedItemCache";
  }
  virtual string endpoints() {
    return "@FeedItemCache";
  }
  virtual size_t cluster() {
    return 1;
  }

  vector<UserFeedsPrx> _managersOneway;
  vector<UserFeedsPrx> _managers;
};

}
;
}
;
#endif /* __FEEDFOCUSADAPTER_H_ */
