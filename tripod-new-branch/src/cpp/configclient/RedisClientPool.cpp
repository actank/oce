#include <RedisClientPool.h>

#include <sstream>
#include <boost/foreach.hpp>

namespace com {
namespace renren {
namespace tripod {

RedisClient::RedisClient(const::string& address, int port): address_(address), port_(port), conn_(0), valid_(false) {
  conn_ = redisConnect(address_.c_str(), port_);
  if (conn_->err) {
    set_valid(false);
    conn_ = 0;
  }
  else {
    set_valid(true);
  }
}

RedisClient::~RedisClient() {
 if (conn_) {
   redisFree(conn_);
 }
}

StrList RedisClient::GetList(const std::string& cache_key, int begin, int limit) {
  StrList res;
  int start_pos = begin;
  int stop_pos;
  if (limit == -1) {
    stop_pos = limit;
  }
  else {
    stop_pos = (begin + limit - 1);
  }
  std::ostringstream otem;
  otem << "LRANGE " << cache_key << " " << start_pos << " " << stop_pos;
  std::string command = otem.str();

  MCE_DEBUG("RedisClient::GetList() command:" << command);

  redisReply* reply = 0;
  reply = (redisReply*) redisCommand(conn_, command.c_str());
  if (!reply) {
    return res;
  }
  if (reply->type != REDIS_REPLY_ARRAY) {
    return res;
  }
  for (size_t j = 0; j < reply->elements; ++j) {
    std::string str(reply->element[j]->str, reply->element[j]->str + reply->element[j]->len);
    res.push_back(str);
  }
  freeReplyObject(reply);
  return res;
}

IntList RedisClient::GetIntList(const std::string& cache_key, int begin, int limit) {
  IntList res;
  int start_pos = begin;
  int stop_pos;
  if (limit == -1) {
    stop_pos = limit;
  }
  else {
    stop_pos = (begin + limit - 1);
  }
  std::ostringstream otem;
  otem << "LRANGE " << cache_key << " " << start_pos << " " << stop_pos;
  std::string command = otem.str();

  MCE_DEBUG("RedisClient::GetIntList() command:" << command);

  redisReply* reply = 0;
  reply = (redisReply*) redisCommand(conn_, command.c_str());
  if (!reply) {
    return res;
  }
  if (reply->type != REDIS_REPLY_ARRAY) {
    return res;
  }
  for (size_t j = 0; j < reply->elements; ++j) {
    int int_value = atoi(reply->element[j]->str);
    res.push_back(int_value);
  }
  freeReplyObject(reply);
  return res;
}

bool RedisClient::RemoveList(const std::string& cache_key) {
  std::ostringstream otem;
  otem << "DEL " << cache_key;
  std::string command = otem.str();
  MCE_DEBUG("RedisClient::RemoveList() command:" << command);
  redisReply* reply = 0;
  reply = (redisReply*) redisCommand(conn_, command.c_str());
  if (!reply) {
    return false;
  }
  if (reply->type == REDIS_REPLY_ERROR) {
    return false;
  }
  freeReplyObject(reply);
  return true;
}

bool RedisClient::SetList(const std::string& cache_key, const StrList& list) {
  std::ostringstream otem;
  otem << "RPUSH " << cache_key << " ";
  BOOST_FOREACH(const std::string& str, list) {
    otem << str << " ";
  }
  std::string command = otem.str();
  MCE_DEBUG("RedisClient::SetList() command:" << command);
  redisReply* reply = 0;
  reply = (redisReply*) redisCommand(conn_, command.c_str());
  if (!reply) {
    return false;
  }
  if (reply->type == REDIS_REPLY_ERROR) {
    return false;
  }
  freeReplyObject(reply);
  return true;
}


//---------
RedisClientPool::RedisClientPool(const std::string& address, int port, size_t core_size, size_t max_size): address_(address), port_(port), core_size_(core_size), max_size_(max_size) {
  used_ = 0;
  MCE_INFO("RedisClientPool::RedisClientPool() address:" << address_ << "\tport:" << port << "\tcore_size:" << core_size
           << "\tmax_size:" << max_size);
}

RedisClientPool::~RedisClientPool() {
  MCE_DEBUG("RedisClientPool::~RedisClientPool");
  boost::mutex::scoped_lock lock(un_used_mutex_);
  std::deque<RedisClient*>::iterator iter = un_used_.begin();
  for (; iter != un_used_.end(); ++iter) {
    Destroy(*iter);
  }
}

RedisClient* RedisClientPool::BorrowItem() {
  RedisClient* client = NULL;
  bool create_new = false;
  {
      boost::mutex::scoped_lock lock(un_used_mutex_);
      if (un_used_.empty() && (used_ < max_size_)) {
          create_new = true;
      } else {
          client = un_used_.front();
          un_used_.pop_front();
      }
  }
  if (create_new) {
      client = Create();
  }
  if (client != NULL) {
      boost::mutex::scoped_lock lock(un_used_mutex_);
      used_++;
  }
  return client;
}

void RedisClientPool::ReturnItem(RedisClient* redis_client) {
  boost::mutex::scoped_lock lock(un_used_mutex_);
  un_used_.push_back(redis_client);
  used_--;

}
RedisClient* RedisClientPool::Create() {
  RedisClient* client = new RedisClient(address_, port_);
  if (client->IsValid()) {
    return client;
  }
  return NULL;
}

void RedisClientPool::Destroy(RedisClient* redis_client) {
  if (redis_client) {
    delete redis_client;
  }
}

}}} // end com::renren::tripod

