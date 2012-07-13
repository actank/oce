#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <ClientFactory.h>
#include <ProducerManagerClient.h>
#include <ConfigClientFactory.h>
#include <CacheConfigClient.h>
#include <TripodClient.h>

int main(int argc, char *argv[]) {
  if(argc < 8) {
      std::cout<<"We need beginId, batchSize, expiredTime, zkAddress, namespaceId, businessId ,warmtype and warmservers! Correct way to use this is : ./exec beginId batchSize expiredTime zkAddress namespaceId businessId warmtype[all|hot] warmservers[ip:port]"<<std::endl;
      return 0;
  }

  std::string beginId(argv[1]);
  int batchSize = boost::lexical_cast<int>(argv[2]);
  int expiredTime = boost::lexical_cast<int>(argv[3]);
  std::string zkAddress(argv[4]);
  std::string namespaceId(argv[5]);
  std::string businessId(argv[6]);
  std::string warmtype(argv[7]);

  if(warmtype != "all" | warmtype != "hot") {
      std::cout<<"Warmtype must be all or hot!"<<std::endl;
      return 0;
  }
  

  bool hotKey = false;
  if(warmtype == "hot") {
      hotKey = true;
  }

  std::vector<std::string> prxVec;

  for(int i = 8; i < argc; ++i) {
      std::string strs(argv[i]);
      std::vector<std::string> parts;
      boost::algorithm::split(parts, strs, boost::algorithm::is_any_of(":"));
      if(parts.size() != 2) {
          std::cout<<"Warmserver must be <ip>:<port>!";
          return 0;
      }
      prxVec.push_back(strs);
  }

  bool warmServers = false;

  if(!prxVec.empty()) {
      warmServers = true;
  }

  com::renren::tripod::ProducerManagerClientPtr producer = com::renren::tripod::ClientFactory::getInstance()->getProducerManagerClient(zkAddress);
  if(producer == NULL) {
      std::cout<<"Can not get producer!"<<std::endl;
      return 0;
  }
  
  com::renren::tripod::CacheConfigClientPtr cacheConfig = com::renren::tripod::ConfigClientFactory::getInstance()->getCacheConfigClient(zkAddress);
  if(cacheConfig == NULL) {
      std::cout<<"Can not get cacheconfig!"<<std::endl;
      return 0;
  }
  
  com::renren::tripod::TripodClient tripodClient(zkAddress, namespaceId, businessId);

  while(true) {
      com::renren::tripod::KeySeq keys; 
      try {
          keys = producer->produceKeys(beginId, batchSize, businessId, hotKey, 300);
      }catch(...) {
          continue;
      }

      if(keys.empty()) {
          break;
      }

      com::renren::tripod::KeySeq warmKeys;

      if(warmServers) {
          warmKeys = cacheConfig->getFilterKeys(keys, prxVec, namespaceId);
      }else {
          warmKeys = keys;
      }

      if(!warmKeys.empty()) {
          try {
              tripodClient.load(warmKeys, expiredTime, false, 10 * 1000);
          }catch(...) {
          }
      }

      beginId = keys[keys.size() -1];

  }

  std::cout<<"Done!"<<std::endl;

  return 1;
}
