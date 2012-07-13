#include "FeedContentAdapter.h"
#include "FeedAssistantNAdapter.h"
#include "FeedMemcLoaderAdapter.h"
using namespace xce::feed;
using namespace std;
using namespace MyUtil;

FeedContentManagerPrx FeedContentAdapter::getManager(Ice::Long id) {
  int i = labs(id % cluster());
  return locate<FeedContentManagerPrx> (_managers, "FCM", i, TWO_WAY);
}

FeedContentManagerPrx FeedContentAdapter::getManagerOneway(Ice::Long id) {
  int i = labs(id % cluster());
  return locate<FeedContentManagerPrx> (_managersOneway, " FCM", i, ONE_WAY);
}

FeedContentDict FeedContentAdapter::getFeedDict(const MyUtil::LongSeq& feedIds,int uid) {
  if(feedIds.empty()){
    return FeedContentDict();
  }

  bool from_memched = false;
  timeval begin,end;
  FeedContentDict dict;
  gettimeofday(&begin, NULL);
  if(true){
  //if(CheckTaskSizeTimer::instance().FromMemc()){
    try{
      //dict = FeedMemcClient::instance().GetFeedDict(feedIds,uid);
      dict = FeedMemcClient::instance().GetFeedDict(feedIds);
    }catch(...){
      MCE_WARN("CollectTask::handle --> FeedMemcClient.getFeedDict");
    }
    from_memched = true;
  }else{
    ParallelQuery<FeedContentDict, MyUtil::LongSeq>::Ptr query =
      new ParallelQuery<FeedContentDict, MyUtil::LongSeq> ();
    dict = query->execute(_cluster, 0, feedIds, 300);
  }
  gettimeofday(&end, NULL);
  float cost = 1000000*(end.tv_sec-begin.tv_sec)+end.tv_usec-begin.tv_usec;
  cost/=1000;
  //MCE_INFO("FeedContentAdapter::getFeedDict --> FeedMemcClient.getFeedDict " << " ids:" << feedIds.size() << " res:" << dict.size() << " cost:" << cost 
          //<< " frommemched:" << from_memched);
  return dict; 
}

/*Feed2WeightMap FeedContentAdapter::getWeight(const MyUtil::LongSeq& feedIds) {
 ParallelQuery<Feed2WeightMap,MyUtil::LongSeq>::Ptr query =
 new ParallelQuery<Feed2WeightMap,MyUtil::LongSeq> ();
 return query->execute(_cluster, 0, feedIds, 300);
 }


 FeedContentDict FeedContentAdapter::getFeedDict2(const UpdateTimeSeq& seq){
 ParallelQuery<FeedContentDict,UpdateTimeSeq>::Ptr query = new ParallelQuery<
 FeedContentDict,UpdateTimeSeq> ();
 return query->execute(_cluster, 0, seq, 300);
 }

 Feed2WeightMap FeedContentAdapter::getWeight2(const UpdateTimeSeq& seq){
 ParallelQuery<Feed2WeightMap,UpdateTimeSeq>::Ptr query =
 new ParallelQuery<Feed2WeightMap,UpdateTimeSeq> ();
 return query->execute(_cluster, 0, seq, 300);
 }*/

int FeedContentAdapter::addFeedContent(const FeedDataPtr& feedData) {
  MCE_DEBUG("call FeedContentAdapter::addFeedContent feedid="<<feedData->feed);
  if (feedData) {
    return getManager(feedData->feed)->addFeedContent(feedData);
  }
  return 0;
}

int FeedContentAdapter::addFeedContentWithReply(const FeedDataPtr& feedData,
    const FeedReplyPtr& reply) {
  MCE_DEBUG("call FeedContentAdapter::addFeedContent feedid="<<feedData->feed);
  if (feedData) {
    int weight = getManager(feedData->feed)->addFeedContent(feedData);
    MyUtil::LongSeq seq;
    seq.push_back(feedData->feed);

    getManagerOneway(feedData->feed)->replaceFeedReply(seq, reply);
    return weight;
  }
  return 0;
}

void FeedContentAdapter::replaceFeedReply(const MyUtil::LongSeq& feedIds,
    const FeedReplyPtr& reply) {
  map<Ice::Long, MyUtil::LongSeq> idMap;
  for (size_t i = 0; i < feedIds.size(); ++i) {
    idMap[feedIds.at(i) % cluster()].push_back(feedIds.at(i));
  }
  for (map<Ice::Long, MyUtil::LongSeq>::iterator it = idMap.begin(); it
      != idMap.end(); ++it) {
    getManagerOneway(it->first)->replaceFeedReply(it->second, reply);
  }

}
void FeedContentAdapter::removeFeed(long feedId) {
  getManagerOneway(feedId)->removeFeed(feedId);
}
void FeedContentAdapter::removeFeeds(const MyUtil::LongSeq& feedIds) {
  map<int, MyUtil::LongSeq> fidMap;
  for (size_t i = 0; i < feedIds.size(); ++i) {
    fidMap[feedIds.at(i) % cluster()].push_back(feedIds.at(i));
  }
  for (map<int, MyUtil::LongSeq>::iterator it = fidMap.begin(); it
      != fidMap.end(); ++it) {
    getManagerOneway(it->first)->removeFeeds(it->second);
  }
}

void FeedContentAdapter::recoveryContent(long feedId)
{
	MCE_INFO("FeedContentAdapter::recoveryContent file:"<<__FILE__ << ", line:"<<__LINE__);
	getManagerOneway(feedId)->recoveryContent(feedId);
}

void FeedContentAdapter::ReplaceXML(const Ice::LongSeq & feedIds ,const string & xml ){
	map<int, MyUtil::LongSeq> fidMap;
	for (size_t i = 0; i < feedIds.size(); ++i) {
		fidMap[feedIds.at(i) % cluster()].push_back(feedIds.at(i));
	}
	for (map<int, MyUtil::LongSeq>::iterator it = fidMap.begin(); it
			!= fidMap.end(); ++it) {
		getManagerOneway(it->first)->ReplaceXML(it->second,xml);
	}
}


FeedDataPtr FeedContentAdapter::getFeed(Ice::Long source, int stype, int actor,Ice::Long & feedid,int uid) {
//  Ice::Long feedid = 0;
  try {
    feedid = FeedAssistantNAdapter::instance().getFeedId(source,stype,actor);
  } catch(Ice::Exception& e) {
    MCE_WARN("FeedContentAdapter::getFeed --> call FeedAssistantNAdapter err:"<<e << ", file:"<<__FILE__ << ", line:"<<__LINE__)
    return NULL;
  }
  if (feedid <= 0) {
    MCE_WARN("FeedContentAdapter::getFeed. call AssistantN. stype=" << stype << " source=" << source << " actor=" << actor << " map to feed " << feedid << " uid:" << uid << " no fid in assistant");
    return NULL;
  }else{
    MCE_INFO("FeedContentAdapter::getFeed. call AssistantN. stype=" << stype << " source=" << source << " actor=" << actor << " map to feed " << feedid << " uid:" << uid);
  }
  FeedDataPtr data =  getFeed(feedid,uid);
  if(!data){
    MCE_INFO("FeedContentAdapter::getFeed. call AssistantN. stype=" << stype << " source=" << source << " actor=" << actor << " call memcloader, uid:" << uid );
    data = getFeedFromMemcLoader(feedid);
  }
  return data;
}
FeedDataPtr FeedContentAdapter::getFeed(Ice::Long feedid,int uid ) {
  MyUtil::LongSeq feedids;
  feedids.push_back(feedid);
  FeedContentDict dict;
  try{
    //dict = FeedMemcClient::instance().GetFeedDict(feedids,uid);
    dict = FeedMemcClient::instance().GetFeedDict(feedids);
  }catch(Ice::Exception& e){
    MCE_WARN("FeedContentAdapter::getFeed --> feedid:"<<feedid << ", err:"<<e << ", file:"<<__FILE__ << ", line:"<<__LINE__ << " uid:" << uid);
    return 0;
  }
  if(dict.empty()){
    return 0;
  }else if(dict.begin()->second && dict.begin()->second->data){
    return dict.begin()->second->data;
  }
  return 0;
}

FeedDataPtr FeedContentAdapter::getFeedFromMemcLoader(Ice::Long feedid) {
  MyUtil::LongSeq feedids;
  feedids.push_back(feedid);
  FeedContentDict dict;
  try{
    dict = FeedMemcLoaderAdapter::instance().GetFeedDict(feedids,200);
  }catch(Ice::Exception& e){
    MCE_WARN("FeedContentAdapter::getFeedFromMemcLoader --> feedid:"<<feedid << ", err:"<<e << ", file:"<<__FILE__ << ", line:"<<__LINE__);
    return 0;
  }
  if(dict.empty()){
    return 0;
  }else if(dict.begin()->second && dict.begin()->second->data){
    return dict.begin()->second->data;
  }
  return 0;
}

//void FeedContentAdapter::removeFeedReply(long feedId, long replyId){
//
//	getManagerOneway(feedId)->removeFeedReply(feedId,replyId);
//}

//---------------------------------------------------------------------------
//FeedContentDict ParallelQuery::execute(size_t cluster, int category,
//		const MyUtil::LongSeq& ids, int timeout) {
//
//	_count = 0;
//	_done = false;
//	vector<MyUtil::LongSeq> targets(cluster);
//	for (size_t i = 0; i < ids.size(); ++i) {
//		unsigned int id = (unsigned Ice::Long)ids.at(i) % cluster;
//		targets[id].push_back(ids.at(i));
//	}
//	for (size_t i = 0; i < targets.size(); ++i) {
//		if (!targets[i].empty()) {
//			TaskManager::instance().execute(new CollectTask(this, i, targets[i], timeout));
//			++_count;
//		}
//	}
//	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
//	//StatFunc stat("ParallelQuery::execute -->timeout");
//	if (_count > 0 && !_mutex.timedWait(IceUtil::Time::milliSeconds(timeout))) {
//		MCE_WARN("ParallelQuery::finish -> count: " << _count << " missing");
//	}
//	_done = true;
//	return _result;
//}
//
//void ParallelQuery::finish(const FeedContentDict& data) {
//	IceUtil::Monitor<IceUtil::Mutex>::Lock lock(_mutex);
//	if (_done) {
//		return;
//	}
//	_result.insert(data.begin(), data.end());
//	if (--_count <= 0) {
//		_mutex.notify();
//	}
//}
void CheckTaskSizeTimer::handle(){
	for(int i=0; i<10; i++){
    // MCE_INFO("CheckTaskSizeTimer::handle --> free client " << FeedMemcClient::instance().size(i));
  }

  int size = TaskManager::instance().size(TASK_LEVEL_PARALLEL_GET);
	MCE_INFO("CheckTaskSizeTimer::handle --> size:" << size);
	/*if(size > 1000){
		MCE_INFO("CheckTaskSizeTimer::handle --> switch to FeedContent");
		from_memc_ = true;
	}else if(size==0){
		MCE_INFO("CheckTaskSizeTimer::handle --> switch to FeedMemcProxy");
		from_memc_ = true;
	}*/
}

