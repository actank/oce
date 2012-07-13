
#include "gtest/gtest.h"

#include "boost/foreach.hpp"

#include "base/logging.h"

#include "socialgraph/socialgraphproto/pukfriendrecommend.pb.h"
#include "socialgraph/socialgraphproto/item.pb.h"

namespace xce {
namespace socialgraph {

bool InitPuk(PukFriendRecommend* puk);
bool SerializePuk(PukFriendRecommend* puk, std::vector<unsigned char>* v);
void PrintPuk(const PukFriendRecommend& puk);
void PrintItems(const Items& items);


TEST(Puk2Item, Convert) {
  PukFriendRecommend puk;
  std::vector<unsigned char> v;
  InitPuk(&puk);
//  PrintPuk(puk);
  ASSERT_EQ(true, SerializePuk(&puk, &v));
  LOG(INFO) << "v.size():" << v.size();
  std::string data;
  ASSERT_TRUE(puk.SerializeToString(&data));
  LOG(INFO) << "data.size():" << data.size();

  Items items;
  
  ASSERT_TRUE(items.ParseFromString(data));
  LOG(INFO) << "items.ByteSize():" << items.ByteSize();
//  PrintItems(items);
}

bool InitPuk(PukFriendRecommend* puk) {
  if (!puk) {
    return false;
  }
  puk->set_timestamp(20110225);
  for (int i = 1; i <= 10; ++i) {
    PukFriendRecommend_CommonFriend* pfrc = puk->add_commonfriendseq();
    pfrc->set_userid(i * 1000 + i);
    pfrc->set_weight(i * 100 + i);
    pfrc->set_sharecount(i * 10 + i);
    pfrc->add_shares(i * 2000);
  }
  return true;
}

bool SerializePuk(PukFriendRecommend* puk, std::vector<unsigned char>* v) {
  if (!puk) {
    return false;
  }
  int byte_size = puk->ByteSize();
  if (!byte_size) {
    return false;
  }
  v->clear();
  v->reserve(byte_size);
  return puk->SerializeToArray(v->data(), byte_size); 
}

void PrintPuk(const PukFriendRecommend& puk) {
  LOG(INFO) << "puk.timestamp():" << puk.timestamp();
  BOOST_FOREACH(const PukFriendRecommend_CommonFriend& pfrc, puk.commonfriendseq()) {
    LOG(INFO) << "\tpfrc.userid():" << pfrc.userid();
    LOG(INFO) << "\tpfrc.weight():" << pfrc.weight();
    LOG(INFO) << "\tpfrc.sharecount():" << pfrc.sharecount();
    BOOST_FOREACH(int share_id, pfrc.shares()) {
      LOG(INFO) << "\t\tshare_id:" << share_id;
    }
  }
}
void PrintItems(const Items& items) {
  LOG(INFO) << "items.timestamp():" << items.timestamp();
  BOOST_FOREACH(const Item& item, items.items()) {
    LOG(INFO) << "\titem.id():" << item.id();
    if (item.has_field()) {
      LOG(INFO) << "\titem.field():" << item.field();
    }
    if (item.has_message()) {
      LOG(INFO) << "\titem.message():" << item.message();
    }

    BOOST_FOREACH(int field_id, item.fields()) {
      LOG(INFO) << "\t\tfield_id:" << field_id;
    }

    BOOST_FOREACH(std::string message, item.messages()) {
      LOG(INFO) << "\t\tmessage:" << message;
    }
  }
}
}}
