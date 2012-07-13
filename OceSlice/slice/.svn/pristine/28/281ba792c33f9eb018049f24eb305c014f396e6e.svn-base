#ifndef __USERROOM_ICE__
#define __USERROOM_ICE__

#include <TalkCommon.ice>


module talk
{
module userroom
{
	interface UserRoomMap 
	{
		com::xiaonei::talk::common::RoomIdSeq getRoomIds(int userId, string domain);
		void addRoomId(int userId, com::xiaonei::talk::common::RoomId rid);
		void removeRoomId(int userId, com::xiaonei::talk::common::RoomId rid);
		void removeRoomIds(MyUtil::IntSeq userIds, com::xiaonei::talk::common::RoomId rid);
	};
};
};
#endif
