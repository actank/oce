#ifndef RESTMEDIATORADAPTER_H_
#define RESTMEDIATORADAPTER_H_

#include "Singleton.h"
#include "AdapterI.h"
#include "FeedInteraction.h"

namespace xce {
namespace feed {

using namespace MyUtil;

enum InteractionType{INTERACTION_CLICK=1, INTERACTION_REPLY=2, INTERACTION_VIEW=3};

class FeedInteractionAdapter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedChannel, FeedInteractionAdapter> {
public:
  //void AddFeedInteraction(long feed, int actor, int range, int time); 
  //void Inc(InteractionType type, Ice::Long feed, int count=1);
  //void IncBatch(InteractionType type, MyUtil::Long2IntMap feedCounts);
  void IncHitCount(const string& content);

  //FeedInteractionCountPtr GetFeedInteractionCount(long feed); 
  //FeedInteractionCountSeq GetFeedInteractionCountSeq(MyUtil::LongSeq feeds);
protected:
	virtual string name() {
		return "FeedInteraction";
	}
	virtual string endpoints() {
		return "@FeedInteraction";
	}
	virtual size_t cluster() {
		return 0;
	}
	FeedInteractionPrx getManagerOneway();
	FeedInteractionPrx getManager();
  FeedInteractionPrx _managerOneway;
  FeedInteractionPrx _manager;
};

/*
class FeedInteractionGetter : public AdapterI,
	public AdapterISingleton<MyUtil::XceFeedChannel, FeedInteractionGetter> {
public:
  void AddFeedInteraction(long feed, int actor, int range, int time); 
  void Inc(InteractionType type, Ice::Long feed, int count=1);
  void IncBatch(InteractionType type, MyUtil::Long2IntMap feedCounts);
  
  FeedInteractionCountPtr GetFeedInteractionCount(long feed); 
  FeedInteractionCountSeq GetFeedInteractionCountSeq(MyUtil::LongSeq feeds);
protected:
	virtual string name() {
		return "FeedInteractionGet";
	}
	virtual string endpoints() {
		return "@FeedInteractionGet";
	}
	virtual size_t cluster() {
		return 0;
	}
	FeedInteractionGetPrx getManager();
	FeedInteractionGetPrx _manager;
};

class FeedInteractionAdapter : public Singleton<FeedInteractionAdapter> {
public:
  void AddFeedInteraction(long feed, int actor, int range, int time); 
  void Inc(InteractionType type, Ice::Long feed, int count=1);
  void IncBatch(InteractionType type, MyUtil::Long2IntMap feedCounts);
  
  FeedInteractionCountPtr GetFeedInteractionCount(long feed); 
  FeedInteractionCountSeq GetFeedInteractionCountSeq(MyUtil::LongSeq feeds);
};
*/

}
}

#endif /*RESTMEDIATORADAPTER_H_*/
