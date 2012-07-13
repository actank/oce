#include "../include/common.h"
#include "../include/map.h"
#include "../include/memcache.h"
#include "../include/lrucache.h"
#include "../include/mapdb.h"

NAMESPACE_HEADER

FriendOfFriendMapDB::FriendOfFriendMapDB(bool use_localcache) : memcache_(EXPIRATION_REMOTE)
{
  if (use_localcache) {
    this->cache_ = new FriendOfFriendMapCache(EXPIRATION_LOCAL);
    pthread_mutex_init(&this->cache_lock_, NULL);
  } else {
    this->cache_ = NULL;
  }
}

FriendOfFriendMapDB::~FriendOfFriendMapDB()
{
  if (this->cache_ != NULL) {
    pthread_mutex_lock(&this->cache_lock_);
    delete this->cache_;
    this->cache_ = NULL;
    pthread_mutex_unlock(&this->cache_lock_);
    pthread_mutex_destroy(&this->cache_lock_);
  }
}

bool FriendOfFriendMapDB::SetServers(const char *servers_string)
{
  return this->memcache_.SetServers(servers_string);
}

bool FriendOfFriendMapDB::GetMap(uint32_t userid, FriendOfFriendMap &map)
{
  char key[50];
  char *value = NULL;
  size_t value_len = 0;

  if (this->cache_ != NULL) {
    pthread_mutex_lock(&this->cache_lock_);
    if (this->cache_->GetMap(userid, map)) {
      pthread_mutex_unlock(&this->cache_lock_);
      return true;
    }
    pthread_mutex_unlock(&this->cache_lock_);
  }

  GenerateKey(userid, key, sizeof(key));
  value = this->memcache_.Get(key, value_len);
  if (value == NULL) {
    MCE_FATAL("Unable to fetch the data with userid " << userid);
    return false;
  }

  if (!map.GenerateFromBytes(value, value_len, false)) {
    MCE_FATAL("Unable to generate the map with the data of " << userid);
    free(value);
    return false;
  }

  return true;
}

bool FriendOfFriendMapDB::SetMap(uint32_t userid, const FriendOfFriendMap &map)
{
  char key[50];
  const char *value = NULL;
  size_t value_len = 0;

  GenerateKey(userid, key, sizeof(key));

  if (!map.GetData(value, value_len)) {
    MCE_FATAL("Unable to get the pointer of the internal data with userid " << userid);
    return false;
  }

  if (!this->memcache_.Set(key, value, value_len)) {
    MCE_FATAL("Unable to store the data with userid " << userid);
    return false;
  }

  if (this->cache_ != NULL) {
    pthread_mutex_lock(&this->cache_lock_);
    this->cache_->SetMap(userid, map);
    pthread_mutex_unlock(&this->cache_lock_);
  }

  return true;
}

bool FriendOfFriendMapDB::EraseMap(uint32_t userid)
{
  char key[50];
  GenerateKey(userid, key, sizeof(key));

  if (this->cache_ != NULL) {
    pthread_mutex_lock(&this->cache_lock_);
    this->cache_->DeleteMap(userid);
    pthread_mutex_unlock(&this->cache_lock_);
  }

  return this->memcache_.Erase(key);
}

void FriendOfFriendMapDB::GenerateKey(uint32_t userid, char *outbuf, size_t bufsize)
{
  snprintf(outbuf, bufsize, "user_%u", userid);
}

NAMESPACE_FOOTER
