#ifndef _BUDDY_SIXDEGREES_CACHE_ICE_
#define _BUDDY_SIXDEGREES_CACHE_ICE_

#include <ObjectCache.ice>

module xce {
module buddy {

	class BuddyData
    {
    	MyUtil::IntSeq friends; 
    };

	class IndexInfo 
	{
		int id;
        int degree;
        MyUtil::IntSeq friends;
    };

	sequence<MyUtil::IntSeq> IntSeqSeq;

	class RouteList
	{
		bool isEnd;
		IntSeqSeq friendsRoutList;
	};

	interface BuddySixDegreesManager 
	{
		bool computeTaskStart(int host, int guest);
		bool computeTaskStop(int host, int guest);
		RouteList getRouteList(int host, int guest, int begin, int limit);

		// old interface
        MyUtil::IntSeq getFriendList(int hostId, int limit);
        IntSeqSeq checkDegreesFriends(int id1, int id2 );
		bool isValid();
		void setValid(bool valid);
	};

};
};

#endif
