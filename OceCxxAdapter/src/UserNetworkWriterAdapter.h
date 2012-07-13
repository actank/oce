#ifndef __USER_NETWORK_WRITER_ADAPTER_H__
#define __USER_NETWORK_WRITER_ADAPTER_H__

#include "UserNetwork.h"
#include "Singleton.h"
#include "AdapterI.h"
#include <ext/hash_set>
#include <IceUtil/IceUtil.h>

namespace mop {
namespace hi {
namespace svc {
namespace adapter {
using namespace MyUtil;
using namespace mop::hi::svc::model;
using namespace __gnu_cxx;
class UserNetworkWriterAdapter : public MyUtil::ReplicatedClusterAdapterI<NetworkWriterPrx>,
	public MyUtil::Singleton<UserNetworkWriterAdapter>, IceUtil::Thread
{
public:
	UserNetworkWriterAdapter() : MyUtil::ReplicatedClusterAdapterI<NetworkWriterPrx>("ControllerUserNetworkWriter",120,300){
		start();
	}

	void reloadAll(int id);

protected:

	void run();

private:

	NetworkWriterPrx getNetworkWriter(int userId);

private:

	IceUtil::Monitor<IceUtil::Mutex> _mutex;
	hash_set<int> _idSet;
};

}
}
}
}

#endif

