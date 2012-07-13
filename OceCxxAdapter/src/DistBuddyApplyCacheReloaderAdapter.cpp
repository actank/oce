#include "DistBuddyApplyCacheReloaderAdapter.h"
#include "BuddyApplyCache.h"

using namespace MyUtil;
using namespace xce::buddyapplycache;

xce::distcache::buddyapplycache::DistBuddyApplyCacheReloaderAdapter::DistBuddyApplyCacheReloaderAdapter() : MyUtil::ReplicatedClusterAdapterI <xce::buddyapplycache::DistBuddyApplyCacheReloaderPrx>("ControllerDistBuddyApplyCacheReloader", 120, 300) {
}

void xce::distcache::buddyapplycache::DistBuddyApplyCacheReloaderAdapter::reload(MyUtil::IntSeq userIds, const Ice::Context& ctx) {
	if(userIds.size() > 0) {
		try {
			getManager( 0 )->reload(userIds);
		}catch(...) {
			MCE_WARN("DistBuddyApplyCacheReloaderAdapter::reloader Exception!");
		}
	}
}

void xce::distcache::buddyapplycache::DistBuddyApplyCacheReloaderAdapter::reload(const int userId, const Ice::Context& ctx) {
	MyUtil::IntSeq ids;
	ids.push_back(userId);
	reload(ids);
}

void xce::distcache::buddyapplycache::DistBuddyApplyCacheReloaderAdapter::add(int accepter, const Ice::Context& ctx) {
  BuddyApplyCacheDataN buddyapply ;
  buddyapply.applicant = atoi(const_cast<Ice::Context&>(ctx)["APPLICANT"].c_str());
  Date date(const_cast<Ice::Context&>(ctx)["TIME"]);
  buddyapply.time = date.toSeconds();
  buddyapply.why = const_cast<Ice::Context&>(ctx)["WHY"];
  buddyapply.statfrom = const_cast<Ice::Context&>(ctx)["STAT_FROM"];
  buddyapply.groupname = const_cast<Ice::Context&>(ctx)["GROUP_NAME"];
  try {
    getManager( 0 )->add(accepter,buddyapply);
  } catch(Ice::TimeoutException e){
    MCE_WARN("[DistBuddyApplyCacheReloaderAdapter::add] id=" << accepter << " " << buddyapply.applicant << " found Ice::TimeoutException : " << e.what() );
  } catch( Ice::Exception e ){
    MCE_WARN("[DistBuddyApplyCacheReloaderAdapter::add] id=" << accepter << " " << buddyapply.applicant << " found Ice::Exception : " << e.what() );
  } catch( ... ){
    MCE_WARN("[DistBuddyApplyCacheReloaderAdapter::add] id=" << accepter << " " << buddyapply.applicant << " found unkown exception : " );
  }
}

void xce::distcache::buddyapplycache::DistBuddyApplyCacheReloaderAdapter::remove(int accepter, int applicant, const Ice::Context& ctx) {
  try {
    getManager( 0 )->remove(accepter, applicant);
  } catch(Ice::TimeoutException e){
    MCE_WARN("[DistBuddyApplyCacheReloaderAdapter::add] id=" << accepter << " " << applicant << " found Ice::TimeoutException : " << e.what() );
  } catch( Ice::Exception e ){
    MCE_WARN("[DistBuddyApplyCacheReloaderAdapter::add] id=" << accepter << " " << applicant << " found Ice::Exception : " << e.what() );
  } catch( ... ){
    MCE_WARN("[DistBuddyApplyCacheReloaderAdapter::add] id=" << accepter << " " << applicant << " found unkown exception : " );
  }

}

xce::buddyapplycache::DistBuddyApplyCacheReloaderPrx xce::distcache::buddyapplycache::DistBuddyApplyCacheReloaderAdapter::getManager(int id) {
	return getProxy(id);	
}
