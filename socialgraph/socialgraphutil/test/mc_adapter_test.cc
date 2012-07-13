
#include <vector>
#include <iostream>

#include "gtest/gtest.h"
#include "base/logging.h"
#include "base/threadpool.h"

#include "OceCxxAdapter/src/BuddyCoreAdapter.h"

#include "socialgraph/socialgraphutil/algostuff.h"
#include "socialgraph/socialgraphutil/mc_adapter.h"
#include "socialgraph/socialgraphproto/item.pb.h"

namespace xce {
namespace socialgraph {

/*
TEST(RcdCacheAdapter, Set) {
  int64_t key = 666666;
  std::vector<unsigned char> data;
  data.push_back('a');
  data.push_back('a');
  data.push_back('a');
  bool flag = BusinessCacheAdapter::GetInstance().Set("RCD", key, data);
  if (!flag) {
    LOG(INFO) << "set failed...";
  }
  else {
    LOG(INFO) << "set good";
  }
}
*/

TEST(BusinessCacheAdapter, Get) {
  int64_t key = 40540;
  std::vector<unsigned char> data;
  bool flag = BusinessCacheAdapter::GetInstance().Get("FFUCR", key, data);
  LOG(INFO) << "data.size():" << data.size();
  if (!flag) {
    LOG(INFO) << "miss...";
  }
  else {
    LOG(INFO) << "hit...";
    Items items;
    items.ParseFromArray(data.data(), data.size());
    BOOST_FOREACH(const Item& item, items.items()) {
      LOG(INFO) << "item.id():" << item.id() 
                << "\titem.field():" << item.field();
      BOOST_FOREACH(int ci, item.fields()) {
        LOG(INFO) << "\t common_id:" << ci;
      }
    }
  }
}

/*
TEST(MemcachedAdapter, Get) { 
  std::string mckey("FFW_27640369");
	MemcachedAdapter& adapter = MemcachedAdapter::GetInstance();
  std::vector<unsigned char> data;
  EXPECT_TRUE(adapter.Get(mckey, data)); 
  LOG(INFO) << "data.size():" << data.size();
  EXPECT_TRUE(true);
}
*/

/*
TEST(BuddyCoreAdapter, getBlock) {
  std::vector<int> block_seq = com::xiaonei::service::buddy::BuddyCoreAdapter::instance().getBlockSeq(237833886, 0, -1);
  xce::PRINT_ELEMENTS(block_seq, "bl:");
}
*/

/*
class WMessage : public xce::Message {
 public:
   WMessage(int i) : id_(i) {
   
   }
   void Run() {
     std::string mckey("FFW_27640369");
	   MemcachedAdapter& adapter = MemcachedAdapter::GetInstance();
     std::vector<unsigned char> data;
     adapter.Get(mckey, data);
   }
 private:
   int id_;
};

TEST(MultiThread, Get) {
  xce::ThreadPool thread_pool(100, 200);
  for (int i = 0; i < 100; i++) {
    thread_pool.Post(new WMessage(i));
  }

  sleep(3);
  thread_pool.Stop();
  thread_pool.Join();
}
*/


}}

