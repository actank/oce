#include <cmath>
#include "FeedGroupAdapter.h"
#include <boost/foreach.hpp>
using namespace MyUtil;
using namespace xce::feed;
using xce::feed::group::ParallelQuery;
using xce::feed::group::ParallelQueryPtr;
using xce::feed::group::CollectTask;
using xce::feed::group::ParallelQuery2;
using xce::feed::group::ParallelQuery2Ptr;
using xce::feed::group::CollectTask2;

FeedGroupManagerPrx FeedGroupAdapter::getManager(int id) {
  return locate<FeedGroupManagerPrx> (_managers, "M", id, TWO_WAY);
}

FeedGroupManagerPrx FeedGroupAdapter::getManagerOneway(int id) {
  return locate<FeedGroupManagerPrx> (_managers, "M", id, ONE_WAY);
}

void FeedGroupAdapter::addFeed(const FeedItem& item,
    const FeedConfigPtr& config, const MyUtil::Int2IntMap& uid2weight) {
  for (MyUtil::Int2IntMap::const_iterator it = uid2weight.begin(); it
      != uid2weight.end(); ++it) {
    getManagerOneway(it->first)->AddFeed(item, config, uid2weight);
  }

}

void FeedGroupAdapter::RefreshFeed(int gid, Ice::Long feed_id, int timestamp){
  getManagerOneway(gid)->RefreshFeed(gid, feed_id, timestamp);
}

void FeedGroupAdapter::ReplyFeed(int gid, Ice::Long feed_id, int timestamp, int owner, int replier){
  getManagerOneway(gid)->ReplyFeed(gid, feed_id, timestamp,owner,replier);
}


MyUtil::Int2IntMap FeedGroupAdapter::GetGroupsUnreadCount(Ice::Int uid, const MyUtil::IntSeq& group_ids) {
  ParallelQueryPtr query = new ParallelQuery();
  MyUtil::Int2IntMap res = query->execute(cluster(), uid, group_ids, 300);
  return res;
}

MyUtil::Int2IntMap FeedGroupAdapter::GetGroupsUnreadCountOneThread(Ice::Int uid, const MyUtil::IntSeq& group_ids) {
	map<int,vector<int> > idx_groups;
	BOOST_FOREACH(int gid, group_ids){
    unsigned int idx = gid % cluster();
    idx_groups[idx].push_back(gid);
	}
  map<int,int> res;
  for(map<int,vector<int> >::iterator mi = idx_groups.begin();mi != idx_groups.end(); ++mi){
    map<int,int> tmp = getManager(mi->first)->GetGroupsUnreadCount(uid,mi->second);
    res.insert(tmp.begin(),tmp.end());
  }

//
//  vector<MyUtil::IntSeq> targets(cluster);
//  for (size_t i = 0; i < group_ids.size(); ++i) {
//    unsigned int id = group_ids.at(i) % cluster;
//    targets[id].push_back(group_ids.at(i));
//  }
//  map<int,int> res;
//  for (size_t i = 0; i < targets.size(); ++i) {
//    getManagerOneway(targets.at(0))->ReplyFeed(uid,);
//    if (!targets[i].empty()) {
//      MyUtil::TaskManager::instance().execute(new CollectTask(this, i, uid, targets[i], timeout));
//      ++_count;
//    }
//  }
//
//
//  ParallelQueryPtr query = new ParallelQuery();
//  MyUtil::Int2IntMap res = query->execute(cluster(), uid, group_ids, 300);
  return res;
}

MyUtil::Int2IntSeqMap FeedGroupAdapter::GetMembersInGroups(const MyUtil::IntSeq& group_ids){
  ParallelQuery2Ptr query = new ParallelQuery2();
  MyUtil::Int2IntSeqMap res = query->execute(cluster(), group_ids, 300);
  return res;
}
void FeedGroupAdapter::ReadAllFeed(int gid,int uid){
  FeedGroupAdapter::instance().getManagerOneway(gid)->ReadAllFeed(gid,uid);
}

MyUtil::Int2IntMap ParallelQuery::execute(size_t cluster, Ice::Int uid, const MyUtil::IntSeq& group_ids, int timeout) {
  _count = 0;
  _done = false;
  vector<MyUtil::IntSeq> targets(cluster);
  for (size_t i = 0; i < group_ids.size(); ++i) {
    unsigned int id = group_ids.at(i) % cluster;
    targets[id].push_back(group_ids.at(i));
  }
  for (size_t i = 0; i < targets.size(); ++i) {
    if (!targets[i].empty()) {
      MyUtil::TaskManager::instance().execute(new CollectTask(this, i, uid, targets[i], timeout));
      ++_count;
    }
  }
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
  if (_count > 0 && !_mutex.timedWait(IceUtil::Time::milliSeconds(timeout))) {
    MCE_WARN("ParallelQuery::finish -> count: " << _count << " missing");
  }
  _done = true;
  MCE_DEBUG("ParallelQuery::execute --> _result size="<<_result.size());

  return _result;
}

void CollectTask::handle() {
  MyUtil::Int2IntMap res;
  try {
    res = FeedGroupAdapter::instance().getManager(_index)->GetGroupsUnreadCount(_uid, _group_ids);
  } catch (Ice::Exception& e){
    MCE_WARN("FeedGroupAdapter.GetGroupsUnreadCount err : " << e);
  }

  // if (res.empty()) {
  //   res.insert(make_pair(_index, 0));
  // }
  _callback->finish(res);
}

//------------------------------------------------------------------------------


MyUtil::Int2IntSeqMap ParallelQuery2::execute(size_t cluster, const MyUtil::IntSeq& group_ids, int timeout) {
  _count = 0;
  _done = false;
  vector<MyUtil::IntSeq> targets(cluster);
  for (size_t i = 0; i < group_ids.size(); ++i) {
    unsigned int id = group_ids.at(i) % cluster;
    targets[id].push_back(group_ids.at(i));
  }
  for (size_t i = 0; i < targets.size(); ++i) {
    if (!targets[i].empty()) {
      MyUtil::TaskManager::instance().execute(new CollectTask2(this, i, targets[i], timeout));
      ++_count;
    }
  }
  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
  if (_count > 0 && !_mutex.timedWait(IceUtil::Time::milliSeconds(timeout))) {
    MCE_WARN("ParallelQuery2::finish -> count: " << _count << " missing");
  }
  _done = true;
  MCE_DEBUG("ParallelQuery2::execute --> _result size="<<_result.size());

  return _result;
}

void CollectTask2::handle() {
  MyUtil::Int2IntSeqMap res;
  try {
    res = FeedGroupAdapter::instance().getManager(_index)->GetMembers(_group_ids);
  } catch (Ice::Exception& e){
    MCE_WARN("FeedGroupAdapter.GetMembers err : " << e);
  }

  // if (res.empty()) {
  //   res.insert(make_pair(_index, 0));
  // }
  _callback->finish(res);
}

