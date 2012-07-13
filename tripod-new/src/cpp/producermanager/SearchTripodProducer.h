#ifndef __SEARCH_TRIPOD_PRODUCER_H__
#define __SEARCH_TRIPOD_PRODUCER_H__

#include "TripodProducer.h"
#include <QueryRunner.h>
#include <boost/shared_ptr.hpp>
#include "search/tripodsearchcache/TripodSearchDbAdapter.h"
#include "search/tripodsearchcache/TripodCacheData.h"
#include <IceUtil/IceUtil.h>

namespace com {
  namespace renren {
    namespace searchtripod {

      class SearchTripodProducer : virtual public com::renren::tripod::TripodProducer {
        public :
          SearchTripodProducer() {
          }
          virtual ~SearchTripodProducer() {}
          virtual com::renren::tripod::DataMap create(const com::renren::tripod::KeySeq& keys, bool isMaster);
          virtual com::renren::tripod::KeySeq createHotKeys(const std::string& beginKey, int limit);
          virtual com::renren::tripod::KeySeq createKeys(const std::string& beginKey, int limit);

        private:
          bool loadSql(const std::vector<std::string>& keys, std::map<std::string, xce::search::tripodcachedata::TripodCacheDataPtr>& id2tripodData);
      };

      class BatchReadThread : public IceUtil::Thread {
        public:
          BatchReadThread(const std::vector<std::string>& ids, std::string tableName, std::string resultFields, std::string condition, int tableCluster, void (xce::search::tripodcachedata::TripodCacheData::*pFunc)(mysqlpp::Row& row), std::map<std::string, xce::search::tripodcachedata::TripodCacheDataPtr>& id2tripodData, std::string dbName) : ids_(ids), tableName_(tableName), resultFields_(resultFields), condition_(condition), tableCluster_(tableCluster), pFunc_(pFunc), id2tripodData_(id2tripodData), dbName_(dbName) { }
          virtual void run();
        private:
          com::xiaonei::xce::Statement generateSql();
          com::xiaonei::xce::Statement generateSql(const std::vector<std::string>& bucketIds, int cluster);
          const std::vector<std::string>& ids_;
          std::string tableName_;
          std::string resultFields_;
          std::string condition_;
          int tableCluster_;
          void (xce::search::tripodcachedata::TripodCacheData::*pFunc_)(mysqlpp::Row& row);
          std::map<std::string, xce::search::tripodcachedata::TripodCacheDataPtr>& id2tripodData_;
          std::string dbName_;
      };

      class BatchReadVectorThread : public IceUtil::Thread {
        public:
          BatchReadVectorThread(const std::vector<std::string>& ids, std::string tableName, std::string resultFields, std::string condition, void (xce::search::tripodcachedata::TripodCacheData::*pFunc)(std::vector<mysqlpp::Row>& rows), std::map<std::string, xce::search::tripodcachedata::TripodCacheDataPtr>& id2tripodData, std::string dbName) : ids_(ids), tableName_(tableName), resultFields_(resultFields), condition_(condition), pFunc_(pFunc), id2tripodData_(id2tripodData), dbName_(dbName) { }
          virtual void run();
        private:
          com::xiaonei::xce::Statement generateSql();
          const std::vector<std::string>& ids_;
          std::string tableName_;
          std::string resultFields_;
          std::string condition_;
          void (xce::search::tripodcachedata::TripodCacheData::*pFunc_)(std::vector<mysqlpp::Row>& rows);
          std::map<std::string, xce::search::tripodcachedata::TripodCacheDataPtr>& id2tripodData_;
          std::string dbName_;
      };
    }
  }
}

#endif
