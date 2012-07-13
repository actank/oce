/*
 * FeedNewsAdapter.cpp
 *
 *  Created on: Oct 15, 2009
 *      Author: antonio
 */

#include <cmath>
#include "FeedNewsAdapter.h"

using namespace MyUtil;
using namespace xce::feed;

FeedNewsManagerPrx FeedNewsAdapter::getManager(int id) {
	return locate<FeedNewsManagerPrx> (_managers, "FIM", id, TWO_WAY);
}

FeedNewsManagerPrx FeedNewsAdapter::getManagerOneway(int id) {
	return locate<FeedNewsManagerPrx> (_managersOneway, "FIM", id, ONE_WAY);
}

FeedDataResPtr FeedNewsAdapter::getLiveFeedDataByStype(int uid,
		const MyUtil::IntSeq& stypes, int begin, int limit){
	return getManager(uid)->GetLiveFeedDataByStype(uid,stypes,begin, limit);
}

FeedDataResPtr FeedNewsAdapter::getLiveFeedDataByType(int uid,
		const MyUtil::IntSeq& types, int begin, int limit){
	return getManager(uid)->GetLiveFeedDataByType(uid,types,begin, limit);
}

FeedDataResPtr FeedNewsAdapter::getLiveFeedData(int uid, int begin, int limit){
	return getManager(uid)->GetLiveFeedData(uid,begin, limit);
}

FeedDataResPtr FeedNewsAdapter::getHotFeedData(int uid, int begin, int limit){
	return getManager(uid)->GetHotFeedData(uid,begin, limit);
}

void FeedNewsAdapter::addFeed(const FeedItem& item, const FeedConfigPtr& config, const MyUtil::Int2IntMap& uid2weight){
	map<int , MyUtil::Int2IntMap> tmap;
	for(MyUtil::Int2IntMap::const_iterator it = uid2weight.begin(); it != uid2weight.end(); ++it){
		tmap[abs(it->first) % _cluster][it->first] = it->second;
	}
	for(map<int , MyUtil::Int2IntMap>::iterator it = tmap.begin(); it != tmap.end(); ++it){
		//if(it->first%_cluster != 11){
    //  continue;
    //}
    try{
		  //FeedNewsManagerPrx::uncheckedCast(getManagerOneway(it->first)->ice_datagram())->AddFeed(item, it->second);
		  getManagerOneway(it->first)->AddFeed(item, config, it->second);
		}catch(Ice::Exception& e){
			MCE_WARN("FeedNewsAdapter::addFeed --> call addFeed file:"<<__FILE__<<" err:"<<e);
		}
	}
}

int FeedNewsAdapter::GetUnviewedCount(int uid, int type){
  return getManager(uid)->GetUnviewedCount(uid, type);
}

void FeedNewsAdapter::setRead(int uid, int stype, Ice::Long source){
	/*if(uid%_cluster != 11){
    return;
  }*/
  getManagerOneway(uid)->SetRead(uid,stype,source);
}
void FeedNewsAdapter::setReadById(int uid, Ice::Long feedId){
	/*if(uid%_cluster != 11){
    return;
  }*/
  getManagerOneway(uid)->SetReadById(uid,feedId);
}
void FeedNewsAdapter::setReadAll(int uid){
	/*if(uid%_cluster != 11){
    return;
  }*/
  getManagerOneway(uid)->SetReadAll(uid);
}

void FeedNewsAdapter::load(int uid){
	//return FeedInteractionPrx::uncheckedCast(locate<FeedInteractionPrx>(_managers, "M", id, ONE_WAY)->ice_datagram());
//  if(uid%cluster() == 11){
	  FeedNewsManagerPrx::uncheckedCast(getManagerOneway(uid)->ice_datagram())->load(uid);
//  }
}

FeedDataSeq FeedNewsAdapter::GetFeedDataByIdsWithUid(int uid,const MyUtil::LongSeq& fids, Ice::Long first,int stype){
  return getManager(uid)->GetFeedDataByIdsWithUid(uid,fids,first,stype);

}



FeedDataResPtr FeedNewsAdapter::GetHotFeedDataByStype(int uid, MyUtil::IntSeq stypes, int begin, int limit){
  return getManager(uid)->GetHotFeedDataByStype(uid, stypes,begin,limit);
}
FeedDataResPtr FeedNewsAdapter::GetHotFeedDataByType(int uid, MyUtil::IntSeq stypes, int begin, int limit){
  return getManager(uid)->GetHotFeedDataByType(uid, stypes,begin,limit);
}
FeedDataSeq FeedNewsAdapter::GetFeedDataByIds(MyUtil::LongSeq fids){
  if(fids.empty()){
    return FeedDataSeq();
  }
  return getManager(fids.at(0))->GetFeedDataByIds(fids);

}
bool FeedNewsAdapter::HasFeed(int uid){
  return getManager(uid)->HasFeed(uid);
}
void FeedNewsAdapter::EraseUser(int uid){
  FeedNewsManagerPrx::uncheckedCast(getManagerOneway(uid)->ice_datagram())->EraseUser(uid);
//  return getManager(uid)->EraseUser(uid);
}


FeedItemSeq FeedNewsAdapter::GetOriginalFeedData(int uid){
  return getManager(uid)->GetOriginalFeedData(uid);
}
