#ifndef __MUC_LOGIC_ICE__
#define __MUC_LOGIC_ICE__

#include <MucCommon.ice>
#include <TalkCacheLoader.ice>
module com
{
module xiaonei
{
module talk
{
module muc
{
	class MucGateManager
	{
		//for memcache test
		com::xiaonei::talk::cache::loader::TalkUser GetUserById(int userid);
		com::xiaonei::talk::cache::loader::TalkUserMap GetUserBySeq(MyUtil::IntSeq uids);
		//------------------------RECEIVE REQUEST-------
		//FOR GROUPS
		void ChatSetingChange(int userid, MucRoomId roomid, int set);
		void CreateGroup(MucRoomId roomid);
		void CreateGroupForUgc(int groupowner, MucRoomId roomid, MyUtil::IntSeq inviteusers, Permisions permision);
		void RemoveGroup(MucRoomId roomid);
		void ReloadGroupInfo(int roomid, string groupname);
		void AddGroupMember(int userid, MucRoomId roomid, Permisions permision);
		void AddGroupMemberBySeq(MyUtil::IntSeq userids, MucRoomId roomid, Permisions permision);
		void RemoveGroupMember(int userid, MucRoomId roomid);
		void SetPermision(int opuser, MucRoomId roomid, int targetuser, Permisions permision);
		//FOR IM
		void EnterRoom(MucActiveUser enteruser);
		bool SetPermisionForIM(int opuser, MucRoomId roomid, int targetuser, Permisions permision);
		MucActiveUserSeq GetUserByPermision(MucRoomId roomid, Permisions permision);
		void UserInviteUser(com::xiaonei::talk::common::Jid opjid, MucRoomId roomid, int targetuser);
		void SendPrivateMsg(com::xiaonei::talk::common::Jid sender, MucUserIdentity recidentity, string msg);
		//FOR PAGER
		string GetGroupName(int groupid);
		MucUserIdentitySeq GetRoomUserList(MucRoomId roomid);
		//IM && PAGER(groups.renren.com)
		void UserOnline(com::xiaonei::talk::common::Jid opjid);
		void UserOffline(com::xiaonei::talk::common::Jid opjid);
		MucRoomIdSeq GetUserRooms(int userid, string domain);
		void AwayRoom(com::xiaonei::talk::common::Jid opjid, MucRoomId roomid);
		void SendTalkMsg(com::xiaonei::talk::common::Jid opjid, MucRoomId roomid, string msg);
		void SendTalkMsgAttachment(com::xiaonei::talk::common::Jid opjid, MucRoomId roomid, string msg, string attachment);

		//------------------------RESPONSE---------
		void NotifyUserEnterRoom(MucActiveUser enteruser, MucActiveUserSeq presenceUsers, int mucstatus);
		void NotifyUserAwayRoom(MucActiveUser awayuser, com::xiaonei::talk::common::JidSeq presenceUsers, int mucstatus);
		void NotifyGroupChat(MucActiveUser awayuser, com::xiaonei::talk::common::JidSeq presenceUsers, string msg);
		void NotifySetPermision(MucActiveUser opuser, com::xiaonei::talk::common::JidSeq presenceUsers, MucActiveUserSeq targetuser, Permisions permision);
		void InviteIMUsers(MucRoomId roomid, com::xiaonei::talk::common::JidSeq imusers);
		void NotifyPrivateChat(MucActiveUser sender, int targetuser, string msg);
		//void NotifyInvite(MucRoomId roomid, com::xiaonei::talk::common::Jid fromuser, com::xiaoeni::talk::common::Jid touser);
		void NotifyNicknameError(MucRoomId roomid, com::xiaonei::talk::common::Jid jid);
		void NotifyChatSetingChange(MucRoomId roomid, com::xiaonei::talk::common::JidSeq jids, int set);
    // 通过groupid获取成员
    MyUtil::IntSeq GetUserListByGroupId(int groupid);
    // 通过用户id获取用户加入的群列表
    MyUtil::IntSeq GetGroupListByUserId(int userid);
    MyUtil::Str2StrMap GetGroupNameList(int userid);
	};
};
};
};
};

#endif
