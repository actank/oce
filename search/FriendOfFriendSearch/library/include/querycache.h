#ifndef _FRIENDOFFRIENDSEARCH_LIBRARY_QUERYCACHE_H
#define _FRIENDOFFRIENDSEARCH_LIBRARY_QUERYCACHE_H

#include "common.h"
#include "userdata.h"
#include "memcache.h"

NAMESPACE_HEADER

/// @brief FriendOfFriendQueryCache, serve to cache the result of query in memcached
class FriendOfFriendQueryCache {
  enum {
    /// @brief version mark
    VERSION_1 = 0x0700,
  };

  public:
    /// @brief Cache, serve to keep the data of the cache
    class Cache {
      friend class FriendOfFriendQueryCache;

      public:
        /// @brief default constructor
        Cache();

        /// @brief constructor
        /// @param total_hits (in) the number of the total hits of the query result
        /// @param users      (in) the pointer to `users'
        /// @param result_hits (in) the count number of `users'
        Cache(uint32_t total_hits, const UserData *users, uint32_t result_hits);

        Cache(const Cache&);
        
        /// @brief destructor
        ~Cache();

        /// @brief GetCache() return the pointer to `users' and its count number
        /// @param users (out) the pointer to `users'
        /// @param result_hits (out) the count number of `users'
        ///
        /// @return return `true' if the operation is OK, otherwise return `false'
        bool GetResult(const UserData* &users, uint32_t &result_hits) const;

        /// @brief GetTotalHits() return the number of the total hits
        uint32_t GetTotalHits() const;

      protected:
        /// @brief internal data block
        struct CacheData {
          /// @brief version mark
          uint32_t version;
          /// @brief the size of the whole data block
          uint32_t size;
          /// @brief the count number of `users'
          uint32_t result_hits;
          /// @brief the number of the total hits
          uint32_t total_hits;
          /// @brief the array storing the information of users
          UserData users[0];
        } *data_;

      private:
        Cache& operator=(const Cache&);
    };

    FriendOfFriendQueryCache(time_t expiration);
    ~FriendOfFriendQueryCache();


    /// @brief SetServers() set the addresses of the memcached servers
    /// @param servers_string (in) e.g. "localhost, foo:555, foo, bar"
    ///
    /// @return return `true' if `servers_string' is parsed well, otherwise, return `false'
    bool SetServers(const char *servers_string);

    /// @brief GetCache() get the cache according to `query_key'
    /// @param query_key (in)
    /// @param cache (out)
    /// @return return `true' if the operation is OK, otherwise return `false'
    bool GetCache(const char *query_key, Cache &cache);

    /// @brief SetCache() store the cache according to `query_key'
    /// @param query_key (in)
    /// @param cache (in)
    /// @return return `true' if the operation is OK, otherwise return `false'
    bool SetCache(const char *query_key, const Cache &cache);

    /// @brief EraseCache() erase the cache according to `query_key'
    /// @param query_key (in)
    /// @return return `true' if the operation is OK, otherwise return `false'
    bool EraseCache(const char *query_key);

  protected:

    /// @brief handler of the connector of memcached
    FriendOfFriendMemcache memcache_;

  private:

    /// disable the copy constructor and operator overloading temporarily
    FriendOfFriendQueryCache(const FriendOfFriendQueryCache&);
    FriendOfFriendQueryCache& operator=(const FriendOfFriendQueryCache&);

};

NAMESPACE_FOOTER

#endif // _FRIENDOFFRIENDSEARCH_LIBRARY_QUERYCACHE_H
