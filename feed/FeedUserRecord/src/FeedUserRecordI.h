#include<Ice/Ice.h>
#include"FeedUserRecord.h"
#include"Singleton.h"
#include<IceUtil/Handle.h>
#include<boost/multi_index_container.hpp>
#include<boost/multi_index/member.hpp>
#include<boost/multi_index/ordered_index.hpp>
#include<boost/multi_index/sequenced_index.hpp>
#include<boost/multi_index/hashed_index.hpp>
#include<QueryRunner.h>
#include"TaskManager.h"
using namespace std;
using namespace xce::feed;
using namespace MyUtil;
using namespace boost::multi_index;
const string FeedDBInstance = "feed_db";
class FeedUserRecordDataWrapper{
public:
  FeedUserRecordResPtr ToFeedUserRecordRes() const{
    FeedUserRecordResPtr res = new FeedUserRecordRes;
    res->data = data_;
    return res;
  }
  void SetDirty(bool dirty=true){
    dirty_ = dirty;
  }
  void UpdateData(int type, long value) const;  

  mutable int   uid_;
  mutable bool  dirty_;
  mutable FeedUserRecordData data_;
};
//typedef IceUtil::Handle<NewestFeedIdMap> NewestFeedIdMapPtr;

const int SqlItemCount = 1000;//一次写入数据库的条数

typedef boost::multi_index_container<
  FeedUserRecordDataWrapper,
    indexed_by<
      sequenced<> ,
      ordered_unique<
        BOOST_MULTI_INDEX_MEMBER(FeedUserRecordDataWrapper, int, uid_)
      >
  >
>FeedUserRecordMap;

class FeedUserRecordI : public FeedUserRecord, public Singleton<FeedUserRecordI>{
public:
  static const int POOL_SIZE = 100;
  virtual void                  Set(::Ice::Int uid, ::Ice::Long feedid, ::Ice::Int type, const Ice::Current&);
  virtual FeedUserRecordResPtr  Get(const int uid, const Ice::Current&);
    

  FeedUserRecordData GetFromDB(int uid);
  void FlushDirtyData();
  int GetIndex(int uid) {return uid%POOL_SIZE; }
  
  bool CheckLoad(int uid);
  bool Load(int uid);
  friend class FeedNewestTask;
private:
  typedef FeedUserRecordMap::nth_index<0>::type SeqIndex;
  typedef FeedUserRecordMap::nth_index<1>::type UidIndex;
  string _getFeedUserRecordTab(int uid) {
    ostringstream tab;
    tab << "feed_user_record_" << uid % 10;
    return tab.str();
  }
  string _getExpressTab(int uid) {
    ostringstream tab;
    tab << "feed_mini_" << (uid % 10)*10;
    return tab.str();
  }
  FeedUserRecordMap pool_[POOL_SIZE];
  IceUtil::Mutex mutexs_[POOL_SIZE];
};

class FlushDBTimer : public Timer{
  public:
    FlushDBTimer() : Timer(10*1000) {};
    virtual void handle();
};
