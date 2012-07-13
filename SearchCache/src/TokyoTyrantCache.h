/*
 * TokyoTyrantCache.h
 *
 *  Created on: 2009-7-30
 *      Author: changqing.shao@opi-corp.com
 */

#ifndef SEARCHCACHE_SRC_TOKYOTYRANTCACHE_H_
#define SEARCHCACHE_SRC_TOKYOTYRANTCACHE_H_

#include <tcrdb.h>
#include "IDbCache.h"
#include "BinString.h"

#define GET_INT_VALUE(x) do {\
    x = *(int*)p;\
    p+=sizeof(int);\
    total -= sizeof(int);\
    if (total < 0) { \
        MCE_WARN("GET_INT_VALUE is Error! total<0"); \
        return false;\
    }\
}while(0)

#define GET_STRING_VALUE(x) do {\
    int strlen = *(int*)p;\
    p+=sizeof(int);\
    total -= sizeof(int);\
    if (total < 0 || strlen < 0) { \
        MCE_WARN("total=" << total << "strlen =" << strlen); \
        return false;\
    }\
    total -= strlen;\
    if (total < 0) { \
        MCE_WARN("total < 0"); \
        return false;\
    }\
    x = string(p, strlen);\
    p+=strlen;\
}while(0)

namespace xce {
namespace searchcache {
    class TokyoTyrantResult;

    class TokyoTyrantCache : public IDbCache {
        friend class TokyoTyrantResult;
        public:
            TokyoTyrantCache();
            virtual ~TokyoTyrantCache();
            //初始化
            virtual bool Initialize();
            //查询单个ID
            virtual bool Query(long key, SearchCacheData *data_ptr);
            //查询多个ID
            virtual bool Query(const vector<long> &keys, vector< boost::shared_ptr<SearchCacheData> > *data_array_ptr);
            //遍历数据库
            virtual IDbResult* Query();
            //插入数据
            virtual bool Insert(const vector<SearchCacheData> &data);
            virtual bool Insert(long key, const SearchCacheData &data);
            //删除数据
            virtual bool Remove(const vector<long> &key);
            virtual bool Remove(long key);
            virtual void Close();
        private:
            //序列化
            static boost::shared_ptr<BinString> Serialize(const SearchCacheData &data);
            //反序列
            static bool Unserialize(SearchCacheData *data_ptr, const char *buf, int lens);
			static int GenerateKeyName(long userid, char *keybuf, int keybuflen);

            bool Connect();

            TCRDB *db_handle_;
			std::string db_host_;
			int db_port_;
    };

    class TokyoTyrantResult : public IDbResult {
		friend class TokyoTyrantCache;

        public:
            ~TokyoTyrantResult();
            SearchCacheData* FetchObject();
        private:
            TokyoTyrantResult();
			bool Initialize(const char *db_host, int db_port);

            TCRDB *db_handle_;
    };

}
}
#endif
