#include "Broker.h"
#include "BrokerConnection.h"
#include "AsyncConsumer.h"
#include "SyncProducer.h"
#include "MessageData.h"

//				boost::shared_lock<boost::shared_mutex> lock(cacheConfigClientMaplock_);
//        boost::upgrade_lock<boost::shared_mutex> lock(cacheConfigClientMaplock_);
//        boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock1(lock);

namespace xce{
namespace messagepipe{

  boost::shared_mutex Broker::global_mutex_;
 
  bool Broker::send(const MessageDataPtr& data){
			std::map<std::string,SyncProducerPtr>::iterator it;
			bool exist = false;
			{
				boost::unique_lock<boost::shared_mutex> lock(producer_mutex_);
		//	  IceUtil::Mutex::Lock lock(mutex_producer_);
				it = subject2Producer_.find(data->getSubject());
				if(it == subject2Producer_.end()){
					boost::unique_lock<boost::shared_mutex> g_lock(global_mutex_);
					SyncProducerPtr producer(new SyncProducer(host_, port_, data->getSubject()));
					try{
						std::cout<<"SyncProducer connecting thread_id:" << boost::this_thread::get_id() <<std::endl;
						producer->connect();
						std::pair<std::map<std::string,SyncProducerPtr>::iterator,bool> res =
										subject2Producer_.insert(std::make_pair<std::string,SyncProducerPtr>(data->getSubject(),producer));
						it = res.first;
						exist = true;
					}catch(cms::CMSException& e){
						std::cout << "Producer Connect Occur " << e.what() << std::endl;
					}catch(...){
						std::cout << "Producer Connect Occur Unknown Exception"<< std::endl;
					}
				}else{
					exist = true;
				}
			}
			if(exist){
				try{
					it->second->send(data->getData());
					return true;
				}catch(cms::CMSException& e){
					std::cout << "Broker send fail " << e.what() << std::endl;
				}catch(...){
					std::cout << "Broker send UnKnown Exception" << std::endl;
				}
			}
			return false;
	}	
  void Broker::receive(const std::string& subject,std::vector<BytesMessagePtr>& messages){
			std::map<std::string,AsyncConsumerPtr>::iterator it;
			bool exist = false;
			{
					boost::unique_lock<boost::shared_mutex> lock(consumer_mutex_);
//				  IceUtil::Mutex::Lock lock(mutex_consumer_);
					it = subject2Consumer_.find(subject);
					if(it == subject2Consumer_.end()){
						boost::unique_lock<boost::shared_mutex> g_lock(global_mutex_);
						AsyncConsumerPtr consumer(new AsyncConsumer(host_,port_, subject));
						try{
							consumer->connect();
//							consumer->start();
							std::pair<std::map<std::string,AsyncConsumerPtr>::iterator,bool> res = 
									subject2Consumer_.insert(std::make_pair<std::string,AsyncConsumerPtr>(subject,consumer));
							it = res.first;
							exist = true;
						}catch(cms::CMSException& e){
							std::cout << "Consumer Connect Occur " << e.what() << std::endl;
						}catch(...){
							std::cout << "Consumer Connect Occur Unknown Exception"<< std::endl;
						}
					}else {
						exist = true;
					}
			}
			if(exist){
				try{
					it->second->receive(messages);
				}catch(cms::CMSException& e){
					std::cout << "Broker receive fail " << e.what() << std::endl;
				}catch(...){
					std::cout << "Broker receive UnKnown Exception" << std::endl;
				}
			}
	}
}
}
