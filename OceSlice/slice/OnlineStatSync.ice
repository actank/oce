#ifndef __ONLINE_STAT_SYNC_ICE__
#define __ONLINE_STAT_SYNC_ICE__

#include <Util.ice>
#include <TalkCommon.ice>

module talk
{
module online
{
	class BuddyOnlineNotify{
		com::xiaonei::talk::common::Avatar host;
		MyUtil::IntSeq buddyList;
	};
	sequence<BuddyOnlineNotify> BuddyOnlineNotifySeq;
	
	class DispatchManager{
		void presence(com::xiaonei::talk::common::Jid jid, com::xiaonei::talk::common::OnlineStat stat, string ver, string ext);
		void offline(com::xiaonei::talk::common::Jid jid);
		void offlineBatch(com::xiaonei::talk::common::JidSeq jids);
	};

};
};

#endif
