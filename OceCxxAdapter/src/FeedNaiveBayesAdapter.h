#ifndef FEED_NAIVE_ADAPTER_H
#define FEED_NAIVE_ADAPTER_H

#include "FeedNaiveBayes.h"
#include "Singleton.h"
#include "AdapterI.h"

namespace xce{
namespace feed{

using namespace MyUtil;

class FeedNaiveBayesAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedChannel, FeedNaiveBayesAdapter>
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
	FeedNaiveBayesAdapter();
	~FeedNaiveBayesAdapter();

	MyUtil::Int2IntMap GetScoreDict(int fromid,const MyUtil::IntSeq & toids, int stype){
		return getManager(fromid)->GetScoreDict(fromid,  toids,stype);
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
