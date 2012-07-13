#include<Ice/Ice.h>
#include"FeedNewest.h"
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
using namespace demo;
using namespace MyUtil;
using namespace boost::multi_index;

class NewestFeedIdMap {
  public:  
    bool getStat() const {
      return dirty;
    }
    void setStat(bool stat) const {dirty = stat;}
  public:
    int uid;
    NewestFeedIndexPtr IndexFeedtype;
  private:
  mutable bool dirty;
};

//typedef IceUtil::Handle<NewestFeedIdMap> NewestFeedIdMapPtr;

const int containnum = 10;
const int SqlItemCount = 1000;//一次写入数据库的条数
typedef boost::multi_index_container<
  NewestFeedIdMap,
    indexed_by<
      sequenced<> ,
      ordered_unique<
        BOOST_MULTI_INDEX_MEMBER(NewestFeedIdMap, int,uid)
      >
    >
  >NewestFeedIdMapContainer;

class FeedNewestI : public FeedNewest, public Singleton<FeedNewestI>{
  public:
    virtual void set(::Ice::Int uid, ::Ice::Long feedid, ::Ice::Int type, const Ice::Current&);
    virtual NewestFeedIndexPtr get(const int uid, const Ice::Current&);
    NewestFeedIndexPtr getDB(int uid);
    void setDB(NewestFeedIdMapContainer* container_);
    int getContainIndex(int uid) {return uid%containnum; }
    friend class FeedNewestTask;
  private:
    typedef NewestFeedIdMapContainer::nth_index<0>::type SeqIndex;
    typedef NewestFeedIdMapContainer::nth_index<1>::type UidIndex;
    typedef NewestFeedIdMapContainer::iterator iterator;
  		

    NewestFeedIdMapContainer container_[containnum];
    IceUtil::Mutex mutex_[containnum];

		
};
class FeedNewestTask : public Task{
  public:
    virtual void handle();
};
class FeedNewestTimer : public Timer{
  public:
    FeedNewestTimer() : Timer(60*50) {};
    virtual void handle();
};
