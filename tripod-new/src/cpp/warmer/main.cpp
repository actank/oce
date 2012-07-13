#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <ClientFactory.h>
#include <ProducerManagerClient.h>
#include <ConfigClientFactory.h>
#include <CacheConfigClient.h>
#include <TripodClient.h>
#include <util/cpp/TimeCost.h>

int main(int argc, char *argv[]) {
  MCE_DEFAULT_INIT("DEBUG");
  if(argc < 8) {
      std::cout<<"We need type beginId, endId, batchSize, expiredTime, zkAddress, namespaceId, businessId ,warmtype and warmservers! Correct way to use this is : ./exec [warm|load] beginId endId batchSize expiredTime zkAddress namespaceId businessId warmtype[all|hot] warmservers[ip:port]"<<std::endl;
      return 0;
  }

  bool warmFlag = false;
  if(std::string(argv[1]) == "warm") {
      warmFlag = true;
  }
  std::string beginId(argv[2]);
  int endId = boost::lexical_cast<int>(argv[3]);
  int batchSize = boost::lexical_cast<int>(argv[4]);
  int expiredTime = boost::lexical_cast<int>(argv[5]);
  std::string zkAddress(argv[6]);
  std::string namespaceId(argv[7]);
  std::string businessId(argv[8]);
  std::string warmtype(argv[9]);

  if(warmtype != "all" && warmtype != "hot") {
      std::cout<<"Warmtype must be all or hot!"<<std::endl;
      return 0;
  }
  

  bool hotKey = false;
  if(warmtype == "hot") {
      hotKey = true;
  }

  std::vector<std::string> prxVec;

  for(int i = 10; i < argc; ++i) {
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

  int maxId = boost::lexical_cast<int>(beginId);

  while(true) {

      std::ostringstream os;
      os<<"beginId = "<<beginId;
      MyUtil::TimeCost tc = MyUtil::TimeCost::create(os.str());
      com::renren::tripod::KeySeq keys; 
      try {
          keys = producer->produceKeys(beginId, batchSize, businessId, hotKey, 300);
      }catch(...) {
          beginId = boost::lexical_cast<std::string>(boost::lexical_cast<int>(beginId)+1);
          ++maxId;
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
              if(warmFlag) {
                  tripodClient.load(warmKeys, expiredTime, false, 10 * 1000);
              }else{
                  tripodClient.getWithMissed(warmKeys, expiredTime, false, 10 * 1000);
              }
          }catch(...) {
          }
      }

      std::map<int, string> keyMap;
      for(com::renren::tripod::KeySeq::iterator it = keys.begin(); it != keys.end(); ++it) {
          keyMap.insert(make_pair(boost::lexical_cast<int>(*it), *it));
      }
      if(!keys.empty()) {
          beginId = keyMap.rbegin()->second;
          beginId = boost::lexical_cast<std::string>(boost::lexical_cast<int>(beginId)+1);
          maxId = keyMap.rbegin()->first;
          if(maxId >= endId) {
              break;
          }
      }

  }

  std::cout<<"Done!"<<std::endl;

  return 1;
}
