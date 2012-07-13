#ifndef __MUC_TALKROOM_ICE__
#define __MUC_TALKROOM_ICE__

#include <MucCommon.ice>
#include <OnlineCenter.ice>
module com
{
module xiaonei
{
module talk
{
module muc
{
	class MucTalkRoomManager{
		void CreateRoom(MucRoomId roomid);
		void RemoveRoom(MucRoomId roomid);
		
		void AddRoomMember(int userid, MucRoomId roomid);
		void RemoveRoomMember(int userid, MucRoomId roomid); 
		MyUtil::IntSeq GetRoomMember(MucRoomId roomid);
		MucActiveUserSeq GetRoomActiveUserSeq(MucRoomId roomid);
	};
};
};
};
};

#endif
