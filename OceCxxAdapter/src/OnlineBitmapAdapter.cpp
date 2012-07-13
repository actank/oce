#include "OnlineBitmapAdapter.h"

namespace talk{
namespace onlinebitmap{

OnlineBitmapManagerPrx OnlineBitmapAdapter::getManager(){
	if(!two_way_proxy_) {
	  two_way_proxy_ = OnlineBitmapManagerPrx::uncheckedCast(locate<OnlineBitmapManagerPrx>("M", TWO_WAY));
  }
  return two_way_proxy_;
}

void OnlineBitmapAdapter::SyncBitmap(const SyncBitmapDataPtr & sync) {
		getManagerOneway()->SyncBitmap(sync);
}

	OnlineBitmapManagerPrx OnlineBitmapAdapter::getManagerOneway(){
		if(!_proxy){
			_proxy = OnlineBitmapManagerPrx::uncheckedCast(locate<OnlineBitmapManagerPrx>("M", ONE_WAY));
		}
		return _proxy;
	}
	void OnlineBitmapAdapter::setUserStatByOnlineType(const UserOnlineTypePtr& onlineTypePtr){
		getManager()->setUserStatByOnlineType(onlineTypePtr);
	}
	void OnlineBitmapAdapter::load(const UserOnlineTypeSeq& onlineTypeSeq){
		getManagerOneway()->load(onlineTypeSeq);
	}

	UserOnlineTypePtr OnlineBitmapAdapter::getUserType(int userId){
		return getManager()->getUserType(userId);
	}
	UserOnlineTypeSeq OnlineBitmapAdapter::getUserTypeByIdSeq(const MyUtil::IntSeq& ids){
		return getManager()->getUserTypeByIdSeq(ids);
	}

	UserOnlineTypePtr OnlineBitmapAdapter::getUserStat(int userId){
		return getManager()->getUserStat(userId);
	}
	UserOnlineTypeSeq OnlineBitmapAdapter::getUserStatByIdSeq(const MyUtil::IntSeq& ids){
		return getManager()->getUserStatByIdSeq(ids);
	}

	UserOnlineTypeSeq OnlineBitmapAdapter::getOnlineUsers(const MyUtil::IntSeq& ids){
		return getManager()->getOnlineUsers(ids);
	}

  Ice::Int OnlineBitmapAdapter::getOnlineCount(const MyUtil::IntSeq& ids){
		return getManager()->getOnlineCount(ids);
	}
};
};


