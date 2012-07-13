/*
 * ZhanFeedNewsAdapter.h
 *
 *  Created on: Oct 15, 2009
 *      Author: antonio
 */

#ifndef FEEDNEWSADAPTER_H_
#define FEEDNEWSADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "RFeed.h"

namespace xce {
namespace feed {

using namespace MyUtil;


class ZhanFeedNewsAdapter : public MyUtil::ReplicatedClusterAdapterI<FeedNewsManagerPrx>,
  public MyUtil::Singleton<ZhanFeedNewsAdapter>{

public:

  ZhanFeedNewsAdapter() : MyUtil::ReplicatedClusterAdapterI<FeedNewsManagerPrx> ("ControllerZhanFeedNewsR",120,300,new ZhanFeedChannel){
  }

	FeedDataResPtr getLiveFeedData(int uid, int begin, int limit);
	FeedDataResPtr getLiveFeedDataByStype(int uid, const MyUtil::IntSeq& stypes,
			int begin, int limit);
	FeedDataResPtr getLiveFeedDataByType(int uid, const MyUtil::IntSeq& stypes, int begin,
			int limit);
	
  FeedDataResPtr getHotFeedData(int uid, int begin, int limit);

	void addFeed(const FeedItem& item, const FeedConfigPtr& config, const MyUtil::Int2IntMap& uid2weight);

	void setRead(int uid, int stype, Ice::Long source);
	void setReadById(int uid, Ice::Long feedId);
	void setReadAll(int uid);

  int GetUnviewedCount(int uid, int type);	
  void load(int uid);

  FeedDataSeq GetFeedDataByIdsWithUid(int uid,const MyUtil::LongSeq& fids, Ice::Long first,int stype);


//--------------新加
  FeedDataResPtr GetHotFeedDataByStype(int uid, MyUtil::IntSeq stypes, int begin, int limit);
  FeedDataResPtr GetHotFeedDataByType(int uid, MyUtil::IntSeq stypes, int begin, int limit);
  FeedDataSeq GetFeedDataByIds(MyUtil::LongSeq fids);
  bool HasFeed(int uid);
  FeedItemSeq GetOriginalFeedData(int uid);
//-----------------


protected:


	FeedNewsManagerPrx getManager(int id);
	FeedNewsManagerPrx getManagerOneway(int id);


};

}
;
}
;
#endif /* FEEDITEMADAPTER_H_ */
