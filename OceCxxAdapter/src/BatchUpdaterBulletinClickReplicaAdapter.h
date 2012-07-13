#ifndef __BATCHUPDATER_BULLETINCLICK_REPLICA_ADAPTER_H__
#define __BATCHUPDATER_BULLETINCLICK_REPLICA_ADAPTER_H__

#include "BatchUpdater.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace com  {
namespace xiaonei  {
namespace adapter  {

using namespace MyUtil;
using namespace mop::hi::svc::model;

const static int INDEX = 0;

class BatchUpdaterBulletinClickReplicaAdapter : 
	public MyUtil::Singleton<BatchUpdaterBulletinClickReplicaAdapter>  {

public:
	BatchUpdaterBulletinClickReplicaAdapter():
		_clientCS("ControllerBatchUpdaterBulletinClick", 120, 300) {
	}

	void click(int userId, int bulletinId);

	void click(int userId, long bulletinId);

private:
	virtual void initialize();

	xce::clusterstate::ClientInterface<BatchUpdaterManagerPrx> _clientCS;   /* @brief ClusterState的接口 */

	int _cluster;

};

};
};
};

#endif
