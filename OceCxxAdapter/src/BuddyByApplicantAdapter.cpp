/*
 * =====================================================================================
 *
 *       Filename:  BuddyByApplicantCacheAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年06月25日 16时43分59秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "BuddyByApplicantAdapter.h"

using namespace xce::buddy;
using namespace xce::buddy::adapter;
using namespace xce::clusterstate;
using namespace MyUtil;

BuddyByApplicantDataPtr BuddyByApplicantCacheAdapter::getAccepters (Ice::Int id,
		Ice::Int begin, Ice::Int limit) {
	BuddyByApplicantDataPtr result;
	try {
		result = getProxy(id)->getAccepters(id, begin, limit);
	} catch (Ice::TimeoutException& e) {
		MCE_WARN( "[BuddyByApplicantCacheAdapter::getAccepters] "<< e << " id=" << id);
		return NULL;
	}
	return result;
}

void BuddyByApplicantCacheAdapter::addAccepter (Ice::Int id, const AccepterInfo& accepter) {
	try {
		vector<BuddyByApplicantCacheManagerPrx> prxs = getAllProxySeq( id );
		for( vector<BuddyByApplicantCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
			(*itPrx)->addAccepter(id, accepter);
		}
	} catch (Ice::TimeoutException& e) {
		MCE_WARN( "[BuddyByApplicantCacheAdapter::addAccepter] "<< e << " id=" << id);
		return;
	}
}

void BuddyByApplicantCacheAdapter::removeAccepter (Ice::Int id, Ice::Int accepterId) {
	try {
		vector<BuddyByApplicantCacheManagerPrx> prxs = getAllProxySeq( id );
		for( vector<BuddyByApplicantCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
			(*itPrx)->removeAccepter(id, accepterId);
		}
	} catch (Ice::TimeoutException& e) {
		MCE_WARN( "[BuddyByApplicantCacheAdapter::removeAccepter] "<< e << " id=" << id);
		return;
	}
}

void BuddyByApplicantCacheAdapter::reload (Ice::Int id, const Ice::ObjectPtr& data) {
	try {
		vector<BuddyByApplicantCacheManagerPrx> prxs = getAllProxySeq( id );
		for( vector<BuddyByApplicantCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
			(*itPrx)->reload(id, data);
		}
	} catch (Ice::TimeoutException& e) {
		MCE_WARN( "[BuddyByApplicantCacheAdapter::addAccepter] "<< e << " id=" << id);
		return;
	}
}

void BuddyByApplicantCacheAdapter::clear (Ice::Int id) {
	try {
		vector<BuddyByApplicantCacheManagerPrx> prxs = getAllProxySeq( id );
		for( vector<BuddyByApplicantCacheManagerPrx>::iterator itPrx = prxs.begin(); itPrx != prxs.end(); ++itPrx ){
			(*itPrx)->clear(id);
		}
	} catch (Ice::TimeoutException& e) {
		MCE_WARN( "[BuddyByApplicantCacheAdapter::clear] "<< e << " id=" << id);
		return;
	}
}

//-----------------------------------------------------------------------------------------------
void BuddyByApplicantLoaderAdapter::addAccepter (Ice::Int id, const AccepterInfo& accepter) {
	try {
		getProxy(id)->addAccepter(id, accepter);
	} catch (Ice::TimeoutException& e) {
		MCE_WARN( "[BuddyByApplicantLoaderAdapter::addAccepter] "<< e << " id=" << id);
		return;
	}
}

void BuddyByApplicantLoaderAdapter::removeAccepter (Ice::Int id, Ice::Int accepterId) {
	try {
		getProxy(id)->removeAccepter(id, accepterId);
	} catch (Ice::TimeoutException& e) {
		MCE_WARN( "[BuddyByApplicantLoaderAdapter::removeAccepter] "<< e << " id=" << id);
		return;
	}
}

void BuddyByApplicantLoaderAdapter::reload (Ice::Int id) {
	try {
		getProxy(id)->reload(id);
	} catch (Ice::TimeoutException& e) {
		MCE_WARN( "[BuddyByApplicantLoaderAdapter::reload] "<< e << " id=" << id);
		return;
	}
}
