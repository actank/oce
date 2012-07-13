/*
 * FeedItem.h
 *
 *  Created on: Aug 28, 2009
 *      Author: antonio
 */
#ifndef FEEDMINI_H_
#define FEEDMINI_H_

#include <list>
#include <vector>
#include <map>
#include <IceUtil/IceUtil.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <string>

#include "RFeed.h"
#include "ServiceI.h"

#include "FeedContentAdapter.h"

namespace xce {
namespace feed {

using namespace boost::multi_index;
using namespace MyUtil;
const string FeedDBInstance = "feed_db";

const int PageFeedCount = 10;//一页显示新鲜事的条数
const unsigned int LoadFeedCount = PageFeedCount * 2 + 10;//一个用户在内存中保存的新鲜事最少条数

//---------------------------------------------------------------------------

class Feed: public IceUtil::Shared {
public:
  Ice::Long feed;
  Ice::Long merge;
  unsigned int type; // 包含 NewsMergeType MiniMergeType stype, type , 其中 mergeType 为最高两位，其它的位由 type,stype平分
  unsigned int time;
  unsigned int weight;

  Feed(const FeedItem& item, unsigned int w) {
    feed = item.feed;
    merge = item.merge;
    type = (unsigned int) item.type;
    time = (unsigned int) item.time;
    weight = (unsigned int) w;
  }
  ;
  inline bool isValid() {
    return feed > 0;
  }
  ;
  inline int bigType() const {
    return int((type & 0xFFFF) / 100) * 100;
  }
  ;
  inline int smallType() const {
    return type & 0xFFFF;
  }
  ;
  inline int newsMergeType() const {
    return type >> 30;
  }
  ;
  inline int miniMergeType() const {
    return (type & 0x3FFFFFFF) >> 28;
  }
  ;
  //inline int replyType() const {
  //  return (type & 0x0cFFFFFF) >> 26;
  //}
  inline int friendrank() const {
    return weight >> 16;
  }
  inline int getWeight() const {
    return weight & 0x0000FFFF;
  }
  inline void setWeight(int w) {
    weight = (weight & 0xffff0000) | (0x0000FFFF & w);
  }
};
typedef IceUtil::Handle<Feed> FeedPtr;
typedef std::vector<FeedPtr> FeedSeq;

//---------------------------------------------------------------------------
class FeedMergeGroup: public IceUtil::Shared {
public:
  FeedMergeGroup(const FeedPtr& feed) {
    addFeed(feed);
  }

  inline bool updateWeight();
  inline bool addFeed(const FeedPtr& feed);
  inline int type() const;
  inline int stype() const;
  inline Ice::Long merge() const;
  inline void remove(Ice::Long feedid);
  inline int timestamp() const;

  FeedSeq get(); //获取包含的所有Feed
  FeedPtr getNewestFeed();
  MyUtil::LongSeq getFeedIdSeq();  //获取包含的所有FeedId

  bool empty();

private:
  int _weight;

  typedef std::list<FeedPtr> FeedList; //保存的是Feed类
  FeedList _feedList;
};

typedef IceUtil::Handle<FeedMergeGroup> FeedMergeGroupPtr;

//---------------------------------------------------------------------------

typedef multi_index_container<FeedMergeGroupPtr, indexed_by<ordered_non_unique<
    composite_key<FeedMergeGroup,
        BOOST_MULTI_INDEX_CONST_MEM_FUN(FeedMergeGroup,int,stype),
        BOOST_MULTI_INDEX_CONST_MEM_FUN(FeedMergeGroup,Ice::Long,merge)
    > > , ordered_non_unique<
    BOOST_MULTI_INDEX_CONST_MEM_FUN(FeedMergeGroup,int,timestamp),
    std::greater<int> > > >
//						BOOST_MULTI_INDEX_CONST_MEM_FUN(FeedMergeGroup,Ice::Long,getWeightForSort),std::greater<Ice::Long> > > >
FeedMergeGroupSet;

typedef FeedMergeGroupSet::nth_index<0>::type StypeMergeIndex; //以stype和merge作为索引
typedef FeedMergeGroupSet::nth_index<1>::type WeightIndex; //以时间为权重

//---------------------------------------------------------------------------

class UserFeedSet: public Ice::Object {
public:
  UserFeedSet(int user) :
    _lastAccessTime(time(NULL)), _miniTime(0x7FFFFFFF), _loadStat(0), _user(
        user) {
  }
  ;
  time_t getLastAccessTime() {
    return _lastAccessTime;
  }
  void addFeed(const FeedPtr& feed, const FeedConfigPtr& config);
  void setRead(int stype, Ice::Long merge);
  void setReadAll();
  FeedDataSeq getFeedDataSeq(int begin, int limit);
  FeedDataSeq getFeedDataSeqByStype(const IntSeq& stypes, int begin, int limit);
  FeedDataSeq getFeedDataSeqByType(const IntSeq& types, int begin, int limit);

  void GetFeedItemSeqSeq(int limit,FeedItemSeqSeq & itemseqseq); //新增函数，返回最近10个FeedItem

  void resort();
  void load();

  int size() {
    IceUtil::Mutex::Lock lock(_mutex);
    WeightIndex& index = _mergeGroupSet.get<1> ();
    return index.size();
  }

private:
  void _addFeed(const FeedPtr& feed, const FeedConfigPtr& config);

  template<class Index>
  void _buildFeedContents(Index& index,
      std::vector<typename Index::iterator>& itSeq, FeedDataSeq& seq);

  int _user;
  FeedMergeGroupSet _mergeGroupSet;
  IceUtil::Mutex _mutex;
  //	int _lastSortTime;
  time_t _lastAccessTime;
  time_t _miniTime;
  bool _loadStat; // 0 表示可以Load， 1表示正在Load， 2表示没有可Load的了
};

typedef IceUtil::Handle<UserFeedSet> UserFeedSetPtr;

class UserFeedSetFactory: public MyUtil::ServantFactory {
public:
  virtual Ice::ObjectPtr create(Ice::Long id);

};

//---------------------------------------------------------------------------

class FeedItemManagerI: public FeedItemManager, public MyUtil::Singleton<
    FeedItemManagerI> {
public:

  virtual FeedDataSeq getFeedDataByStype(int uid, const MyUtil::IntSeq& stypes,
      int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedDataSeq getFeedDataByType(int uid, const MyUtil::IntSeq& types,
      int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedDataSeq getFeedData(int uid, int begin, int limit,
      const Ice::Current& = Ice::Current());
  virtual FeedDataSeq getFeedData2(int uid, int begin, int limit,
      const Ice::Current& = Ice::Current());
  virtual FeedDataSeq getFeedDataByIds(const MyUtil::LongSeq&,
      const Ice::Current& = Ice::Current()) {
    return FeedDataSeq();
  }

  virtual vector<FeedItemSeq> GetFeedItemSeqSeqByUsers(const MyUtil::IntSeq& ids,
      const Ice::Current& = Ice::Current());
  virtual void
      addFeed(const FeedItem& item, const FeedConfigPtr& config,
          const MyUtil::Int2IntMap& id2weight, const Ice::Current& =
              Ice::Current());

  virtual void setRead(int uid, int stype, Ice::Long merge,
      const Ice::Current& = Ice::Current());
  virtual void setReadById(int uid, Ice::Long feedId, const Ice::Current& =
      Ice::Current());
  virtual void setReadAll(int uid, const Ice::Current& = Ice::Current());
  virtual bool hasFeed(int uid, const Ice::Current& = Ice::Current());

  virtual void PressureTest(Ice::Int, Ice::Int, Ice::Int, const Ice::Current&){}
  virtual FeedItemSeq GetOriginalFeedData(Ice::Int, const Ice::Current&){
    return FeedItemSeq();
  }
  virtual void Reload(Ice::Int, const Ice::Current&){}

  void _retrySetReadById(int uid, Ice::Long feedId);

  UserFeedSetPtr locateUserFeedSet(int uid);
  UserFeedSetPtr findUserFeedSet(int uid);

  void loadeFeed(Ice::Long uid, int feedCount);

  void kickUser() {
    time_t now = time(NULL);
    IceUtil::Mutex::Lock lock(_mutex);
    UserFeedSetPtr feedSet;
    while (!_kickList.empty()) {
      if (now - _kickList.front().second > 5 * 60) {
        feedSet = findUserFeedSet(_kickList.front().first);
        if (feedSet) {
          if (now - feedSet->getLastAccessTime() > 5 * 60) {
            MCE_DEBUG("FeedItemManagerI::kickUser  --> kick userid = "<<_kickList.front().first);
            ServiceI::instance().removeObject(_kickList.front().first % 10,
                _kickList.front().first);
          } else {
            _kickList.push_back(pair<int, time_t> (_kickList.front().first,
                feedSet->getLastAccessTime()));
          }
        }
        _kickList.pop_front();
      } else {
        break;
      }
    }
  }
  ;
  void addkickUser(int user) {
    IceUtil::Mutex::Lock lock(_mutex);
    _kickList.push_back(pair<int, time_t> (user, time(NULL)));
  }
  ;
private:

  string _getFeedMiniTab(int uid) {
    ostringstream tab;
    tab << "feed_mini_" << uid % 100;
    return tab.str();
  }
  string _getFeedIndexTab(Ice::Long feedId) {
    ostringstream tab;
    tab << "feed_index_" << feedId % 10;
    return tab.str();
  }

  list<pair<int, time_t> > _kickList;
  IceUtil::Mutex _mutex;

};

//class SortTask: public MyUtil::Task {
//public:
//	SortTask(const UserFeedSetPtr& feedset) :
//		Task(MyUtil::TASK_LEVEL_RELOAD), _feedset(feedset) {
//	}
//	;
//	virtual void handle();
//private:
//	UserFeedSetPtr _feedset;
//};

class KickUserTimer: public MyUtil::Timer {
public:
  KickUserTimer() :
    Timer(5 * 1000) {
  }
  ;
  virtual void handle() {
    FeedItemManagerI::instance().kickUser();
  }
  ;
};

//class LoadExtraPageTask: public MyUtil::Task {
//public:
//	LoadExtraPageTask(Ice::Long uid, int feedCount) :
//		_uid(uid), _feedCount(feedCount) {
//		;
//	}
//	virtual void handle();
//private:
//	Ice::Long _uid;
//	int _feedCount;
//
//};

}
;
}
;

#endif /* FEEDMINI_H_ */
