#ifndef __MUC_USERROOM_ICE__
#define __MUC_USERROOM_ICE__

#include <MucCommon.ice>
module com
{
module xiaonei
{
module talk
{
module muc
{
	class MucUserRoomManager{
		MucRoomIdSeq GetUserRooms(int userid, string domain);
		bool AddRoom(int userid, MucRoomId roomid, MucMember member);
		bool RemoveRoom(int userid, MucRoomId roomid);
		
		MucMember GetMemberPermision(int userid, MucRoomId roomid);
		MucMemberMap GetMemberPermisionBySeq(MyUtil::IntSeq userids, MucRoomId roomid);

		MucRoomMemberSeq GetUserRoomAndMember(int userid);
		//MucRoomMemberSeq GetUserRoomAndMemberBySeq(MyUtil::IntSeq);

		void SetUserPermision(int targetuser, MucRoomId roomid, Permisions permision);
		//this interface is only use for minigroup not IM
		//---------------------------------PUSH----------------------------------------
		void UserOnline(com::xiaonei::talk::common::Jid onlinejid);
		void UserOffline(com::xiaonei::talk::common::Jid offlinejid);
		void EnterRoomForTemporary(MucActiveUser enteruser);
		void AwayRoomForTemporary(com::xiaonei::talk::common::Jid awayjid, MucRoomId roomid);

	};
};
};
};
};

#endif
