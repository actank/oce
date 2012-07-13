#ifndef _FRIENDOFFRIENDSEARCH_LIBRARY_USERDATA_H
#define _FRIENDOFFRIENDSEARCH_LIBRARY_USERDATA_H

NAMESPACE_HEADER

/// @brief user's data, including ID, type and the number of mutual friends
struct UserData {
  uint32_t id_      : 32;
  uint32_t type_    : 16;
  uint32_t mutual_  : 16;

  uint32_t GetID() const {
    return id_;
  }
  uint32_t GetType() const {
    return type_;
  }
  uint32_t GetMutual() const {
    return mutual_;
  }
};

NAMESPACE_FOOTER

#endif // _FRIENDOFFRIENDSEARCH_LIBRARY_USERDATA_H
