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
  SelectedAdGroupSeq GetZoneGroups3(MatchHandlerParamter& para, long adzone, int count);
  AdGroupPtr GetZonePosFreshGroup(int userid, FreshAdQualityMap & fresh_groups, set<AdGroupPtr>& bid_ids, set<AdGroupPtr>& selected_ids, long adzone, int pos, bool is_reserved);
  bool IsCompetedGroup(AdGroupPtr candidate_group, set<AdGroupPtr> & selected_ids);
  string GetFreshKey(Ice::Long zone_id, int pos);
  SelectedAdGroupSeq GetZoneGroups4Reserve(MatchHandlerParamter& para, long adzone, int count);
  //vector<AdGroupPtr> RandomSelectAdgroup(set<AdGroupPtr>& bid_gids, set<AdGroupPtr>&all_selected_gids, Ice::Long zone_id ,int need_count);
  vector<pair<AdGroupPtr,int> > SelectCpmMemberAdgroup(MatchHandlerParamter& para, set<AdGroupPtr>& groups, Ice::Long adzone, int count); 
  vector<AdGroupPtr> RandomSelectAdgroup(const UserProfile & user_profile, set<AdGroupPtr>& bid_gids, set<AdGroupPtr>& all_selected_gids, Ice::Long zone_id ,int & policy_type, int need_count);
  vector<pair<AdGroupPtr,int> > SelectCpmAdgroup(set<AdGroupPtr>& groups, set<AdGroupPtr>& selected_groups,
    Ice::Long adzone,int count);
  vector<pair<AdGroupPtr,int> > RandomSelectCpmAdgroup(vector<pair<AdGroupPtr,CpmAdgroup> >& bid_gids,     
    set<AdGroupPtr>& all_selected_gids,int need_count);
  void FilterTaobao(set<AdGroupPtr>& bid_groups);
  void GameReduce(set<AdGroupPtr>& bid_groups, bool is_vip, int count, long adzone_id);


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
  void FilterCpm(set<AdGroupPtr>& bid_groups);
};

class UserRankHandler : public AdMatchHandler {
public:
  virtual bool handle(MatchHandlerParamter& para, AdResultMap& result);
};
////////////////////////////////////////////////
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
class CpmLoaderCache : public MyUtil::Singleton<CpmLoaderCache> {
public:
    CpmAdgroupMap GetCpmAdGroupMap(){
    CpmAdgroupMap result;
    {
      IceUtil::RWRecMutex::RLock lock(mutex_);    
      result = cpm_groups_;
    }
    return result;
  }
  void Load() {
    MCE_INFO("[cpm loader],CpmLoaderCache::Load start");
    CpmAdgroupMap pool;
    try {
      pool = AdDemoQualityAdapter::instance().GetCpmAdGroupMap(EngineConfig::instance().server_index());
      IceUtil::RWRecMutex::WLock lock(mutex_);
      cpm_groups_.swap(pool);
      MCE_INFO("[cpm loader]: Load stop, CpmLoaderCache size is:"  << cpm_groups_.size());          
    }catch (Ice::Exception& e) {
      MCE_WARN("CpmLoaderCache::Load --> call AdDemoQualityNAdapter CpmLoaderCache ERR  line:"
          << __LINE__ << " err:" << e); 
    } catch (std::exception& e) {
      MCE_WARN("CpmLoaderCache::load --> call AdDemoQualityNAdapter CpmLoaderCache ERR  line:"
          << __LINE__ << " err:" << e.what());
    }

  }
private:
  CpmAdgroupMap cpm_groups_;
  IceUtil::RWRecMutex mutex_;
};
class TestAdImprRatioAssignCache : public MyUtil::Singleton<TestAdImprRatioAssignCache> {
public:
  TestAdImprRatioAssignCache(): yestoday_reserve_impr_(182308123), yestoday_unreserve_impr_(329373678)
  ,today_hour_reserve_impr_(80303989),today_hour_unreserve_impr_(159975961),today_predict_reserve_impr_(18230812),
  max_test_ad_impr(40000000),min_test_ad_impr(5000000),max_unreserve_test_ratio(0.03) {
  }
  double GetTestRatio(bool is_reserve){
    double ratio = 0.0;
    if(today_predict_reserve_impr_ < min_test_ad_impr) {
      if(!is_reserve) { //非余量需要测试
        ratio = (double)(min_test_ad_impr - today_predict_reserve_impr_) / (double)yestoday_unreserve_impr_;
        ratio = min(ratio, max_unreserve_test_ratio);
        MCE_INFO("TestAdImprRatioAssignCache, very Important,the reserve has no enough impr for new_ad,need unreserve test ratio :" << max_unreserve_test_ratio);
        return ratio;
      } else {//余量需要全部测试

        MCE_INFO("TestAdImprRatioAssignCache, very Important,the reserve has no enough impr for new_ad,need reserve test ratio 1.0 ");
        return 1.0;
      }
    }
    if(!is_reserve) {//不需要非余量测试
      return 0.0;
    }
    if(today_predict_reserve_impr_ < max_test_ad_impr) { //如果测试量不够，则需要全部进行测试
      return 1.0;
    } else {
      return (double)max_test_ad_impr / (double)today_predict_reserve_impr_;
    }
  }
  void Load();
private:
  Ice::Long yestoday_reserve_impr_;
  Ice::Long yestoday_unreserve_impr_;
  Ice::Long today_hour_reserve_impr_;
  Ice::Long today_hour_unreserve_impr_;
  Ice::Long today_predict_reserve_impr_;  //今天预测的余量数目
  const Ice::Long max_test_ad_impr;//最大测试广告impr不超过40000000(400);
  const Ice::Long min_test_ad_impr;//余量最小能测试50个广告(5000000)，如果余量低于50个测试广告，需要利用非余量测试
  const double max_unreserve_test_ratio;
  IceUtil::RWRecMutex mutex_;
};
class TestAdImprRatioAssignCacheTimer : public MyUtil::Timer {
  public:
  TestAdImprRatioAssignCacheTimer(int time =0 ) :Timer(time){
      }
    virtual void handle() {
      MCE_INFO("TestAdImprRatioAssignCacheTimer begin");
      TestAdImprRatioAssignCache::instance().Load();
      MCE_INFO("TestAdImprRatioAssignCacheTimer stop");
      MyUtil::TaskManager::instance().schedule(new TestAdImprRatioAssignCacheTimer(30 * 60 * 1000));//半小时计算一次
    }
};

class BigWidgetReduceRatioCache : public MyUtil::Singleton<BigWidgetReduceRatioCache> {
public:
  int GetBigWidgetReduceRatio(Ice::Long adzone_id){
    int res = -1;
    map<Ice::Long, int>::iterator it;
    IceUtil::RWRecMutex::RLock lock(mutex_);
    it = reduce_ratio.find(adzone_id);
    if (it != reduce_ratio.end()) {
        res = it->second;
    }
    return res;
  }
  void Load();
private:
  IceUtil::RWRecMutex mutex_;
  map<Ice::Long, int> reduce_ratio;
};

class BigWidgetReduceRatioCacheTimer : public MyUtil::Timer {
public:
  BigWidgetReduceRatioCacheTimer() :
    Timer(4 * 60 * 1000) { //4分钟重新计算一次, 
   // Timer(1 * 60 * 1000) { //1分钟重新计算一次, 
  }
  virtual void handle() {
    BigWidgetReduceRatioCache::instance().Load();
  }
};

class FreshAdGroupsCacheTimer : public MyUtil::Timer {
public:
  FreshAdGroupsCacheTimer() :
    Timer(5 * 60 * 1000) { //5分钟重新计算一次
  }
  virtual void handle() {
    FreshAdGroupsCache::instance().Load();
  }
};
class CpmLoaderCacheTimer : public MyUtil::Timer {
public:
  CpmLoaderCacheTimer() :
    Timer(2 * 60 * 1000) { //2分钟重新计算一次
  }
  virtual void handle() {
    CpmLoaderCache::instance().Load();
  }
};

class AdSortPosCache : public MyUtil::Singleton<AdSortPosCache> {
public:
  PosSeq GetAppSortPos(string app_name){
    PosSeq sort_pos;
    {
      IceUtil::RWRecMutex::RLock lock(mutex_);
      if (app_sort_pos.find(app_name) != app_sort_pos.end()) {
        sort_pos = app_sort_pos[app_name];
        MCE_DEBUG("[AdSortPosCache:]sort app: find app " << app_name << ",sort is " << sort_pos[0] << " " << sort_pos[1] << " " <<sort_pos[2]);
      }else{
        sort_pos.push_back(1);
        sort_pos.push_back(2);
        sort_pos.push_back(3);
        MCE_DEBUG("[AdSortPosCache:]sort app:find no app, app is " << app_name);
      }
    }
    return sort_pos;
  }
  void Load() {
    MCE_INFO("[AdSortPosCache:]App sort pos::Load start");
    AdSortPosMap local_app_sort_pos;
    try {
      local_app_sort_pos = AdDemoQualityAdapter::instance().GetAppSortPos(EngineConfig::instance().server_index());
    }catch (Ice::Exception& e) {
      MCE_WARN("AdSortPosCache::Load --> call AdDemoQualityNAdapter ERR  line:"
          << __LINE__ << " err:" << e);
    } catch (std::exception& e) {
      MCE_WARN("AdSortPosCache::load --> call AdDemoQualityNAdapter ERR  line:"
          << __LINE__ << " err:" << e.what());
    }

    IceUtil::RWRecMutex::WLock lock(mutex_);
    app_sort_pos.swap(local_app_sort_pos);
    MCE_INFO("[App sort pos:]: Load stop, GetAppSortPosMap size is:"  << app_sort_pos.size());
  }
private:
  AdSortPosMap app_sort_pos;
  IceUtil::RWRecMutex mutex_;
};

class AdSortPosCacheTimer : public MyUtil::Timer {
public:
  AdSortPosCacheTimer(int time =0) :
    Timer(time) { //30分钟重新计算一次
  }
  virtual void handle() {
    AdSortPosCache::instance().Load();
    MyUtil::TaskManager::instance().schedule(new AdSortPosCacheTimer(30 * 60 * 1000));
  }
};

class AdsQualityCache : public MyUtil::Singleton<AdsQualityCache> {
public:
  double GetAdQuScore(Ice::Long group_id,Ice::Long adzone){
    double score = 1.0;
    stringstream gidzid;
    gidzid<<group_id<<adzone;
    string score_name= gidzid.str();
    
    AdQualityScoreMap::iterator it;
    {
      IceUtil::RWRecMutex::RLock lock(mutex_score);
      it = quality_score.find(score_name);
      if (it != quality_score.end()) {
        score = it->second;
        MCE_DEBUG("groupid zoneid : "<<gidzid<<" ,score is "<<score);
      }
    }
    return score;
  }
  void Load() {
    MCE_INFO("[AdsQualityCache:]App sort pos::Load Begin");
    AdQualityScoreMap quality_score_pool;
    try {
      quality_score_pool = AdDemoQualityAdapter::instance().GetAdsQuality(EngineConfig::instance().server_index());
      if(!quality_score_pool.empty()){
        IceUtil::RWRecMutex::WLock lock(mutex_score);
        quality_score.swap(quality_score_pool);
      } else {  
        MCE_WARN("AdQualityScoreCache:: Load QualityScore empty");
      }
      /*
      AdQualityScoreMap::iterator it;
      for(it = quality_score.begin();it != quality_score.end();++it){
        MCE_INFO("groupid_zoneid : "<<it->first<<"  quality_score is "<<it->second);
      }
      */
      MCE_INFO("GetAdsQuality size is:"  <<  quality_score.size());

     }catch (Ice::Exception& e) {
      MCE_WARN("AdQualityScore::Load --> call  AdQualityScoreNAdapter ERR  line:"
          << __LINE__ << " err:" << e);
    } catch (std::exception& e) {
      MCE_WARN("AdQualityScoreCache::load --> call  AdQualityScoreNAdapter ERR  line:"
          << __LINE__ << " err:" << e.what());
    }
    MCE_INFO("[AdsQualityCache:]App sort pos::Load End");
  }

private:
  AdQualityScoreMap quality_score;
  IceUtil::RWRecMutex mutex_score;
};

class AdsQualityTimer : public MyUtil::Timer {
public:
  AdsQualityTimer(int time = 0) :Timer(time) { 
  }
  virtual void handle() {
    AdsQualityCache::instance().Load();
    MyUtil::TaskManager::instance().schedule(new AdsQualityTimer(6 * 60 * 60 * 1000)); // 6 小时 更新一次
  }
};

//部分二跳计价的用户ecpm打折
//cache
class DiscountCache : public MyUtil::Singleton<DiscountCache> {
public:
    double GetDiscountRatio(Ice::Long member_id){
        IceUtil::RWRecMutex::RLock lock(mutex_);
        map<Ice::Long, double>::iterator itdis = discount_map_.find(member_id);
        if(itdis != discount_map_.end()) {
           return itdis->second;
        }
    return 1.0;
    }
  void Load();
private:
  map<Ice::Long,double> discount_map_;
  IceUtil::RWRecMutex mutex_;
};

//timer
class DiscountCacheTimer : public MyUtil::Timer {
public:
  DiscountCacheTimer() :
    Timer(5 * 60 * 1000) { //5分钟重新计算一次
  }
  virtual void handle() {
	MCE_DEBUG("DiscountCacheTimer begin");
    DiscountCache::instance().Load();
    MCE_DEBUG("DiscountCacheTimer stop");
  }
};

class Widget2TypeCache : public MyUtil::Singleton<Widget2TypeCache> {
public:
  int GetWidgetType(Ice::Long widget_id) {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, int>::iterator itdis = widget2Type_map_.find(widget_id);
    if(itdis != widget2Type_map_.end()) {
       return itdis->second;
    }
    return 0;
  }
  void Load();
private:
  map<Ice::Long, int> widget2Type_map_;
  IceUtil::RWRecMutex mutex_;
};

class Widget2TypeCacheTimer : public MyUtil::Timer {
public:
  Widget2TypeCacheTimer() :
    Timer(5 * 60 * 1000) { //5分钟重新加载一次
  }
  virtual void handle() {
   MCE_DEBUG("Wiget2TypeCacheTimer begin");
   Widget2TypeCache::instance().Load();
     MCE_DEBUG("Wiget2TypeCacheTimer stop");
   }
};

}
}

#endif // _USER_RANK_HANDLER_H_
