#ifndef __BATCHUPDATERUSERCOUNTREPLICA_ADAPTER_H__
#define __BATCHUPDATERUSERCOUNTREPLICA_ADAPTER_H__

#include "BatchUpdater.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace MyUtil;
using namespace mop::hi::svc::model;

//const static int INDEX = 0;

class BatchUpdaterUserCountReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<BatchUpdaterManagerPrx> ,
	public MyUtil::Singleton<BatchUpdaterUserCountReplicaAdapter>  {
public:
	BatchUpdaterUserCountReplicaAdapter() : MyUtil::ReplicatedClusterAdapterI <BatchUpdaterManagerPrx> ("ControllerBatchUpdaterUserCount", 120, 300){
	}

	void inc(int id, int type, int count);
	void dec(int id, int type, int count);
	void set(int id, int type, int count);

	void incLong(long id, int type, int count);
        void decLong(long id, int type, int count);
        void setLong(long id, int type, int count);


//private:
//	virtual void initialize();

//	xce::clusterstate::ClientInterface<BatchUpdaterManagerPrx> _clientCS;   /* @brief ClusterState的接口 */

//	int _cluster;
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

