#ifndef FEED_ASSISTANT_ADAPTER_H
#define FEED_ASSISTANT_ADAPTER_H

#include "RFeed.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce{
namespace feed{

using namespace MyUtil;

class FeedAssistantTestAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedTestChannel, FeedAssistantTestAdapter>
{

protected:
	virtual string name() {
		return "FeedAssistant";
	}
	virtual string endpoints() {
		return "@FeedAssistant";
	}
	virtual size_t cluster() {
		return 1;
	}

public:
	FeedAssistantTestAdapter();
	~FeedAssistantTestAdapter();
	void removeFeedById(Ice::Long feedId);
	void removeFeedBySource(Ice::Long source, int stype);
	void removeFeedByParent(Ice::Long parent, int ptype);
	void removeFeed(Ice::Long source, int stype, int actor);


	void removeFeedReply(Ice::Long source, int stype, int actor, Ice::Long removeReplyId, const FeedReplyPtr& reply);



	void readNews(int user, Ice::Long feed);
	void readAllNews(int user);
	//void readMini(int user, Ice::Long feedid);

	void dispatch(const FeedSeedPtr& seed);
void dispatchWithReply(const FeedSeedPtr& seed, const FeedReplyPtr& reply);
	void dispatchEDM(const FeedSeedPtr& seed);

	void addFeedReply(Ice::Long source, int stype, int actor, const FeedReplyPtr & reply);

	Ice::Long createFeedId();
private:

	FeedAssistantPrx getManager();
	FeedAssistantPrx getManagerOneway();

	vector<FeedAssistantPrx> _managersOneway;
	vector<FeedAssistantPrx> _managers;
};


};
};
#endif
