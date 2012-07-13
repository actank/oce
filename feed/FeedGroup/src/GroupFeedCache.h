#ifndef FEED_CACHE_H_
#define FEED_CACHE_H_

#include <vector>
#include <list>
#include <string>
#include <set>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include "Common.h"
#include "RFeed.h"
#include "TaskManager.h"

using namespace boost::multi_index;

namespace xce {
namespace feed {

using namespace std;
using namespace MyUtil;
// using namespace __gnu_cxx;

const int kPageFeedCount = 20; // 一页显示新鲜事的条数 TODO 随时修改
const unsigned int kLoadFeedCount = kPageFeedCount * 2; // 一个用户在内存中保存的新鲜事最少条数

class TypeFilter;
class TypeChecker;
typedef IceUtil::Handle<TypeChecker> TypeCheckerPtr;

//---------------------------------------------------------------------

struct GroupFeedItem {
  GroupFeedItem(const FeedItem& item){
    id_ = item.feed;
    merge_ = item.merge;
    type_ = (unsigned int) item.type;
    actor_ = item.actor;
    time_ = item.time;
  }

  GroupFeedItem(){}

  int BigType() const {
    return (int) ((type_ & 0x00FFFFFF) / 100) * 100;
  }
  int SmallType() const {
    return type_ & 0x00FFFFFF;
  }
  int NewsMergeType() const {
    return (((unsigned int)type_) >> 30);
  }
  int MiniMergeType() const {
    return (type_ & 0x3FFFFFFF) >> 28;
    // return (type_ >> 28) & 0x03;
  }

  Ice::Long id_;
  Ice::Long merge_;
  int type_;
  time_t time_;
  int actor_;
};

class GroupFeeds : public IceUtil::Shared {
public:
  GroupFeeds(int group_id) : group_id_(group_id), time_oldest_(time(NULL)), all_loaded_(false), max_feed_(-1) {}

  void AddFeedItem(const FeedItem& item);
  void AddNewFeedItem(const FeedItem& item);
  vector<GroupFeedItem> RemoveById(Ice::Long feed_id, bool merge = true);
  vector<GroupFeedItem> Remove(int stype, Ice::Long merge);
  void RemoveAll();
  void RefreshFeedItem(Ice::Long feed_id, int timestamp);

  vector<FeedItem> GetAllItems();
  vector<FeedItem> GetLatestItems(int limit);

  //vector<vector<vector<GroupFeedItem> > > GetFeed(const TypeFilter & type_filter, int begin, int limit);

  vector<vector<vector<GroupFeedItem> > > GetFeed(const TypeCheckerPtr & checker, int begin, int limit);
  void EvictOversize();
  int Size() {
    return feeds_.size();
  }
  bool CountUnread(Ice::Long max_read_feed, int* unread_count,int member_id) const;
  int GetOldestTime() {
    return time_oldest_;
  }
  void SetOldestTime(int t) {
    time_oldest_ = t;
  }
  int group_id() const { return group_id_; }
  bool all_loaded() const { return all_loaded_; }
  void set_all_loaded(bool b) { all_loaded_ = b; }
  Ice::Long max_feed() const { return max_feed_; }

private:
  void MergeFeed(vector<vector<GroupFeedItem*> >& merge_feeds, multimap<Ice::Long, int>& order_by_time) const;

  const int group_id_;
  int time_oldest_;
  bool all_loaded_;
  Ice::Long max_feed_;

  mutable IceUtil::Mutex feeds_mutex_;
  list<GroupFeedItem> feeds_;
};
typedef IceUtil::Handle<GroupFeeds> GroupFeedsPtr;

//------------------------------------------------------------------
class TypeChecker: public IceUtil::Shared {
public:
  TypeChecker(const IntSeq& types,bool by_stype):by_stype_(by_stype){
    for (size_t i = 0; i < types.size(); ++i) {
      type_set_.insert(types.at(i));
    }
  }
  bool Check(const GroupFeedItem* group_item){
    if(type_set_.empty()){
      return true;
    }
    if(!by_stype_){
      return type_set_.find(group_item->BigType()) != type_set_.end();
    }else{
      return type_set_.find(group_item->SmallType()) != type_set_.end();
    }
  }
private:
  set<int> type_set_;
  bool by_stype_;
};


class TypeCheckerFactory{
public:
  static TypeCheckerPtr Create(const IntSeq& types,bool by_stype){
    return new TypeChecker(types,by_stype);
  }
};
//-------------------------------------------------------
class TypeFilter {
public:
  TypeFilter(const IntSeq& types) {
    for (size_t i = 0; i < types.size(); ++i) {
      type_set_.insert(types.at(i));
    }
  }
  virtual ~TypeFilter(){}
  virtual bool IsValid(const GroupFeedItem *) const {
    return true;
  }
protected:
  set<int> type_set_;
};

class BigTypeFilter : public TypeFilter {
public:
  BigTypeFilter(const IntSeq& types) : TypeFilter(types) {}
  virtual ~BigTypeFilter(){}
  virtual bool IsValid(const GroupFeedItem * group_item) const {
    return type_set_.find(group_item->BigType()) != type_set_.end();
  }
};

class SmallTypeFilter : public TypeFilter {
public:
  SmallTypeFilter(const IntSeq& types) : TypeFilter(types) {}
  virtual ~SmallTypeFilter(){}
  virtual bool IsValid(const GroupFeedItem * group_item) const {
    return type_set_.find(group_item->SmallType()) != type_set_.end();
  }
};
//---------------------------------------------------
typedef multi_index_container<
    GroupFeedsPtr, 
    indexed_by<
      sequenced<>,
      hashed_unique<const_mem_fun<GroupFeeds, int, &GroupFeeds::group_id> > >
  > UserFeedMap;

typedef UserFeedMap::nth_index<0>::type SeqIndex;
typedef UserFeedMap::nth_index<1>::type UidIndex;

class GroupFeedCache : public MyUtil::Singleton<GroupFeedCache> {
public:
  static const unsigned int MAX_USER_COUNT = 1000;
  static const int POOL_SIZE = 2000;

  void  Add(const GroupFeedsPtr & user_feed);
  void  Remove(int group_id);
  GroupFeedsPtr Find(int gid, bool relocate = false);
  bool Load(int gid, int begin, int limit);

  void InsertEvictMap(int user, int time);
  int PoolTotalSize();

  void Evict();
private:
  int GetPoolIndex(int group_id) {
    return (group_id / kUserClusterSize) % POOL_SIZE;
  }

  UserFeedMap     pool_[POOL_SIZE];
  IceUtil::Mutex  mutexs_[POOL_SIZE];

  map<int, int>   evict_map_;
  IceUtil::Mutex  evict_mutex_;
};

class EvictTimer: public MyUtil::Timer {
public:
  EvictTimer() : Timer(60 * 1000) {}

  virtual void handle() {
    GroupFeedCache::instance().Evict();
  }
};

}
}

#endif // FEED_CACHE_H_
