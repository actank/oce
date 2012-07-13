#ifndef __PRESENCE_ICE__
#define __PRESENCE_ICE__

#include <TalkCommon.ice>


module com
{
module xiaonei
{
module talk
{

	class PresenceManager
	{	
		void bind(common::Jid jid);
		void unbind(common::Jid jid);
		
		void presence(common::Jid jid, string show, string ver, string ext);
		
		common::JidSeqMap getIMOnlineUserJidSeqMap(MyUtil::IntSeq ids);
		common::JidSeq getOnlineUserJidSeq(int id);
	 	common::OnlineInfoMap filterOnlineUsers(MyUtil::IntSeq ids);
	 	common::OnlineInfo filterOnlineUser(common::Jid jid);
	 	common::OnlineInfoSeq getOnlineUsersInfo(common::JidSeq jids);
	 	
	 	bool has(common::Jid jid);
	 	
	 	void closeProxyServer(string endpoint);	
	 	
	};	

	
};
};
};

#endif
