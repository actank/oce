#ifndef _USER_RANK_HANDLER_H_
#define _USER_RANK_HANDLER_H_

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
#include "AdInvertedIndex.h"
#include "AdStruct.h"
#include "MatchHandlers.h"

using boost::multi_index_container;
using namespace boost::multi_index;

namespace xce {
namespace ad {

class AdMatchHandler;

const static int kCacheClusterCount = 20;
const static size_t kPerCacheSize = 1000;

struct AdItem : public IceUtil::Shared {
  AdItem(Ice::Long group_id, int max_price, double quality, double score) 
    : group_id_(group_id)
    , max_price_(max_price)
    , quality_(quality)
    , score_(score) {
  }

  Ice::Long group_id_;
  int max_price_;
  double quality_;
  double score_;
};

typedef IceUtil::Handle<AdItem> AdItemPtr;
typedef vector<AdItemPtr> AdItemSeq;

class AdItemList : public IceUtil::Shared {
public:
  AdItemList(const UserProfile & user_profile, Ice::Long zone_id, const set<AdGroupPtr> & groups) 
      : zone_id_(zone_id), update_time_(time(NULL)), total_score_(0.0) {
    InitScoreByThreshosld(user_profile, groups);
  }
  virtual SelectedAdGroupSeq Select(MatchHandlerParamter& para, int uid, int count);
  bool IsStale() {
    return time(NULL) - update_time_ > 5 * 60;
  }
private:
  void InitScoreByThreshosld(const UserProfile & user_profile, const set<AdGroupPtr> & groups);
  double FilterCompete(int uid, AdGroupPtr group, set<Ice::Long> * evaluated_ids);
  double TotalScore(const set<long> & evaluated_ids) {
    // if (total_score_ > 0.0) {
    //   return total_score_;
    // }

    GidIndex & gid_index = items_.get<0>();
    GidIndex::const_iterator it = gid_index.begin();
    double total = 0.0;
    for(; it != gid_index.end(); ++it) {
      if (evaluated_ids.find((*it)->group_id_) == evaluated_ids.end()) {
        total += (*it)->score_;
      }
    }
    total_score_ = total;
    return total;
  }
  
  typedef multi_index_container<
      AdItemPtr,
      indexed_by<
          hashed_unique<member<AdItem, Ice::Long, &AdItem::group_id_> >,
          ordered_non_unique<member<AdItem, double, &AdItem::score_> >,
          ordered_non_unique<member<AdItem, int, &AdItem::max_price_> >
          // ordered_non_unique<member<AdItem, double, &AdItem::quality_> >
      >
  > ItemContainer;
  typedef ItemContainer::nth_index<0>::type GidIndex;
  typedef ItemContainer::nth_index<1>::type ScoreIndex;
  typedef ItemContainer::nth_index<2>::type PriceIndex;
  // typedef ItemContainer::nth_index<4>::type QualityIndex;

  ItemContainer items_;
  Ice::Long zone_id_;
  int update_time_;
  double total_score_;
};
typedef IceUtil::Handle<AdItemList> AdItemListPtr;

class UserIndex : public IceUtil::Shared {
public:
  UserIndex(int uid) : uid_(uid) {}
  int uid() const { return uid_; }
  SelectedAdGroupSeq GetZoneGroups(MatchHandlerParamter& para, long adzone, int count);
private:
  int uid_;
  map<long, AdItemListPtr> user_zone_items_;
  IceUtil::Mutex user_mutex_;
};
typedef IceUtil::Handle<UserIndex> UserIndexPtr;

class UserIndexCache : public Singleton<UserIndexCache> {
public:
  UserIndexPtr GetUserIndex(int uid) {
    int cluster_index = ClusterIndex(uid);

    IceUtil::Mutex::Lock lock(mutex_[cluster_index]);
    UserIndexContainer & cache = index_cache_[cluster_index];
    UidIndex & uid_idx = cache.get<1>();
    UidIndex::iterator it = uid_idx.find(uid);
    if (it != uid_idx.end()) {
      SeqIndex & seq_index = cache.get<0>();
      seq_index.relocate(seq_index.begin(), cache.project<0>(it));
      return *it;
    }

    UserIndexPtr user_index = new UserIndex(uid);
    if (cache.size() > kPerCacheSize) {
      // MCE_DEBUG("user " << cache.back()->uid() << " evicted from UserIndexCache " << cluster_index)
      cache.pop_back();
    }
    cache.push_front(user_index);
    // MCE_DEBUG("user " << uid << " pushed into UserIndexCache " << cluster_index << ". size=" << cache.size())
    return user_index;
  }
private:
  int ClusterIndex(int uid) {
    return uid / 100 % kCacheClusterCount;
  }
  typedef multi_index_container<
      UserIndexPtr,
      indexed_by<
          sequenced<>,
          hashed_unique<const_mem_fun<UserIndex, int, &UserIndex::uid> >
      >
  > UserIndexContainer;

  typedef UserIndexContainer::nth_index<0>::type SeqIndex;
  typedef UserIndexContainer::nth_index<1>::type UidIndex;

  UserIndexContainer index_cache_[kCacheClusterCount];
  IceUtil::Mutex mutex_[kCacheClusterCount];
};

class UserAdMatcher : public MyUtil::Singleton<UserAdMatcher> {
public:
  set<AdGroupPtr> GetAds(UserProfile& profile, Ice::Long adzone);
};

class UserRankHandler : public AdMatchHandler {
public:
  virtual bool handle(MatchHandlerParamter& para, AdResultMap& result);
};

}
}

#endif // _USER_RANK_HANDLER_H_
