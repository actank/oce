#ifndef _____src_ReplicaClusterDemoI_h__
#define _____src_ReplicaClusterDemoI_h__

#include "ReplicaClusterDemo.h"
#include "ServiceI.h"

namespace xce
{

namespace replicaclusterdemo
{

class DemoManagerI : virtual public DemoManager,
		     public MyUtil::Singleton<DemoManagerI>  {
public:

    virtual std::string PrintClients(const Ice::Current&);

    void SetServerName(const std::string& serverName);

private:
    string _serverName;
};

}

}

#endif
