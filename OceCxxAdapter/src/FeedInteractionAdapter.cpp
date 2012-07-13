#include "FeedInteractionAdapter.h"

using namespace xce::feed;


FeedInteractionPrx FeedInteractionAdapter::getManagerOneway() {
	if (!_managerOneway) {
		_managerOneway = FeedInteractionPrx::uncheckedCast(locate<FeedInteractionPrx >("M", ONE_WAY)->ice_datagram());
	}
  return _managerOneway;
}

FeedInteractionPrx FeedInteractionAdapter::getManager() {
	if (!_manager) {
		_manager= locate<FeedInteractionPrx >("M", TWO_WAY);
	}
  return _manager;
}
/*
void FeedInteractionAdapter::AddFeedInteraction(long feed, int actor, int range, int time){
	getManagerOneway()->AddFeedInteraction(feed, actor, range, time);
}

void FeedInteractionAdapter::Inc(InteractionType type, Ice::Long feed, int count){
	getManagerOneway()->Inc((int)type, feed, count);
}

void FeedInteractionAdapter::IncBatch(InteractionType type, MyUtil::Long2IntMap feedCounts){
	getManagerOneway()->IncBatch((int)type, feedCounts);
}

FeedInteractionCountPtr FeedInteractionAdapter::GetFeedInteractionCount(long feed){
	return getManager()->GetFeedInteractionCount(feed);
}

FeedInteractionCountSeq FeedInteractionAdapter::GetFeedInteractionCountSeq(MyUtil::LongSeq feeds){
	return getManager()->GetFeedInteractionCountSeq(feeds);
}*/
void FeedInteractionAdapter::IncHitCount(const string& content){
	getManagerOneway()->IncHitCount(content);
}
////////////////////////////////////////////////////////////////////////////////////////////
/*
FeedInteractionGetPrx FeedInteractionGetter::getManager() {
	if (!_manager) {
		_manager= locate<FeedInteractionGetPrx >("M", TWO_WAY);
	}
  return _manager;
}

FeedInteractionCountPtr FeedInteractionGetter::GetFeedInteractionCount(long feed){
  getManager()->GetFeedInteractionCount(feed);
}

FeedInteractionCountSeq FeedInteractionGetter::GetFeedInteractionCountSeq(MyUtil::LongSeq feeds){
  getManager()->GetFeedInteractionCountSeq(feeds);
}

////////////////////////////////////////////////////////////////////////////////////////////
void FeedInteractionAdapter::AddFeedInteraction(long feed, int actor, int range, int time){
  FeedInteractionSetter::instance().AddFeedInteraction(feed, actor, range, time);
}

void FeedInteractionAdapter::Inc(InteractionType type, Ice::Long feed, int count){
  FeedInteractionSetter::instance().Inc(type, feed, count);

}

void FeedInteractionAdapter::IncBatch(InteractionType type, MyUtil::Long2IntMap feedCounts){
  FeedInteractionSetter::instance().IncBatch(type, feedCounts);
}
  
FeedInteractionCountPtr FeedInteractionAdapter::GetFeedInteractionCount(long feed){
  FeedInteractionGetter::instance().GetFeedInteractionCount(feed);
}

FeedInteractionCountSeq FeedInteractionAdapter::GetFeedInteractionCountSeq(MyUtil::LongSeq feeds){
  FeedInteractionGetter::instance().GetFeedInteractionCountSeq(feeds);
}*/
