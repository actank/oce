
#include "Producer.h"

namespace xce{
namespace messagepipe{

bool Producer::send(const std::string& zk_address, const MessageDataPtr& producerData) {
	if(producerData->getCluster().empty() || producerData->getSubject().empty() || producerData->getData().empty()){
		return false;
	}
	BrokerConnectionPtr brokerConnection = ClusterManager::getInstance()->getConnection(zk_address,producerData->getCluster());
	if (brokerConnection != NULL) {
		return brokerConnection->send(producerData);
	} else {
		return false;
	}
}

void Producer::sendBatch(const std::string& zk_address, const std::vector<MessageDataPtr>& producerDatas) {
	for (std::vector<MessageDataPtr>::const_iterator it=producerDatas.begin();it!=producerDatas.end();++it) {
		send(zk_address, *it);
	}
}
/*
ProducerPtr Producer::getInstance(const std::string& zk_address) {

  boost::mutex::scoped_lock lock(map_mutex_);
  std::map<std::string, ProducerPtr>::const_iterator iter = Producer::producer_map_.find(zk_address);
  if (iter != producer_map_.end()) {
    return iter->second;
  }else{
		ProducerPtr producer = new Producer(zk_address);	
		Producer::producer_map_.insert(make_pair<std::string,ProducerPtr>(zk_address,producer));
	}
}
*/

}
}
