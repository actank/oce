#ifndef _DIST_BUDDYBYIDCACHERELOADERADAPTER_H_
#define _DIST_BUDDYBYIDCACHERELOADERADAPTER_H_

#include <BuddyByIdCache.h>
#include <AdapterI.h>

namespace xce 
{
namespace distcache 
{
namespace buddybyidcache 
{

class DistBuddyByIdCacheReloaderAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::buddy::DistBuddyByIdCacheReloaderPrx>, public MyUtil::Singleton<DistBuddyByIdCacheReloaderAdapter >{
public:
	DistBuddyByIdCacheReloaderAdapter();
	virtual void reload(MyUtil::IntSeq userIds, const Ice::Context& ctx = Ice::Context());
  virtual void reload(int userId, const Ice::Context& ctx = Ice::Context());
  virtual void statusUpdate(int userId, int status, const Ice::Context& ctx = Ice::Context());
  virtual void add(int host, int guest, const Ice::Context& ctx = Ice::Context());
  virtual void remove(int host, int guest, const Ice::Context& ctx = Ice::Context());
private:
	xce::buddy::DistBuddyByIdCacheReloaderPrx getManager(int id);
};

}
}
}

#endif
