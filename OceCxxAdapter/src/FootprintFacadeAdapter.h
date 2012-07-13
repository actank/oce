/*
 * =====================================================================================
 *
 *       Filename:  FootprintFacadeAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年09月07日 11时54分45秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __FOOTPRINT_FACADE_ADAPTER_H__
#define __FOOTPRINT_FACADE_ADAPTER_H__

#include "FootprintUnite.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace footprint {

class FootprintFacadeAdapter: 
	public MyUtil::Singleton<FootprintFacadeAdapter> {
public:
	FootprintFacadeAdapter();

	void interview(const FootprintInfoPtr& fpt);	

private:	
	xce::clusterstate::ClientInterface<FootprintFacadeManagerPrx> _clientCS;   /* @brief ClusterState的接口 */

	int _cluster;

	MyUtil::OceChannel _channel;
};

};
};

#endif


