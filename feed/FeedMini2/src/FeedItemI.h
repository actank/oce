/*
 * FeedItemI.h
 *
 *  Created on: 2010-10-21
 *      Author: wu
 */

#ifndef FEEDITEMI_H_
#define FEEDITEMI_H_

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include <map>
#include <vector>
#include <string>
#include <IceUtil/IceUtil.h>
#include "TaskManager.h"
#include "RFeed.h"
#include "LiveFeedCache.h"

using namespace std;
using namespace MyUtil;
using namespace xce::feed;
using namespace boost::multi_index;


const string FeedDBInstance = "feed_db";
const int UserCluster = 20;
const int DBCluster = 100;

class UserFeed{
public:
  UserFeed(){
  }
  UserFeed(int uid) : userid_(uid){
  }
  mutable int       userid_;
  mutable LiveFeed  live_feed_;
};


typedef multi_index_container<UserFeed, indexed_by<
        sequenced<> ,
        hashed_unique<member<UserFeed, int, &UserFeed::userid_> >
        > >UserFeedMap;
typedef UserFeedMap::nth_index<0>::type SeqIndex;
typedef UserFeedMap::nth_index<1>::type UidIndex;

class FeedItemManagerI: public FeedItemManager, public MyUtil::Singleton<
    FeedItemManagerI> {
public:
	static const unsigned int MAX_USER_COUNT = 1000;
	static const int POOL_SIZE = 2000;

  virtual ~FeedItemManagerI(){}

  virtual FeedDataSeq getFeedDataByStype(int uid, const MyUtil::IntSeq& stypes, int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedDataSeq getFeedDataByType(int uid, const MyUtil::IntSeq& types, int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedDataSeq getFeedData(int uid, int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedDataSeq getFeedData2(int uid, int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedItemSeq GetOriginalFeedData(int uid, const Ice::Current& = Ice::Current());
  virtual vector<FeedItemSeq> GetFeedItemSeqSeqByUsers(const MyUtil::IntSeq& ids, const Ice::Current& = Ice::Current());
  


  virtual void addFeed(const FeedItem& item, const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight, const Ice::Current& = Ice::Current());
  virtual void setRead(int  uid, int stype, Ice::Long merge, const Ice::Current& = Ice::Current());
  virtual void setReadById(int uid, Ice::Long feedId, const Ice::Current& = Ice::Current());
  virtual void setReadAll(int uid, const Ice::Current& = Ice::Current());
  virtual bool hasFeed(int uid, const Ice::Current& = Ice::Current());
  virtual void Reload(int uid, const Ice::Current& = Ice::Current());

  virtual void        PressureTest(int uid, int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedDataSeq getFeedDataByIds(const MyUtil::LongSeq&, const Ice::Current& = Ice::Current()) {
      return FeedDataSeq();
  }

  void CleanUser();
protected:
	inline int GetIndex(int userid) {
    return (userid/UserCluster) % POOL_SIZE;
  }
  const UserFeed* GetUserFeed(int uid, bool relocate = false);
  void  AddUserFeed(UserFeed& user_feed);
  void  RemoveUserFeed(int uid);
  FeedDataSeq GetFeedDataImpl(int uid, const IntSeq& types, bool stype, int begin, int limit);
  FeedDataPtr BuildAFeed(int uid, const vector<vector<LiveFeedItem> >& group, const FeedContentDict& fdict);
  FeedDataSeq BuildFeedContents(int uid, vector<vector<vector<LiveFeedItem> > >& feeds);
  void CheckLoad(int uid, vector<Ice::Long>& fids, FeedContentDict & dict);
  //FeedItemSeq GetFeedItemSeq(int uid, int limit);
  int Size();

  void NeedClean(int user, int time=time(NULL));

  bool LoadFeed(int uid, int begin,int limit);

  string GetTableName(int id) const {
    ostringstream os;
    os << "feed_mini_" << id % DBCluster;
    return os.str();
  }
  //void AddFeedItem(int uid, const FeedItem& item);
private:
	UserFeedMap 		pool_[POOL_SIZE];
	IceUtil::Mutex  clean_mutex_;
	IceUtil::Mutex  mutexs_[POOL_SIZE];
	//list<pair<int, time_t> > kickList_;
  map<int, int>   clean_map_;
};

class CleanUserTimer: public MyUtil::Timer {
public:
  CleanUserTimer() :Timer(30*1000) {}

  virtual void handle()
  {
    FeedItemManagerI::instance().CleanUser();
  }
};


#endif /* FEEDITEMI_H_ */
