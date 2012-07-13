/*
 * =====================================================================================
 *
 *       Filename:  HomeFootprintReplicaAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月30日 15时28分23秒
 *       Revision:  none
 *       Compiler:  g++
 *
 *         Author:  min.shang (Jesse Shang), min.shang@renren-inc.com
 *        Company:  renren-inc
 *
 * =====================================================================================
 */
#ifndef __HOME_FOOTPRINT_REPLICA_ADAPTER_H__
#define __HOME_FOOTPRINT_REPLICA_ADAPTER_H__

#include "FootprintUnite.h"
#include <AdapterI.h>
#include <ServiceI.h>
#include <IceUtil/IceUtil.h>

namespace xce {
namespace footprint {
class HomeFootprintReplicaAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::footprint::HomeFootprintReplicaPrx>,
    public MyUtil::Singleton<HomeFootprintReplicaAdapter> {
public:
  HomeFootprintReplicaAdapter() : MyUtil::ReplicatedClusterAdapterI<xce::footprint::HomeFootprintReplicaPrx> ("ControllerHomeFootprintReplica", 120,
            300, new MyUtil::OceChannel, true, "HFR") {};
  void setFootprint(const MyUtil::ObjectResultPtr& pData, int category);
  void setUserVisitCount(const MyUtil::ObjectResultPtr& pData);
private:
  HomeFootprintReplicaPrx getHomeFootprintReplicaPrx(int id = 0);
  vector<HomeFootprintReplicaPrx> getAllHomeFootprintReplicaPrx(int id = 0);
  map<int, MyUtil::ObjectResultPtr> seperateObjectResultByMod(const MyUtil::ObjectResultPtr& ptr, int mod);
};
}
}

#endif
