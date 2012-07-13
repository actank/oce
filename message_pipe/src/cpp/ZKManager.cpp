
#include "ZKManager.h"

using xce::messagepipe::BrokerPtr;

void xce::messagepipe::ZKManager::subscribeClusterChanges(const std::string& zk_address, const std::string& cluster,
			com::renren::messageconfig::NamespaceListenerPtr listener){

		com::renren::messageconfig::ZkClient::GetInstance(zk_address)->AddNamespaceListener(listener);
}

BrokerPtr xce::messagepipe::ZKManager::getZKBrokerInfo(const std::string& cluster, const std::string& brokerInfo) {
		std::vector<std::string> infos;  
		BrokerPtr bp;
		boost::algorithm::split(infos, brokerInfo, boost::algorithm::is_any_of("|"));
		if(infos.size()==3){
			std::string host = infos[0];
			int port = 61616;
			try{
			  port = boost::lexical_cast<int>(infos[1]);
			}catch(...){
				return bp;
			}
			std::string status = infos[2];
			if(status=="ONLINE" || status=="OFFLINE"){
				bp.reset(new Broker(cluster, host, port, status));
			}	
		}
		return bp;
}

void xce::messagepipe::ZKManager::getZKBrokers(const std::string& zk_address, const std::string& cluster,std::vector<BrokerPtr>& brokers) {
		std::list<std::string> childs;
		//childs.push_back("localhost|61616|ONLINE");
		//childs.push_back("10.3.23.200|61616|ONLINE");
		com::renren::messageconfig::ZkClient::GetInstance(zk_address)->GetNodes(cluster,&childs);
		for (std::list<std::string>::const_iterator it=childs.begin();it!=childs.end();++it) {
			BrokerPtr p = getZKBrokerInfo(cluster,*it);
			if(p.get())
				brokers.push_back(p);
		}
}
