#ifndef SEARCHCACHE_SRC_IDBCACHE_H__
#define SEARCHCACHE_SRC_IDBCACHE_H__

#include <boost/shared_ptr.hpp>
#include <boost/cstdint.hpp>
#include "SearchCacheData.h"

namespace xce {
namespace searchcache {
    class IDbResult {
        public:
            virtual SearchCacheData* FetchObject() = 0;
            virtual ~IDbResult() {};
    };

    class IDbCache {
        public:
            virtual bool Initialize() = 0;
            //查询单个ID
            virtual bool Query(long key, SearchCacheData *data_ptr) = 0;
            //查询多个ID
            virtual bool Query(const vector<long> &keys, vector< boost::shared_ptr<SearchCacheData> > *data_array_ptr) = 0;
            //遍历数据库
            virtual IDbResult* Query() = 0;
            //插入数据
            virtual bool Insert(const vector<SearchCacheData> &data) = 0;
            virtual bool Insert(long key, const SearchCacheData &data) = 0;
            //删除数据
            virtual bool Remove(const vector<long> &key) = 0;
            virtual bool Remove(long key) = 0;
            virtual void Close() = 0;
            virtual ~IDbCache() {};
    };
};
};
#endif
