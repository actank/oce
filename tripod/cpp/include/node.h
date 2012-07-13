/*
 * node.h
 *
 *  Created on: 2011-4-11
 *      Author: guanghe.ge
 */

#ifndef NODE_H_
#define NODE_H_

#include <boost/shared_ptr.hpp>
#include "data.h"

namespace com {
namespace renren {
namespace tripod {

enum NodeType {
	MEMCACHE_NODE, ICE_NODE
};

class Node {
public:
	virtual void Initialize(const std::string& config) = 0;
	virtual DataPtr Read(const std::string& key) = 0;
	virtual void Write(const std::string& key, DataPtr data) = 0;
	virtual void Delete(const std::string& key) = 0;
};// end Node
typedef boost::shared_ptr<Node> NodePtr;

class MemcacheNode: public Node {
public:
	void Initialize(const std::string& config);
	DataPtr Read(const std::string& key);
	void Write(const std::string& key, DataPtr data);
	void Delete(const std::string& key);
};// end ReadNode

class ICENode: public Node {
public:
	void Initialize(const std::string& config);
	DataPtr Read(const std::string& key);
	void Write(const std::string& key, DataPtr data);
	void Delete(const std::string& key);
};// end ReadNode

class NodeFactory {
public:
	static NodePtr Create(NodeType type, const std::string& config);
};// end NodeFactory

}
}
}

#endif /* NODE_H_ */
