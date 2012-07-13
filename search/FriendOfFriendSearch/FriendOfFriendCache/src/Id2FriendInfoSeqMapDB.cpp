#include "Id2FriendInfoSeqMapDB.h"

NAMESPACE_HEADER

Id2FriendInfoSeqMapDB::Id2FriendInfoSeqMapDB() : memcache_(EXPIRATION_TIME) { }

Id2FriendInfoSeqMapDB::~Id2FriendInfoSeqMapDB() { }

bool Id2FriendInfoSeqMapDB::SetServers(const char *servers_string)
{
  return this->memcache_.SetServers(servers_string);
}

bool Id2FriendInfoSeqMapDB::Get(uint32_t userid, FriendInfoSeq& friend_info_seq) {
  char key[50];
  char *value = NULL;
  size_t value_len = 0;

  GenerateKey(userid, key, sizeof(key));
  value = this->memcache_.Get(key, value_len);
  if (value == NULL) {
    MCE_FATAL("Unable to fetch the data with userid " << userid);
    return false;
  }

  size_t size = value_len / sizeof(FriendInfo);

  friend_info_seq.assign((FriendInfo*)value, ((FriendInfo*)value)+size);

  free(value);
  return true;
}

bool Id2FriendInfoSeqMapDB::Set(uint32_t userid, const FriendInfoSeq& friend_info_seq)
{
  char key[50];
  const char *value = NULL;
  size_t value_len = 0;
  
  GenerateKey(userid, key, sizeof(key));

  value = reinterpret_cast<const char*>(friend_info_seq.data());
  value_len = friend_info_seq.size() * sizeof(FriendInfo);

  if (!this->memcache_.Set(key, value, value_len)) {
    MCE_FATAL("Unable to store the data with userid " << userid);
    return false;
  }

  return true;
}


bool Id2FriendInfoSeqMapDB::Erase(uint32_t userid)
{
  char key[50];
  GenerateKey(userid, key, sizeof(key));

  return this->memcache_.Erase(key);
}

void Id2FriendInfoSeqMapDB::GenerateKey(uint32_t userid, char *outbuf, size_t bufsize)
{
  snprintf(outbuf, bufsize, "friend_info_%u", userid);
}

NAMESPACE_FOOTER
