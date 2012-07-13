#ifndef __CONTROLLER_ADAPTER_H__
#define __CONTROLLER_ADAPTER_H__

#include "ReplicaCluster.h"
#include "Channel.h"
#include "Singleton.h"
#include <boost/lexical_cast.hpp>

namespace xce {
namespace clustercontroller {
using namespace MyUtil;

class ControllerAdapter : public IceUtil::Mutex,
	public MyUtil::Singleton<ControllerAdapter>  {
public:
	void initialize(const std::string& endpoints, int timeout=300) {
		endpoints_ = endpoints;
		timeout_ = timeout;
	}

	xce::clusterstate::ClusterStateInfoPtr getClusterState();

	int getCluster();

private:
	friend class MyUtil::Singleton<ControllerAdapter>;
	ControllerAdapter(): prx_(NULL) {
	}

	OceChannel channel_;
	std::string endpoints_;
	int timeout_;
	ControllerManagerPrx prx_;

	ControllerManagerPrx getControllerManager();

};

};
};

#endif

