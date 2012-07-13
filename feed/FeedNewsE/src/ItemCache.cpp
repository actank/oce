/*
 * ItemCache.cpp
 *
 *  Created on: May 5, 2010
 *      Author: antonio
 */
#include "ItemCache.h"
//#include "IceLogger.h"
#include "FeedInteractionNAdapter.h"
#include "FeedItemI.h"
//#include "PerformanceStat.h"

//using namespace xce::feed;
namespace xce{
namespace feed{

SharedFeedPtr SharedFeedPool::Add(const FeedItem& item) {
  int i = GetIndex(item.feed);
  SharedFeed* f = NULL;
  {
    IceUtil::Mutex::Lock lock(mutex_[i]);

    FeedMap::iterator it = feed_map_[i].find(item.feed);

    if (it == feed_map_[i].end()) {
      f = new SharedFeed(item);
      feed_map_[i].insert(pair<Ice::Long, SharedFeed*> (item.feed, f));
    } else {
      f = it->second;
    }
    ++(f->ref_count_);
  }
  SharedFeedPtr ptr = f;
  {
    IceUtil::Mutex::Lock lock(mutex_[i]);
    --(f->ref_count_);
  }
  return ptr;
}

SharedFeedPtr SharedFeedPool::Get(Ice::Long feedid) {
  int i = GetIndex(feedid);
  SharedFeed* f = NULL;
  {
    IceUtil::Mutex::Lock lock(mutex_[i]);

    FeedMap::iterator it = feed_map_[i].find(feedid);
    if (it == feed_map_[i].end()) {
      return NULL;
    } else {
      ++(it->second->ref_count_);
      f = it->second;
    }
  }
  SharedFeedPtr ptr = f;
  {
    IceUtil::Mutex::Lock lock(mutex_[i]);
    --(f->ref_count_);
  }
  return ptr;
}

void SharedFeedPool::Inc(SharedFeed* f) {
  if (f == NULL) {
    return;
  }
  int i = GetIndex(f->id_);
  IceUtil::Mutex::Lock lock(mutex_[i]);
  ++(f->ref_count_);
}

void SharedFeedPool::Release(SharedFeed* f) {
  if (f == NULL) {
    return;
  }
  int i = GetIndex(f->id_);
  IceUtil::Mutex::Lock lock(mutex_[i]);

  if (--(f->ref_count_) == 0) {
    //MCE_DEBUG("delete feed id:"<< f->id);
    feed_map_[i].erase(f->id_);
    delete f;

  }
}

int SharedFeedPool::TotalCount() {
  int count = 0;
  for(int i=0; i<kPoolSize; i++){
    IceUtil::Mutex::Lock lock(mutex_[i]);
    count += feed_map_[i].size();
  }
  return count;
}

void SharedFeedPool::UpdateInteraction(){
  MCE_INFO("FeedPool::UpdateInteraction --> begin cache size:" << TotalCount());
  int miss_size = 0;
  for(int i=0; i<kPoolSize; i++){
    LongSeq feed_ids;
    set<Ice::Long> idset;
    //map<Ice::Long, int> reply_counts;
    FeedInteractionCountSeq counts;
    {
      IceUtil::Mutex::Lock lock(mutex_[i]);
      hash_map<Ice::Long, SharedFeed*>::iterator it = feed_map_[i].begin();
      for(; it!=feed_map_[i].end(); ++it){
        feed_ids.push_back(it->first);
        idset.insert(it->first);
      }
    }
    //MCE_INFO("FeedPool::UpdateInteraction --> pool " << i << " size:" << feed_ids.size()); 
    LongSeq tmp_ids;
    for(size_t j=0; j<feed_ids.size(); j++){
      //MCE_INFO("@@@@@@@@ " << feed_ids.at(j));
      tmp_ids.push_back(feed_ids.at(j));
      if(tmp_ids.size()==1000 || j==feed_ids.size()-1){
        FeedInteractionCountSeq count_seq;
        try{
          MCE_INFO("FeedPool::UpdateInteraction --> update size:" << tmp_ids.size());
          //TimeStat ts;
          count_seq = FeedInteractionNAdapter::instance().GetFeedInteractionCountSeq(tmp_ids, false);
          //PerformanceStat::instance().stat("QueryInteractionCache", ts.getTime());
          //fdict = FeedInteractionAdapter::instance().getFeedDict(tmp_ids);
        }catch(Ice::Exception& e){
          MCE_WARN("FeedPool::UpdateInteraction --> call FeedInteractionN size:" << tmp_ids.size() << ", " << e);
        }
        counts.insert(counts.end(), count_seq.begin(), count_seq.end());
        tmp_ids.clear();
      }
    }

    {
      IceUtil::Mutex::Lock lock(mutex_[i]);
      for(size_t n=0; n<counts.size(); n++){
        idset.erase(counts.at(n)->feed);
        //set<Ice::Long>::iterator it = idset.find(counts.at(n)->feed);
        //MCE_INFO("@@@Interaction --> feed:" << counts.at(n)->feed << " click:" << counts.at(n)->clickCount << " reply:" << counts.at(n)->replyCount);
        FeedMap::iterator tmp_it = feed_map_[i].find(counts.at(n)->feed);
        if(tmp_it != feed_map_[i].end()){
          tmp_it->second->reply_count_ = counts.at(n)->replyCount;
          tmp_it->second->click_count_ = counts.at(n)->clickCount;
          tmp_it->second->range_ = counts.at(n)->range;
        }
      }
    }
    MCE_INFO("FeedPool::UpdateInteraction --> " << feed_ids.size() << " miss " << idset.size());
    miss_size += idset.size(); 
    set<Ice::Long>::iterator it = idset.begin();
    for(; it!=idset.end(); ++it){
      UpdateInteractionInDBTimer::instance().insert(*it);
    }
  } 
  MCE_INFO("FeedPool::UpdateInteraction --> end miss:" << miss_size);
}

void SharedFeedPool::UpdateInteraction(const LongSeq& feeds, bool load){
  FeedInteractionCountSeq count_seq;
  try{
    MCE_INFO("FeedPool::UpdateInteraction --> load size:" << feeds.size());
    count_seq = FeedInteractionNAdapter::instance().GetFeedInteractionCountSeq(feeds, load);
  }catch(Ice::Exception& e){
    MCE_WARN("FeedPool::UpdateInteraction --> call FeedInteractionN size:" << feeds.size() << ", " << e);
  }
  for(size_t n=0; n<count_seq.size(); n++){
    if(count_seq.at(n)->feed < 0){
      continue;
    }
    int index = count_seq.at(n)->feed%kPoolSize;
    //MCE_INFO("@@@@@@@@@ --> " << count_seq.at(n)->feed << " " << index);
    IceUtil::Mutex::Lock lock(mutex_[index]);
    FeedMap::iterator tmp_it = feed_map_[index].find(count_seq.at(n)->feed);
    if(tmp_it != feed_map_[index].end()){
      tmp_it->second->reply_count_ = count_seq.at(n)->replyCount;
      tmp_it->second->click_count_ = count_seq.at(n)->clickCount;
      tmp_it->second->range_ = count_seq.at(n)->range;
    }
  }
}

};
};
