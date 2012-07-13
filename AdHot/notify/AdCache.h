#ifndef ADCACHE_H_
#define ADCACHE_H_

#include "AdNotify.h"

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include "TaskManager.h"

#include <ext/hash_set>

namespace std {
  using __gnu_cxx::hash_map;
  using __gnu_cxx::hash_set;
  
  using __gnu_cxx::hash;
}

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace std;
using namespace boost::multi_index;

class ReceivedCountCache : public Singleton<ReceivedCountCache> {
public:
  void IncWeeklyCount(Ice::Int uid) {
    IceUtil::RWRecMutex::WLock lock(weekly_mutex_);  // TODO : read lock, 容忍误差
    hash_map<Ice::Int, Ice::Int>::iterator it = weekly_map_.find(uid);
    if (it == weekly_map_.end()) {
      weekly_map_[uid] = 1;
    } else {
      ++(it->second);
    }
  }

  void DecWeeklyCount(Ice::Int uid) {
    IceUtil::RWRecMutex::WLock lock(weekly_mutex_);  // TODO : read lock, 容忍误差
    hash_map<Ice::Int, Ice::Int>::iterator it = weekly_map_.find(uid);
    if (it != weekly_map_.end() && it->second > 0) {
      --(it->second);
    }
  }

  Ice::Int GetWeeklyCount(Ice::Int uid) const {
    IceUtil::RWRecMutex::RLock lock(weekly_mutex_);
    hash_map<Ice::Int, Ice::Int>::const_iterator it = weekly_map_.find(uid);
    if (it != weekly_map_.end()) {
      return it->second;
    }
    return 0;
  }

  void SetWeeklyCount(Ice::Int uid, int count) {
    IceUtil::RWRecMutex::WLock lock(weekly_mutex_);
    weekly_map_[uid] = count;
  }

  void ClearWeeklyCount() {
    hash_map<Ice::Int, Ice::Int> empty;
    IceUtil::RWRecMutex::WLock lock(weekly_mutex_);
    weekly_map_.swap(empty);
  }

private:
  IceUtil::RWRecMutex weekly_mutex_;
  hash_map<Ice::Int, Ice::Int> weekly_map_;
};

struct AdConfig : public IceUtil::Shared {
  AdConfig(int ad_id) : id_(ad_id), daily_posted_(0) {}
  bool UnderDailyLimit() {
    if (max_daily_post_ == 0 ) {
      return true;
    }
    return max_daily_post_ < daily_posted_;
  }
  bool Expired() {
    return end_time_ < time(NULL);
  }

  int id_;
  int level_;
  time_t start_time_;
  time_t end_time_;
  int min_partners_;
  int max_daily_post_;
  int daily_posted_; // 当天已经发送量
  set<int> partners_;
};
typedef IceUtil::Handle<AdConfig> AdConfigPtr;

class AdReceivedCache : public Singleton<AdReceivedCache> {
public:
  void Add(int uid, int ad_id);
  void Cancel(int uid, int ad_id);
  bool HasReceived(int uid, int ad_id) const;
private:
  map<int, set<int> > received_users_;
  mutable IceUtil::RWRecMutex received_mutex_;
};

class AdBlockedCache : public Singleton<AdBlockedCache> {
public:
  void Load(int ad_id);
  void Block(int uid, int ad_id);
  void Unblock(int uid, int ad_id);
  bool IsBlocked(int uid, int ad_id) const;
private:
  map<int, set<int> > blocked_users_;
  mutable IceUtil::RWRecMutex blocked_mutex_;
};

// 记录各个 ad 所有未读的用户, 同步 home hot count.
class AdUnreadCache : public Singleton<AdUnreadCache> {
public:
  void MarkRead(int uid, int ad_id);

  void AddUnread(int uid, int ad_id);

  int GetUnreadCount(int uid);
  void RemoveAd(int ad_id);
private:
  int GetUnreadCountNoLock(int uid);
  void UpdateHomeHotCount(int uid, int count);
  map<int, set<int> > unread_users_;
  IceUtil::RWRecMutex unread_mutex_;
  // map<int, IceUtil::RWRecMutex> blocked_mutex_;
};

// load全部的promotion
class ReloadTask : public Task {
public:
  ReloadTask() {}
  virtual void handle();
};

class CacheManager : public Singleton<CacheManager> {
public:
  enum State{READY, LOADING, REMOVING};
  CacheManager() : state_(READY) {} 
  bool Init();
  void Reload();

  void Login(int uid);
  AdConfigPtr LoadOneAd(int ad);
  void MarkReadByUid(int uid);
  void Block(int uid, int ad); // 用户uid屏蔽ad
  void DelAd(int ad_id);
  void ClearDailyPosted();

  void GetToPostAds(int uid, int quota_left, map<int, pair<int, vector<int> > > * to_post_ads);

  HotAdInfoSeq GetAdInfoSeq();
  void RemoveExpired();

  // void Unblock(int uid, int ad_id);

  bool ResetUserAd(Ice::Int uid, Ice::Int ad_id);
  UserHotAdState GetUserAdState(Ice::Int uid, Ice::Int ad_id);

  void set_state(State s) {
    state_ = s;
  }
  State state() {
    return state_;
  }

private:
  State state_;   // 广告是否ready
  map<int, AdConfigPtr> ad_config_map_; // 广告信息
  IceUtil::RWRecMutex ad_config_mutex_;
};

class ScanTask: public Timer {
public:
  ScanTask() : Timer(1000 * 60 * 2) {} //半小时执行一次
  virtual void handle();
};

class ClearAdDailyPostedNumTask: public Timer {
public:
	ClearAdDailyPostedNumTask(int delay) : Timer(delay) {}
  virtual void handle();
  static int DelayToXClock(int clock); // 到最近的某个整点的毫秒数
};

class ClearUserWeeklyReceivedTask: public Timer {
public:
	ClearUserWeeklyReceivedTask(int delay) : Timer(delay) {}
  virtual void handle();
};

}
}

#endif // ADCACHE_H_
