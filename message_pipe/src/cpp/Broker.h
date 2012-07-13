#ifndef _MP_BROKER_H__
#define _MP_BROKER_H__


#include "Common.h"
#include "SyncProducer.h"
#include "AsyncConsumer.h"

namespace xce{
namespace messagepipe{

class Broker 
{

public:
	std::string cluster_;
	std::string host_;
	int port_;
	std::string status_;
	bool send(const MessageDataPtr& data);
	void receive(const std::string& subject,std::vector<BytesMessagePtr>& buffer);

	Broker(const std::string& cluster,const std::string& host, int port,const std::string& status):
					cluster_(cluster),host_(host),port_(port),status_(status){
	}
	bool isValid(){
		return status_ == "ONLINE";
	}
	std::string toString(){
		std::ostringstream oss;
		oss<<"cluster:"<<cluster_<<" host:"<<host_<<" port:"<<port_<<" status:";
		return oss.str();
	}
	bool equals(const BrokerPtr& op){
		return (cluster_ == op->cluster_) && (host_ == op->host_) && (port_ == op->port_); 
	}
private:
	std::map<std::string,SyncProducerPtr> subject2Producer_;
	std::map<std::string, AsyncConsumerPtr> subject2Consumer_;
	boost::shared_mutex producer_mutex_; 
	boost::shared_mutex consumer_mutex_; 
	IceUtil::Mutex mutex_producer_;
	IceUtil::Mutex mutex_consumer_;
	static boost::shared_mutex global_mutex_;
};

}
}

#endif
