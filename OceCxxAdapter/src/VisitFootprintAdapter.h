/*
 * =====================================================================================
 *
 *       Filename:  VisitFootprintAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年08月11日 17时27分22秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __VISITFOOTPRINT_ADAPTER_H__
#define __VISITFOOTPRINT_ADAPTER_H__

#include "FootprintUnite.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce{
namespace footprint {

class VisitFootprintAdapter :
	public MyUtil::Singleton<VisitFootprintAdapter> {
public:
	VisitFootprintAdapter();
	void visit(FootprintInfoPtr fpt);
	int getSize(Ice::Long id);
private:
	xce::clusterstate::ClientInterface<UgcFootprintCachePrx> _clientCS;   /* @brief ClusterState的接口 */
	int _cluster;
};

}
;
}
;
#endif


