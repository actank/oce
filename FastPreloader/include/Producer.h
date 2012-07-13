#ifndef __FASTPRELOADERPRODUCER_H__
#define __FASTPRELOADERPRODUCER_H__

#include "Share.h"

namespace xce{
namespace fastpreloader{

class Producer : virtual public IceUtil::Thread {
public:
	Producer(xce::atomicstl::BlockingQueue<int>& queue):
			_queue(queue),_isfinished(false){
	}
	
	virtual bool produce(std::vector<int>& data) = 0;
	
	virtual bool isFinished(){
		return _isfinished;
	}

	virtual void run(){
		std::vector<int> data; 
               	while(true){
			if (!produce(data)) {
				_isfinished = true;
				break;
			}
                	_queue.pushSeq(data);
			data.clear();
		} 	
	}

private:
	xce::atomicstl::BlockingQueue<int>& _queue;
	bool _isfinished;
};
typedef IceUtil::Handle<Producer> ProducerPtr;

class FacadeProducer : virtual public IceUtil::Thread {
public:
        FacadeProducer(std::map<int,BlockingQueueIntPtr >& map, int cluster):
                        _map(map),_cluster(cluster),_isfinished(false){
        }

        virtual bool produce(std::vector<int>& data) = 0;
  
	virtual bool isFinished(){
                return _isfinished;
        }

	virtual int getCluster(){
		return _cluster;
	}
	      
        virtual void run(){
                std::vector<int> data;
                while(true){
                        if (!produce(data)) {
                                //std::cout << " producer finished" << std::endl;
				MCE_INFO(" producer finished ");
				_isfinished = true;
				break;
                        }
			for(std::vector<int>::iterator it = data.begin(); it!=data.end(); ++it){
				_map[*it%_cluster]->push(*it);
			}
                        MCE_INFO(" map[" << data.front()%_cluster <<"] size = " << _map[data.front()%_cluster]->getSize());
			data.clear();
                } 
        }

private:
        std::map<int,BlockingQueueIntPtr >& _map;
	int _cluster;
	bool _isfinished;
};
typedef IceUtil::Handle<FacadeProducer> FacadeProducerPtr;

}
}
#endif
