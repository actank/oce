#include "../include/common.h"
#include "../include/lrucache.h"

NAMESPACE_HEADER

FriendOfFriendMapCache::FriendOfFriendMapCache(uint32_t expiration) : expiration_(expiration)
{
  int i = 0;
  for (i = 0; i < HASHSIZE; ++i) {
    bucket_[i] = BUCKETEND;
  }

  for (i = 0; i < BUCKETSIZE; ++i) {
    entries_[i].next_entry = entries_[i].prev_entry =
      entries_[i].next_lru = entries_[i].prev_lru = 
      entries_[i].next_expire = entries_[i].prev_expire = 
      BUCKETEND;
    entries_[i].hash = HASHSIZE;
    entries_[i].userid = 0;
    entries_[i].data = NULL;
    entries_[i].data_size = 0;

    empty_stack_[i] = i;
  }

  lru_list_head_ = BUCKETEND;
  expire_list_head_ = BUCKETEND;
  empty_stack_top_ = BUCKETEND;
}

FriendOfFriendMapCache::~FriendOfFriendMapCache()
{
  int i = 0;
  for (i = 0; i < BUCKETSIZE; ++i) {
    if (entries_[i].data != NULL) {
      free(entries_[i].data);
    }
  }
}

bool FriendOfFriendMapCache::SetMap(uint32_t userid, const FriendOfFriendMap &map)
{
  PosType hash = Hash(userid);
  PosType head = bucket_[hash];

  if (head == BUCKETEND) { /// empty bucket
    return AddEntry(userid, map, hash);
  }

  PosType curr = head;
  BucketEntry *entry = NULL;
  for (;;) {
    entry = entries_ + curr;

    if (entry->hash == hash && entry->userid == userid) { // found
      RaiseEntry(curr);
      return SetEntry(curr, userid, map);
    }

    curr = entry->next_entry;
    if (curr == head) {
      break;
    }
  }

  return AddEntry(userid, map, hash);
}

bool FriendOfFriendMapCache::AddEntry(uint32_t userid, const FriendOfFriendMap &map,
    FriendOfFriendMapCache::PosType hash)
{
  /// fetch an empty entry
  PosType entry_pos = GetEmptyEntry();

  /// store data
  if (!SetEntry(entry_pos, userid, map)) {
    PutEmptyEntry(entry_pos);
    return false;
  }

  PosType bucket_head = bucket_[hash];
  BucketEntry *entry = entries_ + entry_pos;
  entry->hash = hash;

  /// insert the entry into the bucket
  if (bucket_head == BUCKETEND) { // empty bucket
    entry->next_entry = entry->prev_entry = entry_pos;
  } else {
    BucketEntry *entry_head = entries_ + bucket_head;
    BucketEntry *entry_head_prev = entries_ + entry_head->prev_entry;
    entry->next_entry = bucket_head;
    entry->prev_entry = entry_head->prev_entry;
    entry_head->prev_entry = entry_pos;
    entry_head_prev->next_entry = entry_pos;
  }
  bucket_[hash] = entry_pos;

  /// insert the entry into the LRU chain
  if (lru_list_head_ == BUCKETEND) { // empty
    lru_list_head_ = entry_pos;
    entry->next_lru = entry->prev_lru = entry_pos;
  } else {
    BucketEntry *entry_head = entries_ + lru_list_head_;
    BucketEntry *entry_head_prev = entries_ + entry_head->prev_lru;
    entry->next_lru = lru_list_head_;
    entry->prev_lru = entry_head->prev_lru;
    entry_head->prev_lru = entry_pos;
    entry_head_prev->next_lru = entry_pos;
    lru_list_head_ = entry_pos;
  }

  /// insert the entry into the expire chain
  if (expire_list_head_ == BUCKETEND) { // empty
    expire_list_head_ = entry_pos;
    entry->next_expire = entry->prev_expire = entry_pos;
  } else {
    BucketEntry *entry_head = entries_ + expire_list_head_;
    BucketEntry *entry_head_prev = entries_ + entry_head->prev_expire;
    entry->next_expire = expire_list_head_;
    entry->prev_expire = entry_head->prev_expire;
    entry_head->prev_expire = entry_pos;
    entry_head_prev->next_expire = entry_pos;
    expire_list_head_ = entry_pos;
  }

  return true;
}

bool FriendOfFriendMapCache::SetEntry(PosType entry_pos, uint32_t userid, const FriendOfFriendMap &map)
{
  const char *data = NULL;
  size_t data_size = 0;

  if (!map.GetData(data, data_size)) {
    MCE_FATAL("Unable to fetch the pointer of the map object: userid: " << userid);
    return false;
  }

  char *newdata = (char*)malloc(data_size);
  if (newdata == NULL) {
    MCE_FATAL("Unable to allocate a memory block with " << data_size << " bytes.");
    return false;
  }

  memcpy(newdata, data, data_size);

  BucketEntry *entry = entries_ + entry_pos;
  entry->userid = userid;
  if (entry->data != NULL) {
    free(entry->data);
  }
  entry->data = newdata;
  entry->data_size = data_size;
  entry->expiration = time(NULL) + expiration_;

  return true;
}

void FriendOfFriendMapCache::RaiseEntry(PosType entry_pos)
{
  if (entry_pos == lru_list_head_) {
    return;
  }
  
  // remove first
  BucketEntry *entry = entries_ + entry_pos;
  BucketEntry *entry_prev = entries_ + entry->prev_lru;
  BucketEntry *entry_next = entries_ + entry->next_lru;
  entry_prev->next_lru = entry->next_lru;
  entry_next->prev_lru = entry->prev_lru;

  // add to the head
  BucketEntry *entry_head = entries_ + lru_list_head_;
  BucketEntry *entry_head_prev = entries_ + entry_head->prev_lru;
  entry->next_lru = lru_list_head_;
  entry->prev_lru = entry_head->prev_lru;
  entry_head->prev_lru = entry_pos;
  entry_head_prev->next_lru = entry_pos;
  lru_list_head_ = entry_pos;
}

FriendOfFriendMapCache::PosType FriendOfFriendMapCache::GetEmptyEntry()
{
  if (empty_stack_top_ == 0) {
    PosType oldest = entries_[expire_list_head_].prev_expire;
    /// full, clear an entry
    if (time(NULL) >= entries_[oldest].expiration) { // expired
      ReleaseEntry(oldest);
    } else { // least recent used
      ReleaseEntry(entries_[lru_list_head_].prev_lru);
    }
  }

  PosType ret = empty_stack_[empty_stack_top_ - 1];
  --empty_stack_top_;

  return ret;
}

void FriendOfFriendMapCache::PutEmptyEntry(PosType entry_pos)
{
  empty_stack_[empty_stack_top_] = entry_pos;
  ++empty_stack_top_;
}

void FriendOfFriendMapCache::ReleaseEntry(FriendOfFriendMapCache::PosType entry_pos)
{
  BucketEntry *entry = entries_ + entry_pos;

  /// clear in expire list
  if (entry->next_expire == entry->prev_expire) { // only one
    expire_list_head_ = BUCKETEND;
  } else {
    BucketEntry *entry_prev = entries_ + entry->prev_expire;
    BucketEntry *entry_next = entries_ + entry->next_expire;
    entry_prev->next_expire = entry->next_expire;
    entry_next->prev_expire = entry->prev_expire;

    if (entry_pos == expire_list_head_) {
      expire_list_head_ = entry->next_expire;
    }
  }

  /// clear in lru list
  if (entry->next_lru == entry->prev_lru) { // only one
    lru_list_head_ = BUCKETEND;
  } else {
    BucketEntry *entry_prev = entries_ + entry->prev_lru;
    BucketEntry *entry_next = entries_ + entry->next_lru;
    entry_prev->next_lru = entry->next_lru;
    entry_next->prev_lru = entry->prev_lru;

    if (entry_pos == lru_list_head_) {
      lru_list_head_ = entry->next_lru;
    }
  }

  /// clear in entries
  if (entry->next_entry == entry->prev_entry) { // only one, clear the hashtable
    bucket_[entry->hash] = BUCKETEND;
  } else {
    BucketEntry *entry_prev = entries_ + entry->prev_entry;
    BucketEntry *entry_next = entries_ + entry->next_entry;
    entry_next->prev_entry = entry->prev_entry;
    entry_prev->next_entry = entry->next_entry;

    if (bucket_[entry->hash] == entry_pos) {
      bucket_[entry->hash] = entry->next_entry;
    }
  }

  /// OK, clear the entry itself
  entry->next_entry = entry->prev_entry =
    entry->next_lru = entry->prev_lru = BUCKETEND;
  entry->hash = HASHSIZE;
  entry->userid = 0;
  if (entry->data != NULL) {
    free(entry->data);
    entry->data = NULL;
  }
  entry->data_size = 0;

  /// finally, append to empty stack
  PutEmptyEntry(entry_pos);
  
}

bool FriendOfFriendMapCache::GetMap(uint32_t userid, FriendOfFriendMap &map)
{
  PosType hash = Hash(userid);
  PosType head = bucket_[hash];

  if (head == BUCKETEND) { /// empty bucket
    return false;
  }

  PosType curr = head;
  BucketEntry *entry = NULL;
  for (;;) {
    entry = entries_ + curr;

    if (entry->hash == hash && entry->userid == userid) { // found
      if (time(NULL) >= entry->expiration) { // found but expired
        ReleaseEntry(curr);
        return false;
      }
      RaiseEntry(curr);
      return map.GenerateFromBytes(entry->data, entry->data_size, true);
    }

    curr = entry->next_entry;
    if (curr == head) {
      break;
    }
  }

  MCE_DEBUG("Unable to find the map object, userid: " << userid);
  return false;
}

bool FriendOfFriendMapCache::DeleteMap(uint32_t userid)
{
  PosType hash = Hash(userid);
  PosType head = bucket_[hash];

  if (head == BUCKETEND) { /// empty bucket
    return false;
  }

  PosType curr = head;
  BucketEntry *entry = NULL;
  for (;;) {
    entry = entries_ + curr;

    if (entry->hash == hash && entry->userid == userid) { // found
      ReleaseEntry(curr);
      return true;
    }

    curr = entry->next_entry;
    if (curr == head) {
      break;
    }
  }
  return false;
}

FriendOfFriendMapCache::PosType FriendOfFriendMapCache::Hash(uint32_t userid)
{
  userid = (userid+0x7ed55d16) + (userid<<12);
  userid = (userid^0xc761c23c) ^ (userid>>19);
  userid = (userid+0x165667b1) + (userid<<5);
  userid = (userid+0xd3a2646c) ^ (userid<<9);
  userid = (userid+0xfd7046c5) + (userid<<3);
  userid = (userid^0xb55a4f09) ^ (userid>>16);
  return (PosType)(userid & HASHMASK);
}

void FriendOfFriendMapCache::Check()
{
  uint32_t count = 0;
  PosType max, min, maxid, minid;
  max = min = BUCKETEND;
  maxid = minid = BUCKETEND;
  for (int32_t i = 0; i < HASHSIZE; ++i) {
    count = 0;
    if (bucket_[i] != BUCKETEND) {
      PosType head = bucket_[i];
      PosType curr = head;
      for (;;) {
        BucketEntry *entry = entries_ + curr;
        ++count;
        curr = entry->next_entry;
        if (curr == head) {
          break;
        }
      }
    }

    if (max == BUCKETEND || max < count) {
      max = count;
      maxid = i;
    }
    if (min == BUCKETEND || min > count) {
      min = count;
      minid = i;
    }
    // LOGP("bucket %d has %u entries.", i, count);
  }

  LOGP("largest bucket is %u, with %u entries.", maxid, max);
  LOGP("smallest bucket is %u, with %u entries.", minid, min);

  PosType lru_head = lru_list_head_;

  if (lru_list_head_ == BUCKETEND) {
    LOGP("lru chain is empty");
  } else {

    PosType lru_curr = lru_head;
    count = 0;
    for (;;) {
      ++count;
      lru_curr = entries_[lru_curr].next_lru;
      if (lru_curr == lru_head) {
        break;
      }
    }

    LOGP("lru chain has %u entries.", count);
    LOGP("head: %u, tail: %u", lru_list_head_, entries_[lru_list_head_].next_lru);
  }

  int expired_count = 0;
  PosType expire_head = expire_list_head_;
  if (expire_list_head_ == BUCKETEND) {
    LOGP("expire chain is empty");
  } else {

    PosType expire_curr = expire_head;
    count = 0;
    for (;;) {
      ++count;
      if (time(NULL) >= entries_[expire_curr].expiration) {
        ++expired_count;
      }
      expire_curr = entries_[expire_curr].next_expire;
      if (expire_curr == expire_head) {
        break;
      }
    }

    LOGP("expire chain has %u entries, %u of which is expired.", count, expired_count);
    LOGP("head: %u, tail: %u", expire_list_head_, entries_[expire_list_head_].next_expire);
  }
}

NAMESPACE_FOOTER
