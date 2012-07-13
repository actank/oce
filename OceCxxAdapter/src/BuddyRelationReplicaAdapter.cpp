#include "BuddyRelationReplicaAdapter.h"

using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;
using namespace std;

// ========= BuddyRelationCacheAdapter =================================

BuddyDesc BuddyRelationCacheAdapter::getRelation( const Relationship& relation ){
	try {
		BuddyDesc desc = getProxy(relation.from)->getRelation(relation);
		return desc;

	} catch(Ice::Exception& e) {
		MCE_WARN("BuddyRelationCacheAdapter::getRelation, relation[" << relation.from << "," << relation.to << "] " << e.what());
		return NoPath;
	}
}

void BuddyRelationCacheAdapter::setRelation(const Relationship& relation, BuddyDesc desc){
	try {
		vector<BuddyRelationCacheManagerPrx> prxs = getAllProxySeq(relation.from);
		for( vector<BuddyRelationCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
			(*itPrx)->setRelation(relation, desc);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("BuddyRelationCacheAdapter::setRelation, relation[" << relation.from << "," << relation.to << "] " << e.what());
	}
}

void BuddyRelationCacheAdapter::addBuddyRelationData(Ice::Int id, const BuddyRelationDataPtr& data ){
	try {
		vector<BuddyRelationCacheManagerPrx> prxs = getAllProxySeq( id % getCluster() );
		for( vector<BuddyRelationCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
			(*itPrx)->addBuddyRelationData(id, data);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("BuddyRelationCacheAdapter::addBuddyRelationData, id=" << id << " " << e.what());
	}
}
// ========= BuddyRelationTTAdapter =================================

BuddyDesc BuddyRelationTTAdapter::getRelation( const Relationship& relation ){
	try {
		BuddyDesc desc = getProxy(relation.from)->getRelation(relation);
		return desc;

	} catch(Ice::Exception& e) {
		MCE_WARN("BuddyRelationTTAdapter::getRelation, relation[" << relation.from << "," << relation.to << "] " << e.what());
		return NoPath;
	}
}

void BuddyRelationTTAdapter::setRelation(const Relationship& relation, BuddyDesc desc){
	try {
		vector<BuddyRelationTTManagerPrx> prxs = getAllProxySeq(relation.from);
		for( vector<BuddyRelationTTManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
			(*itPrx)->setRelation(relation, desc);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("BuddyRelationTTAdapter::setRelation, relation[" << relation.from << "," << relation.to << "] " << e.what());
	}
}

void BuddyRelationTTAdapter::addBuddyRelationData(Ice::Int id, const BuddyRelationDataPtr& data ){
	try {
		vector<BuddyRelationTTManagerPrx> prxs = getAllProxySeq( id % getCluster() );
		for( vector<BuddyRelationTTManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
			(*itPrx)->addBuddyRelationData(id, data);
		}
	} catch (Ice::Exception& e) {
		MCE_WARN("BuddyRelationTTAdapter::addBuddyRelationData, id=" << id << " " << e.what());
	}
}
// ========= BuddyRelationLoaderAdapter =================================

void BuddyRelationLoaderAdapter::reload( Ice::Int id ){
	try {
 	       getProxy( id )->reload( id );
	} catch (Ice::Exception& e) {
		MCE_WARN("BuddyRelationLoaderAdapter::reload, id=" << id << " " << e.what());
	}
}

void BuddyRelationLoaderAdapter::setRelation(const Relationship& relation, BuddyDesc desc){
	try {
 	       getProxy(relation.from)->setRelation(relation, desc);
	} catch (Ice::Exception& e) {
		MCE_WARN("BuddyRelationLoaderAdapter::setRelation, relation[" << relation.from << "," << relation.to << "] " << e.what());
	}
}
