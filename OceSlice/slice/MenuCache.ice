#ifndef _MENUCACHE_ICE_
#define _MENUCACHE_ICE_

#include <Util.ice>
#include <ObjectCache.ice>
module xce {
module menu {
	enum CacheType {
		INSTALLED, RECENTLY, FAVORITE
	};
	sequence<CacheType> CacheTypeSeq;
	
	sequence<int> AppIdSeq;
	sequence<int> UserIdSeq;
	dictionary<CacheType, AppIdSeq> CacheType2AppIdSeqMap;
	dictionary<int, AppIdSeq> UserId2AppIdSeqMap;

	class MenuCacheRecord {
		int userId;
		CacheType type;
		AppIdSeq infoArray;
	};
	sequence<MenuCacheRecord> MenuCacheRecordSeq;
	
	interface MenuCacheManager {		
		
		CacheType2AppIdSeqMap get(int userId, CacheTypeSeq typeArray);
		
		CacheType2AppIdSeqMap getAll(int userId);
	
    void deleteBatch(MenuCacheRecordSeq records);
    void addBatch(MenuCacheRecordSeq records);
		void set(int userId, CacheType type, AppIdSeq infoArray);
		
		/* @brief 批量set接口 -- by zhanghan 091118 */
		void setBatch(MenuCacheRecordSeq records);		

		/* @brief 批量setData 到objectCache */				
		void setData(MyUtil::ObjectResult userData,CacheType type);

		void load(UserIdSeq userIds);
		bool isValid();
		void setValid(bool newState);
		
	};
	
	interface RecentMenuLogicManager {
	
		UserId2AppIdSeqMap get(UserIdSeq userIds);
		void track(int userId, int appId);
		void tracks(int userId, AppIdSeq appIds);
		void remove(int userId, int appId);
		void removes(int userId, AppIdSeq appIds);
		void clean(int userId);
		void preload(MenuCacheManager* menuCacheMgr);
	};
	
	interface FavoriteMenuLogicManager {
	
		UserId2AppIdSeqMap get(UserIdSeq userIds);
		void add(int userId, int appId);
		void adds(int userId, AppIdSeq appIds);
		void reorder(int userId, AppIdSeq appIds);
		void remove(int userId, int appId);
		void removes(int userId, AppIdSeq appIds);
		void preload(MenuCacheManager* menuCacheMgr);
	
	};
	
};
};

#endif
