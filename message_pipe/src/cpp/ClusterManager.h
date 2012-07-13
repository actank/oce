#ifndef _MP_CLUSTERMANAGER_H__
#define _MP_CLUSTERMANAGER_H__

#include "Common.h"
#include "BrokerConnection.h"
#include "ZKManager.h"
#include "Singleton.h"

namespace xce{
namespace messagepipe{

class ClusterManager : public Singleton<ClusterManager>{	

private :
  boost::shared_mutex mutex_;
	std::map<std::string,BrokerConnectionPtr> pool_;
public:
	BrokerConnectionPtr getConnection(const std::string& zk_address, const std::string& cluster);

};

}
}
#endif
