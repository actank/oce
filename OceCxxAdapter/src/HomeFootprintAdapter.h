#ifndef __HOME_FOOTPRINT_ADAPTER_H__
#define __HOME_FOOTPRINT_ADAPTER_H__

#include "FootprintUnite.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace footprint {

class HomeFootprintAdapter: 
	public MyUtil::Singleton<HomeFootprintAdapter> {
public:
	HomeFootprintAdapter();
  HomeVisitHistoryPtr getHomeFootprintObj(long id, int category);
  UserVisitCountPtr getUserVisitCountObj(int id);
	void visit(const FootprintInfoPtr& fpt, const map<string,string>& );	
	void remove(const FootprintInfoPtr& fpt, const map<string, string>&);

	HomeInfoPtr visitAndGet(const FootprintInfoPtr& fpt,int begin,int nLimit,int tLimit,const map<string,string>&);

	int getSize(int uid);

	int getUserVisitCount(int uid);

	void setUserVisitCount(int uid, int count);
	
	HomeInfoPtr getAll(int uid,int begin,int nLimit,int tLimit);

private:	
	xce::clusterstate::ClientInterface<HomeFootprintCachePrx> _clientCS;   /* @brief ClusterState的接口 */

	int _cluster;

	MyUtil::OceChannel _channel;
};

};
};

#endif

