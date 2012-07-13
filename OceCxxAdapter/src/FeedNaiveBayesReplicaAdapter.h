#ifndef FEED_NAIVE_REPLICA_ADAPTER_H
#define FEED_NAIVE_REPLICA_ADAPTER_H

#include "FeedNaiveBayes.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce{
namespace feed{

using namespace MyUtil;

class FeedNaiveBayesAdapter :
	public MyUtil::ReplicatedClusterAdapterI<FeedNaiveBayesPrx>,
	public MyUtil::Singleton<FeedNaiveBayesAdapter>
{



protected:
	virtual string name() {
		return "FeedNaiveBayes";
	}
	virtual string endpoints() {
		return "@FeedNaiveBayes";
	}
	virtual size_t cluster() {
		return 0;
	}

public:
//	FeedNaiveBayesAdapter();
	~FeedNaiveBayesAdapter();
	FeedNaiveBayesAdapter(): MyUtil::ReplicatedClusterAdapterI <FeedNaiveBayesPrx> ("ControllerFeedNaiveBayesR", 120, 300, new XceFeedChannel()) {
//    _managers.resize(cluster() > 0 ? cluster() : 1);
//    _managersOneway.resize(cluster() > 0 ? cluster() : 1);
  }

	MyUtil::Int2IntMap GetScoreDict(int fromid, const MyUtil::IntSeq & toids, int stype){
		return getManager(fromid)->GetScoreDict(fromid,  toids,stype);
	}

	int GetSendFrequency(int fromid){
		return getManager(fromid)->GetSendFrequency(fromid);
	}
private:

	FeedNaiveBayesPrx getManager(int id);
	FeedNaiveBayesPrx getManagerOneway(int id);

	vector<FeedNaiveBayesPrx> _managersOneway;
	vector<FeedNaiveBayesPrx> _managers;
};


};
};
#endif
