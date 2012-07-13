#ifndef FEED_NAIVE_ADAPTER_H
#define FEED_NAIVE_ADAPTER_H

#include "FeedSort.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce{
namespace feed{

using namespace MyUtil;

class FeedSortAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedChannel, FeedSortAdapter>
{

protected:
	virtual string name() {
		return "FeedSort";
	}
	virtual string endpoints() {
		return "@FeedSort";
	}
	virtual size_t cluster() {
		return 1;
	}

public:
	FeedSortAdapter();

	~FeedSortAdapter();

	virtual SortResultPtr Sort(int uid, const ::xce::feed::SortParamPtr & param,int timeout) {
		return getManager(uid,timeout)->Sort(uid,param);
	}

private:

	FeedSortPrx getManager(int id,int timeout = 40);
	FeedSortPrx getManagerOneway(int id);

	vector<FeedSortPrx> _managersOneway;
	vector<FeedSortPrx> _managers;
};


};
};
#endif
