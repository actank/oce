#ifndef __ILIKECACHEPRELOADER_H__
#define __ILIKECACHEPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloaderProducer.h"
#include <ILikeIt.h>

namespace xce {
namespace ilikepreloader{

using namespace xce::ilikeit;

const std::string TABLE_NAME = "like_ship_";
const int TABLE_MOD = 100;
const std::string DB_SOURCE = "ilikeit";
const std::string COLUMN_UID = "uid";
const std::string COLUMN_GID = "gid";

const int PRODUCE_BATCH_SIZE = 10000;
const int CONSUME_BATCH_SIZE = 20000;
const int CONSUME_BATCH_THREAD_SIZE = 8;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class ILikeItPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<ILikeItPreloader> {
public:
        ILikeItPreloader () {}
        virtual ~ILikeItPreloader () {}
};

class ILikeItPreloaderFactory : virtual public xce::generalpreloader::IdListFactory {
public:
        ILikeItPreloaderFactory() {}
        virtual ~ILikeItPreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids) {
          return 0;
        }

        virtual MyUtil::ObjectResultPtr createWithLongIds(const MyUtil::LongSeq& ids);
};

class BatchILikeItPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchILikeItPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};

class BatchGlobalIdsProducer : public virtual xce::generalpreloader::Producer {
  public:
    virtual std::vector<int> produce(const int beginId, const int load_batch_size, int& maxId, int& tableMod) {
      vector<int> result;
      return result;
    }

    virtual std::vector<long> produceLongIds(long beginId, int load_batch_size, long& maxId, int& tableMod);
};

template<typename T>
class BatchIdResultHandler : public com::xiaonei::xce::ResultHandler {
  public:
    BatchIdResultHandler(const string& idColumnName, vector<T>& ids);
    virtual bool handle(mysqlpp::Row&) const;
  private:
    string _idColumnName;
    vector<T>& _ids;
};

}
}

#endif
