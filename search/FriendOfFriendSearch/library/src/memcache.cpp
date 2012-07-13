#include "../include/common.h"
#include "../include/memcache.h"

#include <libmemcached/memcached.h>

NAMESPACE_HEADER

FriendOfFriendMemcache::FriendOfFriendMemcache(time_t expiration)
{
  this->handle_ = NULL;
  this->expiration_ = expiration;
  pthread_mutex_init(&this->memcache_lock_, NULL);
}

FriendOfFriendMemcache::~FriendOfFriendMemcache()
{
  if (this->handle_ != NULL) {
    memcached_free(this->handle_);
  }
  this->handle_ = NULL;
  pthread_mutex_destroy(&this->memcache_lock_);
}

bool FriendOfFriendMemcache::SetServers(const char *servers_string)
{
  bool result = true;

  pthread_mutex_lock(&this->memcache_lock_);

  if (this->handle_ == NULL) {
    this->handle_ = memcached_create(NULL);
  } else {
    memcached_servers_reset(this->handle_);
  }

  memcached_return_t ret;

  memcached_server_list_st servers = memcached_servers_parse(servers_string);
  if (servers == NULL) {
    MCE_FATAL("Invalid servers_string: `" << servers_string);
    result = false;
    goto _exit;
  }

  ret = memcached_server_push(this->handle_, servers);
  memcached_server_free(servers);
  if (ret != MEMCACHED_SUCCESS) {
    MCE_FATAL("Unable to push servers info memcached handler: " << memcached_strerror(this->handle_, ret));
    result = false;
    goto _exit;
  }

  ret = memcached_behavior_set(this->handle_, MEMCACHED_BEHAVIOR_KETAMA, 1);
  if (ret != MEMCACHED_SUCCESS) {
    MCE_FATAL("Unable to switch to the consistent hashing: " << memcached_strerror(this->handle_, ret));
    result = false;
    goto _exit;
  }

_exit:
  pthread_mutex_unlock(&memcache_lock_);
  return result;
}

char* FriendOfFriendMemcache::Get(const char *key, size_t &value_len)
{
  char *value;

  pthread_mutex_lock(&this->memcache_lock_);

  if (this->handle_ == NULL) {
    MCE_FATAL("There isnot a connection!");
    value = NULL;
    goto _exit;
  }

  memcached_return_t ret;
  uint32_t flags;

  value = memcached_get(this->handle_, key, strlen(key), &value_len, &flags, &ret);
  if (value == NULL) {
    if (ret != MEMCACHED_NOTFOUND) {
      MCE_FATAL("Unable to fetch the data with the key `" << key
          << "': " << memcached_strerror(this->handle_, ret));
    }
  }

_exit:
  pthread_mutex_unlock(&memcache_lock_);
  return value;
}

bool FriendOfFriendMemcache::Set(const char *key, const char *value, size_t value_len)
{
  bool result = true;

  pthread_mutex_lock(&this->memcache_lock_);

  if (this->handle_ == NULL) {
    MCE_FATAL("There isnot a connection!");
    result = false;
    goto _exit;
  }

  memcached_return_t ret;

  ret = memcached_set(this->handle_, key, strlen(key), value, value_len, this->expiration_, 0);
  if (ret == MEMCACHED_SUCCESS) {
    result = true;
    goto _exit;
  } else {
    MCE_FATAL("Unable to store the data with the key `" << key << "' of " << value_len << " bytes: "
        << memcached_strerror(this->handle_, ret));
    result = false;
    goto _exit;
  }
_exit:
  pthread_mutex_unlock(&this->memcache_lock_);
  return result;
}

bool FriendOfFriendMemcache::Erase(const char *key)
{
  bool result = true;

  pthread_mutex_lock(&this->memcache_lock_);

  if (this->handle_ == NULL) {
    MCE_FATAL("There isnot a connection!");
    result = false;
    goto _exit;
  }

  memcached_return_t ret;

  ret = memcached_delete(this->handle_, key, strlen(key), 0);
  if (ret == MEMCACHED_SUCCESS) {
    result = true;
    goto _exit;
  } else {
    if (ret != MEMCACHED_NOTFOUND) {
      MCE_FATAL("Unable to delete the key `" << key
          << "': " << memcached_strerror(this->handle_, ret));
    }
    result = false;
    goto _exit;
  }
_exit:
  pthread_mutex_unlock(&this->memcache_lock_);
  return result;
}

NAMESPACE_FOOTER
