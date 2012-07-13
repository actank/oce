/*
 * FeedNewsReplicaAdapter.cpp
 *
 *  Created on: Oct 15, 2009
 *      Author: antonio
 */

#include <cmath>
#include "FeedNewsReplicaAdapter.h"

using namespace MyUtil;
using namespace xce::feed;

FeedNewsManagerPrx FeedNewsReplicaAdapter::getManager(long id) {
  return getProxy(id);
//  return locate<FeedNewsManagerPrx> (_managers, "FIM", id, TWO_WAY);
}

FeedNewsManagerPrx FeedNewsReplicaAdapter::getManagerOneway(int id) {
  return getProxyOneway(id);
//  return locate<FeedNewsManagerPrx> (_managersOneway, "FIM", id, ONE_WAY);
}

FeedDataResPtr FeedNewsReplicaAdapter::getLiveFeedDataByStype(int uid,
    const MyUtil::IntSeq& stypes, int begin, int limit) {
  return getManager(uid)->GetLiveFeedDataByStype(uid, stypes, begin, limit);
}

FeedDataResPtr FeedNewsReplicaAdapter::getLiveFeedDataByType(int uid,
    const MyUtil::IntSeq& types, int begin, int limit) {
  return getManager(uid)->GetLiveFeedDataByType(uid, types, begin, limit);
}

FeedDataResPtr FeedNewsReplicaAdapter::getLiveFeedData(int uid, int begin, int limit) {
  return getManager(uid)->GetLiveFeedData(uid, begin, limit);
}

FeedDataResPtr FeedNewsReplicaAdapter::getHotFeedData(int uid, int begin, int limit) {
  return getManager(uid)->GetHotFeedData(uid, begin, limit);
}

void FeedNewsReplicaAdapter::addFeed(const FeedItem& item,
    const FeedConfigPtr& config, const MyUtil::Int2IntMap& uid2weight) {
  map<int, MyUtil::Int2IntMap> tmap;
  for (MyUtil::Int2IntMap::const_iterator it = uid2weight.begin(); it
      != uid2weight.end(); ++it) {
//    tmap[abs(it->first) % _cluster][it->first] = it->second;
    tmap[abs(it->first) % getCluster()][it->first] = it->second;
  }
  for (map<int, MyUtil::Int2IntMap>::iterator it = tmap.begin(); it
      != tmap.end(); ++it) {
    //if(it->first%_cluster != 11){
    //  continue;
    //}
    try {
      //FeedNewsManagerPrx::uncheckedCast(getManagerOneway(it->first)->ice_datagram())->AddFeed(item, it->second);
      getManagerOneway(it->first)->AddFeed(item, config, it->second);
    } catch (Ice::Exception& e) {
      MCE_WARN("FeedNewsReplicaAdapter::addFeed --> call addFeed file:"<<__FILE__<<" err:"<<e);
    }
  }
}

int FeedNewsReplicaAdapter::GetUnviewedCount(int uid, int type) {
  return getManager(uid)->GetUnviewedCount(uid, type);
}

void FeedNewsReplicaAdapter::setRead(int uid, int stype, Ice::Long source) {
  /*if(uid%_cluster != 11){
   return;
   }*/
  getManagerOneway(uid)->SetRead(uid, stype, source);
}
void FeedNewsReplicaAdapter::setReadById(int uid, Ice::Long feedId) {
  /*if(uid%_cluster != 11){
   return;
   }*/
  getManagerOneway(uid)->SetReadById(uid, feedId);
}
void FeedNewsReplicaAdapter::setReadAll(int uid) {
  /*if(uid%_cluster != 11){
   return;
   }*/
  MCE_INFO("FeedNewsReplicaAdapter::setReadAll. uid:" << uid);
  getManagerOneway(uid)->SetReadAll(uid);
//  try{
//    getManager(uid)->SetReadAll(uid);
//  } catch (Ice::Exception& e) {
//    MCE_WARN("FeedNewsReplicaAdapter::setReadAll --> err:" << e);
//  } catch (std::exception& e) {
//    MCE_WARN("FeedNewsReplicaAdapter::setReadAll --> err:" << e.what());
//  }
}

void FeedNewsReplicaAdapter::load(int uid) {
  //return FeedInteractionPrx::uncheckedCast(locate<FeedInteractionPrx>(_managers, "M", id, ONE_WAY)->ice_datagram());



//  FeedItem f;
//  f.actor = uid;
//  f.feed = 323;
//  f.merge = 324;
//  f.time = 3443434;
//  f.type = 333;
//  f.weight = 344;
//  MyUtil::Int2IntMap ids;
//  ids[100] = 1;
//  FeedConfigPtr c = new FeedConfig;
//
//  try {
//    FeedNewsManagerPrx::uncheckedCast(getManagerOneway(uid)->ice_datagram())->load(
//        uid);
//    if(id % 100 == 1)
//    FeedNewsManagerPrx::uncheckedCast(getManagerOneway(uid)->ice_datagram())->AddFeed(
//        f, c, ids);
//  } catch (Ice::Exception& e) {
//    MCE_WARN("FeedNewsReplicaAdapter::load --> call addFeed "<<__FILE__<<" err:"<<e);
//  }
}

FeedDataSeq FeedNewsReplicaAdapter::GetFeedDataByIdsWithUid(int uid,
    const MyUtil::LongSeq& fids, Ice::Long first, int stype) {
  return getManager(uid)->GetFeedDataByIdsWithUid(uid, fids, first, stype);

}


FeedDataResPtr FeedNewsReplicaAdapter::GetHotFeedDataByStype(int uid, MyUtil::IntSeq stypes, int begin, int limit){
  return getManager(uid)->GetHotFeedDataByStype(uid, stypes,begin,limit);
}
FeedDataResPtr FeedNewsReplicaAdapter::GetHotFeedDataByType(int uid, MyUtil::IntSeq stypes, int begin, int limit){
  return getManager(uid)->GetHotFeedDataByType(uid, stypes,begin,limit);
}
FeedDataSeq FeedNewsReplicaAdapter::GetFeedDataByIds(MyUtil::LongSeq fids){
  if(fids.empty()){
    return FeedDataSeq();
  }
  return getManager(fids.at(0))->GetFeedDataByIds(fids);

}
bool FeedNewsReplicaAdapter::HasFeed(int uid){
  return getManager(uid)->HasFeed(uid);
}


FeedItemSeq FeedNewsReplicaAdapter::GetOriginalFeedData(int uid){
  return getManager(uid)->GetOriginalFeedData(uid);
}
