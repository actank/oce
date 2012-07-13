#ifndef USERAPICACHERELOADERADAPTER_H_
#define USERAPICACHERELOADERADAPTER_H_

#include <UserApiCache.h>
#include <AdapterI.h>

namespace xce {
namespace userapicache {

class UserApiCacheReloaderAdapter : public MyUtil::ReplicatedClusterAdapterI<UserApiCacheReloaderPrx>, public MyUtil::Singleton<UserApiCacheReloaderAdapter>{
public:
	UserApiCacheReloaderAdapter();
	virtual void reload(MyUtil::IntSeq userIds, const Ice::Context& ctx = Ice::Context());
private:
	UserApiCacheReloaderPrx getManager(int id);
};

}
}

#endif
