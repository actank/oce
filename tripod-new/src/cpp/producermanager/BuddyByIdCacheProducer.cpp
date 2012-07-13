#include "BuddyByIdCacheProducer.h"
#include "boost/foreach.hpp"
#include "listorhashvalue.pb.h"

namespace xce {
namespace buddy {

BuddyByIdCacheProducer::BuddyByIdCacheProducer() {

}

BuddyByIdCacheProducer::~BuddyByIdCacheProducer() {

}

com::renren::tripod::DataMap BuddyByIdCacheProducer::create(const com::renren::tripod::KeySeq& keys, bool is_master) {
  com::renren::tripod::DataMap result;
  BOOST_FOREACH(const std::string& key, keys) {
    com::renren::tripod::Data data;
    bool ret = CreateById(key, data, is_master);
    if (ret) {
      result.insert(std::make_pair(key, data));
    }
  }
  return result;
}

com::renren::tripod::KeySeq BuddyByIdCacheProducer::createHotKeys(const std::string& begin_key, int limit) {
  return com::renren::tripod::KeySeq();
}

com::renren::tripod::KeySeq BuddyByIdCacheProducer::createKeys(const std::string& begin_key, int limit) {
  return com::renren::tripod::KeySeq();
}

bool BuddyByIdCacheProducer::CreateById(const std::string& key, com::renren::tripod::Data& data, bool is_master) {
  MCE_INFO("BuddyByIdCacheProducer::CreateById() key:" << key << "\tis_master:" << is_master);
  com::renren::tripod::ListOrHashValue list_value;
  list_value.add_values("fuckworld");
  list_value.add_values("kissworld");
  int size = list_value.ByteSize();
  if (size <= 0) {
    return false;
  }
  std::string s;
  list_value.SerializeToString(&s);
  data.reserve(s.size());
  data.assign(s.data(), s.data() + s.size());
  return true;
}

}} // xce::buddy

