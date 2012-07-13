#ifndef FeedSINKI_H_
#define FeedSINKI_H_

#include "RFeed.h"
#include "FeedSink.h"
#include "Singleton.h"
#include <IceUtil/IceUtil.h>
#include <TaskManager.h>
using namespace std;
using namespace MyUtil;

namespace xce {
namespace feed {


class FeedSinkI: public FeedSink, public MyUtil::Singleton<FeedSinkI> {
public:
	virtual void	AddFeed(const FeedItem& item, const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight, const Ice::Current& = Ice::Current());
	void Init();
private:
	Int2IntMap GetTester(const MyUtil::Int2IntMap& id2weight);
	set<int> testers_;
};
//-----------------------------------------------------------------------------
class AddFeedTask: public MyUtil::Task {
public:
	AddFeedTask(const FeedItem& item, const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight) :
    item_(item), config_(config), id2weight_(id2weight) {
  };
	virtual void handle();
private:
	FeedItem item_;
	FeedConfigPtr config_;
	Int2IntMap id2weight_;
};
//-------------------------------------------------------------------------------
class FeedNews50Handler:public MyUtil::Singleton<FeedNews50Handler>{
public:
	void Init();
	Int2IntMap GetTester(const MyUtil::Int2IntMap& id2weight);
	void Handle(const FeedItem& fitem,
	    const FeedConfigPtr& config, const MyUtil::Int2IntMap& id2weight);
	set<int> testers_;
};

class TargetPrinter{
	static void Handle(const FeedItem& fitem,const MyUtil::Int2IntMap& id2weight);
};
}
}
#endif
