#ifndef FOLLOWCACHE_ICE_
#define FOLLOWCACHE_ICE_

#include <Util.ice>
#include <FollowLogic.ice>
module xce
{
module follow
{


class CacheManager
{
//	int addRelation(int publisher, int subscriber, int relation);
//	int cancelRelation(int publisher, int subscriber, int relation);

	
	void sync(int publisher,int subscriber, int relation);

	MyUtil::IntSeq getUserList(int publisher, int relation, int begin, int limit);
	int getUserListCount(int publisher,int relation);
	
	int getRelation(int publisher,int subscriber);
	
	MyUtil::Int2IntMap getRelations(int publisher, MyUtil::IntSeq ids);
	bool isValid();
};
};
};

#endif /*FOLLOWCACHE_ICE_*/
