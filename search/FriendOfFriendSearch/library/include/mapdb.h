#ifndef _FRIENDOFFRIENDSEARCH_LIBRARY_MAPDB_H
#define _FRIENDOFFRIENDSEARCH_LIBRARY_MAPDB_H

#include "common.h"
#include "map.h"
#include "memcache.h"
#include "lrucache.h"

NAMESPACE_HEADER

/// @brief database connector of memcached
class FriendOfFriendMapDB {
  enum {
    /// @brief expiration time of every entry (seconds)
    EXPIRATION_REMOTE = 86400,
    /// @brief expiration time of every local entry (seconds)
    EXPIRATION_LOCAL = 3600
  };
  public:
    /// @brief constructor
    FriendOfFriendMapDB(bool use_localcache = false);

    /// @brief destructor
    ~FriendOfFriendMapDB();

    /// @brief SetServers() set the addresses of the memcached servers
    /// @param servers_string (in) e.g. "localhost, foo:555, foo, bar"
    ///
    /// @return return `true' if `servers_string' is parsed well, otherwise, return `false'
    bool SetServers(const char *servers_string);

    /// @brief GetMap() fetch the data of the user with ID `userid', and fill the object `map' with the data
    /// @param userid (in)
    /// @param map (in)
    ///
    /// @return return `true' if the entry exists, otherwise, return `false'
    bool GetMap(uint32_t userid, FriendOfFriendMap &map);

    /// @brief SetMap() store the data of the user with ID `userid', and fill the object `map' with the data
    /// @param userid (in)
    /// @param map (in)
    ///
    /// @return return `true' if the operation is success, otherwise, return `false'
    bool SetMap(uint32_t userid, const FriendOfFriendMap &map);

    /// @brief EraseMap() erase the data of a particular user
    /// @param userid (in)
    ///
    /// @return return `true' if the operation is success, otherwise, return `false'
    bool EraseMap(uint32_t userid);

    static void GenerateKey(uint32_t userid, char *outbuf, size_t bufsize);

  protected:
    /// @brief handler of the connector of memcached
    FriendOfFriendMemcache memcache_;

    /// @brief local lru cache
    FriendOfFriendMapCache *cache_;
    pthread_mutex_t cache_lock_;

  private:

    /// disable the copy constructor and operator overloading temporarily
    FriendOfFriendMapDB(const FriendOfFriendMapDB&);
    FriendOfFriendMapDB& operator=(const FriendOfFriendMapDB&);
};

NAMESPACE_FOOTER

#endif // _FRIENDOFFRIENDSEARCH_LIBRARY_DB_H
