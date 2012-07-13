#include "SyncProducer.h"

namespace xce{
namespace messagepipe{

bool SyncProducer::connect() {
		using namespace activemq::core;
		using namespace cms;
		std::ostringstream oss;
		oss<<"tcp://"<<host_<<":"<<port_;

		std::cout << "activemq connect "<< oss.str() <<std::endl;

		//std::auto_ptr<ConnectionFactory> connectionFactory(
			//							ConnectionFactory::createCMSConnectionFactory(oss.str()));

		// Create a Connection
		//connection_ = connectionFactory->createConnection();
		//connection_->start();

		// Create a Session
		//session_ = connection_->createSession( Session::AUTO_ACKNOWLEDGE );

		// Create the destination (Topic or Queue)
		//destination_ = session_->createQueue( subject_ );

		// Create a MessageProducer from the Session to the Topic or Queue
		//producer_ = session_->createProducer( destination_ );

		ActiveMQConnectionFactory connectionFactory(oss.str());

		connectionFactory.setAlwaysSyncSend(true);

		connection_.reset( connectionFactory.createConnection() );

    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(connection_.get());

    session_.reset(dynamic_cast<ActiveMQSession*>(
            amqConnection->createSession() ));

		std::auto_ptr<cms::Destination> destination(session_->createQueue(subject_));

		amqConnection->start();

  	amqConnection->setExceptionListener(this);
	  producer_.reset(dynamic_cast<ActiveMQProducer*>(
            session_->createProducer( destination.get() )));
		return true;
}

bool SyncProducer::send(const std::vector<unsigned char>& data){
		cms::BytesMessage* message = session_->createBytesMessage();
		message->writeBytes(data);
		producer_->send(message);
		return true;
}

void SyncProducer::close() {
		try {
			connection_->close();
		} catch (cms::CMSException& e) {
			std::cout<<"close occur exception "<< e.what();
		}
}

void SyncProducer::onException(const cms::CMSException& ex) {
		std::cout << "onException " << ex.what() << std::endl;
		do {
			try {
				if (connect())
					break;
			} catch (std::exception& e) {
				std::cout<<"connect error " << e.what() << std::endl; 
			}
		} while (true);
}

}
}

