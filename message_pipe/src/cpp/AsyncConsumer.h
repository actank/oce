#ifndef _MP__ASYNCCONSUMER_H__
#define _MP__ASYNCCONSUMER_H__


#include "Common.h"

namespace xce{
namespace messagepipe{

class AsyncConsumer : public IceUtil::Thread ,virtual public cms::ExceptionListener, public cms::MessageListener{

private:
	std::string host_;
	int port_;
	std::string subject_;

	time_t last_;
	ActiveMQSessionPtr session_;
	ConnectionPtr connection_;
//	ActiveMQConsumerPtr consumer_;
	std::auto_ptr<activemq::core::ActiveMQConsumer> consumer_;

	std::vector<BytesMessagePtr> buffers_;
	boost::condition_variable push_cdt_;
	boost::mutex push_mutex_;
	IceUtil::Monitor<IceUtil::Mutex> mutex_push_;

public:
	AsyncConsumer(const std::string& host, int port, const std::string& subject):
			host_(host),port_(port),subject_(subject),session_(),connection_(),consumer_(){
		last_ = time(NULL);
	}
 	virtual ~AsyncConsumer() throw(){
	}

	bool connect();
 
	void receive(std::vector<BytesMessagePtr>& res) ;

	void push(std::vector<BytesMessagePtr>& messages);

	virtual void run() ;

	virtual void onException(const cms::CMSException& ex) ;
	virtual void onMessage(const cms::Message* ) throw();

	void close();	

};

}
}
#endif
