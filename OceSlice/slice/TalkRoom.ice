#ifndef __TALK_ROOM_ICE__
#define __TALK_ROOM_ICE__

#include <TalkCommon.ice>

module talk
{
module room
{
	class RoomManager
	{
		com::xiaonei::talk::common::Room 	createRoom(com::xiaonei::talk::common::RoomJid rjid, com::xiaonei::talk::common::Jid jid);
		void	removeRoom(com::xiaonei::talk::common::RoomId rid);
		bool	setRoom(com::xiaonei::talk::common::RoomId rid,com::xiaonei::talk::common::Room rm);
		com::xiaonei::talk::common::Room	getRoom(com::xiaonei::talk::common::RoomId rid);
		//com::xiaonei::talk::common::RoomSeq getRoomSeq(int userid);
		//com::xiaonei::talk::common::RoomId  createUniqRoomName(com::xiaonei::talk::common::Jid host,string domain);
		
		com::xiaonei::talk::common::Affiliation getAffiliation(com::xiaonei::talk::common::RoomId rid,int userid);
		void	setRoomMember(com::xiaonei::talk::common::RoomMember member);
		//bool	changeMemberNickName(int userId,com::xiaonei::talk::common::RoomJid rJid, string nick);
		//void	changeMemberAffiliation(com::xiaonei::talk::common::RoomMember member);
		//void 	removeMember(com::xiaonei::talk::common::RoomMember member);
		MyUtil::IntSeq 	getRoomMembers(com::xiaonei::talk::common::RoomId rid, com::xiaonei::talk::common::Affiliation aType);	
		//com::xiaonei::talk::common::RoomMember getRoomMember(com::xiaonei::talk::common::RoomId rid, int userId);
	};

};
};

#endif
