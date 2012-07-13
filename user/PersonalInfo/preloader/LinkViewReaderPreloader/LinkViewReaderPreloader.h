#ifndef __XCE_LINKVIEWREADERPRELOADER_H__
#define __XCE_LINKVIEWREADERPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloaderProducer.h"
#include <PersonalInfo.h>

namespace xce {
namespace linkviewreaderpreloader {

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class LinkViewReaderPreloader : virtual public Ice::Object,
    virtual public MyUtil::Singleton<LinkViewReaderPreloader> {
public:
  LinkViewReaderPreloader() {}
  virtual ~LinkViewReaderPreloader() {}
};

class LinkViewReaderPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
  LinkViewReaderPreloaderFactory() {}
  virtual ~LinkViewReaderPreloaderFactory() {}
  virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
  virtual MyUtil::ObjectResultPtr create(Ice::Int minId, Ice::Int maxId);
};

class BatchLinkViewReaderPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
  BatchLinkViewReaderPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
  virtual bool handle(mysqlpp::Row& row) const;
private:
  MyUtil::ObjectResultPtr _results;
};

class BatchLinkUserIdProducer : public virtual xce::generalpreloader::Producer {
  virtual std::vector<int> produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod);
};

class BatchLinkUserIdHandlerI: public com::xiaonei::xce::ResultHandler {
public: 
  BatchLinkUserIdHandlerI(std::vector<int>& results, Ice::Int& maxId);
  virtual bool handle(mysqlpp::Row& row) const; 

private: 
  std::vector<int>& _results;
  Ice::Int& _maxId;
};

}
}
#endif
