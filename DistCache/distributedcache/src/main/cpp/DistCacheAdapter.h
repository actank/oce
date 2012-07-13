#ifndef DISTCACHEADAPTER_H
#define DISTCACHEADAPTER_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include <map>
#include <string>
#include "ClusterManager.h"
#include "PreferManager.h"
#include "Distribution.h"

namespace xce {
namespace distcache {

class DistCacheAdapter;

typedef boost::shared_ptr<DistCacheAdapter> DistCacheAdapterPtr;

class DistCacheAdapter {
public:
	static bool connect(DistCacheAdapterPtr& res, const std::string& name, const std::string& zkAddress = "" );
	std::vector<unsigned char> get(long id);
	std::map<long, std::vector<unsigned char> > get(const std::vector<long>& ids);
	std::pair<uint64_t ,std::vector<unsigned char> > getWithCAS(long id);
	std::map<xce::distcache::node::ItemPtr, std::vector<unsigned char> > getWithCAS(const std::vector<long>& ids);
	bool set(long id, const std::vector<unsigned char>& data);
	bool set(long id, const std::string& data);
	std::vector<long> setBatch(const std::map<long , std::string >& data);
	std::vector<long> setBatch(const std::map<long , std::vector<unsigned char> >& data);
	bool setCAS(long id, const std::vector<unsigned char>& data, uint64_t cas);
	bool setCAS(long id, const std::string& data, uint64_t cas);
	std::vector<long> setBatchCAS(const std::map<xce::distcache::node::ItemPtr, std::string >& data);
	std::vector<long> setBatchCAS(const std::map<xce::distcache::node::ItemPtr, std::vector<unsigned char> >& data);
	int getCluster();
	std::string getName();
private:
	std::string name_;
	ClusterManager* clusterManager_;
	PreferManager preferManager_;
	DistributionPtr dist_;
	int blockSize_;

	int initialize(const std::string& name, const std::string& zkAddress = "");
	DistCacheAdapter() {}
};

}
}

#endif
