#ifndef FEED_SORT_REPLICA_ADAPTER_H
#define FEED_SORT_REPLICA_ADAPTER_H

#include "FeedSort.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce{
namespace feed{

using namespace MyUtil;

class FeedSortAdapter : public AdapterI,
public MyUtil::ReplicatedClusterAdapterI<FeedSortPrx>,
public MyUtil::Singleton<FeedSortAdapter>
{

protected:
	virtual string name() {
		return "FeedSort";
	}
	virtual string endpoints() {
		return "@FeedSort";
	}
	virtual size_t cluster() {
		return 0;
	}

public:
	FeedSortAdapter();

	~FeedSortAdapter();

	virtual SortResultPtr Sort(int uid, const ::xce::feed::SortParamPtr & param) {
		return getProxy(uid)->Sort(uid,param);
	}
private:

//	FeedSortPrx getManager(int id);
//	FeedSortPrx getManagerOneway(int id);
//
//	vector<FeedSortPrx> _managersOneway;
//	vector<FeedSortPrx> _managers;
};


};
};
#endif
