#ifndef _SOME_MEMBER_HANDLER_H_
#define _SOME_MEMBER_HANDLER_H_

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
#include "Date.h"

using boost::multi_index_container;
using namespace boost::multi_index;

namespace xce {
namespace ad {

class AdMatchHandler;

const static int kMCacheClusterCount = 20;
const static size_t kMPerCacheSize = 1000;

class SomeMemberItem : public IceUtil::Shared {
public:
  SomeMemberItem(Ice::Long member_id, int pv_limit) 
    : member_id_(member_id)
  , pv_limit_(pv_limit)
  , pv_(0){
  }
  void IncPv() {
    pv_++;
  }
  bool IsOverLimit() {
    return pv_ >= pv_limit_;
  }

  Ice::Long member_id_;
  int pv_limit_;
  int pv_;
};
typedef IceUtil::Handle<SomeMemberItem> SomeMemberItemPtr;

class SomeMemberPool : public MyUtil::Singleton<SomeMemberPool>, virtual public MyUtil::Timer {
public:
  SomeMemberPool() : Timer(RELOAD_INTERVAL) {
    reload();
  }
  void IncPv(Ice::Long mid) {
    IceUtil::Mutex mutex_;
    map<Ice::Long, SomeMemberItemPtr>::iterator it = pool_.find(mid);
    if (it != pool_.end()) {
      it->second->IncPv();
    }
  }
  bool IsAvailable(Ice::Long mid) {
    IceUtil::Mutex mutex_;
    map<Ice::Long, SomeMemberItemPtr>::iterator it = pool_.find(mid);
    if (it != pool_.end() && !it->second->IsOverLimit()) {
      return true;
    }
    return false;
  }

  int GetPvLimit(Ice::Long mid) {
    IceUtil::Mutex mutex_;
    map<Ice::Long, SomeMemberItemPtr>::iterator it = pool_.find(mid);
    if (it != pool_.end()) {
      return it->second->pv_limit_;
    }
  }

  void reload() {
    MCE_INFO("SomeMemberPool reload start");
    string file_name = "../etc/some_member";
    try {
      ifstream fin(file_name.c_str());
      string line;
      set<int> new_mids;
      if (fin) {
        while (getline(fin, line)) {
          MCE_INFO("SomeMemberPool reload " << line);
          vector<string> para_strs;
          boost::algorithm::split_regex( para_strs, line,regex( "\\," ) ) ;
          if (para_strs.size() >= 2) {
            Ice::Long mid = boost::lexical_cast<Ice::Long>(para_strs[0]);
            int pv_limit = boost::lexical_cast<Ice::Long>(para_strs[1]);
            new_mids.insert(mid);
            map<Ice::Long, SomeMemberItemPtr>::iterator it = pool_.find(mid);
            if (it != pool_.end()) {
              it->second->member_id_ = mid;
              it->second->pv_limit_ = pv_limit;
            } else {
              SomeMemberItemPtr mi = new SomeMemberItem(mid, pv_limit / 10);  // 有10个服务，所以平分成十份
              pool_[mid] = mi;
            }
          }
        }
      }
      int sec = time(NULL) % (24 * 60 * 60);
      MCE_INFO("SomeMemberPool reload sec = " << sec);
      if (sec < (RELOAD_INTERVAL / 1000)) {
        MCE_INFO("SomeMemberPool reload sec = " << sec << ", need clear pv");
        for(map<Ice::Long, SomeMemberItemPtr>::iterator it = pool_.begin(); it != pool_.end(); it++) {
          it->second->pv_ = 0;
        }
      }
      for(map<Ice::Long, SomeMemberItemPtr>::iterator it = pool_.begin(); it != pool_.end();) {
        if (new_mids.find(it->first) == new_mids.end()) {
          pool_.erase(it++);
        } else {
          MCE_INFO("SomeMember reload mid=" << it->second->member_id_ << ",pv limit=" << it->second->pv_limit_ << ",pv=" << it->second->pv_);
          it++;
        }
      }
    } catch (Ice::Exception& e) {
      MCE_WARN("SomeMember reload error : " << e);
    } catch (...) {
      MCE_WARN("SomeMember reload error : unkown error \"...\"");
    }
    MCE_INFO("SomeMemberPool reload end");
  }

  virtual void handle() {
    reload();
  }
private:
  static const int RELOAD_INTERVAL = 60 * 1000;
  map<Ice::Long, SomeMemberItemPtr> pool_;
  IceUtil::Mutex user_mutex_;
};


struct MAdItem : public IceUtil::Shared {
  MAdItem(Ice::Long group_id, int max_price, double score) 
    : group_id_(group_id)
    , max_price_(max_price)
    , score_(score) {
  }

  Ice::Long group_id_;
  int max_price_;
  double score_;
};

typedef IceUtil::Handle<MAdItem> MAdItemPtr;
typedef vector<MAdItemPtr> MAdItemSeq;

class MAdItemList : public IceUtil::Shared {
public:
  MAdItemList(const UserProfile & user_profile, Ice::Long zone_id, const set<AdGroupPtr> & groups) 
      : zone_id_(zone_id), update_time_(time(NULL)), total_score_(0.0) {
    InitScoreByThreshosld(user_profile, groups);
  }
  virtual SelectedAdGroupSeq Select(int uid, int count);
  bool IsStale() {
    return time(NULL) - update_time_ > 5 * 60;
  }
private:
  void InitScoreByThreshosld(const UserProfile & user_profile, const set<AdGroupPtr> & groups);
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
      MAdItemPtr,
      indexed_by<
          hashed_unique<member<MAdItem, Ice::Long, &MAdItem::group_id_> >,
          ordered_non_unique<member<MAdItem, double, &MAdItem::score_> >,
          ordered_non_unique<member<MAdItem, int, &MAdItem::max_price_> >
      >
  > ItemContainer;
  typedef ItemContainer::nth_index<0>::type GidIndex;
  typedef ItemContainer::nth_index<1>::type ScoreIndex;
  typedef ItemContainer::nth_index<2>::type PriceIndex;

  ItemContainer items_;
  Ice::Long zone_id_;
  int update_time_;
  double total_score_;
};
typedef IceUtil::Handle<MAdItemList> MAdItemListPtr;

class MUserIndex : public IceUtil::Shared {
public:
  MUserIndex(int uid) : uid_(uid) {}
  int uid() const { return uid_; }
  SelectedAdGroupSeq GetZoneGroups(const UserProfile & user_profile, long adzone, int count);
private:
  int uid_;
  map<long, MAdItemListPtr> user_zone_items_;
  IceUtil::Mutex user_mutex_;
};
typedef IceUtil::Handle<MUserIndex> MUserIndexPtr;

class MUserIndexCache : public Singleton<MUserIndexCache> {
public:
  MUserIndexPtr GetMUserIndex(int uid) {
    int cluster_index = ClusterIndex(uid);

    IceUtil::Mutex::Lock lock(mutex_[cluster_index]);
    MUserIndexContainer & cache = index_cache_[cluster_index];
    UidIndex & uid_idx = cache.get<1>();
    UidIndex::iterator it = uid_idx.find(uid);
    if (it != uid_idx.end()) {
      SeqIndex & seq_index = cache.get<0>();
      seq_index.relocate(seq_index.begin(), cache.project<0>(it));
      return *it;
    }

    MUserIndexPtr user_index = new MUserIndex(uid);
    if (cache.size() > kMPerCacheSize) {
      // MCE_DEBUG("user " << cache.back()->uid() << " evicted from UserIndexCache " << cluster_index)
      cache.pop_back();
    }
    cache.push_front(user_index);
    // MCE_DEBUG("user " << uid << " pushed into UserIndexCache " << cluster_index << ". size=" << cache.size())
    return user_index;
  }
private:
  int ClusterIndex(int uid) {
    return uid / 100 % kMCacheClusterCount;
  }
  typedef multi_index_container<
      MUserIndexPtr,
      indexed_by<
          sequenced<>,
          hashed_unique<const_mem_fun<MUserIndex, int, &MUserIndex::uid> >
      >
  > MUserIndexContainer;

  typedef MUserIndexContainer::nth_index<0>::type SeqIndex;
  typedef MUserIndexContainer::nth_index<1>::type UidIndex;

  MUserIndexContainer index_cache_[kMCacheClusterCount];
  IceUtil::Mutex mutex_[kMCacheClusterCount];
};

class MUserAdMatcher : public MyUtil::Singleton<MUserAdMatcher> {
public:
  set<AdGroupPtr> GetAds(UserProfile& profile, Ice::Long adzone);
};

class SomeMemberHandler : public AdMatchHandler {
public:
  virtual bool handle(MatchHandlerParamter& para, AdResultMap& result);
};

}
}

#endif // _SOME_MEMBER_HANDLER_H_
