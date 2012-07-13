#include "LinkViewReaderPreloader.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "user/PersonalInfo/share/LinkViewDataI.cpp"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::generalpreloader;
using namespace xce::personalinfo;
using namespace xce::linkviewreaderpreloader;

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&LinkViewReaderPreloader::instance(), service.createIdentity("M", ""));
  string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("LinkViewReader.Controller", "ControllerLinkViewReader");
  int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
  int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("LinkViewReader.Proxy.Timeout", 300);
  int cluster= 10;

  GeneralPreloader<LinkViewReaderPrx>::instance().initialize(
      new BatchLinkUserIdProducer,
      new LinkViewReaderPreloaderFactory,
      ctlEndpoint,
      cluster,
      subInterval,
      timeout,
      PRX_DATAQUEUE_MAX_SIZE ,
      PRODUCE_BATCH_SIZE ,
      CONSUME_BATCH_SIZE ,
      WRITE_BATCH_SIZE ,
      CONSUME_BATCH_THREAD_SIZE);
}


ObjectResultPtr LinkViewReaderPreloaderFactory::create(Ice::Int minId, Ice::Int maxId) {
  MCE_INFO("LinkViewReaderPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
  ObjectResultPtr results = new ObjectResult;
  BatchLinkViewReaderPreloaderResultHandler handler(results );
  Statement sql;
  sql << "SELECT id, link, linkStatus" << " FROM " << TABLE_PERSONAL
    << " WHERE id >= "<< minId <<" AND id <= "<< maxId;
  QueryRunner(DB_PERSONAL, CDbRServer).query(sql, handler);
  return results;
}

ObjectResultPtr LinkViewReaderPreloaderFactory::create(const MyUtil::IntSeq& ids) {
  MCE_INFO("LinkViewReaderPreloader::create group -> id size: "<<ids.size());
  ObjectResultPtr results = new ObjectResult;
  BatchLinkViewReaderPreloaderResultHandler handler(results );
  Statement sql;
  sql << "SELECT id, link, linkStatus" << " FROM " << TABLE_PERSONAL << " WHERE id IN (";
  for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
    if (it != ids.begin()) {
      sql << ",";
    }
    sql << *it;
  }
  sql << ")";
  QueryRunner(DB_PERSONAL, CDbRServer).query(sql, handler);
  return results;
}


BatchLinkViewReaderPreloaderResultHandler::BatchLinkViewReaderPreloaderResultHandler(
                MyUtil::ObjectResultPtr& results) :
        _results(results) {
}

bool BatchLinkViewReaderPreloaderResultHandler::handle(mysqlpp::Row& row) const {
  int id = (int) row[CPIID.c_str()];
  MyUtil::ObjectMap::iterator it = _results->data.find(id);
  if (it == _results->data.end())  {
    LinkViewDataIPtr rs = new LinkViewDataI;
    rs->parse(row);
    _results->data.insert(make_pair(id, rs));
  } else {
    MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
  }
  return true;
}

std::vector<int> BatchLinkUserIdProducer::produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod) {
  MCE_INFO("BatchLinkUserIdProducer::produce beginId=" << beginId
    << " load_batch_size=" << load_batch_size << " maxId=" << maxId);
  std::vector<int> results;
  Statement sql;
  sql << "SELECT " << CPIID << " FROM " << TABLE_PERSONAL << " WHERE " << CPIID << " >= " << beginId
    << " ORDER BY " << CPIID << " LIMIT " << load_batch_size;
  BatchLinkUserIdHandlerI handler(results, maxId);
  QueryRunner(DB_PERSONAL, CDbRServer).query(sql, handler);
  MCE_INFO("BatchLinkUserIdProducer::produce finished, maxId=" << maxId); 
  return results;
}

BatchLinkUserIdHandlerI::BatchLinkUserIdHandlerI(std::vector<int>& results, Ice::Int& maxId) :
    _results(results), _maxId(maxId) {
}

bool BatchLinkUserIdHandlerI::handle(mysqlpp::Row& row) const {
  int id = (int) row[CPIID.c_str()];
  _maxId = id > _maxId ? id : _maxId;
  _results.push_back( id );
  return true;
}
