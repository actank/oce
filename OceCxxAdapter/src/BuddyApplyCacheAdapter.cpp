#include "BuddyApplyCacheAdapter.h"

using namespace ::xce::buddyapplycache;

/*void BuddyApplyCacheAdapter::initialize() {
//	_managersOneway = locate<BuddyApplyCacheManagerPrx>("M", MyUtil::ONE_WAY);
//	_managers = locate<BuddyApplyCacheManagerPrx>("M", MyUtil::TWO_WAY);
}

BuddyApplyCacheDataSeq BuddyApplyCacheAdapter::getBuddyApplyCache(int id, int begin, int limit) {
	return getBuddyApplyCacheManager(id)->getBuddyApplyCache( id, begin, limit );
}

void BuddyApplyCacheAdapter::removeBuddyApplyCache(int id, int applicant) {
	getBuddyApplyCacheManager(id)->removeBuddyApplyCache( id, applicant );
}

void BuddyApplyCacheAdapter::clear(int id ) {
	getBuddyApplyCacheManager(id)->clear( id );
}*/
/*
void BuddyApplyCacheAdapter::rebuildBuddyApplyCache(int id, MyUtil::Str2StrMap& props) {
	MCE_DEBUG("------------------BuddyApplyCacheAdapter::rebuildBuddyApplyCache(" << id << ")" );
	getBuddyApplyCacheManager(id)->rebuildBuddyApplyCache( id, props );
}
*/
/*
void BuddyApplyCacheAdapter::rebuildBuddyApplyCache(int id) {
        MCE_DEBUG("------------------BuddyApplyCacheAdapter::rebuildBuddyApplyCache(" << id << ")" );
        getBuddyApplyCacheManager(id)->rebuildBuddyApplyCache( id );
}

BuddyApplyCacheManagerPrx BuddyApplyCacheAdapter::getBuddyApplyCacheManager(int id) {
	return locate<BuddyApplyCacheManagerPrx>(_managers, "M", id, MyUtil::TWO_WAY);
}

BuddyApplyCacheManagerPrx BuddyApplyCacheAdapter::getBuddyApplyCacheManagerOneway(int id) {
	return locate<BuddyApplyCacheManagerPrx>(_managersOneway, "M", id, MyUtil::ONE_WAY);
}
*/

/***********************************************************************************/

void BuddyApplyCacheAdapterN::initialize() {
//      _managersOneway = locate<BuddyApplyCacheManagerPrx>("M", MyUtil::ONE_WAY);
//      _managers = locate<BuddyApplyCacheManagerPrx>("M", MyUtil::TWO_WAY);
}

BuddyApplyCacheDataSeqN BuddyApplyCacheAdapterN::getBuddyApplyCache(int id, int begin, int limit) {
        return getProxy(id)->getBuddyApplyCache( id, begin, limit );
}

void BuddyApplyCacheAdapterN::removeBuddyApplyCache(int id, int applicant) {
	MCE_DEBUG("------------------BuddyApplyCacheAdapterN::removeBuddyApplyCache(" << id << " : " << applicant <<")" );
	vector<BuddyApplyCacheManagerNPrx> prxs = getAllProxySeq( id );
        for( vector<BuddyApplyCacheManagerNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->removeBuddyApplyCache( id, applicant );
                } catch(Ice::TimeoutException e){
                        MCE_WARN("[BuddyApplyCacheAdapter::removeBuddyApplyCache] id=" << id << " found Ice::TimeoutException : " << e.what() );
                        continue;
                } catch( Ice::Exception e ){
                        MCE_WARN("[BuddyApplyCacheAdapter::removeBuddyApplyCache] id=" << id << " found Ice::Exception : " << e.what() );
                        continue;
                } catch( ... ){
                        MCE_WARN("[BuddyApplyCacheAdapter::removeBuddyApplyCache] id=" << id << " found unkown exception : " );
                        continue;
                }
        }
}

void BuddyApplyCacheAdapterN::clear(int id ) {
	vector<BuddyApplyCacheManagerNPrx> prxs = getAllProxySeq( id );
	for( vector<BuddyApplyCacheManagerNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->clear( id );
                } catch(Ice::TimeoutException e){
                        MCE_WARN("[BuddyApplyCacheAdapter::clear] id=" << id << " found Ice::TimeoutException : " << e.what() );
                        continue;
                } catch( Ice::Exception e ){
                        MCE_WARN("[BuddyApplyCacheAdapter::clear] id=" << id << " found Ice::Exception : " << e.what() );
                        continue;
                } catch( ... ){
                        MCE_WARN("[BuddyApplyCacheAdapter::clear] id=" << id << " found unkown exception : " );
                        continue;
                }
        }
}
/*
void BuddyApplyCacheAdapterN::rebuildBuddyApplyCache(int id, MyUtil::Str2StrMap& props) {
        MCE_DEBUG("------------------BuddyApplyCacheAdapterN::rebuildBuddyApplyCache(" << id << ")" );
        getProxy(id)->rebuildBuddyApplyCache( id, props );
}
*/

void BuddyApplyCacheAdapterN::rebuildBuddyApplyCache(int id, const Ice::Context& ctx) {
        MCE_DEBUG("------------------BuddyApplyCacheAdapterN::rebuildBuddyApplyCache(" << id << ")" );
	vector<BuddyApplyCacheManagerNPrx> prxs = getAllProxySeq( id );
        for( vector<BuddyApplyCacheManagerNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->rebuildBuddyApplyCache( id, ctx );
                } catch(Ice::TimeoutException e){
                        MCE_WARN("[BuddyApplyCacheAdapter::rebuildBuddyApplyCache] id=" << id << " found Ice::TimeoutException : " << e.what() );
                        continue;
                } catch( Ice::Exception e ){
                        MCE_WARN("[BuddyApplyCacheAdapter::rebuildBuddyApplyCache] id=" << id << " found Ice::Exception : " << e.what() );
                        continue;
                } catch( ... ){
                        MCE_WARN("[BuddyApplyCacheAdapter::rebuildBuddyApplyCache] id=" << id << " found unkown exception : " );
                        continue;
                }
        }
}

void BuddyApplyCacheAdapterN::reload(int id) {
        MCE_DEBUG("------------------BuddyApplyCacheAdapterN::reload(" << id << ")" );
	vector<BuddyApplyCacheManagerNPrx> prxs = getAllProxySeq( id );
        for( vector<BuddyApplyCacheManagerNPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
                try {
                        (*itPrx)->reload( id );
                } catch(Ice::TimeoutException e){
                        MCE_WARN("[BuddyApplyCacheAdapter::removeBuddyApplyCache] id=" << id << " found Ice::TimeoutException : " << e.what() );
                        continue;
                } catch( Ice::Exception e ){
                        MCE_WARN("[BuddyApplyCacheAdapter::removeBuddyApplyCache] id=" << id << " found Ice::Exception : " << e.what() );
                        continue;
                } catch( ... ){
                        MCE_WARN("[BuddyApplyCacheAdapter::removeBuddyApplyCache] id=" << id << " found unkown exception : " );
                        continue;
                }
        }
}
