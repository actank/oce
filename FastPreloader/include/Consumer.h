#ifndef __FASTPRELOADERCONSUMER_H__
#define __FASTPRELOADERCONSUMER_H__

#include <unistd.h>
#include "Share.h"
#include "Producer.h"

namespace xce{
namespace fastpreloader{

class Consumer : virtual public IceUtil::Thread {
public:
        Consumer(ProducerPtr& producer, xce::atomicstl::BlockingQueue<int>& queue, int batchsize):
                        _producer(producer),_queue(queue),_batchsize(_batchsize){
        }

        virtual void consume(std::vector<int>& data) = 0;

        virtual void run(){
        	try {
                        while (!_producer->isFinished()) {
				std::cout<< " wait for producer " << std::endl;
				/*std::vector<int> data;
				_queue.popfrontSeq( data, _consume_batch_size);
                                try {
                                        consume(data);
                                } catch (Exception e) {
                                        e.printStackTrace();
                                }*/
				sleep(1);
                        }
                        while (true) {
                                std::vector<int> data;
                                _queue.popfrontSeq( data, _batchsize);
				if (data.empty()) {
                                        break;
                                }
                                try {
                                        consume(data);
                                } catch (Exception e) {
                                        std::cout<<e.what();
                                }
                        }
                } catch (Exception e) {
                	std::cout<<e.what();
		}
	}

private:
	ProducerPtr _producer;
	xce::atomicstl::BlockingQueue<int>& _queue;
	int _batchsize;
};
typedef IceUtil::Handle<Consumer> ConsumerPtr;

class FacadeConsumer : virtual public IceUtil::Thread {
public:
        FacadeConsumer( FacadeProducerPtr& producer, std::map<int,BlockingQueueIntPtr >& map,
			vector<int>& loadmod,int batchsize, int threadid):
                        _producer(producer),_map(map),_loadmod(loadmod),_batchsize(batchsize),_threadid(threadid){
        }

        virtual void consume(std::vector<int>& data) = 0;

        virtual void run(){
                try {
                        while (true) {
				/*if (!_producer->isFinished()) {
	                        	//std::cout<< "Consumer" << _threadid << " wait for producer " << std::endl;
					MCE_INFO("Consumer" << _threadid << " wait for producer " );
					sleep(10);
					continue;	
				}*/

				if(_loadmod.empty()){
					//std::cout<< "Consumer" << _threadid << " finished " << std::endl;
					MCE_INFO("Consumer" << _threadid << " finished ");
					break;
				}
				for(vector<int>::iterator it = _loadmod.begin(); it!= _loadmod.end();++it){
					std::vector<int> data;
					_map[*it]->popfrontSeq( data, _batchsize);
					if (data.empty()) {
						it = _loadmod.erase(it);
						continue;
					}
					try {
						consume(data);
					} catch (Exception e) {
						std::cout<<e.what();
					}
				}
                        }
                } catch (Exception e) {
                        std::cout<<e.what();
                }
        }

private:
	FacadeProducerPtr _producer;
	std::map<int,BlockingQueueIntPtr >& _map;
	vector<int> _loadmod;
	int _batchsize;
protected:
        int _threadid;
};
typedef IceUtil::Handle<FacadeConsumer> FacadeConsumerPtr;

}
}
#endif

