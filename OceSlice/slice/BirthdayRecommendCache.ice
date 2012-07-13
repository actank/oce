#ifndef _BIRTHDAY_RECOMMEND_CACHE_ICE_
#define _BIRTHDAY_RECOMMEND_CACHE_ICE_

#include <Util.ice>

module xce {
module socialgraph {

interface BirthdayRecommendCache {
	void Set(int hostId, MyUtil::IntSeq birthdayRecommendList);

	int Get(int hostId);
	int GetAndRemove(int hostId);
	MyUtil::IntSeq GetList(int hostId, int limit);
	void Remove(int hostId, int friendId);
};

};
};

#endif
