#ifndef FEEDITEM_H_
#define FEEDITEM_H_

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
#include <IceUtil/IceUtil.h>
#include "TaskManager.h"
#include "RFeed.h"
#include "LiveFeedCache.h"
#include "NewsUtil.h"
#include "XceLoggerAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace xce::feed;
using namespace boost::multi_index;
using namespace com::xiaonei::service;

typedef vector<Ice::Long> Long1D;
typedef vector<Long1D> Long2D;
typedef vector<Long2D> Long3D;

typedef vector<SharedFeedPtr> SharedFeedPtr1D;
typedef vector<SharedFeedPtr1D> SharedFeedPtr2D;
typedef vector<SharedFeedPtr2D> SharedFeedPtr3D;

//typedef pair<LiveFeed, HotFeed> UserFeed;
class UserFeed{
public:
  UserFeed(){
    //update_hotfeed_time_ = -1;
  	last_get_time_= -1;
  }
  UserFeed(int uid) : userid_(uid){
  	last_get_time_= -1;
    //update_hotfeed_time_ = -1;
  }
  mutable int       userid_;
  mutable UniqueFeedList  unique_feed_list_;
  mutable time_t last_get_time_;
  //mutable HotFeed   hot_feed_;
  //mutable int       update_hotfeed_time_;
  //mutable int       newfeed_count_;
};
//typedef map<int, UserFeed> UserFeedMap;

typedef multi_index_container<UserFeed, indexed_by<
        sequenced<> , 
        hashed_unique<member<UserFeed, int, &UserFeed::userid_> > 
        > >UserFeedMap;
typedef UserFeedMap::nth_index<0>::type SeqIndex;
typedef UserFeedMap::nth_index<1>::type UidIndex;
//typedef pair<int, Ice::Long> MergeKey;
//typedef map<MergeKey, vector<FeedPtr> > MergeFeed;
//typedef map<MergeKey, vector<FeedPtr> >::iterator MergeFeedIt;

//typedef map<MergeKey, vector<MergeFeedIt> > GatherFeed;
//typedef map<MergeKey, vector<MergeFeedIt> >::iterator GatherFeedIt;

typedef map<Ice::Long, vector<SharedFeedPtr> > GatherFeed;
typedef map<Ice::Long, vector<SharedFeedPtr> >::iterator GatherFeedIt;
typedef map<Ice::Long, vector<SharedFeedPtr> >::const_iterator ConstGatherFeedIt;
typedef multimap<int, GatherFeed > OrderedFeed;
typedef multimap<int, GatherFeed>::iterator OrderedFeedIt;

class FeedItemManagerI : public FeedNewsManager, public Singleton<FeedItemManagerI>{
public:
	static const int POOL_SIZE = 10000;
	static const int MAX_USER_COUNT = 100;
  static const int UPDATE_HOTFEED_INTERVAL = 5*60;
  virtual FeedDataResPtr		GetLiveFeedData(int uid, int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedDataResPtr		GetLiveFeedDataByStype(int uid, const IntSeq& stypes, int begin, int limit, const Ice::Current& = Ice::Current());
	virtual FeedDataResPtr		GetLiveFeedDataByType(int uid, const IntSeq& types, int begin, int limit, const Ice::Current& = Ice::Current());
	//virtual FeedDataSeq		getFeedData(int uid, int begin, int limit, const Ice::Current& = Ice::Current());
	//virtual FeedDataSeq		getFeedData2(int uid, int begin, int limit, const Ice::Current& = Ice::Current());
	virtual FeedDataResPtr		GetHotFeedData(int uid, int begin, int limit, const Ice::Current& = Ice::Current());
  virtual FeedDataResPtr		GetHotFeedDataByStype(int uid, const IntSeq& stypes, int begin, int limit, const Ice::Current& = Ice::Current());
	virtual FeedDataResPtr		GetHotFeedDataByType(int uid, const IntSeq& types, int begin, int limit, const Ice::Current& = Ice::Current());
	
  FeedDataResPtr GetFeedDataImpl(int uid, const IntSeq& types, bool stype, int begin, int limit, bool hot);
 
  FeedDataResPtr GetFeedDataRemote(int uid, const IntSeq& types, bool stype, int begin, int limit, bool hot);

  virtual int           GetUnviewedCount(int uid, int type, const Ice::Current& = Ice::Current());
 
  virtual void			    AddFeed(const FeedItem& item, const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight, const Ice::Current& = Ice::Current());
	virtual void			    load(int uid, const Ice::Current& = Ice::Current());
	virtual void			    SetRead(int uid, int stype, Ice::Long merge, const Ice::Current& = Ice::Current());
	virtual void			    SetReadById(int uid, Ice::Long feedId, const Ice::Current& = Ice::Current());
	virtual void			    SetReadAll(int uid, const Ice::Current& = Ice::Current());
  virtual FeedItemSeqSeq GetFeedItemSeqSeqByUsers(const IntSeq& uids, const Ice::Current& = Ice::Current());
  virtual FeedItemSeq   GetOriginalFeedData(int uid, const Ice::Current& = Ice::Current());
  //void                  CheckEnterAll();
  FeedDataSeq GetFeedDataByIds(const MyUtil::LongSeq& fids, const Ice::Current& = Ice::Current());
  FeedDataSeq GetFeedDataByIdsWithUid(int uid,const MyUtil::LongSeq& fids, Ice::Long first,int stype, const Ice::Current& = Ice::Current());
  bool HasFeed(int uid, const Ice::Current& = Ice::Current()){
    return true;
  }
  int Size();
  virtual void EraseUser(int uid, const Ice::Current& = Ice::Current());
  virtual void IncView(int uid,set<long> & showfid);
protected:
	inline int GetIndex(int userid){
		if(userid <0){
			MCE_WARN("Bad arg. uid:" << userid << " line:" << __LINE__);
		}
		return (abs(userid)/20)%POOL_SIZE;
//		return (userid/20)%POOL_SIZE;
	}
	void GetFeedContentDict(int uid,LongSeq & feed_ids,FeedContentDict & fdict);
  const UserFeed*       GetUserFeed(int uid, bool relocate=false);
  void                  AddUserFeed(const UserFeed& user_feed);
  bool                  LoadFeed(int uid);
  FeedDataSeq           BuildBasicFeedData(int uid, const vector<vector<vector<SharedFeedPtr> > >& feeds, int begin, int limit, 
                                            const vector<vector<vector<SharedFeedPtr> > >& random, int edm_count);
  FeedDataSeq           BuildBasicFeedDataV2(int uid,
  		Long3D & fid3d, const hash_map<long,SharedFeedPtr> & feedmap,int begin, int limit
  		,set<Ice::Long> & show_fids  );
  FeedDataPtr           BuildAFeed(int uid, const vector<vector<SharedFeedPtr> >& group, const FeedContentDict& dict);
  static void                  CheckLoad(int uid, vector<Ice::Long>& fids, FeedContentDict & dict);
  //void                  EnterHotFeed(int uid);
//  void 	Fids2Feeds(vector<vector<vector<long> > > & fid3d,
//  		const hash_map<long,FeedPtr> & feedmap,vector<vector<vector<FeedPtr> > > & feed3d );
private:
	UserFeedMap 		pool_[POOL_SIZE];
	IceUtil::Mutex	mutexs_[POOL_SIZE];
};



class UpdateInteractionInCacheTimer : public Timer, public Singleton<UpdateInteractionInCacheTimer>{
public:
  UpdateInteractionInCacheTimer() : Timer(1*60*1000, 0){}
  void handle();
};

class UpdateInteractionInDBTimer : public Timer, public Singleton<UpdateInteractionInDBTimer>{
public:
  UpdateInteractionInDBTimer() : Timer(1000, 1){}
  void handle();
  void insert(Ice::Long feedid){
    IceUtil::Mutex::Lock lock(mutex_);
    feedids_.insert(feedid);
  }
private:
  IceUtil::Mutex    mutex_;
  set<Ice::Long>    feedids_;
};
class GetLogger: public Singleton<GetLogger>{
  public:
      void Init();
			void SendLog(int uid,const FeedDataSeq & data);
  private:
          XceLoggerPtr logger_;
            vector<string> path_;
};

/*
class CheckEnterHotFeedTimer : public Timer, public Singleton<CheckEnterHotFeedTimer>{
public:
  CheckEnterHotFeedTimer() : Timer(30*1000, 2), ready(false){}
  void handle();
  void SetReady(){
    ready = true;
  }
private:
  bool ready;
};*/
#endif
