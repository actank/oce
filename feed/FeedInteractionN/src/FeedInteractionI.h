/*
 * FeedInteractionI.h
 *
 *  Created on: 2010-4-22
 *      Author: ld
 */

#ifndef __INTERACTION_FEEDINTERACTION_ICE__
#define __INTERACTION_FEEDINTERACTION_ICE__


#include <string>
#include <vector>
#include <map>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "ServiceI.h"
#include "TaskManager.h"
#include "QueryRunner.h"
#include "Singleton.h"
#include "FeedInteraction.h"


namespace xce {

namespace feed {

const Ice::Long kCacheSize = 100000000;
const Ice::Long kMutexSize = 1;//5000000;

class DbHelper{
public:
  static const int kTableCount = 100;
  static const string kDbTableName;
  static const string kDbLogicName;
  static const string kColFeedId;
  static const string kColReplyCount;
  static const string kColClickCount;
  static const string kColViewCount;
  //static const string kColRange;
  static const string kColDispatchCount;
  static const string kColActor;
  static const string kColTime;

  static string GetTableName(Ice::Long feedid){
    ostringstream os;
    os << kDbTableName << "_" << feedid%kTableCount;
    return os.str();
  }
  static string GetExpr(Ice::Long feedid){
    ostringstream os;
    os << "feed_interaction" << "_" << feedid%kTableCount;
    return os.str();
  }
};

const int kInterval = 1000 * 60 * 5;    //temp
//const int kClearDirtyCacheThreshold = 5 ; //temp

const Ice::Long kReplyCountStep = ((Ice::Long)1) << 16;
const Ice::Long kViewCountStep = ((Ice::Long)1) << 32;
class ClearSaveDbVectorTimer : public MyUtil::Timer, public MyUtil::Singleton<ClearSaveDbVectorTimer>{
public:
	ClearSaveDbVectorTimer():Timer(kInterval, 1) {}
  virtual void handle();
};

struct InteractionCacheItem {
	  Ice::Long feed_;
	  Ice::Int  range_;
	  Ice::Long count_; //底16位:click_count,  高16位:reply_count
	  InteractionCacheItem():feed_(-1),range_(-1),count_(-1){}
};

class InteractionCacheItemObject : public Ice::Object{
public:
  InteractionCacheItem item;
};
typedef IceUtil::Handle<InteractionCacheItemObject> InteractionCacheItemObjectPtr;


struct FeedInteractionDbItem {
	Ice::Long feed_;
  Ice::Int reply_count_;
  Ice::Int click_count_;
  Ice::Int view_count_;
  Ice::Int range_;
  Ice::Int actor_;
  Ice::Int time_;
  
  FeedInteractionDbItem():feed_(-1),reply_count_(-1),click_count_(-1),
      range_(-1),actor_(-1),time_(-1) {}

};


class FeedInteractionCache : public MyUtil::Singleton<FeedInteractionCache>{
public:
	void                          InsertFeedInteraction(const FeedInteractionInfoPtr& interaction_info);
	bool                          UpdateFeedInteraction(Ice::Long feed, int type, bool load=false);
	InteractionCacheItem          FindFeedInteraction(Ice::Long feed, bool load=true);
  vector<InteractionCacheItem>  FindFeedInteraction(const MyUtil::LongSeq feeds, bool load=true); 

  void ClearDirtyCache();

	bool IsHitted(Ice::Long feed);

  void Init();

  int Size(){
    int count = 0;
    for(int i=0; i<kCacheSize; i++){
      if(cache_array_[i].feed_ > 0){
        count++;
      }
    }
    return count;
  }
  Ice::Long GetCacheIndex(Ice::Long feedid){
    return (feedid/10)%kCacheSize;
  }
  Ice::Long GetMutexIndex(Ice::Long feedid){
    return (feedid/10)%kMutexSize;
  }
private:
	//bool isEmptyElement(int cache_index);
	//FeedInteractionDbItem FetchInteractionFromDb ();
  map<Ice::Long, FeedInteractionDbItem>::iterator
      SaveDb(const map<Ice::Long, FeedInteractionDbItem>& temp_map,
      map<Ice::Long, FeedInteractionDbItem>::iterator it);

  
  pair<bool, InteractionCacheItem> FindInDirtyCache(Ice::Long feed);

	InteractionCacheItem QueryCacheItemFromDb(Ice::Long feed);
  vector<InteractionCacheItem> QueryCacheItemFromDb(const MyUtil::LongSeq& feeds);
  InteractionCacheItem UpdateCache(int cache_index, int type);

	InteractionCacheItem cache_array_[kCacheSize];
	IceUtil::RWRecMutex cache_rw_mutex_[kMutexSize];
  IceUtil::Mutex dirty_mutex_;
	map<int, map<Ice::Long,FeedInteractionDbItem> > dirty_cache_items_;
};

class FeedInteractionI: public FeedInteraction,
    public MyUtil::Singleton<FeedInteractionI> {
public:
	FeedInteractionI();
	virtual ~FeedInteractionI();

  virtual void AddFeedInteraction(long feed, int actor, int range, int time, const ::Ice::Current& = ::Ice::Current());
  virtual void Inc(int type, Ice::Long feed, int count, const ::Ice::Current& = ::Ice::Current());
  virtual void IncBatch(int type, const MyUtil::Long2IntMap& feedCounts, const ::Ice::Current& = ::Ice::Current());
  virtual void IncHitCount(const string& content, const ::Ice::Current& = ::Ice::Current());

  virtual FeedInteractionCountPtr GetFeedInteractionCount(Ice::Long feed, bool load, const ::Ice::Current& = ::Ice::Current());
  virtual FeedInteractionCountSeq GetFeedInteractionCountSeq(const ::MyUtil::LongSeq&, bool load, const ::Ice::Current& = ::Ice::Current());
  virtual string GetFeedInteractionSeq(const string&, bool load, const ::Ice::Current& = ::Ice::Current());
  void ClearDirtyCache();
private:
	//FeedInteractionCache feed_interaction_cache_;
};

class ResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
  ResultHandlerI(MyUtil::ObjectResultPtr& result) :
    _result(result) {
  }
  virtual bool handle(mysqlpp::Row& row) const;
private:
  MyUtil::ObjectResultPtr _result;
};

class UpdateTimer : public MyUtil::Timer, public MyUtil::Singleton<UpdateTimer>{
public:
  UpdateTimer() : Timer(10*1000, 2){}
  void handle();
  void PushClick(Ice::Long feedid){
    IceUtil::Mutex::Lock lock(mutex_);
    clicks_.push_back(feedid);
  }
  void PushReply(Ice::Long feedid){
    IceUtil::Mutex::Lock lock(mutex_);
    replys_.push_back(feedid);
  }
private:
  IceUtil::Mutex    mutex_;
  vector<Ice::Long> clicks_;
  vector<Ice::Long> replys_;
};

}
}
#endif /* __INTERACTION_FEEDINTERACTION_ICE__ */
