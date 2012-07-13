/**********************/
/***author: qun.liu****/
/*****2011.04.14******/
/**********************/

#include "feed/feedview/feedcache.h"
#include "IceLogger.h"

namespace xce {
namespace feed {

int FeedCache::GetIndex(int uid) {
  int index = (uid/100) % MAX_FEEDCACHE_SIZE; //目前FeedViewN服务散50份
  return index;
}

bool FeedCache::Add(int uid, feed::FeedDataSeq& feedDataSeq) {
  int index = GetIndex(uid); 
  IceUtil::RWRecMutex::WLock lock(cache_rw_mutex_[index]);
/*
  static bool flag = false;
  static long step ;
  static long temp_uid=  225092473;
  static int cnt =0;
  cnt++;

  uid = temp_uid;

  if (!flag) {
    flag = true;
  } else {
    temp_uid +=   100000000;
    flag = false;
  }

  if (cnt ==8) {
    uid = 225092473;
    MCE_INFO("last");
  }
*/

  CachedData* p_tmpCacheData = NULL;

  for(list<CachedData*>::iterator iter = cacheArray_[index].begin(); iter != cacheArray_[index].end(); ++iter) {
    if((*iter)&&(*iter)->uid == uid) {
      p_tmpCacheData = *iter;
      p_tmpCacheData->feedSeq = feedDataSeq;
      cacheArray_[index].erase(iter);
      cacheArray_[index].push_front(p_tmpCacheData);
      update_count_++;
      MCE_INFO("FeedCache::Add -->  update list.  miss:" << miss_count_ << " hit:" << hit_count_ << " evict:" << evict_count_ << " add:" << add_count_ << " update:" << update_count_ << " uid:" << uid);
      return true;
    }
  }
  
  if(cacheArray_[index].size() < MAX_FEEDCACHE_BUCKET_SIZE) {
    p_tmpCacheData = new CachedData(uid, feedDataSeq);
    if(!p_tmpCacheData) {
      MCE_INFO("FeedCache::Add --> new error.  miss:" << miss_count_ << " hit:" << hit_count_ << " evict:" << evict_count_ << " add:" << add_count_ << " update:" << update_count_ << " uid:" << uid);
      return false;
    }
    add_count_++;
    MCE_INFO("FeedCache::Add -->  new CacheData.  miss:" << miss_count_ << " hit:" << hit_count_ << " evict:" << evict_count_ << " add:" << add_count_ << " update:" << update_count_ << " uid:" << uid);
    cacheArray_[index].push_front(p_tmpCacheData);
  }
  else {
    if (cacheArray_[index].empty()) {
      MCE_INFO("FeedCache::Add -->  evict error. Bucket Size is 0!  miss:" << miss_count_ << " hit:" << hit_count_ << " evict:" << evict_count_ << " add:" << add_count_ << " update:" << update_count_ << " uid:" << uid);
      return false;
    }
 
    p_tmpCacheData = (CachedData*)cacheArray_[index].back();
    if(p_tmpCacheData) {
      cacheArray_[index].pop_back();
      p_tmpCacheData->uid = uid;
      p_tmpCacheData->feedSeq = feedDataSeq;
      cacheArray_[index].push_front(p_tmpCacheData);
      evict_count_++;
      MCE_INFO("FeedCache::Add -->  evict CacheData.  miss:" << miss_count_ << " hit:" << hit_count_ << " evict:" << evict_count_ << " add:" << add_count_ << " update:" << update_count_ << " uid:" << uid);
    }
    else {
      MCE_INFO("FeedCache::Add -->  evict error.  miss:" << miss_count_ << " hit:" << hit_count_ << " evict:" << evict_count_ << " add:" << add_count_ << " update:" << update_count_ << " uid:" << uid);
      cacheArray_[index].pop_back();
      return false;
    }
  }
  
  return true;
}

bool FeedCache::Get(int uid, feed::FeedDataSeq& feedDataSeq) {
  int index = GetIndex(uid); 
  IceUtil::RWRecMutex::RLock lock(cache_rw_mutex_[index]);

  for(list<CachedData*>::iterator iter = cacheArray_[index].begin(); iter != cacheArray_[index].end(); ++iter) {
    if (!(*iter)) {
      MCE_INFO("FeedCache::Get --> get cache data is NULL!  miss:" << miss_count_ << " hit:" << hit_count_ << " evict:" << evict_count_ << " add:" << add_count_ << " update:" << update_count_ << " uid:" << uid);
      cacheArray_[index].erase(iter);
      return false;
    }
    if((*iter)->uid == uid) {
      feedDataSeq = (*iter)->feedSeq;
      hit_count_++;
      MCE_INFO("FeedCache::Get --> get hit.  miss:" << miss_count_ << " hit:" << hit_count_ << " evict:" << evict_count_ << " add:" << add_count_ << " update:" << update_count_ << " uid:" << uid);
      return true;
    }
  }

  miss_count_++;
  MCE_INFO("FeedCache::Get --> get miss.  miss:" << miss_count_ << " hit:" << hit_count_ << " evict:" << evict_count_ << " add:" << add_count_ << " update:" << update_count_ << " uid:" << uid);
  return false;
}

}
}
