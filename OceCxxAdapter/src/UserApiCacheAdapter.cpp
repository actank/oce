#include "UserApiCacheAdapter.h"

using ::xce::userapicache::UserApiCacheData1Ptr;
using ::xce::userapicache::UserApiCacheAdapter;
using ::xce::userapicache::UserApiCacheMemData;
using ::xce::userapicache::UserApiCacheMemDataPtr;

UserApiCacheData1Ptr UserApiCacheAdapter::GetUserApiCache(int userId) {
	try
	{
		UserApiCacheMemDataPtr data = get(userId);
		if(data)
		{
			return data->getApiCacheData1();
		}
	}catch(...)
	{
		MCE_WARN("UserApiCacheAdapter::GetUserApiCache Exception!");
	}
	return NULL;
}

map<int,UserApiCacheData1Ptr> UserApiCacheAdapter::GetUserApiCacheList(MyUtil::IntSeq userIds) {
	if (userIds.empty()) {
		return map<int,UserApiCacheData1Ptr>();
	}
	std::vector<long> lIds(userIds.begin(), userIds.end());
	std::vector<long> missIds;
	map<int,UserApiCacheData1Ptr> res;
	try {
		map<long, UserApiCacheMemDataPtr> datas = get(lIds, missIds);
		for(map<long, UserApiCacheMemDataPtr>::const_iterator it = datas.begin(); it != datas.end(); ++it )
		{
			res.insert(make_pair(it->first, it->second->getApiCacheData1()));
		}
	}catch(...)
	{
		MCE_WARN("UserApiCacheAdapter::GetUserApiCacheList Exception!");
	}
	return res;
}
