/*
 * FeedNewsAdapter.h
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

class FeedNewsAdapter: public MyUtil::AdapterI,
		public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel,
				FeedNewsAdapter> {
public:
	FeedNewsAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
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

  //------
  FeedDataResPtr GetHotFeedDataByStype(int uid, MyUtil::IntSeq stypes, int begin, int limit);
  FeedDataResPtr GetHotFeedDataByType(int uid, MyUtil::IntSeq stypes, int begin, int limit);
  FeedDataSeq GetFeedDataByIds(MyUtil::LongSeq fids);
  bool HasFeed(int uid);
  FeedItemSeq GetOriginalFeedData(int uid);
  //----------

  void EraseUser(int uid);
protected:
	virtual string name() {
		return "FeedNews";
	}
	virtual string endpoints() {
		return "@FeedNews";
	}
	virtual size_t cluster() {
		return 20;
	}

	FeedNewsManagerPrx getManager(int id);
	FeedNewsManagerPrx getManagerOneway(int id);

	vector<FeedNewsManagerPrx> _managersOneway;
	vector<FeedNewsManagerPrx> _managers;
};

}
;
}
;
#endif /* FEEDITEMADAPTER_H_ */
