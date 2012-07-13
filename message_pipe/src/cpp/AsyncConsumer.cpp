#include "AsyncConsumer.h"

namespace xce{
namespace messagepipe{

bool AsyncConsumer::connect() {
		using namespace activemq::core;
		std::ostringstream oss;
		oss<<"tcp://"<<host_<<":"<<port_;
    ActiveMQConnectionFactory connectionFactory(oss.str());

    connection_.reset( connectionFactory.createConnection() );

    ActiveMQConnection* amqConnection = dynamic_cast<ActiveMQConnection*>(connection_.get());

    session_.reset(dynamic_cast<ActiveMQSession*>(
            amqConnection->createSession(cms::Session::CLIENT_ACKNOWLEDGE) ));

	 	std::auto_ptr<cms::Queue> queue( session_->createQueue(subject_) );
//    std::auto_ptr<cms::Destination> destination(session_->createQueue(subject_));

    // Create a consumer
    consumer_.reset(
        dynamic_cast<ActiveMQConsumer*>( session_->createConsumer( queue.get() ) ) );

		consumer_->setMessageListener(this);
    amqConnection->setExceptionListener(this);
    amqConnection->start();
		return true;
}


void AsyncConsumer::receive(std::vector<BytesMessagePtr>& res) {
	//lock and swap	
		 boost::unique_lock<boost::mutex> lock(push_mutex_);
//	   IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_push_);
		 if(!buffers_.empty()){
		 		res.swap(buffers_);
		 }
}

void AsyncConsumer::push(std::vector<BytesMessagePtr>& messages){
//		 boost::unique_lock<boost::mutex> lock(push_mutex_);
//	  IceUtil::Monitor<IceUtil::Mutex>::Lock lock(mutex_push_);

/*
		 if(!buffers_.empty()){
				mutex_push_.wait();
		 }

		 buffers_.swap(messages);
*/
}

void AsyncConsumer::run() {
		std::vector<BytesMessagePtr> messages;
		time_t last = time(NULL);
		while (true) {
			try {
				cms::Message* message;
				if ((message = consumer_->receive(1000)) != NULL) {
					BytesMessagePtr textMessage(dynamic_cast< cms::BytesMessage* >( message ));
					messages.push_back(textMessage);
				}
				if(messages.size() >= 1000 || (time(NULL) - last)>=1){
						push(messages);
						last = time(NULL);
				}
			} catch (cms::CMSException& ex) {
				std::cout << "AsyncConsumer Thread Occur " << ex.what() << std::endl;
				sleep(1);
			} catch (...){
				std::cout << "AsyncConsumer Thread Occur UnKnown Exception" << std::endl;
			}
		}
}

void AsyncConsumer::onException(const cms::CMSException& ex) {
		std::cout<<"onException : " << ex.what() << std::endl;
		do {
			try {
				if (connect())
					break;
			} catch (std::exception& ex) {
				std::cout<<"connect occur exception "<<ex.what()<<std::endl;
			}
		} while (true);
}

void AsyncConsumer::onMessage( const cms::Message* message ) throw(){

		try{
				const	cms::BytesMessage*  mp = dynamic_cast<const cms::BytesMessage* >( message );
				BytesMessagePtr byteMessage(mp->clone());
		 		boost::unique_lock<boost::mutex> lock(push_mutex_);
				buffers_.push_back(byteMessage);
				
		} catch (cms::CMSException& e) {
				e.printStackTrace();
		}
}


void AsyncConsumer::close() {
		try {
			connection_->close();
		} catch (cms::CMSException& ex) {
				std::cout<<"close occur exception "<<ex.what()<<std::endl;
		}
}

}
}

