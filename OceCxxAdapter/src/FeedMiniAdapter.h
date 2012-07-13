/*
 * FeedMiniAdapter.h
 *
 *  Created on: Oct 15, 2009
 *      Author: antonio
 */

#ifndef FEEDMINIADAPTER_H_
#define FEEDMINIADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "RFeed.h"

namespace xce {
namespace feed {

using namespace MyUtil;

class FeedMiniAdapter: public MyUtil::AdapterI,
    public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel, FeedMiniAdapter> {
public:
  FeedMiniAdapter() {
    _managers.resize(cluster() > 0 ? cluster() : 1);
    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

  FeedDataSeq getFeedDataByStype(int uid, const MyUtil::IntSeq& stypes,
      int begin, int limit);
  FeedDataSeq getFeedDataByType(int uid, const MyUtil::IntSeq& stypes,
      int begin, int limit);
  FeedDataSeq getFeedData(int uid, int begin, int limit);

  void addFeed(const FeedItem& item, const FeedConfigPtr& config,
      const MyUtil::Int2IntMap& id2weight);

  void setRead(int uid, int stype, Ice::Long source);
  void setReadById(int uid, Ice::Long feedId);
  void setReadAll(int uid);

  FeedItemSeqSeq GetFeedItemSeqSeqByUsers(const MyUtil::IntSeq & Users);

  FeedItemManagerPrx getManager(int id);
  FeedItemManagerPrx getManagerOneway(int id);

  void PressureTest(int uid, int begin, int limit);

protected:
  virtual string name() {
    return "FeedMini";
  }
  virtual string endpoints() {
    return "@FeedMini";
  }
  virtual size_t cluster() {
    return 20;
  }

  vector<FeedItemManagerPrx> _managersOneway;
  vector<FeedItemManagerPrx> _managers;
};

//------------------------------------------------------------------------------------
class CollectTaskN;
//------------------------------------------------------------------------------------

class ParallelQueryN: public IceUtil::Shared {
public:
  FeedItemSeqSeq Execute(size_t cluster, int category,
      const MyUtil::IntSeq& ids, //ids是uid
      int timeout);
  typedef IceUtil::Handle<ParallelQueryN> Ptr;
private:
  friend class CollectTaskN;
  void Finish(const FeedItemSeqSeq & data);

  IceUtil::Monitor<IceUtil::Mutex> mutex_;
  FeedItemSeqSeq result_;
  int count_;
  bool done_;
};

//------------------------------------------------------------------------------------
class CollectTaskN: public MyUtil::Task {
public:
  CollectTaskN(ParallelQueryN::Ptr callback, int index,
      const MyUtil::IntSeq& ids, int timeout);
  virtual void handle();
private:

  void Process(FeedItemSeqSeq& t);

  ParallelQueryN::Ptr callback_;
  int index_;
  MyUtil::IntSeq ids_;
  int timeout_;
};

}
;
}
;
#endif /* FEEDMINIADAPTER_H_ */
