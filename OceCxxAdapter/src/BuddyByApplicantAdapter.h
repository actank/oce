/*
 * =====================================================================================
 *
 *       Filename:  BuddyByApplicantCacheAdapter.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2010年06月25日 16时05分34秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */
#ifndef __BUDDY_BYAPPLICANT_CACHE_ADAPTER_H__
#define __BUDDY_BYAPPLICANT_CACHE_ADAPTER_H__

#include <BuddyByApplicantCache.h>
#include <AdapterI.h>

namespace xce {
namespace buddy {
namespace adapter {

class BuddyByApplicantCacheAdapter : public MyUtil::ReplicatedClusterAdapterI <BuddyByApplicantCacheManagerPrx>,
	public MyUtil::Singleton<BuddyByApplicantCacheAdapter> {
public:
	BuddyByApplicantCacheAdapter() : MyUtil::ReplicatedClusterAdapterI <BuddyByApplicantCacheManagerPrx>
					 ("ControllerBuddyByApplicantCache", 120, 300) {
	}

	xce::buddy::BuddyByApplicantDataPtr getAccepters(Ice::Int id, Ice::Int begin, Ice::Int limit);
	void addAccepter(Ice::Int id, const AccepterInfo& accepter);
	void removeAccepter(Ice::Int id, Ice::Int accepterId);
	void reload(Ice::Int id, const Ice::ObjectPtr& data);
	void clear(Ice::Int id);
};

class BuddyByApplicantLoaderAdapter : public MyUtil::ReplicatedClusterAdapterI <BuddyByApplicantLoaderPrx>,
	public MyUtil::Singleton<BuddyByApplicantLoaderAdapter> {
public:
	BuddyByApplicantLoaderAdapter() : MyUtil::ReplicatedClusterAdapterI <BuddyByApplicantLoaderPrx>
					 ("ControllerBuddyByApplicantLoader", 120, 300) {
	}

	void addAccepter(Ice::Int id, const AccepterInfo& accepter);
	void removeAccepter(Ice::Int id, Ice::Int accepterId);

	void reload(Ice::Int id);
};

};
};
};
#endif

