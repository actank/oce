#include "ClusterManager.h"

namespace xce{
namespace messagepipe{

BrokerConnectionPtr ClusterManager::getConnection(const std::string& zk_address, const std::string& cluster){
			std::map<std::string,BrokerConnectionPtr>::iterator it; 
			std::string key = zk_address + "_" + cluster;
			{
				boost::shared_lock<boost::shared_mutex> lock(mutex_);
     		it = pool_.find(key);
			}
      if(it == pool_.end()){
				std::cout<<"cluster "<<cluster<<" not found"<<std::endl;
        boost::unique_lock<boost::shared_mutex> lock(mutex_);
        try{
						BrokerConnectionPtr brokerConnection(new BrokerConnection(zk_address,cluster));
						ZKManager::subscribeClusterChanges(zk_address, cluster,	brokerConnection);
            std::pair<std::map<std::string,BrokerConnectionPtr>::iterator,bool> res
								 = pool_.insert(std::make_pair<std::string,BrokerConnectionPtr>(key,brokerConnection));
            it = res.first;
        }catch(cms::CMSException& e){
          std::cout << "Producer Connect Occur " << e.what() << std::endl;
        }catch(...){
          std::cout << "Producer Connect Occur Unknown Exception"<< std::endl;
        }
      }
      if(it!=pool_.end())
        return it->second;
			else
				return BrokerConnectionPtr();
}
}
}
