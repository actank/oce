/*
 * ItemCache.h
 *
 *  Created on: Apr 26, 2010
 *      Author: antonio
 */

#ifndef ITEMCACHE_H_
#define ITEMCACHE_H_

#include <ext/hash_map>
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include <Singleton.h>
#include "RFeed.h"

namespace xce {
namespace feed {

using namespace std;
using namespace __gnu_cxx;
using namespace MyUtil;

class Feed;
typedef hash_map<Ice::Long, Feed*> FeedMap;
typedef IceUtil::Handle<Feed> FeedPtr;
typedef vector<FeedPtr> FeedSeq;

class FeedPool : public MyUtil::Singleton<FeedPool>{
public:
  static const int kPoolSize = 1000;
  FeedPtr Add(const FeedItem& item);
  FeedPtr Get(Ice::Long feed_id);
  void    Inc(Feed* feed);
  void    Release(Feed* feed);
  int     TotalCount();
  void    UpdateInteraction();
  void    UpdateInteraction(const LongSeq& feeds, bool load = true);
private:
  int GetIndex(Ice::Long feed_id) {
    feed_id = feed_id > 0 ? feed_id : (0 - feed_id);
    return feed_id % kPoolSize;
  }

  FeedMap feed_map_[kPoolSize];
  IceUtil::Mutex mutex_[kPoolSize];
};

class Feed {
public:
  Feed(const Feed& f) {
    id_ = f.id_;
    merge_ = f.merge_;
    type_ = f.type_;
    actor_ = f.actor_;
    time_ = f.time_;
    ref_count_ = 0;
    click_count_ = 0;
    reply_count_ = 0;
    range_ = 0;
  }
  Feed(const FeedItem& item) {
    id_ = item.feed;
    merge_ = item.merge;
    type_ = (unsigned int) item.type;
    actor_ = item.actor;
    time_ = item.time;
    ref_count_ = 0;
    click_count_ = 0;
    reply_count_ = 0;
    range_ = 0;
  }
  inline Ice::Long feed_id(){
    return id_;
  }
  inline Ice::Long merge(){
    return merge_;
  }

  inline int BigType() const {
    return (int) ((type_ & 0xFFFF) / 100) * 100;
  }
  ;
  inline int SmallType() const {
    return type_ & 0xFFFF;
  }
  inline int VerAndSmallType(){
    return type_ & 0xFFFFFF;
  } 
  inline int Ver(){
    return (type_&0xFF0000)>>16;
  }
  inline int NewsMergeType() const {
    return (((unsigned int)type_) >> 30);
  }
  ;
  inline int MiniMergeType() const {
    return (type_ & 0x3FFFFFFF) >> 28;
  }
  inline int EnterTime(){
    return 0;
  }
  inline int HitCount(){
    return click_count_;
  }
  inline int ReplyCount(){
    return reply_count_;
  }
  inline pair<int, int> GatherKey(){
    if(SmallType() == 2002){
      return make_pair<int, int>(0, SmallType());
    }else{
      return make_pair<int, int>(actor_, SmallType());
    }
  }
  inline int actor(){
    return actor_;
  }
  //-------------------------------------------------------------------------
  void __incRef() {
    FeedPool::instance().Inc(this);
  }
  void __decRef() {
    FeedPool::instance().Release(this);
  }
  int __getRef() const {
    return 0;
  }
  void __setNoDelete(bool) {
  }
  //-------------------------------------------------------------------------
  Ice::Long id_;
  Ice::Long merge_;
  int type_;
  //int weight_;
  time_t time_;
  int actor_;
  int click_count_;
  int reply_count_;
  int ref_count_;
  int range_;
};

}
}

#endif /* ITEMCACHE_H_ */
