#ifndef __RECMENULOGICMANAGER_ADAPTER_H__
#define __RECMENULOGICMANAGER_ADAPTER_H__

#include <MenuCache.h>
#include <Channel.h>
#include <Singleton.h>
#include <AdapterI.h>

namespace com {
namespace xiaonei {
namespace service {
using namespace MyUtil;
using namespace ::xce::menu;

class RecentMenuLogicManagerAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::WServiceChannel, RecentMenuLogicManagerAdapter>,
	public Ice::Object{
public:
	RecentMenuLogicManagerAdapter() {
		//_managers.resize(cluster() > 0 ? cluster() : 1);
		//_managersOneway.resize(cluster() > 0 ? cluster() : 1);
	}


	UserId2AppIdSeqMap get(UserIdSeq userIds);
	void preload(const MenuCacheManagerPrx& menuCacheManager,int mod,int cluster);

	// RecentMenuLogicAgent will use these function
	RecentMenuLogicManagerPrx getRecentMenuLogicManager();
	RecentMenuLogicManagerPrx getRecentMenuLogicManagerOneway();

private:
	virtual string name() {
		return "M";
	}
	virtual string endpoints() {
		return "@RecentMenuLogic";
	}
	virtual size_t cluster() {
		return 1;
	}


	//typedef vector<RecentMenuLogicManagerPrx> RecentMenuLogicManagerSeq;

	//typedef vector<AppIds> IntSeqSeq;
	//void seperateRecentMenuLogicIds(const AppIds& userIds,
	//		Int2AppIdsMap& seperatedRecentMenuLogicIds);

	//RecentMenuLogicManagerSeq _managers;
	//RecentMenuLogicManagerSeq _managersOneway;

};


	typedef IceUtil::Handle<RecentMenuLogicManagerAdapter> RecentMenuLogicManagerAdapterPtr;

}
;
}
;
}

#endif

