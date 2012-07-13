/*
 * TokyoTyrantCache.h
 *
 *  Created on: 2009-7-30
 *      Author: changqing.shao@opi-corp.com
 */

#ifndef SEARCHCACHE_SRC_TOKYOTYRANTCACHE_H_
#define SEARCHCACHE_SRC_TOKYOTYRANTCACHE_H_

#include <string>
#include <map>
#include <vector>
#include <tcrdb.h>
#include "IDbCache.h"
#include "BinString.h"
#include "Util.h"

#define TOKYOTYRANT_DSN_PREFIX "tokyotryant://"

namespace xce {
namespace searchcache {
  using MyUtil::ByteSeq;
    class TokyoTyrantResult;

    class TokyoTyrantCache : public IDbCache {
        friend class TokyoTyrantResult;
        public:
            explicit TokyoTyrantCache(std::string dsn);
            virtual ~TokyoTyrantCache();
            //初始化
            virtual bool Initialize();
            //查询单个ID
            virtual bool Query(long key, SearchCacheData *data_ptr);
            virtual bool Query(int key, SearchCacheData *data_ptr);
            virtual bool Query(long key, SearchMemCacheData *data_ptr);
            virtual bool Query(int key, SearchMemCacheData *data_ptr);
            //查询多个ID
            virtual bool Query(const vector<long> &keys, vector< SearchCacheDataPtr > *data_array_ptr);
            virtual bool Query(const vector<int> &keys, vector< SearchCacheDataPtr > *data_array_ptr);
            virtual bool Query(std::string key, std::vector<int>& vec); 
            virtual bool Query(long key, ByteSeq& value); 
            virtual bool Query(int key, ByteSeq& value); 
            virtual bool Query(const std::vector<long>& keys, std::map<long, ByteSeq>& id_2_user_info);
            virtual bool Query(const std::vector<int>& keys, std::map<int, ByteSeq>& id_2_user_info);
            //遍历数据库
            virtual IDbResult* Query();
            //插入数据
            virtual bool Insert(const vector<SearchCacheData> &data);
            virtual bool Insert(long key, const SearchCacheData &data);
            virtual bool Insert(int key, const SearchCacheData &data);
            virtual bool Insert(const char* key_buf, int key_size, const char* value_buf, int value_size);
            virtual bool Insert(const long key, const ByteSeq& data); 
            virtual bool Insert(const int key, const ByteSeq& data); 
            virtual bool Insert(const std::map<int, ByteSeq>& id_2_info_map); 
            //连接数据
            virtual bool Concatenate(const char* key_buf, int key_size, const char* value_buf, int value_size);
            //删除数据
            virtual bool Remove(const vector<long> &key);
            virtual bool Remove(const vector<int> &key);
            virtual bool Remove(long key);
            virtual bool Remove(int key);
            virtual void Close();
        private:
            //序列化
            static BinStringPtr Serialize(const SearchCacheData &data);
            //反序列
            static bool Unserialize(SearchCacheData *data_ptr, const char *buf, int lens);
            static bool Unserialize(SearchMemCacheData *data_ptr, const char *buf, int lens);
			static int GenerateKeyName(long userid, char *keybuf, int keybuflen);
			static int GenerateKeyName(int userid, char *keybuf, int keybuflen);
      static int DeGenerateKey(const char *keybuf, long& userid);
      static int DeGenerateKey(const char *keybuf, int& userid);

            bool Connect();

            TCRDB *db_handle_;
			std::string db_host_;
			int db_port_;
            std::string dsn_;
    };

    class TokyoTyrantResult : public IDbResult {
		friend class TokyoTyrantCache;

        public:
            ~TokyoTyrantResult();
            SearchCacheData* FetchObject();
            SearchMemCacheData* FetchObject(char*);
            BinString* FetchObject(long int &key);
            BinString* FetchObject(int &key);
        private:
            TokyoTyrantResult();
			bool Initialize(const char *db_host, int db_port);

            TCRDB *db_handle_;
    };

}
}
#endif
