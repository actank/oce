#ifndef __ONLINEBITMAP_ADAPTER_H__
#define __ONLINEBITMAP_ADAPTER_H__

#include "AdapterI.h"
#include "OnlineBitmap.h"

namespace talk{
namespace onlinebitmap{
	using namespace MyUtil;
	using namespace com::xiaonei::talk::common;
	using namespace ::talk::online;

	class OnlineBitmapAdapter : public AdapterI , public AdapterISingleton<MyUtil::TalkChannel, OnlineBitmapAdapter>{
	public:
		void setUserStatByOnlineType(const UserOnlineTypePtr& onlineTypePtr);
		void load(const UserOnlineTypeSeq& onlineTypeSeq);

		void SyncBitmap(const SyncBitmapDataPtr & sync);
		//获取用户在线类型
		UserOnlineTypePtr getUserType(int userId);
		UserOnlineTypeSeq getUserTypeByIdSeq(const MyUtil::IntSeq& ids);
		//获取用户在线状态
		UserOnlineTypePtr getUserStat(int userId);
		UserOnlineTypeSeq getUserStatByIdSeq(const MyUtil::IntSeq& ids);


		UserOnlineTypeSeq getOnlineUsers(const MyUtil::IntSeq& ids);
    Ice::Int getOnlineCount(const MyUtil::IntSeq& ids);
	protected:
		virtual string name(){
			return "OnlineBitmap";
		}
		virtual string endpoints(){
			return "@OnlineBitmap";
		}
		virtual size_t cluster(){
			return 0;
		}
		OnlineBitmapManagerPrx getManagerOneway();
		OnlineBitmapManagerPrx _proxy;

		OnlineBitmapManagerPrx getManager();
		OnlineBitmapManagerPrx two_way_proxy_;
	};
};
};


#endif
