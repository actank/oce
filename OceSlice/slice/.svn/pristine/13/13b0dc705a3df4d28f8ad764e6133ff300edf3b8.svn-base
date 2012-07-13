#ifndef __WTALKPROXY_ICE__
#define __WTALKPROXY_ICE__

#include <TalkCommon.ice>
#include <TalkProxy.ice>

module talk
{
	class OnlineCount{
		int user;
		int avatar;
	};
	interface WTalkProxyManager extends com::xiaonei::talk::DeliverInterface{
		//void deliver(com::xiaonei::talk::common::MessageSeq msgs);
		bool bind(string sessionKey, string ticket ,com::xiaonei::talk::common::Jid j);
		void unbind(string sessionKey, com::xiaonei::talk::common::Jid j);
		void closeAccessServer(string endpoint);
		com::xiaonei::talk::common::JidSeq getAccessJidSeq(int userId);
		bool upDeliver(com::xiaonei::talk::common::Jid jid, string msg);
		OnlineCount getOnlineCount();
		com::xiaonei::talk::common::MessageMap getMsg(int userId, long msgId);

		//-----------------------------------类人人爱听临时发送notify 需求的接口---------------
		bool StartNotify(int schemaid, int duration);//开始一个在线全部临时弹窗
		void StopNotify();//停止一个notify的发送
		void SetIsDeliver(bool isdeliver);//控制是否调用notify的开关
		//-------------------------------------------------------------------------------------
	};
	
};

#endif
