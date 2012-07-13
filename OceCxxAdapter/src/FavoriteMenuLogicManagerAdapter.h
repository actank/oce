#ifndef __FAVMENULOGICMANAGER_ADAPTER_H__
#define __FAVMENULOGICMANAGER_ADAPTER_H__

#include <MenuCache.h>
#include <Channel.h>
#include <Singleton.h>
#include <AdapterI.h>

namespace com {
namespace xiaonei {
namespace service {

class FavoriteMenuLogicManagerAdapter : public MyUtil::AdapterI,
	public MyUtil::AdapterISingleton<MyUtil::WServiceChannel, FavoriteMenuLogicManagerAdapter>,
	public Ice::Object {
public:
	FavoriteMenuLogicManagerAdapter() {
		//_managers.resize(cluster() > 0 ? cluster() : 1);
		//_managersOneway.resize(cluste) > 0 ? cluster() : 1);
	}


	::xce::menu::UserId2AppIdSeqMap get(::xce::menu::UserIdSeq userIds);
	void preload(const ::xce::menu::MenuCacheManagerPrx& menuCacheManager,int mod,int cluster);

	// FavoriteMenuLogicAgent will use these function
	::xce::menu::FavoriteMenuLogicManagerPrx getFavoriteMenuLogicManager();
	::xce::menu::FavoriteMenuLogicManagerPrx getFavoriteMenuLogicManagerOneway();

private:
	virtual string name() {
		return "M";
	}
	virtual string endpoints() {
		return "@FavoriteMenuLogic";
	}
	virtual size_t cluster() {
		return 1;
	}


	//typedef vector<FavoriteMenuLogicManagerPrx> FavoriteMenuLogicManagerSeq;

	//typedef vector<AppIds> IntSeqSeq;
	//void seperateFavoriteMenuLogicIds(const AppIds& userIds,
	//		Int2AppIdsMap& seperatedFavoriteMenuLogicIds);

	//FavoriteMenuLogicManagerSeq _managers;
	//FavoriteMenuLogicManagerSeq _managersOneway;

};


	typedef IceUtil::Handle<FavoriteMenuLogicManagerAdapter> FavoriteMenuLogicManagerAdapterPtr;
	

}
;
}
;
}
;

#endif

