#include "ConfigClientFactory.h"
#include "CacheManagerConfigClient.h"
#include "ProducerManagerConfigClient.h"
#include "CacheConfigClient.h"
#include "RedisClientPool.h"
#include <iostream> 
#include <boost/foreach.hpp>
#include "LogWrapper.h"

int main() {
  using namespace com::renren::tripod;
  MCE_DEFAULT_INIT("WARN");
  std::cout << "configclient main ... " << std::endl;
  RedisClientPool pool("10.3.18.139", 6379); 
  RedisClient* client = pool.BorrowItem();
  if (!client || !client->IsValid()) {
    MCE_DEBUG("client is invalid!!");
    return 0;
  }
  StrList list = client->GetList("test");
  BOOST_FOREACH(const std::string& i, list) {
    std::cout << i << std::endl;
  }

  //client->RemoveList("test123");
  //client->RemoveList("test");

  StrList str_list;
  str_list.push_back("nba");
  str_list.push_back("cba");
  str_list.push_back("wnba");

  std::string key("test");
  client->SetList(key, str_list);

  pool.ReturnItem(client);
  return 0;
}

