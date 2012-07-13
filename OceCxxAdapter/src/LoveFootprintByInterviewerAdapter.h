/*
 * =====================================================================================
 *
 *       Filename:  LoveFootprintAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年09月17日 17时37分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __LOVEFOOTPRINT_BYINTERVIEWER_ADAPTER_H__
#define __LOVEFOOTPRINT_BYINTERVIEWER_ADAPTER_H__

#include "FootprintUnite.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "AdapterI.h"
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce{
namespace footprint {

class LoveFootprintByInterviewerAdapter :
	public MyUtil::Singleton<LoveFootprintByInterviewerAdapter> {
public:
	LoveFootprintByInterviewerAdapter();
	void visit(const FootprintInfoPtr& fpt);
private:
	xce::clusterstate::ClientInterface<CommonFootprintCachePrx> _clientCS;   /* @brief ClusterState的接口 */
	int _cluster;
};

}
;
}
;
#endif


