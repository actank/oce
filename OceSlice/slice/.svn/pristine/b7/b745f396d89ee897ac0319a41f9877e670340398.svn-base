#ifndef __TRIPOD_CACHEMANAGER_ICE__
#define __TRIPOD_CACHEMANAGER_ICE__

#include <TripodData.ice>

module com {
module renren {
module tripod {

class CacheManager {

        /*
         * 描述：cachemiss之后调用该接口返回数据，并根据参数确定如何加载cache
         * 参数：keys，需要取得数据key
         *       namespaceId，加载数据的namespace标识符
         *       businessId，加载数据的business标识符
         *       expiredTime，过期时间，如果不过期则为0
         *       useMaster, 标识是否使用主库加载
         * 返回值：返回所有能够取到数据的keyvalue对，无法取到的key不在map中
         * 异常：基本异常，不作区分
         */
        com::renren::tripod::DataMap get(com::renren::tripod::KeySeq keys, string namespaceId, string businessId, int expiredTime, bool useMaster);
        
        /*
         * 描述：数据更新之后重新将数据加载到cache中
         * 参数：keys，需要取得数据key
         *       namespaceId，加载数据的namespace标识符
         *       businessId，加载数据的business标识符
         *       expiredTime，过期时间，如果不过期则为0
         *       useMaster, 标识是否使用主库加载
         * 返回值：成功返回true，否则返回false
         * 异常：基本异常，不作区分
         */
        bool load(com::renren::tripod::KeySeq keys, string namespaceId, string businessId, int expiredTime, bool useMaster);
        
        /*
         * 描述：删除cache中的数据
         * 参数：keys，需要删除的数据key
         *       namespaceId，加载数据的namespace标识符
         *       businessId，加载数据的business标识符
         * 返回值：成功返回true，否则返回false
         * 异常：基本异常，不作区分
         */
        bool deleteCache(com::renren::tripod::KeySeq keys, string namespaceId, string businessId);


        bool loadListCache(string key, string namespaceId, string businessId, bool userMaster);
        bool deleteListOrHashCache(string key, string namespaceid, string businessId);
        
};

};
};
};
#endif
