#include "Node.h"
#include <iostream>
#include <boost/pointer_cast.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <util/cpp/TimeCost.h>

using std::string;
using std::vector;
using xce::distcache::node::NodePtr;
using xce::distcache::server::MetaServerPrx;
using xce::distcache::node::ReadNodePtr;
using xce::distcache::node::MemcachedReadNode;
using xce::distcache::node::NodeFactory;
using xce::distcache::node::WriteNodePtr;
using xce::distcache::node::MetaNodePtr;
using xce::distcache::node::ICEMetaNodePtr;
using xce::distcache::node::ICEWriteNodePtr;
using xce::distcache::node::ItemPtr;
using xce::distcache::node::Item;
using std::ostringstream;
using xce::distcache::node::ICEMetaNode;
using xce::distcache::node::ICEWriteNode;
using xce::distcache::node::MemcachedReadNodePtr;
using xce::distcache::node::Type;
using xce::distcache::node::ICE;
using xce::distcache::node::Node;
using xce::distcache::server::WriteServerPrx;
using xce::distcache::NodeTypeEnum;
using xce::distcache::NodeCategoryEnum;
using boost::dynamic_pointer_cast;

string Item::str() const {
	ostringstream oss;
	oss << "ReadOnlyNode.Item[" << block_ << "/" << id_ << "]";
	return oss.str();
}

void Node::initialize(const string& name, const string& config) {
	name_ = name;
	config_ = config;
}

void ICEMetaNode::alloc(const ItemPtr& item) {
	prx_->alloc(item->id_, item->block_);
}

std::vector<long> ICEMetaNode::get(int blockId) {
	return prx_->get(blockId);
}

void ICEMetaNode::close() {
	if (ic_ != 0) {
		ic_->destroy();
	}
}

void ICEMetaNode::initialize(const string& name, const string& config) {
	Node::initialize(name, config);
	ic_ = Ice::initialize();
	prx_ = MetaServerPrx::uncheckedCast(ic_->stringToProxy(config));
}

std::vector<long> ICEWriteNode::setBatchCAS(const std::string& name, const xce::distcache::server::CASDataTable& data) {
	return prx_->setBatchCAS(name, data);
}

std::vector<long> ICEWriteNode::setBatch(const std::string& name, const std::map<long, std::vector<unsigned char> >& data) {
	return prx_->setBatch(name, data, true);
}

void ICEWriteNode::setBatchAsync(const std::string& name, const std::map<long, std::vector<unsigned char> >& data) {
	prx_->setBatch(name, data, false);
}

bool ICEWriteNode::set(const std::string& name, const ItemPtr& id, const vector<unsigned char>& data) {
	return prx_->set(name,id->id_, id->block_, data, true);
}

bool ICEWriteNode::setCAS(const std::string& name, const ItemPtr& id, const vector<unsigned char>& data) {
	return prx_->setCAS(name,id->id_, id->block_, data, id->cas_);
}

void ICEWriteNode::setAsync(const std::string& name, const ItemPtr& id, const vector<unsigned char>& data) {
	prx_->set(name,id->id_, id->block_, data, false);
}

void ICEWriteNode::close() {
	if (ic_ != 0) {
		ic_->destroy();
	}
}

void ICEWriteNode::initialize(const string& name, const string& config) {
	Node::initialize(name, config);
	ic_ = Ice::initialize();
	prx_ = WriteServerPrx::uncheckedCast(ic_->stringToProxy(config));
}

MemcachedReadNode::~MemcachedReadNode() {
	close();
}

void MemcachedReadNode::initialize(const string& name, const string& config) {
	Node::initialize(name, config);
	vector<string> part;
	boost::algorithm::split(part, config, boost::algorithm::is_any_of(":") );
	if (part.size() != 3) {
		ostringstream oss;
		oss << "The config string" << config << " not match the format: <host>:<port>:<key_prefix>";
		throw std::logic_error(oss.str());
	}
	ipStr_ = part[0].c_str();
	portNum_ = boost::lexical_cast<int>(part[1]);
	clientPool_ = boost::shared_ptr<MemcachedClientPool>(new MemcachedClientPool(10, ipStr_, portNum_));
	/*client_ = memcached_create(NULL);
	memcached_return result = memcached_server_add(client_, ipStr.c_str(), portNum);
	if (result != MEMCACHED_SUCCESS) {
		throw std::runtime_error("Error while construct MemcachedClient, see stderr for stack detail.");
	}*/
	prefix_ = part[2];
}

std::vector<unsigned char> MemcachedReadNode::get(const ItemPtr& id) {
	vector<unsigned char> result;
	size_t value_length;
	string key = getKey(id);
	unsigned int flags;
	memcached_return rc;
	MemcachedClientAgentPtr agentPtr;
	if( clientPool_->get( agentPtr ) == false )
	{
		MCE_WARN("MemcachedReadNode::get there is not enough connections!");
		agentPtr = boost::shared_ptr<MemcachedClientAgent>(clientPool_->createAgent(ipStr_, portNum_));
	}
	char *value = memcached_get(agentPtr->client, key.c_str(), key.length(), &value_length, &flags, &rc);
	if( rc !=MEMCACHED_SUCCESS && rc !=MEMCACHED_NOTFOUND )
	{
		
		if( value != NULL)
		{
			free( value );
		}
	
	}else
	{
		if (value != NULL) {
			result.insert(result.end(), value, value + value_length);
			free(value);
		}
		
		clientPool_->pushBack( agentPtr );
	}
	
	return result;
}

void MemcachedReadNode::internalSet(const ItemPtr& id, const std::vector<unsigned char>& data) {
	string key = getKey(id);
	MemcachedClientAgentPtr agentPtr;
	if( clientPool_->get( agentPtr ) == false )
	{
		MCE_WARN("MemcachedReadNode::internalSet there is not enough connections!");
		agentPtr = boost::shared_ptr<MemcachedClientAgent>(clientPool_->createAgent(ipStr_, portNum_));
	}
	memcached_set(agentPtr->client, key.c_str(), key.length(), (char*)&(*data.begin()) , data.size(), INT_MAX, 0);
	clientPool_->pushBack( agentPtr );
}

void MemcachedReadNode::close() {
	//memcached_free(client_);
}

std::map<ItemPtr, std::vector<unsigned char> > MemcachedReadNode::get(const std::vector<ItemPtr>& id) {
	vector<string> keys;
	keys.resize(id.size());
	for (std::vector<ItemPtr>::const_iterator i = id.begin(); i != id.end(); ++i) {
		keys.push_back(getKey(*i));
	}
	char** keyss = new char*[keys.size()];
	size_t* key_length = new size_t[keys.size()];
	for (unsigned int i = 0; i < keys.size(); ++i) {
		keyss[i] = (char*)keys[i].c_str();
		key_length[i] = keys[i].size();
	}
	

	std::map<ItemPtr, std::vector<unsigned char> > result;

	MemcachedClientAgentPtr agentPtr;
	if( clientPool_->get( agentPtr ) == false )
	{
		MCE_WARN("MemcachedReadNode::get there is not enough connections!");
		agentPtr = boost::shared_ptr<MemcachedClientAgent>(clientPool_->createAgent(ipStr_, portNum_));
	}

	memcached_return rc = memcached_mget(agentPtr->client, keyss, key_length, keys.size());

	if( rc !=MEMCACHED_SUCCESS && rc !=MEMCACHED_NOTFOUND )
	{
		if (keyss != NULL) {
			delete[] keyss;
		}
		
		if (key_length != NULL) {
			delete[] key_length;
		}

		return result;
		
	}
	
	char *return_value = NULL;
	char return_key[MEMCACHED_MAX_KEY];
	size_t return_key_length;
	size_t return_value_length;
	unsigned int flags;
	while ((return_value = memcached_fetch(agentPtr->client, return_key, &return_key_length, &return_value_length, &flags, &rc))) {
		return_key[return_key_length]=0;
		result.insert(make_pair(getKey(return_key), vector<unsigned char>(return_value, return_value + return_value_length)));
		
		if (return_value != NULL)
		{
			free( return_value );
			return_value = NULL;
		}
	}
	if (keyss != NULL) {
		delete[] keyss;
	}
	if (key_length != NULL) {
		delete[] key_length;
	}
	
	if( rc !=MEMCACHED_END )
	{
	
		MCE_WARN("MemcachedReadNode::get rc != MEMCACHED_END!");
	}else
	{
		clientPool_->pushBack( agentPtr );
	}
	
	return result;
}

string MemcachedReadNode::getKey(const ItemPtr& item) const {
	ostringstream oss;
	oss << prefix_ << "_" << item->block_ << "_" << item->id_;
	return oss.str();
}

ItemPtr MemcachedReadNode::getKey(const std::string& str) const {
	if (str.find(prefix_) == 0) {
		int pos_block_start = prefix_.length() + 1;
		int pos_block_end = str.find("_", pos_block_start);
		int pos_id_start = pos_block_end + 1;
		int pos_id_end = str.length();
		int block = boost::lexical_cast<int>(str.substr(pos_block_start, pos_block_end - pos_block_start));
		long id = boost::lexical_cast<long>(str.substr(pos_id_start, pos_id_end - pos_id_start));
		return ItemPtr(new Item(block, id));
	}
	MCE_WARN("MemcachedReadNode::getKey invalid str "<< str);
	return ItemPtr(new Item(-1,-1));
}

ItemPtr MemcachedReadNode::getKeyWithCAS(const std::string& str, uint64_t cas) const {
	if (str.find(prefix_) == 0) {
		int pos_block_start = prefix_.length() + 1;
		int pos_block_end = str.find("_", pos_block_start);
		int pos_id_start = pos_block_end + 1;
		int pos_id_end = str.length();
		int block = boost::lexical_cast<int>(str.substr(pos_block_start, pos_block_end - pos_block_start));
		long id = boost::lexical_cast<long>(str.substr(pos_id_start, pos_id_end - pos_id_start));
		return ItemPtr(new Item(block, id, cas));
	}
	MCE_WARN("MemcachedReadNode::getKeyWichCAS invalid str "<< str);
	return ItemPtr(new Item(-1,-1,-1));
}
	
bool MemcachedReadNode::getStats(std::map<std::string, std::string>& result){
		
	MemcachedClientAgentPtr agentPtr;
	if( clientPool_->get( agentPtr ) == false )
	{
		MCE_WARN("MemcachedReadNode::getStats there is not enough connections!");
		agentPtr = boost::shared_ptr<MemcachedClientAgent>(clientPool_->createAgent(ipStr_, portNum_));
	}
	
	memcached_return_t rc; 
	
	memcached_stat_st stats; 
	
	memcached_server_instance_st instance= memcached_server_instance_by_position(agentPtr->client, 0);

	rc= memcached_stat_servername(&stats, NULL,
                                memcached_server_name(instance),
                                memcached_server_port(instance));

	char ** keys = memcached_stat_get_keys( agentPtr->client, &stats, &rc );
	
	if( rc != MEMCACHED_SUCCESS )
	{
		MCE_WARN("MemcachedReadNode::getStats Can not get stats get keys from memcached rc = "<<rc);
		if( keys )
		{
			delete[] keys;
		} 
			
		clientPool_->pushBack( agentPtr );
		
		return false;
	}
	
	char **ptr;
	
	for (ptr= keys; *ptr; ptr++)
	{
		char *value= memcached_stat_get_value(agentPtr->client, &stats, *ptr, &rc);

		if( rc != MEMCACHED_SUCCESS )
		{
			MCE_WARN("MemcachedReadNode::getStats Can not get stats get_value from memcached rc = "<<rc);
			
			if( keys )
			{
				delete[] keys;
			} 
			
			clientPool_->pushBack( agentPtr );

			return false;
		}
		
		result[std::string(*ptr)] = std::string(value);
		if( value )
		{
			free(value);
		}
	}
	
	if( keys )
	{
		delete[] keys;
	}

	clientPool_->pushBack( agentPtr );

	bool flag = false;

	for( std::map<std::string, std::string>::const_iterator it = result.begin(); it != result.end(); ++it )
	{
		if( it->second != "0.0" && it->second != "0" && it->second != "" )
		{
			flag = true;
		}
	}
	return flag;
}

std::pair<uint64_t,std::vector<unsigned char> > MemcachedReadNode::getWithCAS(const ItemPtr& id) {
	std::vector< ItemPtr > iVec;
	iVec.push_back( id );

	std::map<ItemPtr, std::vector<unsigned char> > resMap = getWithCAS( iVec );

	if( resMap.size() > 0 )
	{
		return make_pair( resMap.begin()->first->cas_, resMap.begin()->second );
	}

	return make_pair( 0, std::vector<unsigned char>() );
}

std::map<ItemPtr, std::vector<unsigned char> > MemcachedReadNode::getWithCAS(const std::vector<ItemPtr>& id) {
	vector<string> keys;
	keys.resize(id.size());
	for (std::vector<ItemPtr>::const_iterator i = id.begin(); i != id.end(); ++i) {
		keys.push_back(getKey(*i));
	}
	char** keyss = new char*[keys.size()];
	size_t* key_length = new size_t[keys.size()];
	for (unsigned int i = 0; i < keys.size(); ++i) {
		keyss[i] = (char*)keys[i].c_str();
		key_length[i] = keys[i].size();
	}
	

	std::map<ItemPtr, std::vector<unsigned char> > result;

	MemcachedClientAgentPtr agentPtr;
	if( clientPool_->get( agentPtr ) == false )
	{
		MCE_WARN("MemcachedReadNode::getWithCAS there is not enough connections!");
		agentPtr = boost::shared_ptr<MemcachedClientAgent>(clientPool_->createAgent(ipStr_, portNum_));
	}

	memcached_return rc = memcached_mget(agentPtr->client, keyss, key_length, keys.size());

	if( rc !=MEMCACHED_SUCCESS && rc !=MEMCACHED_NOTFOUND )
	{
		
		MCE_WARN("MemcachedReadNode::getWithCAS memcached_mget failed!");
		
		if (keyss != NULL) {
			delete[] keyss;
		}
		
		if (key_length != NULL) {
			delete[] key_length;
		}

		return result;
		
	}

	memcached_result_st *resultSt;

	while ((resultSt = memcached_fetch_result(agentPtr->client, 0, &rc)) != NULL) 
	{
	
		const char* return_key = memcached_result_key_value( resultSt );
		const char* return_value = memcached_result_value( resultSt );
		uint64_t return_cas = memcached_result_cas( resultSt );
	
		result.insert(make_pair( getKeyWithCAS(return_key, return_cas), vector<unsigned char>(return_value, return_value + memcached_result_length( resultSt ))));
		
		if ( resultSt != NULL)
		{
			memcached_result_free( resultSt );
			resultSt = NULL;
		}
	}
	if (keyss != NULL) {
		delete[] keyss;
	}
	if (key_length != NULL) {
		delete[] key_length;
	}
	
	if( rc !=MEMCACHED_END )
	{
		
		MCE_WARN("MemcachedReadNode::get rc != MEMCACHED_END!");
		
	}else
	{
		clientPool_->pushBack( agentPtr );
	}
	
	return result;
}

NodePtr NodeFactory::create(NodeCategoryEnum category, NodeTypeEnum type, const string& name, const string& config) {
	switch (category) {
		case META:
			return createMetaNode(type, name, config);
		case READ:
			return createReadNode(type, name, config);
		case WRITE:
			return createWriteNode(type, name, config);
	}
}

MetaNodePtr NodeFactory::createMetaNode(NodeTypeEnum type, const string& name, const string& config) {
	return createMetaNode(translate(type), name, config);
}

MetaNodePtr NodeFactory::createMetaNode(Type type, const string& name, const string& config) {
	if( type == ICE ) {
		ICEMetaNodePtr b(new ICEMetaNode);
		b->initialize(name, config);
		return b;
	}
	MCE_WARN("NodeFactory::createMetaNode invalid type "<< type);
	return dynamic_pointer_cast<ICEMetaNode>(ICEMetaNodePtr(new ICEMetaNode));
}

Type NodeFactory::translate(NodeTypeEnum type) {
	if( type == MEMCACHED ) {
		return xce::distcache::node::Memcached;
	}else if ( type == xce::distcache::ICE ) {
		return xce::distcache::node::ICE;
	}
	MCE_WARN("NodeFactory::translate invalid type "<< type);
	return xce::distcache::node::NONE;
}

ReadNodePtr NodeFactory::createReadNode(Type type, const string& name, const string& config) {
	if( type == Memcached ) {
		MemcachedReadNodePtr b(new MemcachedReadNode);
		b->initialize(name, config);
		return b;
	}
	MCE_WARN("NodeFactory::createReadNode invalid type "<< type);
	return dynamic_pointer_cast<MemcachedReadNode>(MemcachedReadNodePtr(new MemcachedReadNode));
}

ReadNodePtr NodeFactory::createReadNode(NodeTypeEnum type, const string& name, const string& config) {
	return createReadNode(translate(type), name, config);
}

WriteNodePtr NodeFactory::createWriteNode(Type type, const string& name, const string& config) {
	if( type == ICE ) {
		ICEWriteNodePtr b(new ICEWriteNode);
		b->initialize(name, config);
		return b;
	}
	MCE_WARN("NodeFactory::createWriteNode invalid type "<< type);
	return dynamic_pointer_cast<WriteNode>(ICEWriteNodePtr(new ICEWriteNode));
}

WriteNodePtr NodeFactory::createWriteNode(NodeTypeEnum type, const string& name, const string& config) {
	return createWriteNode(translate(type), name, config);
}
