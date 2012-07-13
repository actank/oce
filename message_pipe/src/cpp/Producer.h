#ifndef _MP_PRODUCER_H__
#define _MP_PRODUCER_H__


#include "Common.h"
#include "Singleton.h"
#include "MessageData.h"
#include "ClusterManager.h"

namespace xce{
namespace messagepipe{

class Producer : public Singleton<Producer>{

public:
	bool send(const std::string& zk_address, const MessageDataPtr& producerData);

	void sendBatch(const std::string& zk_address, const std::vector<MessageDataPtr>& producerDatas); 

/*
	static ProducerPtr getInstance(const std::string& zk_address);
  static std::map<std::string, Producer> producer_map_;
  static boost::mutex map_mutex_;
*/

};

}
}
#endif
