#ifndef __MUC_COMMON_ICE__
#define __MUC_COMMON_ICE__

#include <TalkCommon.ice>

module com
{
module xiaonei
{
module talk
{
module muc
{
	class MucRoomId{
		string roomname;
		string domain;
	};
	sequence<MucRoomId> MucRoomIdSeq;
	
	
	class MucUserIdentity{
		MucRoomId roomid;
		string nickname;
		int userid;
	};
	sequence<MucUserIdentity> MucUserIdentitySeq;


	enum Permisions {POutcast,PNone,PMember,PAdmin,POwner};
	class MucActiveUser{
		MucUserIdentity identity;
		com::xiaonei::talk::common::Jid jid;
		Permisions permision;
		short statuscode;
		int offlinetime;
	};
	
	sequence<MucActiveUser> MucActiveUserSeq;
	
	class MucMember{
		int userid;
		Permisions permision;
	};
	
	sequence<MucMember> MucMemberSeq;
	dictionary<int, MucMember> MucMemberMap;
	
	class MucTalkMessage{
		int userid;
		int groupid;
		int timestamp;
		string message;
	};
	sequence<MucTalkMessage> MucTalkMessageSeq;


	//----------------------------------------------------
	class MucRoomMember{
		MucRoomId roomid;
		MucMember member;
	};
	sequence<MucRoomMember> MucRoomMemberSeq;
	dictionary<int, MucRoomMember> MucRoomMemberMap;

};
};
};
};

#endif
