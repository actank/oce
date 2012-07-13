#ifndef BUDDY_BY_ID_CACHE_PRODUCER_H_
#define BUDDY_BY_ID_CACHE_PRODUCER_H_


#include "TripodProducer.h"
#include <QueryRunner.h>
#include <boost/shared_ptr.hpp>

namespace xce {
namespace buddy {

class BuddyByIdCacheProducer : virtual public com::renren::tripod::TripodProducer {
 public:
  BuddyByIdCacheProducer();
  virtual ~BuddyByIdCacheProducer();
  com::renren::tripod::DataMap create(const com::renren::tripod::KeySeq& keys, bool is_master);
  com::renren::tripod::KeySeq createHotKeys(const std::string& begin_key, int limit);
  com::renren::tripod::KeySeq createKeys(const std::string& begin_key, int limit);

 private:
  // key is userid string
  bool CreateById(const std::string& key, com::renren::tripod::Data& data, bool is_master);

};

}} // xce::buddy

#endif

