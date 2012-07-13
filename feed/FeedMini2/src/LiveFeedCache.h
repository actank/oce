/*
 * LiveFeedCache.h
 *
 *  Created on: 2010-10-21
 *      Author: wu
 */

#ifndef LIVEFEEDCACHE_H_
#define LIVEFEEDCACHE_H_

#include <vector>
#include <string>
#include "RFeed.h"
#include "IceLogger.h"
#include "QueryRunner.h"

namespace xce
{
namespace feed
{

using namespace std;
using namespace MyUtil;
using namespace __gnu_cxx;

const int PageFeedCount = 10;//一页显示新鲜事的条数
const unsigned int LoadFeedCount = PageFeedCount + 10;//一个用户在内存中保存的新鲜事最少条数

class LiveFeedItem{
public:
  LiveFeedItem(const FeedItem& item){
    id_ = item.feed;
    merge_ = item.merge;
    type_ = (unsigned int) item.type;
    actor_ = item.actor;
    time_ = item.time;
  }

  LiveFeedItem(){}

	  inline Ice::Long feed_id() const{
	    return id_;
	  }
	  inline Ice::Long merge() const{
	    return merge_;
	  }

	  inline int BigType() const {
	    return (int) ((type_ & 0xFFFF) / 100) * 100;
	  }
	  ;
	  inline int SmallType() const {
	    return type_ & 0xFFFF;
	  }
    inline int Ver(){
      return (type_&0xFF0000)>>16;
    }
	  inline int VerAndSmallType(){
      return type_ & 0xFFFFFF;
    } 
    inline int NewsMergeType() const {
	    return (((unsigned int)type_) >> 30);
	  }
	  ;
	  inline int MiniMergeType() const {
	    return (type_ & 0x3FFFFFFF) >> 28;
	  }

	  Ice::Long id_;
	  Ice::Long merge_;
	  int type_;
	  time_t time_;
	  int actor_;
};

class LiveFeed{
public:
  LiveFeed(){
    //need_load_ = true;
    time_oldest_ = 0x7FFFFFFF;
  }
  void AddFeedItem(const FeedItem& item);
  void AddNewFeedItem(const FeedItem& item);
  vector<LiveFeedItem> RemoveById(Ice::Long feed_id, bool re = true);
  vector<LiveFeedItem> Remove(int stype, Ice::Long merge);
  void RemoveAll();
  LiveFeedItem GetItem(Ice::Long);
  vector<FeedItem> GetAllItems();
  vector<vector<FeedItem> >               GetLatestItems(int uid, int limit);
  vector<vector<vector<LiveFeedItem> > >  GetFeed(int uid, int begin, int limit);
  vector<vector<vector<LiveFeedItem> > >  GetFeedByType(int uid, const vector<int> types, int begin, int limit);
  vector<vector<vector<LiveFeedItem> > >  GetFeedByStype(int uid, const vector<int> types, int begin, int limit);
 
  void  Clean();
  int   Size(){
    return feeds_.size();
  }
  int GetOldestTime(){
    return time_oldest_;
  }
  void SetOldestTime(int uid, int t){
    time_oldest_ = t;
    MCE_INFO("LiveFeed::SetOldestTime --> set oldest time uid:" << uid << " time:" << time_oldest_);
  }
private:
  void MergeFeed(int uid, vector<vector<LiveFeedItem*> >& merge_feeds, multimap<Ice::Long, int>& order_by_time) const;
private:
  int                   time_oldest_;
  vector<LiveFeedItem>  feeds_;
};

}
}

#endif /* LIVEFEEDCACHE_H_ */
