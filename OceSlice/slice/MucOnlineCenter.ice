#ifndef __MUC_ONLINECENTER_ICE__
#define __MUC_ONLINECENTER_ICE__

#include <MucCommon.ice>

module com
{
module xiaonei
{
module talk
{
module muc
{
	class MucOnlineCenterManager
	{
		//pager用户的聊天设置改变，通知pager的副本
		void ChatSetingChange(int userid, MucRoomId roomid, int set);
		//用户上线
		void UserOnline(MucActiveUser onlineuser);
		//用户上线批量接口
		void UserOnlineBySeq(MucActiveUserSeq onlineuserseq);
		//进入房间
		void EnterRoom(MucActiveUser opuser);
		//离开房间
		void AwayRoom(com::xiaonei::talk::common::Jid opuser, MucRoomId roomid);
		//发送聊天消息
		void SendTalkMessage(com::xiaonei::talk::common::Jid opjid , MucRoomId roomid, string msg);
		//获取在线用户数
		int GetUserCount(MucRoomId roomid);
		//设置某个用户的权限
		void SetPermision(int opuser, Permisions permision, int targetuser, MucRoomId roomid);
		//获取在线用户信息
		MucUserIdentitySeq GetActiveUsers(MucRoomId roomid);

		//获取某个房间的所有在线jid
		com::xiaonei::talk::common::JidSeq GetActiveJids(MucRoomId roomid);

		//FOR IM ONLY
		//按照权限获取用户
		MucActiveUserSeq GetUserByPermision(MucRoomId roomid, Permisions permision);
		//给IM提供的更改权限的接口
		bool SetPermisionForIM(int opuser, Permisions permision, int targetuser, MucRoomId roomid);
		//发送群内私聊
		void SendPrivateMsg(com::xiaonei::talk::common::Jid sendjid, MucUserIdentity recidentity, string msg);
	};
};
};
};
};

#endif
