#include "FeedInteractionNAdapter.h"

using namespace xce::feed;


FeedInteractionPrx FeedInteractionNAdapter::getManagerOneway(Ice::Long id) {
	//_managerOneway = FeedInteractionPrx::uncheckedCast(locate<FeedInteractionPrx >("M", ONE_WAY)->ice_datagram());
	//if(uptime_ < 0 || time(NULL)-uptime_ > 10){
  //  uptime_ = time(NULL);
  //  reset();
  //}
  return FeedInteractionPrx::uncheckedCast(locate<FeedInteractionPrx>(_managersOneway, "M", id, ONE_WAY)->ice_datagram());
}

FeedInteractionPrx FeedInteractionNAdapter::getManager(Ice::Long id) {
	return locate<FeedInteractionPrx> (_managers, "M", id, TWO_WAY);
  /*static const int CONN_COUNT = 100;
  if(_managers.empty()){
    for(int i=0; i<CONN_COUNT; i++){
      vector<FeedInteractionPrx > proxys;
      proxys.resize(cluster() > 0 ? cluster() : 1);
	    locate<FeedInteractionPrx> (proxys, "M", id, TWO_WAY, 500);
      for(size_t j=0; j<proxys.size(); j++){
        ostringstream os;
        os << "InteractionPrx" << i;
        _managers[i].push_back(proxys.at(j)->ice_connectionId(os.str()));
      }
    }
  }
  return _managers[(id/cluster())%CONN_COUNT].at(id%cluster());*/
}

void FeedInteractionNAdapter::AddFeedInteraction(Ice::Long feed, int actor, int range, int time){
	getManagerOneway(feed)->AddFeedInteraction(feed, actor, range, time);
}

void FeedInteractionNAdapter::Inc(InteractionType type, Ice::Long feed, int count){
	getManagerOneway(feed)->Inc((int)type, feed, count);
}

void FeedInteractionNAdapter::IncBatch(InteractionType type, MyUtil::Long2IntMap feedCounts){
  map<int, MyUtil::Long2IntMap> split;
  MyUtil::Long2IntMap::iterator it = feedCounts.begin();
  for(; it!=feedCounts.end(); ++it){
    split[it->first%_cluster][it->first] = it->second;
  }
  map<int, MyUtil::Long2IntMap>::iterator sit = split.begin();
	for(; sit!=split.end(); ++sit){
    getManagerOneway(sit->first)->IncBatch((int)type, sit->second);
  }
}

FeedInteractionCountPtr FeedInteractionNAdapter::GetFeedInteractionCount(long feed, bool load){
	return getManager(feed)->GetFeedInteractionCount(feed, load);
}

FeedInteractionCountSeq FeedInteractionNAdapter::GetFeedInteractionCountSeqImpl(MyUtil::LongSeq feeds, bool load){
	return getManager(feeds.at(0))->GetFeedInteractionCountSeq(feeds, load);
}

FeedInteractionCountSeq FeedInteractionNAdapter::GetFeedInteractionCountSeqImpl2(MyUtil::LongSeq feeds, bool load){
  string feeds_str;
  for(size_t i=0; i<feeds.size(); i++){
    Ice::Long tmp = feeds.at(i);
    feeds_str.insert(feeds_str.end(), (char*)&tmp, ((char*)&tmp)+sizeof(Ice::Long));
  }
  string res = getManager(feeds.at(0))->GetFeedInteractionSeq(feeds_str, load);
  FeedInteractionCountSeq seq;
  int UNIT = sizeof(Ice::Long) + 4*sizeof(int);
  const char* p = res.c_str();
  const char* end = p + res.size();
  for(; p!=end; ){
    FeedInteractionCountPtr fi = new FeedInteractionCount;
    fi->feed = *(Ice::Long*)p;
    fi->clickCount = *(int*)(p+=sizeof(Ice::Long));
    fi->replyCount = *(int*)(p+=sizeof(int));
    fi->viewCount = *(int*)(p+=sizeof(int));
    fi->range = *(int*)(p+=sizeof(int));
    p+=sizeof(int);
    seq.push_back(fi);
  }
  return seq;
}

FeedInteractionCountSeq FeedInteractionNAdapter::GetFeedInteractionCountSeq(MyUtil::LongSeq feeds, bool load){
  /*map<int, MyUtil::LongSeq > split;
  for(size_t i=0; i<feeds.size(); i++){
    split[feeds.at(i)%_cluster].push_back(feeds.at(i));
  }
  FeedInteractionCountSeq res;
  map<int, MyUtil::LongSeq >::iterator it = split.begin();
  for(; it!=split.end(); ++it){
    FeedInteractionCountSeq tmp = getManager(it->first)->GetFeedInteractionCountSeq(it->second, load);
    res.insert(res.end(), tmp.begin(), tmp.end());
  }
  return res;*/
  ParallelQueryInteraction<FeedInteractionCountSeq, MyUtil::LongSeq>::Ptr query = new ParallelQueryInteraction<FeedInteractionCountSeq,MyUtil::LongSeq> ();
	return query->execute(_cluster, 0, feeds, 300, load);
}

void FeedInteractionNAdapter::IncHitCount(Ice::Long feedid, string content){
  getManagerOneway(feedid)->IncHitCount(content);
}
