#ifndef __BATCHUPDATERUSERAPICOUNTREPLICA_ADAPTER_H__
#define __BATCHUPDATERUSERAPICOUNTREPLICA_ADAPTER_H__

#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"
#include "BatchUpdater.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {

//const static int INDEX = 0;

class BatchUpdaterUserApiCountReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<mop::hi::svc::model::BatchUpdaterManagerPrx>,
	public MyUtil::Singleton<BatchUpdaterUserApiCountReplicaAdapter> {
public:

	BatchUpdaterUserApiCountReplicaAdapter() : MyUtil::ReplicatedClusterAdapterI <mop::hi::svc::model::BatchUpdaterManagerPrx> ("ControllerBatchUpdaterUserApiCount", 120, 300){
	}       
	virtual ~BatchUpdaterUserApiCountReplicaAdapter() {
	} 


	void inc(int id, int type, int count);
	void dec(int id, int type, int count);
	void set(int id, int type, int count);

	void incLong(long id, int type, int count);
        void decLong(long id, int type, int count);
        void setLong(long id, int type, int count);


};

}
;
}
;
}
;
}
;

#endif

