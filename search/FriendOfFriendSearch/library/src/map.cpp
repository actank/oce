#include "../include/common.h"
#include "../include/userdata.h"
#include "../include/map.h"
#include "util/cpp/TimeCost.h"

using MyUtil::TimeCost;
NAMESPACE_HEADER

FriendOfFriendMap::FriendOfFriendMap()
{
  entries_ = NULL;
  names_ = NULL;
  items_ = NULL;
  data_ = NULL;
}

FriendOfFriendMap::~FriendOfFriendMap()
{
  Clear();
}

bool FriendOfFriendMap::GenerateFromInfo(
    const FriendInfoSeq &infos,
    const std::vector<std::string>& names
    )
{
  TimeCost cost = TimeCost::create("GenerateMap",1);
  MapItem map_data;
  MapItem::iterator map_data_it;
  UserData item;
  uint32_t entry_count;
  uint32_t name_totallength;
  uint32_t item_count;
  char *data;
  uint32_t data_size;
  uint32_t name_pos;
  uint32_t item_pos;
  FriendOfFriendMap::Entry *entry_cur;

  size_t size = infos.size();
  if (size != names.size()) {
    MCE_FATAL("Different sizes of `infos' and `names'. " << infos.size() << " != " << names.size());
    return false;
  }

  // generate sorted map
  entry_count = 0;
  name_totallength = 0;
  item_count = 0;
  for (uint32_t i = 0; i < size; ++i) {
    const FriendInfo &info = infos.at(i);
    const std::string &name = names.at(i);

    if (name.empty()) {
      continue;
    }

    item.id_ = info.id;
    item.type_ = info.type;
    item.mutual_ = info.commonFriendsNum;
    ++item_count;

    map_data_it = map_data.find(name);
    if (map_data_it == map_data.end()) {
      VecItem items;
      items.push_back(item);
      map_data.insert(make_pair<string,VecItem>(name,items));
      name_totallength += name.length();
      ++entry_count;
    } else {
      map_data_it->second.push_back(item);
    }
  }
  cost.step("sorted map");
  // get size of all data and allocate memory
  // `item_count' per entry may be more than MAXITEMSPERENTRY,
  // but we just allocate the perhaps oversize memory now and
  // 'll strip the unnecessary memory when storing in `memcached'
  data_size = sizeof(Data) +
    name_totallength * sizeof(char) +
    entry_count * sizeof(FriendOfFriendMap::Entry) +
    item_count * sizeof(UserData);

  data = (char*)malloc(data_size);
  if (data == NULL) {
    MCE_FATAL("Unable to alloc a memory block, " << data_size);
    return NULL;
  }

  if (this->data_ != NULL) {
    Clear();
  }

  this->data_ = (Data*)data;
  this->data_->version_ = FriendOfFriendMap::VERSION_1;
  this->data_->entry_count_ = entry_count;
  this->data_->name_totallength_ = name_totallength;
  this->data_->item_count_ = 0;                 // count this later
  this->entries_ = (FriendOfFriendMap::Entry*)this->data_->data_;
  this->names_ = (char*)(this->entries_ + this->data_->entry_count_);
  this->items_ = (UserData*)(this->names_ + this->data_->name_totallength_);

  // fill the allocated memory with data
  entry_cur = this->entries_;
  name_pos = 0;
  item_pos = 0;
  for (map_data_it = map_data.begin(); map_data_it != map_data.end(); ++map_data_it) {
    const std::string &name = map_data_it->first;
    VecItem &items = map_data_it->second;

    size_t name_length = name.length();
    size_t item_size = items.size();

    entry_cur->name_pos_ = name_pos;
    entry_cur->name_length_ = name_length;
    entry_cur->item_pos_ = item_pos;
    entry_cur->item_count_ = 0; //  also count it later

    memcpy(this->names_ + name_pos, name.c_str(), name_length);
    name_pos += name_length;

    sort(items.begin(), items.end(), CompareItem);
    for (size_t i = 0; i < item_size && i < FriendOfFriendMap::MAXITEMSPERENTRY; ++i) {
      UserData &item = items.at(i);
      *(this->items_ + item_pos) = item; // bitwise copy of g++ is sufficient, maybe

      // count all numbers now
      ++item_pos;
      ++this->data_->item_count_;
      ++entry_cur->item_count_;
    }

    ++entry_cur;
  }
  cost.step("serialize");
  return true;
}

bool FriendOfFriendMap::GenerateFromBytes(const char *bytes, size_t size, bool need_copy)
{
  Data *data = (Data*)bytes;
  Data *new_data = NULL;

  // normal checking
  if (size < sizeof(Data)) {
    MCE_FATAL("The size of the fetched data is too small: " << size);
    return false;
  }

  if (data->version_ != FriendOfFriendMap::VERSION_1) {
    return false;
  }

  size_t data_size = SizeofData(data);

  if (data_size != size) {
    MCE_FATAL("Wrong data size. " << size << " != " << data_size);
    return false;
  }

  if (need_copy) {
    // allocate a memory block, copy data into the memory block
    new_data = (Data*)malloc(data_size);
    if (new_data == NULL) {
      MCE_FATAL("Unable to alloc a memory block, " << data_size);
      return false;
    }
    memcpy(new_data, data, data_size);
  } else {
    // just use it
    new_data = data;
  }

  if (this->data_ != NULL) {
    Clear();
  }

  // modify the pointers
  this->data_ = new_data;
  this->entries_ = (FriendOfFriendMap::Entry*)this->data_->data_;
  this->names_ = (char*)(this->entries_ + this->data_->entry_count_);
  this->items_ = (UserData*)(this->names_ + this->data_->name_totallength_);

  return true;
}

bool FriendOfFriendMap::Find(const char* name,
    FriendOfFriendMap::Result &result) const
{
  uint32_t search_low;
  uint32_t search_high;
  uint32_t search_pos;
  size_t name_length;
  int32_t ret;

  if (this->data_ == NULL) {
    MCE_FATAL("Not initialized.");
    return false;
  }

  if (name == NULL) {
    return false;
  }

  name_length = strlen(name);
  if (name_length == 0) {
    return false;
  }
  std::string name_string(name);

  // binary search
  search_low = 0;
  search_high = this->data_->entry_count_ - 1;
  while (search_low <= search_high) {
    search_pos = (search_low + search_high) / 2;
    FriendOfFriendMap::Entry *entry = this->entries_ + search_pos;

    ret = name_string.compare(0, name_string.length(), this->names_ + entry->name_pos_, entry->name_length_);
    if (ret == 0) {
      result.count_ = entry->item_count_;
      result.items_ = items_ + entry->item_pos_;
      return true;
    } else if (ret < 0) {
      if (search_pos == 0) {
        break;
      }
      search_high = search_pos - 1;
    } else {
      search_low = search_pos + 1;
    }
  }

  return false;
}

bool FriendOfFriendMap::GetData(const char* &data_ptr, size_t &data_size) const
{
  if (this->data_ == NULL) {
    MCE_FATAL("Not initialized.");
    return false;
  }

  data_ptr = (const char*)this->data_;
  data_size = SizeofData(this->data_);
  return true;
}

NAMESPACE_FOOTER
