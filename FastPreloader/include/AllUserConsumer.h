#ifndef __FASTPRELOADERALLUSERCONSUMER_H__
#define __FASTPRELOADERALLUSERCONSUMER_H__

#include "Consumer.h"
#include "IceExt/src/AdapterI.h"

namespace xce{
namespace fastpreloader{

template<class T>
class AllUserConsumer : virtual public FacadeConsumer{
public:
	AllUserConsumer(FacadeProducerPtr& producer, 
			std::map<int,BlockingQueueIntPtr >& map,
                        vector<int>& loadmod,
			int batchsize, 
			int threadid,
			int cluster,
			FastPreloaderFactoryPtr& fpfactoryptr,
			std::string controller)
			: FacadeConsumer(producer,map,loadmod,batchsize,threadid),
			  _fpfactoryptr(fpfactoryptr),
			  _sequence(0),
			  _cluster(cluster),
			  _controller(controller){
		_adapter = NULL;
		_adapter = new ReplicatedClusterAdapterI<T>(_controller, 120, 300);
		if(_adapter == NULL)
			return;
		MCE_INFO("_adapter is not empty");
		for(int i=0; i<cluster; i++){
			//std::vector<T> prxs = _adapter->getInvalidProxySeq(i);
			std::vector<T> prxs = _adapter->getAllProxySeq(i);
			
			if(prxs.empty()){
				MCE_INFO("adapter " << i  <<" is empty");
			}
			MCE_INFO("prx is " << prxs.front()->ice_toString());
			_prxmap[i] = prxs.front();
		}
		MCE_INFO("init end");
	}

	virtual void consume(std::vector<int>& data) {
		
		//std::cout << "consumer" << _threadid << " consume "<<_sequenceid <<" "<<data.size() << std::endl;
		MCE_INFO("consumer" << _threadid << " consume "<<_sequence <<" "<<data.size());
		MyUtil::ObjectResultPtr objects = _fpfactoryptr->create(data);	
		try{
			//_prxmap[(data.front()%_cluster)]->setData(objects);
		} catch (Ice::Exception& e) {
                        MCE_WARN("[consume] Ice::Exception " << data.size() << " begin:" << data.front() << " end:" << data.back() << ": " << e.what());
                } catch (std::exception& e) {
                        MCE_WARN("[consume] std::excetpion " << data.size() << " begin:" << data.front() << " end:" << data.back() << ": " << e.what());
                } catch (...) {
                        MCE_WARN("[consume] exception" << data.size() );
                }

		_sequence++;
	}
private:
	FastPreloaderFactoryPtr _fpfactoryptr;
	int _sequence;
	int _cluster;
	std::string _controller;
	std::map<int,T> _prxmap;
	ReplicatedClusterAdapterI<T>* _adapter;
};
//typedef IceUtil::Handle<AllUserConsumer<T> > AllUserConsumerPtr;

}
}
#endif
