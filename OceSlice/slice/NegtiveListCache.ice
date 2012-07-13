#ifndef _NEGTIVE_LIST_CACHE_ICE_
#define _NEGTIVE_LIST_CACHE_ICE_

#include <Util.ice>

module xce {
module socialgraph {

interface NegtiveListCache {
	void Set(int hostId, MyUtil::IntSeq negtiveList);
	MyUtil::IntSeq Get(int hostId, int limit);
	void Remove(int hostId, MyUtil::IntSeq negtiveList);
};

};
};

#endif
