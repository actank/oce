#ifndef _DIST_USERAPICACHERELOADERADAPTER_H_
#define _DIST_USERAPICACHERELOADERADAPTER_H_

#include <UserApiCache.h>
#include <AdapterI.h>

namespace xce 
{
namespace distcache 
{
namespace userapicache 
{

class DistUserApiCacheReloaderAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::userapicache::UserApiCacheReloaderPrx>, public MyUtil::Singleton<DistUserApiCacheReloaderAdapter >{
public:
	DistUserApiCacheReloaderAdapter();
	virtual void reload(MyUtil::IntSeq userIds, const Ice::Context& ctx = Ice::Context());
	virtual void reload(const int userid, const Ice::Context& ctx = Ice::Context());
	virtual void update(const int userid, const Ice::Byte table, const MyUtil::Str2StrMap prop, const Ice::Context& ctx = Ice::Context());
private:
	xce::userapicache::UserApiCacheReloaderPrx getManager(int id);
};

}
}
}

#endif
