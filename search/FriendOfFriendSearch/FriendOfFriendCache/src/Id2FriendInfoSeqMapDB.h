#ifndef _ID_2_FRIEND_INFO_SEQ_MAP_DB_H
#define _ID_2_FRIEND_INFO_SEQ_MAP_DB_H

#include "search/FriendOfFriendSearch/library/include/common.h"
#include "search/FriendOfFriendSearch/library/include/memcache.h"

NAMESPACE_HEADER

/// @brief database connector of memcached
class Id2FriendInfoSeqMapDB {
  enum {
    /// @brief expiration time of every entry (seconds)
    EXPIRATION_TIME= 86400,
  };
  public:
    /// @brief constructor
    Id2FriendInfoSeqMapDB();

    /// @brief destructor
    ~Id2FriendInfoSeqMapDB();

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
    bool Get(uint32_t userid, FriendInfoSeq& friend_info_seq);

    /// @brief SetMap() store the data of the user with ID `userid', and fill the object `map' with the data
    /// @param userid (in)
    /// @param map (in)
    ///
    /// @return return `true' if the operation is success, otherwise, return `false'
    bool Set(uint32_t userid, const FriendInfoSeq& friend_info_seq);

    /// @brief EraseMap() erase the data of a particular user
    /// @param userid (in)
    ///
    /// @return return `true' if the operation is success, otherwise, return `false'
    bool Erase(uint32_t userid);

    static void GenerateKey(uint32_t userid, char *outbuf, size_t bufsize);

  protected:
    /// @brief handler of the connector of memcached
    FriendOfFriendMemcache memcache_;
};

NAMESPACE_FOOTER
#endif 
