#ifndef BUDDYAPPLYCACHEADAPTER_H_
#define BUDDYAPPLYCACHEADAPTER_H_

#include "BuddyApplyCache.h"
#include <vector>
#include "Singleton.h"
#include "AdapterI.h"

namespace xce {
namespace buddyapplycache {
/*
class BuddyApplyCacheAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::OceChannel, BuddyApplyCacheAdapter> {
public:
	BuddyApplyCacheAdapter(){
		_managersOneway.resize(cluster() > 0 ? cluster() : 1);
		_managers.resize(cluster() > 0 ? cluster() : 1);
	}
	
	BuddyApplyCacheDataSeq getBuddyApplyCache(int id, int begin, int limit);
	void removeBuddyApplyCache(int id, int applicant);
	void clear(int id);
	void rebuildBuddyApplyCache(int id);
	//void rebuildBuddyApplyCache(int id, MyUtil::Str2StrMap& props);
	
protected:
	virtual string name() {
		return "BuddyApplyCache";
	}
	virtual string endpoints() {
		return "@BuddyApplyCache";
	}
	virtual size_t cluster() {
		return 10;
	}
	virtual void initialize();

	BuddyApplyCacheManagerPrx getBuddyApplyCacheManager(int id);
	BuddyApplyCacheManagerPrx getBuddyApplyCacheManagerOneway(int id);
	typedef vector<BuddyApplyCacheManagerPrx> BuddyApplyCacheManagerSeq;
	BuddyApplyCacheManagerSeq _managersOneway;
	BuddyApplyCacheManagerSeq _managers;
};*/

class BuddyApplyCacheAdapterN : 
	public MyUtil::ReplicatedClusterAdapterI<BuddyApplyCacheManagerNPrx>, public MyUtil::Singleton<BuddyApplyCacheAdapterN>{
public:
        BuddyApplyCacheAdapterN():
		MyUtil::ReplicatedClusterAdapterI <BuddyApplyCacheManagerNPrx>("ControllerBuddyApplyCacheN", 120, 300){
        }

        BuddyApplyCacheDataSeqN getBuddyApplyCache(int id, int begin, int limit);
        void removeBuddyApplyCache(int id, int applicant);
        void clear(int id);
        //void rebuildBuddyApplyCache(int id, MyUtil::Str2StrMap& props);
	void rebuildBuddyApplyCache(int id, const Ice::Context& ctx);
	void reload(int id);
protected:
        virtual void initialize();

};

}
}

#endif /*BUDDYAPPLYCACHEADAPTER_H_*/
