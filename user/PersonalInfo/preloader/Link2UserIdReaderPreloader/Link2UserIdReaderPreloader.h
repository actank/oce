#ifndef __XCE_LINK2USERIDREADERPRELOADER_H__
#define __XCE_LINK2USERIDREADERPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "GeneralPreloader/src/GeneralPreloaderProducer.h"
#include <PersonalInfo.h>

namespace xce {
namespace link2useridreaderpreloader {

const int PRODUCE_BATCH_SIZE = 5000;
const int CONSUME_BATCH_SIZE = 5000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class Link2UserIdReaderPreloader : virtual public Ice::Object,
    virtual public MyUtil::Singleton<Link2UserIdReaderPreloader> {
public:
  Link2UserIdReaderPreloader() {}
  virtual ~Link2UserIdReaderPreloader() {}
};

class Link2UserIdReaderPreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
  Link2UserIdReaderPreloaderFactory() {}
  virtual ~Link2UserIdReaderPreloaderFactory() {}
  virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
  virtual MyUtil::ObjectResultPtr create(Ice::Int minId, Ice::Int maxId);
};

class BatchLink2UserIdReaderPreloaderResultHandler : public com::xiaonei::xce::ResultHandler {
public:
  BatchLink2UserIdReaderPreloaderResultHandler (MyUtil::ObjectResultPtr& results);
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
