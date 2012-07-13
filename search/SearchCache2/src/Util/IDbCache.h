#ifndef SEARCHCACHE_SRC_IDBCACHE_H__
#define SEARCHCACHE_SRC_IDBCACHE_H__

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include "SearchCacheData.h"
#include "SearchMemCacheData.h"
#include "Util.h"

namespace xce {
namespace searchcache {
    class IDbResult {
        public:
            virtual SearchCacheData* FetchObject() = 0;
            virtual SearchMemCacheData* FetchObject(char*) = 0;
            virtual BinString* FetchObject(long int &key) = 0;
            virtual BinString* FetchObject(int &key) = 0;
            virtual ~IDbResult() {};
    };

    class IDbCache {
        public:
            virtual bool Initialize() = 0;
            //查询单个ID
            virtual bool Query(long key, SearchCacheData *data_ptr) = 0;
            virtual bool Query(int key, SearchCacheData *data_ptr) = 0;
            virtual bool Query(long key, SearchMemCacheData *data_ptr) = 0;
            virtual bool Query(int key, SearchMemCacheData *data_ptr) = 0;
            //查询多个ID
            virtual bool Query(const vector<long> &keys, vector< SearchCacheDataPtr > *data_array_ptr) = 0;
            virtual bool Query(const vector<int> &keys, vector< SearchCacheDataPtr > *data_array_ptr) = 0;
            //用于从TT查询得到待更新的id组, key为时间串, 格式如"src_date_200911301100"或"delete", vec存放用户id列表, 表示指定分钟内要更新的所有id
            virtual bool Query(std::string key, std::vector<int>& vec) = 0;
            virtual bool Query(long key, MyUtil::ByteSeq& value) = 0; 
            virtual bool Query(int key, MyUtil::ByteSeq& value) = 0; 
            virtual bool Query(const std::vector<long>& keys, std::map<long, MyUtil::ByteSeq>& id_2_user_info) = 0;
            virtual bool Query(const std::vector<int>& keys, std::map<int, MyUtil::ByteSeq>& id_2_user_info) = 0;
            //遍历数据库
            virtual IDbResult* Query() = 0;
            //插入数据
            virtual bool Insert(const vector<SearchCacheData> &data) = 0;
            virtual bool Insert(long key, const SearchCacheData &data) = 0;
            virtual bool Insert(int key, const SearchCacheData &data) = 0;
            virtual bool Insert(const long key, const MyUtil::ByteSeq& data) = 0;
            virtual bool Insert(const int key, const MyUtil::ByteSeq& data) = 0;
            virtual bool Insert(const char* key_buf, int key_size, const char* value_buf, int value_size) = 0;
            virtual bool Insert(const std::map<int, MyUtil::ByteSeq>& id_2_info_map) = 0;
            //连接数据
            virtual bool Concatenate(const char* key_buf, int key_size, const char* value_buf, int value_size) = 0;
            //删除数据
            virtual bool Remove(const vector<long> &key) = 0;
            virtual bool Remove(const vector<int> &key) = 0;
            virtual bool Remove(long key) = 0;
            virtual bool Remove(int key) = 0;
            virtual void Close() = 0;
            virtual ~IDbCache() {};
    };
};
};
#endif
