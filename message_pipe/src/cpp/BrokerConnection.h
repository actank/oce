#ifndef _MP_BROKERCONNECTION_H__
#define _MP_BROKERCONNECTION_H__


#include "Common.h"
#include "ZKManager.h"
#include "zkclient.h"
#include "MessageData.h"

namespace xce{
namespace messagepipe{

class BrokerConnection : public com::renren::messageconfig::NamespaceListener {

private:
	static const int DEFAULT_MAX_TRY = 3;
	std::vector<BrokerPtr> allBrokers_;
	std::vector<BrokerPtr> validBrokers_;
 	mutable volatile boost::uint32_t allSeed_; 
	mutable volatile boost::uint32_t validSeed_;

public:
	BrokerConnection(const std::string zk_address, const std::string& cluster) :
				 com::renren::messageconfig::NamespaceListener(cluster),allSeed_(0),validSeed_(0){
		std::cout<<"init "<<cluster<<" connection"<<std::endl;
		ZKManager::getZKBrokers(zk_address, cluster, allBrokers_);
		std::cout<<"get all brokers "<<allBrokers_.size()<<std::endl;
		for (std::vector<BrokerPtr>::const_iterator it=allBrokers_.begin();it!=allBrokers_.end();++it) {
			if ((*it)->isValid())
				validBrokers_.push_back(*it);
		}
		std::cout<<"get valid brokers size "<<validBrokers_.size()<<std::endl;
	}

	bool send(const MessageDataPtr& data) ;

	void receive(const std::string& subject,std::vector<BytesMessagePtr>& part) ;

	void printCurrentNode(); 

	void handleChildChange(const std::string& cluster,const std::list<std::string>& currentChilds);

	virtual bool Update(const std::list<std::string>& childrenNameList) ;

};

}
}
#endif
