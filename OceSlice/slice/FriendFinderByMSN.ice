#ifndef _FRIENDFINDER_BYMSN_ICE_
#define _FRIENDFINDER_BYMSN_ICE_

#include <Util.ice>

module xce {
module friendfinder {
	
	dictionary<int,MyUtil::IntSeq> MSNmap;
	struct MSNdata {
		MSNmap puamap;
		MSNmap pukmap;
	};

	interface FriendFinderByMSNManager {

		MSNdata getMSNdata(int id);

		MSNmap getPUAmap(int id);
		MyUtil::IntSeq getPUAids(int id);
		MyUtil::IntSeq getPUAidsWithMask(int id,MyUtil::IntSeq mask);

		MSNmap getPUKmap(int id);
		MyUtil::IntSeq getPUKids(int id);
		MyUtil::IntSeq getPUKidsRandom(int id,int limit);
		MyUtil::IntSeq getPUKidsWithMask(int id,MyUtil::IntSeq mask);

		void add(int hostId,int guestId,int type);

		void remove(int hostId,int guestId);
		void removePUA(int hostId,int guestId);
		void removePUK(int hostId,int guestId);

		void reload(int id);
	};
};
};

#endif
