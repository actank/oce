#include <RedisClientPool.h>

#include <sstream>
#include <boost/foreach.hpp>
#include "base/stringdigest.h"

namespace com {
namespace renren {
namespace tripod {

const int RedisClient::kWriteBatchSize = 100;
const int RedisClient::kReadBatchSize = 100;

RedisClient::RedisClient(const::string& address, int port, int timeout): address_(address), port_(port), timeout_(timeout), conn_(0), valid_(false) {
  struct timeval tv;
  tv.tv_sec = timeout / 1000;
  tv.tv_usec = 0;
  conn_ = redisConnectWithTimeout(address_.c_str(), port_, tv);
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

int RedisClient::GetListSize(const std::string& cache_key) {
  std::ostringstream otem;
  otem << "LLEN " << cache_key;
  std::string command = otem.str();
  MCE_DEBUG("RedisClient::GetListSize() command:" << command);
  redisReply* reply = 0;
  reply = (redisReply*) redisCommand(conn_, command.c_str());
  if (!reply) {
    return -1;
  }
  if (reply->type == REDIS_REPLY_INTEGER) {
    return reply->integer;
  }
  return 0;
}


bool RedisClient::Remove(const std::string& cache_key) {
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
    freeReplyObject(reply);
    return false;
  }
  freeReplyObject(reply);
  return true;
}

bool RedisClient::GetList(const std::string& cache_key, StrList& value, int begin, int limit) {
  if (begin < 0) {
    return false;
  }
  if (limit <= 0) {
    return false;
  }
  int start_pos = begin;
  int stop_pos = start_pos + limit - 1;
  while (start_pos <= stop_pos) {
    std::ostringstream otem;
    otem << "LRANGE " << cache_key << " ";
    int batch_end = start_pos + kReadBatchSize - 1;
    batch_end = batch_end < stop_pos ? batch_end : stop_pos;
    otem << start_pos << " " << batch_end;

    std::string command = otem.str();
    MCE_DEBUG("RedisClient::GetList() command:" << command);
    // do redis command 
    redisReply* reply = 0;
    reply = (redisReply*) redisCommand(conn_, command.c_str());
    if (!reply) {
      return false;
    }
    if (reply->type != REDIS_REPLY_ARRAY) {
      freeReplyObject(reply);
      return false;
    }
    size_t fetch_size = reply->elements;
    if (fetch_size > 0) {
      for (size_t i = 0; i < fetch_size; ++i) {
        value.push_back(reply->element[i]->str);
      }
    }
    if (fetch_size == kReadBatchSize) {
      start_pos = ++batch_end;
    }
    if (fetch_size >= 0 && fetch_size < kReadBatchSize) {
      start_pos = stop_pos + 1;
    }
    freeReplyObject(reply);
  } 
  return true;
}

bool RedisClient::SetList(const std::string& cache_key, const StrList& list) {
  // 1. generate temp key
  // 2. set list by batch
  // 3. rename 
  if (list.empty()) {
    return true;
  }
  const std::string temp_key = GeneratorTempKey(cache_key);

  MCE_DEBUG("cache_key:" << cache_key << "\ttemp_key:" << temp_key);

  StrList::const_iterator iter = list.begin();
  while (iter != list.end()) {
    std::ostringstream otem;
    otem << "RPUSH " << temp_key << " ";
    for (int i = 0; i < kWriteBatchSize && iter != list.end(); ++i) {
     otem << *iter++ << " ";
    }
    std::string command = otem.str();
    MCE_DEBUG("RedisClient::SetList() command:" + command);
    redisReply* reply = 0;
    reply = (redisReply*) redisCommand(conn_, command.c_str());
    if (reply) {
      freeReplyObject(reply);
    }
  }
  return Rename(temp_key, cache_key);
}

std::string RedisClient::GeneratorTempKey(const std::string cache_key) {
  return xce::MD5(cache_key);
}

bool RedisClient::GetHash(const std::string& cache_key, const StrList& fields, HashPair& value) {
  if (fields.empty()) {
    return true;
  }
  std::ostringstream otem;
  otem << "HMGET " << cache_key << " ";
  BOOST_FOREACH(const std::string& field, fields) {
    otem << field << " ";
  }
  std::string command = otem.str();
  MCE_DEBUG("RedisClient::GetHash() command:" << command);
  redisReply* reply = 0;
  reply = (redisReply*) redisCommand(conn_, command.c_str());
  if (!reply) {
    return false;
  }

  if (reply->type != REDIS_REPLY_ARRAY) {
    freeReplyObject(reply);
    return false;
  }

  StrList::const_iterator iter = fields.begin();
  for (size_t i = 0; i < reply->elements && iter != fields.end(); ++i, ++iter) {
    if (reply->element[i]->str != 0) {
      value.insert(std::make_pair(*iter, reply->element[i]->str));
    }
  }  

  freeReplyObject(reply);
  return true;
}

bool RedisClient::SetHash(const std::string& cache_key, const HashPair& hash_pair) {
  if (hash_pair.empty()) {
    return true;
  }
  // 1. generate temp key
  // 2. set hash by batch
  // 3. rename 
  const std::string temp_key = GeneratorTempKey(cache_key);

  MCE_DEBUG("cache_key:" << cache_key << "\ttemp_key:" << temp_key);

  HashPair::const_iterator iter = hash_pair.begin();
  while (iter != hash_pair.end()) {
    std::ostringstream otem;
    otem << "HMSET " << temp_key << " ";
    for (int i =0; i < kWriteBatchSize && iter != hash_pair.end(); ++i, ++iter) {
      otem << iter->first << " " << iter->second << " ";
    }  
    std::string command = otem.str();
    MCE_DEBUG("RedisClient::SetHash() command:" << command);
    redisReply* reply = 0;
    reply = (redisReply*) redisCommand(conn_, command.c_str());
    if (reply) {
      freeReplyObject(reply);
    }
  }
  return Rename(temp_key, cache_key);
}

bool RedisClient::Rename(const std::string& old_name, const std::string& new_name) {
  std::ostringstream otem;
  otem << "RENAME " << old_name << " " << new_name;
  std::string command = otem.str();
  MCE_DEBUG("RedisClient::Rename() command:" << command);
  redisReply* reply = 0;
  reply = (redisReply*) redisCommand(conn_, command.c_str());
  if (!reply) {
    return false;
  }
  if (reply->type == REDIS_REPLY_ERROR) {
    freeReplyObject(reply);
    return false;
  }
  freeReplyObject(reply);
  return true;
}


//---------
RedisClientPool::RedisClientPool(const std::string& address, int port, size_t core_size, size_t max_size): address_(address), port_(port), core_size_(core_size), max_size_(max_size) {
  used_ = 0;
  MCE_DEBUG("RedisClientPool::RedisClientPool() address:" << address_ << "\tport:" << port << "\tcore_size:" << core_size
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

