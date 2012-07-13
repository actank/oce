#include "Common.h"
#include "ClusterManager.h"

namespace xce {
namespace messagepipe{

class Consumer : IceUtil::Thread {

private:
	BrokerConnectionPtr brokerConnection_;
	std::string zk_address_;
	std::string cluster_;
	std::string subject_;
	int maxDelay_;

public:
	Consumer(const std::string& zk_address,
			 const std::string& cluster, const std::string& subject, int maxDelay=1) {
		zk_address_ = zk_address;
		cluster_ = cluster;
		subject_ = subject;
		maxDelay_ = maxDelay;
		brokerConnection_ = ClusterManager::getInstance()->getConnection(zk_address, cluster);
		start();
	}

	virtual void onMessage(const std::vector<MessageDataPtr>& receiveMessages, std::vector<MessageDataPtr>& failMessages )=0;

	virtual void run() ;

protected:
  std::string getCluster();
};

}
}
