#ifndef FEEDGROUPADAPTER_H_
#define FEEDGROUPADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "RFeed.h"

#include "TaskManager.h"

#include "TaskManager.h"
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedGroupAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedGroupAdapter> {
public:
  FeedGroupAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  void addFeed(const FeedItem& item, const FeedConfigPtr& config,
      const MyUtil::Int2IntMap& id2weight);
  void RefreshFeed(int gid, Ice::Long feed_id, int timestamp);
  void ReplyFeed(int gid, Ice::Long feed_id, int timestamp, int owner, int replyer);

  MyUtil::Int2IntMap GetGroupsUnreadCount(Ice::Int uid,
      const MyUtil::IntSeq& group_ids);

  MyUtil::Int2IntMap GetGroupsUnreadCountOneThread(Ice::Int uid,
      const MyUtil::IntSeq& group_ids);

  MyUtil::Int2IntSeqMap GetMembersInGroups(
      const MyUtil::IntSeq& group_ids);

  void ReadAllFeed(int gid,int uid);
  //protected:
  virtual string name() {
    return "FeedGroup";
  }
  virtual string endpoints() {
    return "@FeedGroup";
  }
  virtual size_t cluster() {
    return 10;
  }

  FeedGroupManagerPrx getManager(int id);
  FeedGroupManagerPrx getManagerOneway(int id);

  vector<FeedGroupManagerPrx> _managersOneway;
  vector<FeedGroupManagerPrx> _managers;
};
}}

namespace xce {
namespace feed {
namespace group {
class CollectTask;

class ParallelQuery: public IceUtil::Shared {
public:
  MyUtil::Int2IntMap execute(size_t cluster, Ice::Int uid,
      const MyUtil::IntSeq& group_ids, int timeout);

  typedef IceUtil::Handle<ParallelQuery> ParallelQueryPtr;
private:
  friend class CollectTask;

  void finish(const MyUtil::Int2IntMap & data) {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
    if (_done) {
      return;
    }
    _result.insert(data.begin(), data.end());
    if (--_count <= 0) {
      _mutex.notify();
    }
  }

  IceUtil::Monitor<IceUtil::Mutex> _mutex;
  MyUtil::Int2IntMap _result;
  int _count;
  bool _done;
};
typedef IceUtil::Handle<ParallelQuery> ParallelQueryPtr;

class CollectTask: public MyUtil::Task {
public:
  CollectTask(ParallelQuery::ParallelQueryPtr callback, int index,
      Ice::Int uid, const MyUtil::IntSeq& group_ids, int timeout) :
    MyUtil::Task(MyUtil::TASK_LEVEL_PARALLEL_GET), _callback(callback), _index(
        index), _uid(uid), _group_ids(group_ids), _timeout(timeout) {
  }
  virtual void handle();
private:
  ParallelQuery::ParallelQueryPtr _callback;
  int _index;
  Ice::Int _uid;
  MyUtil::IntSeq _group_ids;
  int _timeout;
};

//----------------------------------------------------------------
//并发查询group好友
class CollectTask2;

class ParallelQuery2: public IceUtil::Shared {
public:
  MyUtil::Int2IntSeqMap execute(size_t cluster,
      const MyUtil::IntSeq& group_ids, int timeout);

  typedef IceUtil::Handle<ParallelQuery2> ParallelQuery2Ptr;
private:
  friend class CollectTask2;

  void finish(const MyUtil::Int2IntSeqMap & data) {
    IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
    if (_done) {
      return;
    }
    _result.insert(data.begin(), data.end());
    if (--_count <= 0) {
      _mutex.notify();
    }
  }

  IceUtil::Monitor<IceUtil::Mutex> _mutex;
  MyUtil::Int2IntSeqMap _result;
  int _count;
  bool _done;
};
typedef IceUtil::Handle<ParallelQuery2> ParallelQuery2Ptr;

class CollectTask2: public MyUtil::Task {
public:
  CollectTask2(ParallelQuery2::ParallelQuery2Ptr callback, int index,
       const MyUtil::IntSeq& group_ids, int timeout) :
    MyUtil::Task(MyUtil::TASK_LEVEL_PARALLEL_GET), _callback(callback), _index(
        index), _group_ids(group_ids), _timeout(timeout) {
  }
  virtual void handle();
private:
  ParallelQuery2::ParallelQuery2Ptr _callback;
  int _index;
  MyUtil::IntSeq _group_ids;
  int _timeout;
};

}
}
}

#endif
