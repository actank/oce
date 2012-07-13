#ifndef __OCCUPANTS_ICE__
#define __OCCUPANTS_ICE__

#include <TalkCommon.ice>


module talk
{
module occupants
{
	interface OccupantsManager 
	{
		com::xiaonei::talk::common::OccupantSeq getOccupants(com::xiaonei::talk::common::RoomId roomId);
		//common::RoomJidSeq	getRoomJids(common::RoomID roomId);
		com::xiaonei::talk::common::Occupant	getOccupant(com::xiaonei::talk::common::RoomJid roomJid);
		com::xiaonei::talk::common::Occupant	getOccupantByJid(com::xiaonei::talk::common::RoomId roomId, 
						com::xiaonei::talk::common::Jid jid);	
		int enterRoom(com::xiaonei::talk::common::RoomJid roomJid, com::xiaonei::talk::common::Jid jid);
		com::xiaonei::talk::common::Occupant exitRoom(com::xiaonei::talk::common::RoomId rid, com::xiaonei::talk::common::Jid jid);
		bool changeNick(com::xiaonei::talk::common::RoomJid roomJid, com::xiaonei::talk::common::Jid jid);
		
		
		com::xiaonei::talk::common::OccupantSeq setRole(com::xiaonei::talk::common::Jid opJid, com::xiaonei::talk::common::RoomJid targetRJid,   
						com::xiaonei::talk::common::RoomRole role);
		com::xiaonei::talk::common::OccupantSeq setAffiliation(com::xiaonei::talk::common::Jid opJid, com::xiaonei::talk::common::RoomMember member);						
		
		com::xiaonei::talk::common::OccupantSeq setRoomConfig(com::xiaonei::talk::common::Jid opJid, com::xiaonei::talk::common::RoomId roomId, com::xiaonei::talk::common::Room room);
		
		com::xiaonei::talk::common::OccupantSeq destroyRoom(com::xiaonei::talk::common::Jid opJid, com::xiaonei::talk::common::RoomId roomId);
		
		bool invite(com::xiaonei::talk::common::Jid opJid, com::xiaonei::talk::common::RoomId roomId, com::xiaonei::talk::common::Jid jid);
		//int getRoomCount(string domain);
		//com::xiaonei::talk::common::RoomIdSeq getRooms(string domain, int index, int count);
		int getRoomCount();
		int getUserCount();
	};
};
};
#endif
