#ifndef _ObJECT_CACHE_DEMO_ICE
#define _ObJECT_CACHE_DEMO_ICE

#include <ObjectCache.ice>
#include <Util.ice>

module xce {
	module demos {
//		class EmailData{
//			int id;
//			string email;
//		};

		interface ObjectCacheDemo {
			void load();
			void preload(); 
			void reload();
			int size();
			string getEmail(long id); //查询，如不在内存中自动加载
			string touchEmail(long id); //仅查询，若不存在不加载
			void setEmail(long id, string email);
		};
	};
};

#endif