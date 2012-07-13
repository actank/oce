#ifndef FEED_ASSISTANT_ADAPTER_H
#define FEED_ASSISTANT_ADAPTER_H

#include "FeedQueue.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce{
namespace feed{

using namespace MyUtil;

class FeedQueueAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedChannel, FeedQueueAdapter>
{

protected:
	virtual string name() {
		return "FeedQueue";
	}
	virtual string endpoints() {
		return "@FeedQueue";
	}
	virtual size_t cluster() {
		return 1;
	}

public:
	FeedQueueAdapter();
	~FeedQueueAdapter();

	int Push(int index,const SqlInfoPtr & sqlinfo);
private:

	FeedQueuePrx getManager(int id);
	FeedQueuePrx getManagerOneway(int id);

	vector<FeedQueuePrx> _managersOneway;
	vector<FeedQueuePrx> _managers;
};


};
};
#endif
