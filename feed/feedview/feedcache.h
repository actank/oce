/**********************/
/***author: qun.liu****/
/*****2011.04.14******/
/**********************/

#ifndef XCE_FEED_CACHE_FEED_H__
#define XCE_FEED_CACHE_FEED_H__

#include <list>
#include "Singleton.h"
#include <Ice/Ice.h>
#include <IceUtil/IceUtil.h>
#include "RFeed.h"


namespace xce {
namespace feed {

const int MAX_FEEDCACHE_SIZE = 50000;
const int MAX_FEEDCACHE_BUCKET_SIZE = 2;

struct CachedData {
  int uid;
  feed::FeedDataSeq feedSeq;
  CachedData(int id, feed::FeedDataSeq seq):uid(id),feedSeq(seq){}
};

class FeedCache: public MyUtil::Singleton<FeedCache> {
public:
  bool Add(int uid, feed::FeedDataSeq& feedDataSeq);
  bool Get(int uid, feed::FeedDataSeq& feedDataSeq);
  
  FeedCache():miss_count_(0), hit_count_(0), evict_count_(0), add_count_(0), update_count_(0) { }
  ~FeedCache() {
    for(int i=0;i<MAX_FEEDCACHE_SIZE;i++) {
      for(list<CachedData*>::iterator iter = cacheArray_[i].begin(); iter != cacheArray_[i].end(); ++iter) {
        if(*iter) {
          delete (*iter);
        }
      }
    }
  }
private:
  int GetIndex(int uid);
private:
  IceUtil::RWRecMutex cache_rw_mutex_[MAX_FEEDCACHE_SIZE];
  list<CachedData*> cacheArray_[MAX_FEEDCACHE_SIZE];
  long long int miss_count_;
  long long int hit_count_;
  long long int evict_count_;
  long long int add_count_;
  long long int update_count_;

};

}
}

#endif //XCE_FEED_CACHE_FEED_H__
