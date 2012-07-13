/*
 * cluster.h
 *
 *  Created on: 2011-4-11
 *      Author: guanghe.ge
 */

#ifndef CLUSTER_H_
#define CLUSTER_H_

#include "data.h"
#include "node.h"
#include <map>
#include "TripodSchema.pb.h"
#include "communicator.h"
#include "observer.h"

namespace com {
namespace renren {
namespace tripod {
class Cluster: public Observer {
public:
	void Initialize(const std::string& communitor_addr,
			const std::string& cluster_name);

	DataPtr Read(long id);
	void Write(long id, DataPtr data);
	void Delete(long id);

	virtual void Update(const std::string& path);
private:
	std::string cluster_name_;
	int block_size_;

	std::vector<std::vector<NodePtr> > read_nodes_;
	std::vector<std::vector<NodePtr> > write_nodes_;

	CommunicatorPtr communicator_;

	void ParseCluster();
	std::string Key(long id);
	int NodeIndex(long id);
	NodePtr ChooseNode(const std::vector<NodePtr>& nodes);
};// end Cluster
typedef boost::shared_ptr<Cluster> ClusterPtr;

}
}
}

#endif /* CLUSTER_H_ */
