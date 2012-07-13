/*
 * AdCache.h
 *
 *  Created on: Jun 25, 2010
 *      Author: yejingwei
 */

#ifndef ADCACHE_H_
#define ADCACHE_H_

#include <AdNotify.h>
#include <map>
#include <vector>
#include <time.h>
#include <set>
#include <string>
#include <bitset>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <IceUtil/Mutex.h>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>
#include "TaskManager.h"

namespace xce {
namespace ad {

using namespace MyUtil;
using namespace std;
using namespace boost::multi_index;

const size_t kCacheSize = 20000;
const size_t kCache = 100;
const size_t kBitSetSize = 4UL * 1024 * 1024 * 1024;
//------------------------------------------------------------------------
class UserInfo: public IceUtil::Shared {
  friend class AdCache;
public:
  UserInfo(int uid) :
    uid_(uid) {
  }
  void WriteMessages();
  void UpdateDBTime(time_t);
  void Block(int ad);
  void DelAdsInUsers(const set<int> & ads);
private:

  string Formatter(const vector<int> & ids);
  int uid_;
  Interval config_;
//  int unread_count_;
  time_t update_count_time_;
  set<int> received_ads_; //接受到的ad，包括已读和未读的
  set<int> unread_ads_;
  set<int> blocked_ads_;
};

typedef IceUtil::Handle<UserInfo> UserInfoPtr;

//------------------------------------------------------------------------
struct AdConfig : public IceUtil::Shared {
  set<int> partners;
  int level_;
  time_t start_time_;
  time_t end_time_;
  bool block_always_;
  int min_partners_;
  int max_daily_post_;
};

typedef IceUtil::Handle<AdConfig> AdConfigPtr;
//------------------------------------------------------------------------
class PreloadTask: public Task { //load全部的promotion，服务启动时调用
  friend class AdCache;
public:
  PreloadTask() {
  }
  virtual void handle();
};

//------------------------------------------------------------------------
class LoadOneAdTask: public Task { //load一个promotion，新增promotion时调用
  friend class AdCache;
public:
  LoadOneAdTask(int ad):ad_(ad) {
  }
  virtual void handle();
private:
  int ad_;
};
//------------------------------------------------------------------------


class AdCache: public IceUtil::Shared {
  friend class UserInfo;
  friend class ScanTask;
  friend class CacheManager;
  friend class PreloadTask;
  friend class LoadOneAdTask;
public:
  static void Init();

  void Login(int uid);

//  void DelOneAd(int ad);
  void SetInterval(int uid, Interval interval);
  void MarkReadByUid(int uid);
  void Block(int uid, int ad);// 用户uid屏蔽ad

  void DelReceivedAdsInUserInfos(const set<int>& ads);
  static void DelReceivedAdsInDB(const set<int>& ads);
  static void DelAdConfigs(const set<int>& ads);

  static set<int> GetNotBlockAlwaysAds(const set<int> & ads);
  void DelNotBlockAlwaysAdsInUserInfos(const set<int> & ads);
  static void DelNotBlockAlwaysAdsInDB(const set<int> & ads);

  static void LoadOneAd(int ad);
  static void PreloadAds();
  static void ClearAdDailyPostedNum();
  static void GetReachMaxPostAd(set<int> & ad_set);
private:
  UserInfoPtr FindCache(int uid);
  UserInfoPtr FindDB(int uid);
  void InsertCache(UserInfoPtr);
  void Kick();

  static bool CheckPostedInOneDay(int ad, int max_post, set<int>& ads_set);
  static void UpdatePostedNum(map<int, pair<int, vector<int> > >& ad_infos);

  typedef boost::multi_index_container<UserInfoPtr, indexed_by<sequenced<> ,
      ordered_unique<BOOST_MULTI_INDEX_MEMBER(UserInfo, int,uid_)
      > > > UserInfoContainer;
  typedef UserInfoContainer::nth_index<0>::type SeqIndex;
  typedef UserInfoContainer::nth_index<1>::type UidIndex;
  typedef UserInfoContainer::iterator iterator;

  UserInfoContainer container_;

  IceUtil::Mutex mutex_;

  static map<int, AdConfigPtr> ad_config_;
  static IceUtil::RWRecMutex ad_config_mutex_;

  static map<int, int> ad_posted_num_;//每个ad的日发送量记录
  static IceUtil::RWRecMutex ad_posted_num_mutex_;
  static set<int> ad_reach_max_post_;//达到日最高发送量的ad

  static bitset<kBitSetSize> bitset_interval_;//如果设定间隔为周或月，则设为1，立即接受为0

  static bool preload_done_flag_;//表示load结束
};

typedef IceUtil::Handle<AdCache> AdCachePtr;

//----------------------------------------------------------------------------
class CacheManager: public Singleton<CacheManager> {
  friend class AdCache;
  friend class ScanTask;
  friend class PreloadTask;
public:
  void Init();

  void Login(int uid);
  void LoadOneAd(int ad);
  void PreloadAds();
  void SetInterval(int uid, Interval interval);
  void MarkReadByUid(int uid);
  void Block(int uid, int ad);// 用户uid屏蔽ad
  void DelAds(const set<int> & ads);
  void ClearAdDailyPostedNum();
  void GetReachMaxPostAd(set<int> & ad_set);
private:
  vector<int> GetNotBlockAlwaysAds();
  void DelAdConfigs(int ad);//
  //  vector<AdCache> caches_;
  AdCache caches_[kCache];
  map<int, AdConfigPtr> paused_ad_map_;
};
//----------------------------------------------------------------------------

class ScanTask: public Timer {
public:
  ScanTask() :
    Timer(1000 * 60 * 2) { //半小时执行一次
  }
  virtual void handle();
};
//----------------------------------------------------------------------------

class ClearAdDailyPostedNumTask: public Timer {
public:
	ClearAdDailyPostedNumTask(int delay) :
      Timer(delay) {
  }
  virtual void handle();
  static int DelayToXClock(int clock);//到最近的某个整点的毫秒数
};

}
}

#endif /* ADCACHE_H_ */
