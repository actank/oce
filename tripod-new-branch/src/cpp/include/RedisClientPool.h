#ifndef _TRIPOD_REDIS_CLIENT_POOL_H__
#define _TRIPOD_REDIS_CLIENT_POOL_H__

#include <deque>
#include <boost/thread/mutex.hpp>

#include <Common.h>
#include <hiredis.h>
#include <TripodData.h>

namespace com {
namespace renren {
namespace tripod {


class RedisClient {
 public:
  RedisClient(const std::string&, int);
  virtual ~RedisClient();
  bool IsValid() {
    return valid_ == true;
  }
  void set_valid(bool v) {
    valid_ = v;
  }
  StrList GetList(const std::string& cache_key, int begin = 0, int limit = -1);
  IntList GetIntList(const std::string& cache_key, int begin = 0, int limit = -1);
  
  bool RemoveList(const std::string& cache_key);
  bool SetList(const std::string& cache_key, const StrList& list);

 private:
  std::string address_;
  int port_;
  redisContext* conn_;
  bool valid_;
};

class RedisClientPool {
 public:
  RedisClientPool(const std::string& address, int port, size_t core_size = 10, size_t max_size = 100);
  virtual ~RedisClientPool();
 
  RedisClient* BorrowItem();
  void ReturnItem(RedisClient*);
  RedisClient* Create();
  void Destroy(RedisClient*);

 private:
  std::string address_;
  int port_;
 
  size_t core_size_;
  size_t max_size_;
 
  boost::mutex un_used_mutex_;
  std::deque<RedisClient*> un_used_;
  size_t used_;
};

}}} // end com::renren::tripod
#endif

