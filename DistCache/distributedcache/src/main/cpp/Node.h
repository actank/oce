#ifndef NODE_H
#define NODE_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>
#include <map>
#include <Ice/Ice.h>
#include <DistributedCache.h>
#include <DistributedCacheSchema.pb.h>
#include <libmemcached/memcached.h>
#include "MemcachedClientPool.h"

#include <LogWrapper.h>

namespace xce {
namespace distcache {
namespace node {

class Item {
public:
	Item(int block, long id, uint64_t cas = 0 ) : block_(block), id_(id), cas_(cas) {}
	std::string str() const;
	const int block_;
	const long id_;
	const uint64_t cas_;
};

typedef boost::shared_ptr<Item> ItemPtr;

enum Type {
	Memcached, ICE, NONE
};

class Node {
public:
	virtual ~Node(){}
	void initialize(const std::string& name, const std::string& config);
	std::string getConfig() const {return config_;}
	virtual void close() = 0;

	std::string getName()
	{
		return name_;
	}
protected:
	std::string name_;
	std::string config_;
};

typedef boost::shared_ptr<Node> NodePtr;

class MetaNode : virtual public Node {
public:
	virtual ~MetaNode(){}
	virtual void alloc(const ItemPtr& item) = 0;
	virtual std::vector<long> get(int blockId) = 0;
};

typedef boost::shared_ptr<MetaNode> MetaNodePtr;

class WriteNode : virtual public Node {
public:
	virtual ~WriteNode(){}
	virtual bool set(const std::string& name, const ItemPtr& id, const std::vector<unsigned char>& data) = 0;
	virtual bool setCAS(const std::string& name, const ItemPtr& id, const std::vector<unsigned char>& data) = 0;
	virtual void setAsync(const std::string& name, const ItemPtr& id, const std::vector<unsigned char>& data) = 0;
	virtual std::vector<long> setBatch(const std::string& name, const std::map<long, std::vector<unsigned char> >& data) = 0;
	virtual std::vector<long> setBatchCAS(const std::string& name, const xce::distcache::server::CASDataTable& data) = 0;
	virtual void setBatchAsync(const std::string& name, const std::map<long, std::vector<unsigned char> >& data) = 0;
};

typedef boost::shared_ptr<WriteNode> WriteNodePtr;

class ReadNode : virtual public Node {
public:
	virtual ~ReadNode(){}
	virtual std::vector<unsigned char> get(const ItemPtr& id) = 0;
	virtual std::map<ItemPtr, std::vector<unsigned char> > get(const std::vector<ItemPtr>& id) = 0;
	virtual void internalSet(const ItemPtr& id, const std::vector<unsigned char>& data) = 0;
	virtual bool getStats(std::map<std::string, std::string>& result) = 0;
	
	virtual std::pair<uint64_t, std::vector<unsigned char> > getWithCAS(const ItemPtr& id) = 0;
	virtual std::map<ItemPtr, std::vector<unsigned char> > getWithCAS(const std::vector<ItemPtr>& id) = 0;
};

typedef boost::shared_ptr<ReadNode> ReadNodePtr;

class ICEMetaNode : virtual public MetaNode {
public:
	virtual ~ICEMetaNode(){}
	void alloc(const ItemPtr& item);
	std::vector<long> get(int blockId);
	void close();
	void initialize(const std::string& name, const std::string& config);
private:
	Ice::CommunicatorPtr ic_;
	xce::distcache::server::MetaServerPrx prx_;
};

typedef boost::shared_ptr<ICEMetaNode> ICEMetaNodePtr;

class ICEWriteNode : virtual public WriteNode {
public:
	virtual ~ICEWriteNode(){}
	bool set(const std::string& name, const ItemPtr& id, const std::vector<unsigned char>& data);
	bool setCAS(const std::string& name, const ItemPtr& id, const std::vector<unsigned char>& data);
	void setAsync(const std::string& name, const ItemPtr& id, const std::vector<unsigned char>& data);
	std::vector<long> setBatch(const std::string& name, const std::map<long, std::vector<unsigned char> >& data);
	std::vector<long> setBatchCAS(const std::string& name, const xce::distcache::server::CASDataTable& data);
	void setBatchAsync(const std::string& name, const std::map<long, std::vector<unsigned char> >& data);
	void close();
	void initialize(const std::string& name, const std::string& config);

	xce::distcache::server::WriteServerPrx getPrx()
	{
		return prx_;
	}

private:
	Ice::CommunicatorPtr ic_;
	xce::distcache::server::WriteServerPrx prx_;
};

typedef boost::shared_ptr<ICEWriteNode> ICEWriteNodePtr;

class MemcachedReadNode : virtual public ReadNode {
public:
	virtual ~MemcachedReadNode();
	void initialize(const std::string& name, const std::string& config);
	std::vector<unsigned char> get(const ItemPtr& id);
	void internalSet(const ItemPtr& id, const std::vector<unsigned char>& data);
	void close();
	std::map<ItemPtr, std::vector<unsigned char> > get(const std::vector<ItemPtr>& id);

	bool getStats(std::map<std::string, std::string>& result);
	
	std::pair<uint64_t, std::vector<unsigned char> > getWithCAS(const ItemPtr& id);
	std::map<ItemPtr, std::vector<unsigned char> > getWithCAS(const std::vector<ItemPtr>& id);
	
private:
	std::string getKey(const ItemPtr& item) const;
	ItemPtr getKey(const std::string& str) const;
	ItemPtr getKeyWithCAS(const std::string& str, uint64_t cas) const;

	const static int EXPIRE = INT_MAX;
	std::string prefix_;
	memcached_st *client_;
	std::string ipStr_;
	int portNum_;
	MemcachedClientPoolPtr clientPool_;
};

typedef boost::shared_ptr<MemcachedReadNode> MemcachedReadNodePtr;

class NodeFactory {
public:
	static NodePtr create(xce::distcache::NodeCategoryEnum category, xce::distcache::NodeTypeEnum type, const std::string& name, const std::string& config);
	static MetaNodePtr createMetaNode(xce::distcache::NodeTypeEnum type, const std::string& name, const std::string& config);
	static MetaNodePtr createMetaNode(Type type, const std::string& name, const std::string& config);
	static Type translate(xce::distcache::NodeTypeEnum type);
	static ReadNodePtr createReadNode(Type type, const std::string& name, const std::string& config);
	static ReadNodePtr createReadNode(xce::distcache::NodeTypeEnum type, const std::string& name, const std::string& config);
	static WriteNodePtr createWriteNode(Type type, const std::string& name, const std::string& config);
	static WriteNodePtr createWriteNode(xce::distcache::NodeTypeEnum type, const std::string& name, const std::string& config);
};

}
}
}

#endif
