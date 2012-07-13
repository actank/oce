/*
 * =====================================================================================
 *
 *       Filename:  FootprintViewAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年07月07日 18时23分02秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __FOOTPRINT_VIEW_ADAPTER_H__
#define __FOOTPRINT_VIEW_ADAPTER_H__

#include "Channel.h"
#include "Singleton.h"
#include "FootprintView.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"


namespace xce {
namespace footprint {

const int CLUSTER_SIZE = 10;

class FootprintViewAdapter: public MyUtil::Singleton<FootprintViewAdapter> {
public:
	FootprintViewAdapter();
	//~FootprintViewAdapter();
	
	bool verifyVisitCount(const MyUtil::Int2IntMap& counts);
	void setVisitCountLimit(int owner);
private:	
	xce::clusterstate::ClientInterface<FootprintViewManagerPrx> _clientCS;   /* @brief ClusterState的接口 */
	int _cluster;
	MyUtil::OceChannel _channel;
};

};
};

#endif
