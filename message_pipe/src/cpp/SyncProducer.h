#ifndef _MP_SYNCPRODUCER_H__
#define _MP_SYNCPRODUCER_H__


#include "Common.h"
#include "MessageData.h"

namespace xce{
namespace messagepipe{

class SyncProducer : public cms::ExceptionListener {

private:
	std::string host_;
	int port_;

	std::string subject_;

	std::auto_ptr<activemq::core::ActiveMQSession> session_;
	std::auto_ptr<activemq::core::ActiveMQProducer> producer_;
	std::auto_ptr<cms::Connection> connection_; 

 public:
	SyncProducer(const std::string& host, int port,const std::string& subject):session_(),producer_(),connection_(){
		host_ = host;
		port_ = port;
		subject_ = subject;
	}

	bool connect() ;

	bool send(const std::vector<unsigned char>& data);

	void close();

	virtual void onException(const cms::CMSException& ex) ;

};

}
}
#endif
