#ifndef __OCCUPANTSBACKUP_ICE__
#define __OCCUPANTSBACKUP_ICE__

#include <TalkCommon.ice>


module talk
{
module occupants
{

	class BackupManager{
		void put(com::xiaonei::talk::common::Occupant o);
		void puts(com::xiaonei::talk::common::OccupantSeq oseq);
		void remove(com::xiaonei::talk::common::RoomId rid,com::xiaonei::talk::common::Jid jid);
		void removeAll(com::xiaonei::talk::common::RoomId rid);
		com::xiaonei::talk::common::OccupantSeq get(com::xiaonei::talk::common::RoomId rid);
	};

};
};

#endif
