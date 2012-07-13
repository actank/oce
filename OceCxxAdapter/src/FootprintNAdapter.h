#ifndef __FOOTPRINT_N_ADAPTER_H__
#define __FOOTPRINT_N_ADAPTER_H__

#include "FootprintNew.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>
#include "ReplicaCluster/ClusterState/src/client/cxx/ClientInterface.h"

namespace xce {
namespace fptn {
namespace adapter {

class FootprintNAdapter : 
	public MyUtil::Singleton<FootprintNAdapter> {
public:
	FootprintNAdapter();

	void visitHomepageOneway(const xce::fptn::FootprintPtr& fpt, map<string, string> ctx);

	xce::fptn::FootprintInfoPtr visitHomepage(const xce::fptn::FootprintPtr& fpt, int begin, int limit, map<string, string> ctx);
	
	xce::fptn::FootprintInfoPtr visitHomepage(const xce::fptn::FootprintPtr& fpt, map<string, string> ctx);
	
	int getSize(int uid);

	int getUserVisitCount(int uid);
	
	xce::fptn::FootprintInfoPtr getAll(int uid,int begin,int limit);

	void reloadCache(int category, int uid);
	
	void reloadCache(int category, const string& endpoints, int uid);

private:	
	xce::clusterstate::ClientInterface<xce::fptn::FootprintManagerPrx> _clientCS;   /* @brief ClusterState的接口 */

	int _cluster;

	MyUtil::OceChannel _channel;
};

};
};
};

#endif

