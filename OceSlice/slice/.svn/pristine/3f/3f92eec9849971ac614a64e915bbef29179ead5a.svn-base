#ifndef _FRIENDFINDER_BYIP_ICE_
#define _FRIENDFINDER_BYIP_ICE_

#include <Util.ice>

module xce {
module friendfinder {
	
	struct IPNote {
		long ip;
		int onlineCount;
		int awayCount;
		int lastLoginTime;
	};
	sequence<IPNote> IPNoteSeq;

	interface FriendFinderByIPManager {

		IPNoteSeq getFreqIP(int userId,int limit);
		IPNoteSeq getRecIP(int userId,int limit);

		void accessIP(int userId,long ip);
		void accessIPStr(int userId,string ip);
		void accessIPs(int userId,MyUtil::LongSeq ips);
		void accessIPsStr(int userId,MyUtil::StrSeq ips);

		void reload(int userId);

	};
};
};

#endif
