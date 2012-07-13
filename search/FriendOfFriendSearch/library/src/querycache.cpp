#include "../include/common.h"
#include "../include/userdata.h"
#include "../include/querycache.h"

#include <libmemcached/memcached.h>

NAMESPACE_HEADER

FriendOfFriendQueryCache::FriendOfFriendQueryCache(time_t expiration) : memcache_(expiration)
{
}

FriendOfFriendQueryCache::~FriendOfFriendQueryCache()
{
}

bool FriendOfFriendQueryCache::SetServers(const char *servers_string)
{
  return this->memcache_.SetServers(servers_string);
}

bool FriendOfFriendQueryCache::GetCache(const char *query_key, Cache &cache)
{
  char *value = NULL;
  size_t value_len = 0;

  value = this->memcache_.Get(query_key, value_len);
  if (value == NULL) {
    MCE_FATAL("Unable to fetch the cache with key " << query_key);
    return false;
  }

  if (value_len < sizeof(Cache::CacheData)) {
    MCE_FATAL("The size of the fetched data is too small: " << value_len);
    free(value);
    return false;
  }

  Cache::CacheData *data = (Cache::CacheData*)value;
  if (data->version != VERSION_1 || data->size != value_len) {
    MCE_FATAL("The header of the fetched data is error!");
    free(value);
    return false;
  }

  if (cache.data_ != NULL) {
    free(cache.data_);
  }
  cache.data_ = data;

  return true;
}

bool FriendOfFriendQueryCache::SetCache(const char *query_key, const Cache &cache)
{
  if (cache.data_ == NULL) {
    MCE_FATAL("Unable to store an empty cache!");
    return false;
  }

  if (!this->memcache_.Set(query_key, (const char*)cache.data_, cache.data_->size)) {
    MCE_FATAL("Unable to store the cache with key " << query_key);
    return false;
  }

  return true;
}

bool FriendOfFriendQueryCache::EraseCache(const char *query_key)
{
  return this->memcache_.Erase(query_key);
}

FriendOfFriendQueryCache::Cache::Cache()
{
  this->data_ = NULL;
}

FriendOfFriendQueryCache::Cache::Cache(uint32_t total_hits, const UserData *users, uint32_t result_hits)
{
  uint32_t size = sizeof(CacheData) + result_hits * sizeof(UserData);
  CacheData *data = (CacheData*)malloc(sizeof(CacheData) + result_hits * sizeof(UserData));

  if (data == NULL) {
    MCE_FATAL("Unable to allocate a memory block with " << size << "bytes.");
    this->data_ = NULL;
  } else {
    data->version = VERSION_1;
    data->size = size;
    data->result_hits = result_hits;
    data->total_hits = total_hits;
    memcpy(data->users, users, sizeof(UserData) * result_hits);
    this->data_ = data;
  }
}

FriendOfFriendQueryCache::Cache::~Cache()
{
  if (this->data_ != NULL) {
    free(this->data_);
  }
}

bool FriendOfFriendQueryCache::Cache::GetResult(const UserData* &users, uint32_t &result_hits) const
{
  if (this->data_ == NULL) {
    MCE_FATAL("Unable to get any data from an empty cache.");
    return false;
  }

  users = this->data_->users;
  result_hits = this->data_->result_hits;
  return true;
}

uint32_t FriendOfFriendQueryCache::Cache::GetTotalHits() const
{
  if (this->data_ == NULL) {
    MCE_FATAL("Unable to get any data from an empty cache.");
    return 0;
  }

  return this->data_->total_hits;
}

NAMESPACE_FOOTER
