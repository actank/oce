#ifndef __DIST_USERCACHERELOADERADAPTER_H_
#define __DIST_USERCACHERELOADERADAPTER_H_

#include <UserCache.h>
#include <AdapterI.h>
#include <IceUtil/Mutex.h>

#include <util/cpp/TimeCost.h>

namespace xce {
namespace distcache{
namespace usercache {
const int RELOAD_MAZ_LENGTH = 10000;
class DistUserCacheReloaderAdapter : public IceUtil::Thread, public MyUtil::ReplicatedClusterAdapterI<xce::usercache::DistUserCacheReloaderPrx>,
								 public MyUtil::Singleton<DistUserCacheReloaderAdapter> {
public:
	DistUserCacheReloaderAdapter();

	void run();

	void reload(const MyUtil::IntSeq& ids);

private:
	IceUtil::Monitor<IceUtil::Mutex> m_mutex;
	MyUtil::IntSeq m_reloadIds;

};

}
}
}

#endif
