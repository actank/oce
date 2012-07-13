#ifndef _MP_ZKMANAGER_H__
#define _MP_ZKMANAGER_H__


#include "Common.h"
#include "Broker.h"
#include "zkclient.h"

namespace xce{
namespace messagepipe{

class ZKManager {

public:

	static void subscribeClusterChanges(const std::string& zk_address, const std::string& cluster,
			com::renren::messageconfig::NamespaceListenerPtr listener);


	static BrokerPtr getZKBrokerInfo(const std::string& cluster, const std::string& brokerInfo) ;


	static void getZKBrokers(const std::string& zk_address, const std::string& cluster,std::vector<BrokerPtr>& brokers) ;

};

}
}

#endif
