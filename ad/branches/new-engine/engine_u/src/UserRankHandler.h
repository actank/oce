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
#include "AdDemoQualityAdapter.h"
#include "AdDemoQuality.h"
using boost::multi_index_container;
using namespace boost::multi_index;

namespace xce {
namespace ad {

class AdMatchHandler;

const static int kCacheClusterCount = 20;
const static size_t kPerCacheSize = 500;

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
  AdItemList() 
      : total_score_(0.0) {
  }
  AdGroupPtr Select(int uid, set<Ice::Long> & evaluated_ids, double &score);
  AdGroupPtr Select2(int uid, set<Ice::Long> & evaluated_ids);
  void Insert(AdItemPtr item);
private:
  double FilterCompete(int uid, AdGroupPtr group, set<Ice::Long> * evaluated_ids);
  double TotalScore() {
    if (total_score_ > 0.0) {
      return total_score_;
    }

    GidIndex & gid_index = items_.get<0>();
    GidIndex::const_iterator it = gid_index.begin();
    double total = 0.0;
    for(; it != gid_index.end(); ++it) {
      total += (*it)->score_;
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
      >
  > ItemContainer;
  typedef ItemContainer::nth_index<0>::type GidIndex;
  typedef ItemContainer::nth_index<1>::type ScoreIndex;
  typedef ItemContainer::nth_index<2>::type PriceIndex;

  ItemContainer items_;
  //Ice::Long zone_id_;
  double total_score_;
};
typedef IceUtil::Handle<AdItemList> AdItemListPtr;

struct MemberItem : public IceUtil::Shared {
  MemberItem(Ice::Long member_id) 
    : member_id_(member_id), score_(-1.0), ad_count_(0) {
    ad_items_ = new AdItemList(); 
  }

  void Add(AdItemPtr ad_item);
  AdItemListPtr ad_items_;
  Ice::Long member_id_;
  double score_;
  //map<long, AdItemListPtr> zone_items_;
  int ad_count_;
  IceUtil::Mutex m_mutex_;
};

typedef IceUtil::Handle<MemberItem> MemberItemPtr;

class MemberItemList : public IceUtil::Shared {
public:
  MemberItemList(const UserProfile & user_profile, Ice::Long zone_id, const set<AdGroupPtr> & groups) 
      : zone_id_(zone_id), update_time_(time(NULL)), total_score_(0.0), ad_count_(0) {
    zone_ = AdZonePool::instance().GetAdZone(zone_id_);
    //InitScore(user_profile, groups);
  }
  virtual SelectedAdGroupSeq Select(int uid, int count);
  bool IsStale() {
    return time(NULL) - update_time_ > 5 * 60;
  }
private:
  //void InitScore(const UserProfile & user_profile, const set<AdGroupPtr> & groups);
  double FilterCompete(int uid, AdGroupPtr group, set<Ice::Long> * evaluated_ids);
  double TotalScore() {
    if (total_score_ > 0.0) {
      return total_score_;
    }
    double total = 0.0;
    int ad_count = 0;

    for (map<Ice::Long, MemberItemPtr>::iterator it = items_.begin(); it != items_.end(); it++) {
      total += it->second->score_;
    }
    total_score_ = total;
    return total;
  }
  void Insert(Ice::Long mid, AdItemPtr ad_item);
  map<Ice::Long, MemberItemPtr> items_;
  Ice::Long zone_id_;
  AdZonePtr zone_;
  int update_time_;
  double total_score_;
  int ad_count_; // 广告总数
};
typedef IceUtil::Handle<MemberItemList> MemberItemListPtr;

class UserIndex : public IceUtil::Shared {
public:
  UserIndex(int uid) : uid_(uid) {}
  int uid() const { return uid_; }
  SelectedAdGroupSeq GetZoneGroups(const UserProfile & user_profile, long adzone, int count);
  //SelectedAdGroupSeq GetZoneGroups2(const UserProfile & user_profile, long adzone, int count);
  SelectedAdGroupSeq GetZoneGroups3(const MatchHandlerParamter& para, long adzone, int count);
  AdGroupPtr GetZonePosFreshGroup(int userid, FreshAdQualityMap & fresh_groups, set<AdGroupPtr>& bid_ids, set<AdGroupPtr>& selected_ids, long adzone, int pos, bool is_reserved);
  bool IsCompetedGroup(AdGroupPtr candidate_group, set<AdGroupPtr> & selected_ids);
  string GetFreshKey(Ice::Long zone_id, int pos);
private:
  int uid_;
  map<long, MemberItemListPtr> user_zone_items_;
  map<long, SelectedAdGroupSeq> zone_groups_;
  IceUtil::Mutex zone_groups_mutex_;
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

class FreshAdGroupsCache : public MyUtil::Singleton<FreshAdGroupsCache> {
public:
  FreshAdQualityMap GetFreshGroups(){
    FreshAdQualityMap result;
    {
      IceUtil::RWRecMutex::RLock lock(mutex_);    
      result = fresh_groups_;
    }
    return result;
  }
  void Load() {
    MCE_INFO("[Fresh:],FreshAdGroupsCache::Load start");
    FreshAdQualityMap pool;
    try {
      pool = AdDemoQualityAdapter::instance().GetFreshAdMap(EngineConfig::instance().server_index());
    }catch (Ice::Exception& e) {
      MCE_WARN("FreshAdGroupsCache::Load --> call AdDemoQualityNAdapter ERR  line:"
          << __LINE__ << " err:" << e); 
    } catch (std::exception& e) {
      MCE_WARN("FreshAdGroupsCache::load --> call AdDemoQualityNAdapter ERR  line:"
          << __LINE__ << " err:" << e.what());
    }

    IceUtil::RWRecMutex::WLock lock(mutex_);
    fresh_groups_.swap(pool);
    MCE_INFO("[Fresh:]: Load stop, GetFreshAdMap size is:"  << fresh_groups_.size());          
  }
private:
  FreshAdQualityMap fresh_groups_;
  IceUtil::RWRecMutex mutex_;
};

class FreshAdGroupsCacheTimer : public MyUtil::Timer {
  public:
    FreshAdGroupsCacheTimer() :
      Timer(15 * 60 * 1000) { //10分钟重新计算一次
      }
    virtual void handle() {
      FreshAdGroupsCache::instance().Load();
    }
};
class UserRankHandler : public AdMatchHandler {
public:
  virtual bool handle(MatchHandlerParamter& para, AdResultMap& result);
};

}
}

#endif // _USER_RANK_HANDLER_H_
