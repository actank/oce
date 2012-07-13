#ifndef __MESSAGE_HOLDER_ICE__
#define __MESSAGE_HOLDER_ICE__

#include <TalkCommon.ice>

module talk{

class HolderManager{
	
	void message(com::xiaonei::talk::common::Message msg);
	void send(int userId, long messageKey);
	void cleanOfflineMessageCount(int userid);
	int getOfflineMessageCount(int userid);
	void loadOfflineMessage(com::xiaonei::talk::common::Jid jid);


	void LoadUnreadNotifyInfoCount(com::xiaonei::talk::common::Jid jid);//pager用户登录的时候，加载未读的Notify系统的消息(不在新留言以及回复提示那显示的)的数量. add by guoqing.liu
};


};

#endif
