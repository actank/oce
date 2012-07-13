#ifndef __ONLINEBITMAP_ICE__
#define __ONLINEBITMAP_ICE__
#include <OnlineCenter.ice>
module talk{
module onlinebitmap{
class OnlineBitmapManager{
	void setUserStatByOnlineType(talk::online::UserOnlineType onlineType);
	void load(talk::online::UserOnlineTypeSeq onlineTypeSeq);

  // 重新从 online center 加载状态信息
	void Reload();
	//同步
  void SyncBitmap(talk::online::SyncBitmapData data);

	//获取用户在线类型
	talk::online::UserOnlineType getUserType(int userId);
	talk::online::UserOnlineTypeSeq getUserTypeByIdSeq(MyUtil::IntSeq ids);
	//获取用户在线状态
	talk::online::UserOnlineType getUserStat(int userId);
	talk::online::UserOnlineTypeSeq getUserStatByIdSeq(MyUtil::IntSeq ids);

	//获取在线用户
	talk::online::UserOnlineTypeSeq getOnlineUsers(MyUtil::IntSeq ids);
	//获取在线数量
  int getOnlineCount(MyUtil::IntSeq ids);
};
};
};

#endif
