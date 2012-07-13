#ifndef _FRIENDOFFRIENDSEARCH_LIBRARY_MEMCACHE_H
#define _FRIENDOFFRIENDSEARCH_LIBRARY_MEMCACHE_H

#include "common.h"

class memcached_st;

NAMESPACE_HEADER

/// @brief database connector of memcached
class FriendOfFriendMemcache {
  public:
    /// @brief constructor
    FriendOfFriendMemcache(time_t expiration);

    /// @brief destructor
    ~FriendOfFriendMemcache();

    /// @brief SetServers() set the addresses of the memcached servers
    /// @param servers_string (in) e.g. "localhost, foo:555, foo, bar"
    ///
    /// @return return `true' if `servers_string' is parsed well, otherwise, return `false'
    bool SetServers(const char *servers_string);

    /// @brief GetByKey() fetch an individual data corresponding to `key',
    ///                     and any object returned must be released by the caller application
    /// @param key (in)
    /// @param value_len (out) the length of the returned block
    ///
    /// @return return the pointer to the data if `key' exists, or return NULL on error
    char* Get(const char *key, size_t &value_len);

    /// @brief SetByKey() store `value' on the server
    /// @param key (in)
    /// @param value (in)
    /// @param value_len (in)
    ///
    /// @return return `true' if the operation is success, otherwise, return `false'
    bool Set(const char *key, const char *value, size_t value_len);

    /// @brief EraseByKey() erase a particular `key'
    /// @param key (in)
    ///
    /// @return return `true' if the operation is success, otherwise, return `false'
    bool Erase(const char *key);

  protected:
    /// @brief handler of the connector of memcached
    memcached_st *handle_;

    /// @brief expiration of the data to be stored (seconds)
    time_t expiration_;

    /// @brief lock
    pthread_mutex_t memcache_lock_;

  private:

    /// disable the copy constructor and operator overloading temporarily
    FriendOfFriendMemcache(const FriendOfFriendMemcache&);
    FriendOfFriendMemcache& operator=(const FriendOfFriendMemcache&);
};

NAMESPACE_FOOTER

#endif // _FRIENDOFFRIENDSEARCH_LIBRARY_MEMCACHE_H
