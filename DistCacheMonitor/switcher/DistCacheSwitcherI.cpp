#include "DistCacheSwitcherI.h"
#include <util/cpp/TimeCost.h>

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

void MyUtil::initialize() {
	MyUtil::ServiceI& service = MyUtil::ServiceI::instance();
	service.getAdapter()->add(&xce::distcache::monitor::DistCacheSwitcherI::instance(), service.createIdentity(xce::distcache::monitor::DISTCACHE_SWITCHER, ""));

	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerDistCacheSwitcher", &xce::distcache::monitor::DistCacheSwitcherI::instance(), 0, 120);

}

