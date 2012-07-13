#ifndef _TRIPOD_CLIENT_H__
#define _TRIPOD_CLIENT_H__

#include <TripodData.h>
#include <Common.h>

namespace com {
namespace renren {
namespace tripod {

class TripodClient {
    public:
        TripodClient(const std::string& zkAddress, const std::string& namespaceId, const std::string& businessId);

    public:
        DataMap get(const KeySeq& keys, KeySeq& missedKeys, const long timeout = 300);
        
        DataMap getMissed(const KeySeq& keys, long expiredTime, bool useMaster, const long timeout = 1000);
        
        DataMap getWithMissed(const KeySeq& keys, long expiredTime, bool useMaster, const long timeout = 300);
        
        bool load(const KeySeq& keys, long expiredTime, bool useMaster, const long timeout = 1000);
        
        bool deleteCache(const KeySeq& keys, const long timeout = 1000);

    private:
        CacheClientPtr getCacheClient();
        CacheManagerClientPtr getCacheManagerClient();

        void getMissedKeys(const DataMap& res, const KeySeq& keys, KeySeq& missedKeys);

    private:
        std::string zkAddress_;
        std::string namespaceId_;
        std::string businessId_;

        CacheClientPtr cacheClient_;
        CacheManagerClientPtr cacheManagerClient_;

        boost::shared_mutex cacheClientMutex_;
        boost::shared_mutex cacheManagerClientMutex_;


 public:
  void GetList(const std::string& key, StrList& value, int begin, int limit);
  void GetIntList(const std::string& key, IntList& value, int begin, int limit);
  bool LoadListCache(const std::string& key, bool use_master);
  bool DeleteListCache(const std::string& key);

 private:
  RedisCacheClientPtr GetRedisCacheClient();

 private:
  RedisCacheClientPtr redis_cache_client_;
  boost::shared_mutex redis_cache_client_mutex_;

};

}
}
}
#endif
