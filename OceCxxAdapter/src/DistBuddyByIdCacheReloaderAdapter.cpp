#include "DistBuddyByIdCacheReloaderAdapter.h"

using namespace MyUtil;
using namespace xce::buddy;
using namespace xce::distcache::buddybyidcache;

xce::distcache::buddybyidcache::DistBuddyByIdCacheReloaderAdapter::DistBuddyByIdCacheReloaderAdapter() : MyUtil::ReplicatedClusterAdapterI <xce::buddy::DistBuddyByIdCacheReloaderPrx>("ControllerDistBuddyByIdCacheReloader", 120, 300) {
}

void xce::distcache::buddybyidcache::DistBuddyByIdCacheReloaderAdapter::reload(MyUtil::IntSeq userIds, const Ice::Context& ctx) {
	if(userIds.size() > 0) {
		try {
			getManager( 0 )->reload(userIds);
		}catch(...) {
			MCE_WARN("DistBuddyByIdCacheReloaderAdapter::reloader Exception!");
		}
	}
}

void xce::distcache::buddybyidcache::DistBuddyByIdCacheReloaderAdapter::reload( int userId, const Ice::Context& ctx) {
	MyUtil::IntSeq ids;
	ids.push_back(userId);
	reload(ids);
}

void xce::distcache::buddybyidcache::DistBuddyByIdCacheReloaderAdapter::statusUpdate(int userId, int status, const Ice::Context& ctx) {
  try {
    getManager( 0 )->statusUpdate(userId, status);
  } catch(Ice::TimeoutException e){
    MCE_WARN("[DistBuddyByIdCacheReloaderAdapter::statusUpdate] id=" << userId << " status=" << status << " found Ice::TimeoutException : " << e.what() );
  } catch( Ice::Exception e ){
    MCE_WARN("[DistBuddyByIdCacheReloaderAdapter::statusUpdate] id=" << userId << " status=" << status << " found Ice::Exception : " << e.what() );
  } catch( ... ){
    MCE_WARN("[DistBuddyByIdCacheReloaderAdapter::statusUpdate] id=" << userId << " status=" << status << " found unkown exception : " );
  }

}

void xce::distcache::buddybyidcache::DistBuddyByIdCacheReloaderAdapter::add(int host, int guest, const Ice::Context& ctx) {

  try {
    getManager( 0 )->add(host, guest);
  } catch(Ice::TimeoutException e){
    MCE_WARN("[DistBuddyByIdCacheReloaderAdapter::add] host=" << host << " guest=" << guest << " found Ice::TimeoutException : " << e.what() );
  } catch( Ice::Exception e ){
    MCE_WARN("[DistBuddyByIdCacheReloaderAdapter::add] host=" << host << " guest=" << guest << " found Ice::Exception : " << e.what() );
  } catch( ... ){
    MCE_WARN("[DistBuddyByIdCacheReloaderAdapter::add] host=" << host << " guest=" << guest << " found unkown exception : " );
  }
}

void xce::distcache::buddybyidcache::DistBuddyByIdCacheReloaderAdapter::remove(int host, int guest, const Ice::Context& ctx) {

  try {
    getManager( 0 )->remove(host, guest);
  } catch(Ice::TimeoutException e){
    MCE_WARN("[DistBuddyByIdCacheReloaderAdapter::remove] host=" << host << " guest=" << guest << " found Ice::TimeoutException : " << e.what() );
  } catch( Ice::Exception e ){
    MCE_WARN("[DistBuddyByIdCacheReloaderAdapter::remove] host=" << host << " guest=" << guest << " found Ice::Exception : " << e.what() );
  } catch( ... ){
    MCE_WARN("[DistBuddyByIdCacheReloaderAdapter::remove] host=" << host << " guest=" << guest << " found unkown exception : " );
  }
}

xce::buddy::DistBuddyByIdCacheReloaderPrx xce::distcache::buddybyidcache::DistBuddyByIdCacheReloaderAdapter::getManager(int id) {
	return getProxy(id);	
}
