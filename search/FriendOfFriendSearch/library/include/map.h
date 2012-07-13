#ifndef _FRIENDOFFRIENDSEARCH_LIBRARY_MAP_H
#define _FRIENDOFFRIENDSEARCH_LIBRARY_MAP_H

#include "common.h"
#include "userdata.h"
#include <ext/hash_map>
#include "UtilCxx/src/hash_map_string.h"

NAMESPACE_HEADER

/// @brief Definition of class FriendOfFriendMap, used for generating and exporting maps
///
class FriendOfFriendMap {
  enum {
    /// @brief version mark
    VERSION_1 = 0x0700,

    /// @brief maximum count number of items per entry
    MAXITEMSPERENTRY = 2000,
  };

  public:

    /// @brief result of searching friends, including the friends and the count of them
    struct Result {
      uint32_t count_;
      UserData* items_;

      uint32_t GetCount() const {
        return count_;
      }
      const UserData* GetItems() const {
        return items_;
      }
    };

    /// @brief constructor
    FriendOfFriendMap();

    /// @brief destructor
    ~FriendOfFriendMap();

    /// @brief Find() search friends by `name', and then store the results into `result'
    /// @param name   (in)
    /// @param result (out)
    ///
    /// @return return `true' if friends with name `name' exist, otherwise, return `false'
    bool Find(const char* name, FriendOfFriendMap::Result &result) const;

    /// @brief GetData() retrieve the pointer to the internal data and the size of the data
    /// @param data_ptr  (out)
    /// @param data_size (out)
    ///
    /// @return return `false' if the internal data hasn't been constructed well, or normally, return `true'
    bool GetData(const char* &data_ptr, size_t &data_size) const;

    /// @brief GenerateFromInfo() generate the internal mapping data by `infos' and `names'
    /// @param infos (in) all friends' information
    /// @param names (in) all friends' names, which must be corresponding with `infos'
    ///
    /// @return return `true' if the internal data is generated successfully, otherwise, return `false'
    bool GenerateFromInfo(const FriendInfoSeq &infos, const std::vector<std::string> &names);

    /// @brief GenerateFromBytes() generate the internal mapping data by an existed memory block
    /// @param bytes (in) the pointer to the memory block
    /// @param size  (in) the size of the memory block
    /// @param need_copy (in) whether it need allocate a new block and copy the data from `bytes',
    ///                         or use `bytes' directly
    ///
    /// @return return `true' if the internal data is generated successfully, otherwise, return `false'
    bool GenerateFromBytes(const char *bytes, size_t size, bool need_copy = true);
    
  protected:

    /// @brief entry of a name in the internal data
    struct Entry {
      /// @brief the start position of the name in `names' block
      uint32_t name_pos_    : 32;
      /// @brief the start position of the items in `items' block
      uint32_t item_pos_    : 32;
      /// @brief the length of the name
      uint32_t name_length_ : 16;
      /// @brief the count number of the items
      uint32_t item_count_  : 16;
    };

    /// @brief pointer to the entries block
    Entry *entries_;
    /// @brief pointer to the names block
    char *names_;
    /// @brief pointer to the items block
    UserData *items_;

    /// @brief the internal data of a map
    struct Data {
      /// @brief version mark
      uint32_t version_          : 32;
      /// @brief the count number of the entries
      uint32_t entry_count_      : 32;
      /// @brief the length of `names' block
      uint32_t name_totallength_ : 32;
      /// @brief the count number of the items
      uint32_t item_count_       : 32;
      /// @brief the start position of the blocks
      char data_[];
    } *data_;

    /// @brief SizeofData() calculate the real size of the internal data
    /// @param data (in) the block which need calculating
    ///
    /// @return return the real size of `data'
    inline size_t SizeofData(const Data *data) const
    {
      return sizeof(FriendOfFriendMap::Data) +
        data->name_totallength_ * sizeof(char) +
        data->entry_count_ * sizeof(FriendOfFriendMap::Entry) +
        data->item_count_ * sizeof(UserData);
    }

    /// @brief Clear() clear all fields of THIS object
    inline void Clear() {
      if (data_ != NULL) {
        free(data_);
      }

      entries_ = NULL;
      names_ = NULL;
      items_ = NULL;
      data_ = NULL;
    }

    static inline bool CompareItem(const UserData &left, const UserData &right)
    {
      if (left.type_ == right.type_) {
        return left.mutual_ > right.mutual_;
      } else if (left.type_ == ZeroDegree) {
        return true;
      } else if (right.type_ == ZeroDegree) {
        return false;
      } else if (left.type_ == FirstDegree) {
        return true;
      } else {
        return false;
      }
    }

    typedef std::vector<UserData> VecItem;
    typedef __gnu_cxx::hash_map<std::string, VecItem> MapItem;

  private:

    /// disable the copy constructor and operator overloading temporarily
    FriendOfFriendMap(const FriendOfFriendMap&);
    FriendOfFriendMap& operator=(const FriendOfFriendMap&);
};

NAMESPACE_FOOTER

#endif // _FRIENDOFFRIENDSEARCH_LIBRARY_MAP_H
