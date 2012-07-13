/*
 * FeedMiniReplicaAdapter.cpp
 *
 *  Created on: Oct 15, 2009
 *      Author: antonio
 */

#include <cmath>
#include "FeedMiniReplicaAdapter.h"

using namespace MyUtil;
using namespace xce::feed;

template<typename T>
static string Con2Str(T & t) {
  ostringstream oss;
  oss << "(";
  for (typename T::const_iterator itr = t.begin(); itr != t.end(); ++itr) {
    oss << *itr << ",";
  }
  oss << ")";
  return oss.str();
}

FeedItemManagerPrx FeedMiniReplicaAdapter::getManager(int id) {
  return getProxy(id);
//  return locate<FeedItemManagerPrx> (_managers, "FIM", id, TWO_WAY);
}

FeedItemManagerPrx FeedMiniReplicaAdapter::getManagerOneway(int id) {
  return getProxyOneway(id);
//  return locate<FeedItemManagerPrx> (_managersOneway, "FIM", id, ONE_WAY);
}

FeedDataSeq FeedMiniReplicaAdapter::getFeedDataByStype(int uid,
    const MyUtil::IntSeq& stypes, int begin, int limit) {
  return getManager(uid)->getFeedDataByStype(uid, stypes, begin, limit);
}
FeedDataSeq FeedMiniReplicaAdapter::getFeedDataByType(int uid,
    const MyUtil::IntSeq& types, int begin, int limit) {
  return getManager(uid)->getFeedDataByType(uid, types, begin, limit);
}
FeedDataSeq FeedMiniReplicaAdapter::getFeedData(int uid, int begin, int limit) {
  return getManager(uid)->getFeedData(uid, begin, limit);
}

FeedItemSeqSeq FeedMiniReplicaAdapter::GetFeedItemSeqSeqByUsers(
    const MyUtil::IntSeq & targets) {

  ParallelQueryN::Ptr query = new ParallelQueryN();
  return query->Execute(cluster(), 0, targets, 300);

}

void FeedMiniReplicaAdapter::addFeed(const FeedItem& item,
    const FeedConfigPtr& config, const MyUtil::Int2IntMap& uid2weight) {
  map<int, MyUtil::Int2IntMap> tmap;
  for (MyUtil::Int2IntMap::const_iterator it = uid2weight.begin(); it
      != uid2weight.end(); ++it) {
    tmap[abs(it->first) % cluster()][it->first] = it->second;
  }
  for (map<int, MyUtil::Int2IntMap>::iterator it = tmap.begin(); it
      != tmap.end(); ++it) {
    getManagerOneway(it->first)->addFeed(item, config, it->second);
  }
}

void FeedMiniReplicaAdapter::setRead(int uid, int stype, Ice::Long source) {
  getManagerOneway(uid)->setRead(uid, stype, source);
}
void FeedMiniReplicaAdapter::setReadById(int uid, Ice::Long feedId) {
  getManagerOneway(uid)->setReadById(uid, feedId);
}
void FeedMiniReplicaAdapter::setReadAll(int uid) {
  getManagerOneway(uid)->setReadAll(uid);
}

void FeedMiniReplicaAdapter::PressureTest(int uid, int begin, int limit)
{
	  FeedItemManagerPrx::uncheckedCast(getManagerOneway(uid)->ice_datagram())->PressureTest(uid, begin, limit);
}

//------------------------------------------------------------------------------------------------


FeedItemSeqSeq ParallelQueryN::Execute(size_t cluster, int category,
    const MyUtil::IntSeq& ids, //ids是feedid
    int timeout) {
  MCE_DEBUG("ParallelQueryN::Execute --> cluster = " << cluster << ", ids size = " << ids.size() << ",timeout = " << timeout);
  count_ = 0;
  done_ = false;
  vector<MyUtil::IntSeq> targets(cluster);
  for (size_t i = 0; i < ids.size(); ++i) {
    int id = ids.at(i) % cluster;
    targets[id].push_back(ids.at(i));
  }
  for (size_t i = 0; i < targets.size(); ++i) {
    if (!targets[i].empty()) { //只执行不为空的分组
      MyUtil::TaskManager::instance().execute(new CollectTaskN(this, i,
          targets[i], timeout));
      ++count_;
    }
  }
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);
  if (count_ > 0 && !mutex_.timedWait(IceUtil::Time::milliSeconds(timeout))) { //等待
    MCE_WARN("ParallelQueryN::Finish -> count: " << count_ << " missing");
  }
  done_ = true;
  return result_;
}

void ParallelQueryN::Finish(const FeedItemSeqSeq& data) {
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_);

  if (done_) { //时间已经超时，不需要返回这个分组的计算结果
    return;
  }
  result_.insert(result_.end(), data.begin(), data.end());
  if (--count_ <= 0) { //count_ = 0 说明每一个task都执行完了
    mutex_.notify();
  }
}

//---------------------------------------------------------------------------
CollectTaskN::CollectTaskN(ParallelQueryN::Ptr callback, int index,
    const MyUtil::IntSeq& ids, int timeout) :
  MyUtil::Task(MyUtil::TASK_LEVEL_PARALLEL_GET), callback_(callback), index_(
      index), ids_(ids), timeout_(timeout) {
  MCE_DEBUG("CollectTaskN::CollectTaskN --> index_ = " << index_ << ",ids size = " << ids.size() << ",ids = " << Con2Str(ids_) << ",timeout_ = " << timeout_);
}

void CollectTaskN::handle() {
  FeedItemSeqSeq data;
  Process(data);
  callback_->Finish(data);
}

void CollectTaskN::Process(FeedItemSeqSeq & t) {
  MCE_DEBUG("CollectTaskN::Process --> user ids = " << Con2Str(ids_) );
  try {
    int firstuid = ids_.at(0);
    t = FeedMiniReplicaAdapter::instance().getManager(firstuid)->GetFeedItemSeqSeqByUsers(ids_);
  } catch (Ice::Exception& e) {
    MCE_WARN("CollectTaskN::Handle --> FeedMiniReplicaAdapter.GetFeedItemSeqSeqByUsers, index = "<<index_<<" err:"<<e);
  }
}
