#ifndef _REPLYGETER_H_
#define _REPLYGETER_H_
/*

#include "Singleton.h"
#include "AdapterI.h"
#include "RFeed.h"
//#include "FeedReply.h"




namespace xce {
namespace feed {


class FeedReplyAdapter: public MyUtil::AdapterI,
		public MyUtil::AdapterISingleton<MyUtil::XceFeedChannel,
				FeedReplyAdapter> {
public:
	FeedReplyAdapter() {
		_managers.resize(cluster() > 0 ? cluster() : 1);
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}

	FeedReplyPtr getOldFeedReply(
		Ice::Long actor, Ice::Long source, Ice::Long stype);
	FeedReplyPtr getNewFeedReply(
		Ice::Long actor, Ice::Long source, Ice::Long stype);
	FeedReplySeq getFeedReplyPair(
		Ice::Long actor, Ice::Long source, Ice::Long stype);

	FeedReplyManagerPrx getManager(int id);
	FeedReplyManagerPrx getManagerOneway(int id);
protected:
	virtual string name() {
		return "FeedReply";
	}
	virtual string endpoints() {
		return "@FeedReply";
	}
	virtual size_t cluster() {
		return 1;
	}

	FeedReplyManagerPrx getTempManager();

	vector<FeedReplyManagerPrx> _managersOneway;
	vector<FeedReplyManagerPrx> _managers;
};

	

};
};	
*/

#endif /* _REPLYGETER_H_ */
