#ifndef _DIST_BUDDYAPPLYCACHERELOADERADAPTER_H_
#define _DIST_BUDDYAPPLYCACHERELOADERADAPTER_H_

#include <BuddyApplyCache.h>
#include <AdapterI.h>

namespace xce 
{
namespace distcache 
{
namespace buddyapplycache 
{

class DistBuddyApplyCacheReloaderAdapter : public MyUtil::ReplicatedClusterAdapterI<xce::buddyapplycache::DistBuddyApplyCacheReloaderPrx>, public MyUtil::Singleton<DistBuddyApplyCacheReloaderAdapter >{
public:
	DistBuddyApplyCacheReloaderAdapter();
	virtual void reload(MyUtil::IntSeq userIds, const Ice::Context& ctx = Ice::Context());
  virtual void reload(int userId, const Ice::Context& ctx = Ice::Context());
  virtual void add(int accepter, const Ice::Context& ctx = Ice::Context());
  virtual void remove(int accepter, int applicant, const Ice::Context& ctx = Ice::Context());
private:
	xce::buddyapplycache::DistBuddyApplyCacheReloaderPrx getManager(int id);
};

}
}
}

#endif
