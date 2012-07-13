#ifndef _AD_USER_CACHE_ICE
#define _AD_USER_CACHE_ICE

#include <Util.ice>
#include <ObjectCache.ice>

module xce
{
module ad
{
	class AdUserCacheLoader
	{
		void LoadByIdSeq(MyUtil::IntSeq userIds);
		void LoadById(int userid);
		void ReloadUserInfoByIdSeq(MyUtil::IntSeq userIds);
		void ReloadUserInfoById(int userid);
		string GetById(int userid);
	};
};
};

#endif
