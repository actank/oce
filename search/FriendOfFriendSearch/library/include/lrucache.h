#ifndef _FRIENDOFFRIENDSEARCH_LIBRARY_LRUCACHE_H
#define _FRIENDOFFRIENDSEARCH_LIBRARY_LRUCACHE_H

#include "common.h"
#include "map.h"

NAMESPACE_HEADER

/// @brief Definition of class FriendOfFriendMapCache, used for caching the maps locally,
///        NOT thread-safe
class FriendOfFriendMapCache {
  enum {
    /// @brief maximum size of hash buckets
    HASHSIZE = 4096,
    /// @brief mask of hashtable
    HASHMASK = HASHSIZE - 1,
    /// @brief maximum size of each bucket
    BUCKETSIZE = 32768,
    /// @brief invalid position in a bucket
    BUCKETEND = BUCKETSIZE,
  };

  typedef uint16_t PosType;

  public:
    /// @brief constructor
    FriendOfFriendMapCache(uint32_t expiration);

    /// @brief destructor
    ~FriendOfFriendMapCache();

    /// @brief GetMap() try to fetch a map object from the local cache
    /// @param userid (in)
    /// @param map    (out)
    ///
    /// @return return `true' if `userid' exists in the local cache and 
    ///   fill the map well, otherwise, return `false'
    bool GetMap(uint32_t userid, FriendOfFriendMap &map);

    /// @brief SetMap() store a map object in the local cache
    /// @param userid (in)
    /// @param map    (in)
    ///
    /// @return return `true' if `map' is stored successfully, otherwise, return `false'
    bool SetMap(uint32_t userid, const FriendOfFriendMap &map);

    /// @brief DeleteMap() try to delete a map object
    /// @param userid (in)
    ///
    /// @return return `true' if `userid' exists and the object is deleted, otherwise, return `false'
    bool DeleteMap(uint32_t userid);

    /// @brief Check() check the cache and print some information, used for debugging
    void Check();

  protected:

    /// @brief data structure of the entry in a bucket
    struct BucketEntry {
      /// @brief the position of the next entry
      PosType next_entry;
      /// @brief the position of the previous entry
      PosType prev_entry;
      /// @brief the position of the next node in LRU chain
      PosType next_lru;
      /// @brief the position of the previous node in LRU chain
      PosType prev_lru;
      /// @brief the position of the next node in expiration chain
      PosType next_expire;
      /// @brief the position of the previous node in expiration chain
      PosType prev_expire;
      /// @brief the hash code of this entry, and also the index number in the buckets
      PosType hash;
      /// @brief user's ID, as the key
      uint32_t userid;
      /// @brief the pointer to the map data
      char *data;
      /// @brief the size of the map data
      size_t data_size;
      /// @expiration
      time_t expiration;
    };

    /// @brief generate the hash code of the user's ID
    /// @param userid (in)
    ///
    /// @return hash code
    static PosType Hash(uint32_t userid);

    /// @brief add an entry to the bucket, and register it in the LRU chain
    /// @param userid (in)
    /// @param map    (in)
    /// @param hash   (in) the index number of the bucket
    ///
    /// @return return `true' if the operation is OK, otherwise, return `false'
    bool AddEntry(uint32_t userid, const FriendOfFriendMap &map, PosType hash);

    /// @brief release an entry in the bucket and the LRU chain, and free the memory if necessary
    /// @param entry_pos (in) the postion of the entry in `entries_'
    void ReleaseEntry(PosType entry_pos);

    /// @brief store the map data into the entry
    /// @param entry_pos (in) the position of the entryin `entries_'
    /// @param userid    (in) user's ID
    /// @param map       (in) the map object
    ///
    /// @return return `true' if the operation is OK, otherwise, return `false'
    bool SetEntry(PosType entry_pos, uint32_t userid, const FriendOfFriendMap &map);

    /// @brief fetch an empty entry for future use, if there isn't any empty entry, discard the least recently used entry
    ///
    /// @return return the position of the empty entry
    PosType GetEmptyEntry();

    /// @brief add an empty entry to the stack of the empty entries
    /// @param entry_pos (in)
    void PutEmptyEntry(PosType entry_pos);

    /// @brief move the entry to the head of the LRU chain
    /// @param entry_pos (in)
    void RaiseEntry(PosType entry_pos);

    /// @brief expiration of every entry
    uint32_t expiration_;

    /// @brief the hash buckets
    PosType bucket_[HASHSIZE];

    /// @brief the entries array
    BucketEntry entries_[BUCKETSIZE];
    /// @brief the position of the entry which is the head of the LRU chain
    PosType lru_list_head_;
    /// @brief the position of the entry which is the head of the expiration chain
    PosType expire_list_head_;

    /// @brief the empty entries stack
    PosType empty_stack_[BUCKETSIZE];
    /// @brief the top of the stack
    PosType empty_stack_top_;

  private:
    /// disable the following functions temporarily
    FriendOfFriendMapCache(const FriendOfFriendMapCache&);
    FriendOfFriendMapCache& operator=(const FriendOfFriendMapCache&);
};


NAMESPACE_FOOTER

#endif // _FRIENDOFFRIENDSEARCH_LIBRARY_LRUCACHE_H
