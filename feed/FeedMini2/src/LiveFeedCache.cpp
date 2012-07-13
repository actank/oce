/*
 * LiveFeedCache.cpp
 *
 *  Created on: 2010-10-21
 *      Author: wu
 */

#include "LiveFeedCache.h"
#include "IceLogger.h"
#include "FeedContentAdapter.h"
#include <cmath>
#include "QueryRunner.h"


using namespace xce::feed;
using namespace MyUtil;

void LiveFeed::AddFeedItem(const FeedItem& item) {
	LiveFeedItem live_item(item);
	feeds_.push_back(live_item);
  if(time_oldest_ > item.time){
    time_oldest_ = item.time;
  }
}

void LiveFeed::AddNewFeedItem(const FeedItem& item) {
	LiveFeedItem live_item(item);
  vector<LiveFeedItem>::iterator begin_pos = feeds_.begin();
  feeds_.insert(begin_pos, live_item);
}

vector<LiveFeedItem> LiveFeed::Remove(int stype, Ice::Long merge) {
  vector<LiveFeedItem> res;
  vector<LiveFeedItem>::iterator it = feeds_.begin();
  while(it!= feeds_.end()) {
    if(it->SmallType() == stype && it->merge() == merge)
    {
      res.push_back(*it);
      it = feeds_.erase(it);
    }else{
      it++;
    }
  }
  return res;
}

void LiveFeed::MergeFeed(int uid, vector<vector<LiveFeedItem*> >& merge_feeds, multimap<Ice::Long, int>& order_by_time) const{  
  vector<int> update_times;
  map<pair<int, Ice::Long>, int> merge_index;
  for(int i=0; i<feeds_.size(); i++){
    LiveFeedItem* litem = const_cast<LiveFeedItem*>(&feeds_.at(i));
    map<pair<int, Ice::Long>, int>::iterator it = merge_index.find(make_pair<int, Ice::Long>(litem->VerAndSmallType(), litem->merge()));
    if(it != merge_index.end()){
      if(litem->MiniMergeType() == FeedMergeReplace ||
      (litem->MiniMergeType() == FeedMergeAppend && merge_feeds[it->second].size()>=5)){
        continue;  
      }
      merge_feeds[it->second].push_back(litem);
      //update_times[it->second] = litem->time_;
    }else{
        merge_feeds.push_back(vector<LiveFeedItem*>());
        merge_feeds.back().push_back(litem);
        update_times.push_back(litem->time_);
        merge_index.insert(make_pair<pair<int, Ice::Long>, int>(make_pair<int, Ice::Long>(litem->VerAndSmallType(), litem->merge()), merge_feeds.size()-1));
    }
  }
  for(size_t i=0; i<update_times.size(); i++){
    order_by_time.insert(make_pair<Ice::Long, int>(update_times.at(i), i));
  }
}

vector<LiveFeedItem> LiveFeed::RemoveById(Ice::Long feed_id, bool re){
  Ice::Long merge = 0;
  int stype = 0;
  for (vector<LiveFeedItem>::iterator it = feeds_.begin(); it!=feeds_.end(); it++) {
    if(it->feed_id()==feed_id){
      merge = it->merge();
      stype = it->SmallType();
      if(!re){
        vector<LiveFeedItem> seq;
        seq.push_back(*it);
        feeds_.erase(it);
        return seq;
      }
      break;
    }
  }
  return Remove(stype, merge); 
}

void LiveFeed::RemoveAll(){
  vector<LiveFeedItem>().swap(feeds_);
}

void LiveFeed::Clean(){
  int bsize = feeds_.size();
  int count = feeds_.size() - LoadFeedCount;
  for(int i=0; i<count; i++){
    feeds_.pop_back();
  }
  int esize = feeds_.size();
  int time = time_oldest_;
  if(esize > 0 && time_oldest_ < feeds_.back().time_){
    time_oldest_ = feeds_.back().time_;
  }
  MCE_INFO("LiveFeed::Clean --> set oldest time uid:" << feeds_.back().actor_ 
  << " oldtime:" << time << " nowtime:" << time_oldest_ << " size:" << bsize << " " << esize);
}

vector<FeedItem> LiveFeed::GetAllItems(){
  vector<FeedItem> items;
  for(size_t i=0; i<feeds_.size(); i++){
    FeedItem item;
    item.feed = feeds_.at(i).feed_id();
    item.actor = feeds_.at(i).actor_;
    item.merge = feeds_.at(i).merge_;
    item.time = feeds_.at(i).time_;
    item.type = feeds_.at(i).type_;  
    items.push_back(item);
  }
  return items;
}

vector<vector<FeedItem> > LiveFeed::GetLatestItems(int uid, int limit){
  vector<vector<FeedItem> > items;
  vector<vector<LiveFeedItem*> > merge_feeds;
  multimap<Ice::Long, int> order_by_time;
  MergeFeed(uid, merge_feeds, order_by_time);
  int c = 0;
  for(multimap<Ice::Long, int>::reverse_iterator it=order_by_time.rbegin(); it!=order_by_time.rend(); c++, ++it){
    if(c >= limit){
      break;
    }
    items.push_back(vector<FeedItem>());
    for(size_t i=0; i<merge_feeds.at(it->second).size(); i++){
      //MCE_INFO("@@@ --> " << merge_feeds.at(it->second).at(i)->feed_id() << " " << it->second << " " << i);
      FeedItem item;
      item.feed = merge_feeds.at(it->second).at(i)->feed_id();
      item.actor = merge_feeds.at(it->second).at(i)->actor_;
      item.merge = merge_feeds.at(it->second).at(i)->merge_;
      item.time = merge_feeds.at(it->second).at(i)->time_;
      item.type = merge_feeds.at(it->second).at(i)->type_;  
      items.back().push_back(item);
    }
  }
  return items;
}

vector<vector<vector<LiveFeedItem> > >  LiveFeed::GetFeed(int uid, int begin, int limit){
  vector<vector<vector<LiveFeedItem> > > res;
  vector<vector<LiveFeedItem*> > merge_feeds;
  multimap<Ice::Long, int> order_by_time;
  MergeFeed(uid, merge_feeds, order_by_time);
  MCE_INFO("LiveFeed::GetFeed --> merge uid:" << uid << " size:" << merge_feeds.size());
  int c = 0;
  for(multimap<Ice::Long, int>::reverse_iterator it=order_by_time.rbegin(); it!=order_by_time.rend(); c++, ++it){
    if(c < begin){
      continue;
    }else if(c >= begin+limit){
      break;
    }
    res.push_back(vector<vector<LiveFeedItem> >());
    res.back().push_back(vector<LiveFeedItem>());
    for(size_t i=0; i<merge_feeds.at(it->second).size(); i++){
      res.back().back().push_back(*merge_feeds.at(it->second).at(i));
    }
  }
  return res;
}

vector<vector<vector<LiveFeedItem> > >  LiveFeed::GetFeedByType(int uid, const vector<int> types, int begin, int limit){
  set<int> type_set;
  for (size_t i = 0; i < types.size(); ++i) {
    type_set.insert(types.at(i));
  }
  vector<vector<vector<LiveFeedItem> > > res;
  vector<vector<LiveFeedItem*> > merge_feeds;
  multimap<Ice::Long, int> order_by_time;
  MergeFeed(uid, merge_feeds, order_by_time);
  MCE_INFO("LiveFeed::GetFeedByType --> merge uid:" << uid << " size:" << merge_feeds.size());
  int c = 0;
  for(multimap<Ice::Long, int>::reverse_iterator it=order_by_time.rbegin(); it!=order_by_time.rend(); ++it){
    int type = merge_feeds.at(it->second).front()->BigType();
    if(type_set.count(type) > 0){
      if((c++) < begin){
        continue;
      }else if(c > begin+limit){
        break;
      }
      res.push_back(vector<vector<LiveFeedItem> >());
      res.back().push_back(vector<LiveFeedItem>());
      for(size_t i=0; i<merge_feeds.at(it->second).size(); i++){
        res.back().back().push_back(*merge_feeds.at(it->second).at(i));
      }
    }
  }
  return res;
}

vector<vector<vector<LiveFeedItem> > >  LiveFeed::GetFeedByStype(int uid, const vector<int> types, int begin, int limit){
  set<int> type_set;
  for (size_t i = 0; i < types.size(); ++i) {
    type_set.insert(types.at(i));
  }
  vector<vector<vector<LiveFeedItem> > > res;
  vector<vector<LiveFeedItem*> > merge_feeds;
  multimap<Ice::Long, int> order_by_time;
  MergeFeed(uid, merge_feeds, order_by_time);
  MCE_INFO("LiveFeed::GetFeedByType --> merge uid:" << uid << " size:" << merge_feeds.size());
  int c = 0;
  for(multimap<Ice::Long, int>::reverse_iterator it=order_by_time.rbegin(); it!=order_by_time.rend(); ++it){
    int type = merge_feeds.at(it->second).front()->SmallType();
    if(type_set.count(type) > 0){
      if((c++) < begin){
        continue;
      }else if(c > begin+limit){
        break;
      }
      res.push_back(vector<vector<LiveFeedItem> >());
      res.back().push_back(vector<LiveFeedItem>());
      for(int i=0; i<merge_feeds.at(it->second).size(); i++){
        res.back().back().push_back(*merge_feeds.at(it->second).at(i));
      }
    }
  }
  return res;
}
