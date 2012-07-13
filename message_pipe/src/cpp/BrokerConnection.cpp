#include "BrokerConnection.h"

namespace xce{
namespace messagepipe{
	bool BrokerConnection::send(const MessageDataPtr& data) {

		if (validBrokers_.empty()) {
			return false;
		}
		int maxTry = DEFAULT_MAX_TRY;
		int N = validBrokers_.size();
		while (maxTry-- > 0) {
			int index = (( boost::interprocess::detail::atomic_inc32(&validSeed_) % N) + N) % N;
			try{
				if (validBrokers_.at(index)->send(data)) {
			//		std::cout << "sucess send data to " << index << std::endl;
					return true;
				}
			}catch(...){
				std::cout <<"may out of index " << std::endl;
			}
		}
		return false;
	}

	void BrokerConnection::receive(const std::string& subject,std::vector<BytesMessagePtr>& part) {
		if (allBrokers_.empty()) {
			return;
		}
		int maxTry = DEFAULT_MAX_TRY;
		int N = allBrokers_.size();
		while (maxTry-- > 0) {
			int index = (( boost::interprocess::detail::atomic_inc32 (&allSeed_) % N) + N) % N;
			try{
				allBrokers_.at(index)->receive(subject,part);
				if (!part.empty()) {
					std::cout<<"sucess receive data size: " << part.size() << std::endl;
					return;
				} else {
					break;
				}
			}catch(...){
				std::cout <<"may out ofo index" << std::endl; 
			}
		}
		return;
	}

	void BrokerConnection::printCurrentNode() {
		std::cout << "----current----valid----brokers" << std::endl;
		for (std::vector<BrokerPtr>::const_iterator it=validBrokers_.begin();it!=validBrokers_.end();++it) {
			std::cout<<(*it)->toString()<<std::endl;
		}
		std::cout << "----current-----all-----brokers" << std::endl;

		for (std::vector<BrokerPtr>::const_iterator it=allBrokers_.begin();it!=allBrokers_.end();++it) {
			std::cout<<(*it)->toString()<<std::endl;
		}

		std::cout << "-------------------------------" << std::endl;
	}

	// @Override

	// if we shut down a machine ,we must change the status to offline
	// and delete to avoid some message no consumer

	void BrokerConnection::handleChildChange(const std::string& cluster, const std::list<std::string>& currentChilds){
		std::vector<BrokerPtr> curValidBrokers;
		std::vector<BrokerPtr> curAllBrokers;

		printCurrentNode();

		for (std::list<std::string>::const_iterator it = currentChilds.begin(); it!=currentChilds.end(); ++it) {
			std::string child = *it;
			BrokerPtr broker = ZKManager::getZKBrokerInfo(cluster, child);
			size_t index = -1;
			for(;index < allBrokers_.size();++index){
				if(allBrokers_[index]->equals(broker))
					break;
			}
			if (index >= 0 && index < allBrokers_.size()) {
				broker = allBrokers_[index];
			}
			curAllBrokers.push_back(broker);
			if (broker->isValid()) {
				curValidBrokers.push_back(broker);
			}
		}
		allBrokers_.swap(curAllBrokers);
		validBrokers_.swap(curValidBrokers);
		printCurrentNode();
	}

	bool BrokerConnection::Update(const std::list<std::string>& childrenNameList) {
		try {
			handleChildChange(ns(), childrenNameList);
		} catch (std::exception& e) {
			std::cout<< e.what()<<std::endl;
			return false;
		}
		return true;
	}
}
}
