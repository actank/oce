#include "DistCacheAdapter.h"

#include <LogWrapper.h>

#include <DistributedCache.h>

#include "ClusterManagerFactory.h"

using std::string;
using std::vector;
using std::map;
using xce::distcache::DistCacheAdapterPtr;
using xce::distcache::DistCacheAdapter;
using xce::distcache::node::ReadNodePtr;
using xce::distcache::node::WriteNodePtr;
using xce::distcache::node::Item;
using xce::distcache::node::ItemPtr;
bool DistCacheAdapter::connect(DistCacheAdapterPtr& res, const string& name, const string& zkAddress) {
	res = DistCacheAdapterPtr(new DistCacheAdapter);
	if( res->initialize(name, zkAddress) < 0 )
	{
		return false;
	}
	return true;
}

int DistCacheAdapter::initialize(const string& name, const std::string& zkAddress) {
	name_=name;
	clusterManager_ = xce::distcache::ClusterManagerFactory::getInstance()->getClusterManager(name,zkAddress);
	if( clusterManager_ == NULL ) {
		MCE_WARN("DistCacheAdapter::initialize clusterManager initialize failed!");
		return -1;
	}
	blockSize_ = clusterManager_->getNumBlocks();
	dist_ = Distribution::instance(blockSize_);
	srand( time( NULL ) );
	return blockSize_;
}

vector<unsigned char> DistCacheAdapter::get(long id) {
	int block = dist_->get(id);
	vector<ReadNodePtr> backends = clusterManager_->getReadNodes(block);
	
	if( backends.size() <= 0)
	{
		backends = clusterManager_->getReadNodesWithUpgrading(block);
	}

	if( backends.size() <= 0)
	{
		MCE_WARN("DistCacheAdapter::get no backends!");
		return vector<unsigned char>();
	}
	
	ReadNodePtr b = preferManager_.choose(backends);
	return b->get(ItemPtr(new Item(block, id)));
}

std::vector<long> DistCacheAdapter::setBatch(const std::map<long , std::vector<unsigned char> >& data) {
	vector<WriteNodePtr> x = clusterManager_->getWriteNodes();
	if (!x.empty()) {
		return x.at(rand() % x.size() )->setBatch(name_,data);
	}

	MCE_WARN("DistCacheAdapter::setBatch no backends!");
	std::vector<long> res;

	for( std::map<long, std::vector<unsigned char> >::const_iterator it = data.begin(); it != data.end(); ++it )
	{
		res.push_back( it->first );
	}
	return res;
}

std::vector<long> DistCacheAdapter::setBatch(const std::map<long , std::string >& data) {
	std::map<long, std::vector<unsigned char> > res;
	
	for( std::map<long, std::string>::const_iterator it = data.begin(); it!= data.end(); ++it )
	{
		res.insert(make_pair( it->first, std::vector<unsigned char>(it->second.begin(), it->second.end())));
	}

	return setBatch( res );
}

bool DistCacheAdapter::set(long id, const std::string& data) {
	std::vector<unsigned char> res(data.begin(), data.end());
	return set(id, res);
}

bool DistCacheAdapter::set(long id, const std::vector<unsigned char>& data) {
	int block = dist_->get(id);
	vector<WriteNodePtr> x = clusterManager_->getWriteNodes();
	if (!x.empty()) {
		return x.at( rand() % x.size() )->set(name_,ItemPtr(new Item(block, id)), data);
	}
	MCE_WARN("DistCacheAdapter::set no backends!");
	return false;
}

std::vector<long> DistCacheAdapter::setBatchCAS(const std::map<xce::distcache::node::ItemPtr, std::vector<unsigned char> >& data) {
	std::map<long, xce::distcache::server::CASDataPtr> res;
	std::vector<long> resVec;

	for( std::map<xce::distcache::node::ItemPtr, std::vector<unsigned char> >::const_iterator it = data.begin(); it!= data.end(); ++it )
	{
		xce::distcache::server::CASDataPtr casData = new xce::distcache::server::CASData;
		casData->data = it->second;
		casData->cas = it->first->cas_;
		res.insert(std::make_pair( it->first->id_, casData));
		resVec.push_back( it->first->id_ );
	}
	
	vector<WriteNodePtr> x = clusterManager_->getWriteNodes();
	if (!x.empty()) {
		return x.at(rand() % x.size() )->setBatchCAS(name_,res);
	}

	MCE_WARN("DistCacheAdapter::setBatchCAS no backends!");
	return resVec;
}

std::vector<long> DistCacheAdapter::setBatchCAS(const std::map<xce::distcache::node::ItemPtr, std::string >& data) {
	std::map<long, xce::distcache::server::CASDataPtr> res;
	std::vector<long> resVec;

	for( std::map<xce::distcache::node::ItemPtr, std::string>::const_iterator it = data.begin(); it!= data.end(); ++it )
	{
		xce::distcache::server::CASDataPtr casData = new xce::distcache::server::CASData;
		casData->data.insert(casData->data.end(),it->second.begin(), it->second.end());
		casData->cas = it->first->cas_;
		res.insert(std::make_pair( it->first->id_, casData));
		resVec.push_back( it->first->id_ );
	}
	
	vector<WriteNodePtr> x = clusterManager_->getWriteNodes();
	if (!x.empty()) {
		return x.at(rand() % x.size() )->setBatchCAS(name_,res);
	}

	MCE_WARN("DistCacheAdapter::setBatchCAS no backends!");
	return resVec;
}

bool DistCacheAdapter::setCAS(long id, const std::string& data, uint64_t cas) {
	std::vector<unsigned char> res(data.begin(), data.end());
	return setCAS(id, res, cas);
}

bool DistCacheAdapter::setCAS(long id, const std::vector<unsigned char>& data, uint64_t cas) {
	int block = dist_->get(id);
	vector<WriteNodePtr> x = clusterManager_->getWriteNodes();
	if (!x.empty()) {
		return x.at( rand() % x.size() )->setCAS(name_,ItemPtr(new Item(block, id, cas)), data);
	}
	MCE_WARN("DistCacheAdapter::setCAS no backends!");
	return false;
}

std::map<long, std::vector<unsigned char> > DistCacheAdapter::get(const vector<long>& ids) {
	map<int, vector<long> > seperated;
	for (vector<long>::const_iterator id = ids.begin(); id != ids.end(); ++id) {
		int block = dist_->get(*id);
		seperated[block].push_back(*id);
	}

	map<ReadNodePtr, vector<ItemPtr> > categoried;
	for (map<int, vector<long> >::iterator entry = seperated.begin(); entry != seperated.end(); ++entry) {
		int block = entry->first;
		vector<ReadNodePtr> backends = clusterManager_->getReadNodes(block);
		if( backends.size() <= 0)
		{
			backends = clusterManager_->getReadNodesWithUpgrading(block);
		}
		
		if( backends.size() <= 0)
		{
			MCE_WARN("DistCacheAdapter::get no backends!");
			return map<long,vector<unsigned char> >();
		}
	
		ReadNodePtr b = preferManager_.choose(backends, categoried);
		for (vector<long>::iterator id = entry->second.begin(); id != entry->second.end(); ++id) {
			categoried[b].push_back(ItemPtr(new Item(block, *id)));
		}
	}

	map<long, vector<unsigned char> > result;
	for (map<ReadNodePtr, vector<ItemPtr> >::iterator entry = categoried.begin(); entry != categoried.end(); ++entry) {
		map<ItemPtr, vector<unsigned char> > one_batch = entry->first->get(entry->second);
		for (map<ItemPtr, vector<unsigned char> >::iterator one_entry = one_batch.begin(); one_entry != one_batch.end(); ++one_entry) {
			result.insert(make_pair(one_entry->first->id_, one_entry->second));
		}
	}
	return result;
}

std::pair<uint64_t,vector<unsigned char> > DistCacheAdapter::getWithCAS(long id) {
	int block = dist_->get(id);
	vector<ReadNodePtr> backends = clusterManager_->getReadNodes(block);
	
	if( backends.size() <= 0)
	{
		backends = clusterManager_->getReadNodesWithUpgrading(block);
	}

	if( backends.size() <= 0)
	{
		MCE_WARN("DistCacheAdapter::get no backends!");
		return make_pair( 0,vector<unsigned char>() );
	}
	
	ReadNodePtr b = preferManager_.choose(backends);
	return b->getWithCAS(ItemPtr(new Item(block, id)));
}

std::map<ItemPtr, std::vector<unsigned char> > DistCacheAdapter::getWithCAS(const vector<long>& ids) {
	map<int, vector<long> > seperated;
	for (vector<long>::const_iterator id = ids.begin(); id != ids.end(); ++id) {
		int block = dist_->get(*id);
		seperated[block].push_back(*id);
	}

	map<ReadNodePtr, vector<ItemPtr> > categoried;
	for (map<int, vector<long> >::iterator entry = seperated.begin(); entry != seperated.end(); ++entry) {
		int block = entry->first;
		vector<ReadNodePtr> backends = clusterManager_->getReadNodes(block);
		if( backends.size() <= 0)
		{
			backends = clusterManager_->getReadNodesWithUpgrading(block);
		}
		
		if( backends.size() <= 0)
		{
			MCE_WARN("DistCacheAdapter::get no backends!");
			return map<ItemPtr,vector<unsigned char> >();
		}
	
		ReadNodePtr b = preferManager_.choose(backends, categoried);
		for (vector<long>::iterator id = entry->second.begin(); id != entry->second.end(); ++id) {
			categoried[b].push_back(ItemPtr(new Item(block, *id)));
		}
	}

	map<ItemPtr, vector<unsigned char> > result;
	for (map<ReadNodePtr, vector<ItemPtr> >::iterator entry = categoried.begin(); entry != categoried.end(); ++entry) {
		map<ItemPtr, vector<unsigned char> > one_batch = entry->first->getWithCAS(entry->second);
		result.insert( one_batch.begin(), one_batch.end() );
	}
	return result;
}

int DistCacheAdapter::getCluster() {
	return blockSize_;
}

std::string DistCacheAdapter::getName() {
	return name_;
}
