#include "ILikeItPreloader.h"
//#include <UserBase/src/UserBaseDataI.cpp>

#include "GeneralPreloader/src/GeneralPreloader.h"
#include "app/Utils/src/DBUtils.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::ilikepreloader;
using namespace xce::ilikeit;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&ILikeItPreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("ILikeIt.Controller", "ControllerILikeItCacheM");
    int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
    int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("ILikeIt.Proxy.Timeout", 300);
    int cluster = 1;
		
		GeneralPreloader<ILikeItCachePrx>::instance().initialize(
                new BatchGlobalIdsProducer,
                new ILikeItPreloaderFactory,
                ctlEndpoint,
				        cluster,
                subInterval,
                timeout,
				        PRX_DATAQUEUE_MAX_SIZE ,
                PRODUCE_BATCH_SIZE ,
                CONSUME_BATCH_SIZE ,
				        WRITE_BATCH_SIZE ,
                CONSUME_BATCH_THREAD_SIZE,
				        "", false, TABLE_MOD);
}


ObjectResultPtr ILikeItPreloaderFactory::createWithLongIds(const MyUtil::LongSeq& ids) {
	MCE_INFO("[ILikeItPreloaderFactory::createWithLongIds] ids.size = " << ids.size());
	ObjectResultPtr results = new ObjectResult;
  map<long, vector<long> > seperatedIds = xce::utils::DBUtils::seperateIdsByMod<long>(ids, TABLE_MOD);
  for (map<long, vector<long> >::const_iterator it = seperatedIds.begin(); it != seperatedIds.end(); ++it) {
    Statement sql;
    string tableName = TABLE_NAME + boost::lexical_cast<string>(it->first);
    sql << "select " << COLUMN_GID << ", " << COLUMN_UID << " from " << tableName << " where "
        << COLUMN_GID << " in " << xce::utils::DBUtils::getIdsStr<long>(it->second);
	  BatchILikeItPreloaderResultHandler handler(results);
    com::xiaonei::xce::QueryRunner(DB_SOURCE, com::xiaonei::xce::CDbRServer).query(sql, handler);
  }
	return results;
}

BatchILikeItPreloaderResultHandler::BatchILikeItPreloaderResultHandler(
                MyUtil::ObjectResultPtr& results) :
        _results(results) {
}

bool BatchILikeItPreloaderResultHandler::handle(mysqlpp::Row& row) const {
    long gid = (long) row["gid"];
    int uid = (int) row["uid"];
    MyUtil::ObjectMap::iterator it = _results->data.find(gid);
    if (it == _results->data.end()) {
		  LikeDataPtr rs = new LikeData;	
		  rs->users.push_back(uid);
		  _results->data.insert(make_pair(gid, rs));
    } else {
		  LikeDataPtr data = LikeDataPtr::dynamicCast(it->second);
	    MyUtil::IntSeq::iterator itSeq = find(data->users.begin(), data->users.end(), uid);
		  if(itSeq == data->users.end()){
			  LikeDataPtr::dynamicCast(it->second)->users.push_back(uid);
		  }
    }
    return true;
}

std::vector<long> BatchGlobalIdsProducer::produceLongIds(long beginId, int load_batch_size, long& maxId, int& tableMod) {
  MCE_INFO("[BatchGlobalIdsProducer::produceLongIds] beginId = " << beginId << ", load_batch_size = " << load_batch_size
      << ", maxId = " << maxId << ", tableMod = " << tableMod);
  vector<long> globalIds;
  Statement sql;
  string tableName = TABLE_NAME + boost::lexical_cast<string>(tableMod);
  sql << "select " << COLUMN_GID << " from " << tableName << " where " << COLUMN_GID << " >= " << beginId
      << " order by " << COLUMN_GID << " limit " << load_batch_size;
  BatchIdResultHandler<long> handler(COLUMN_GID, globalIds);
  com::xiaonei::xce::QueryRunner(DB_SOURCE, com::xiaonei::xce::CDbRServer).query(sql, handler);
  if (!globalIds.empty()) {
    maxId = *(globalIds.end() - 1);
  }
  return globalIds;
}

template<typename T>
BatchIdResultHandler<T>::BatchIdResultHandler(const string& idColumnName, vector<T>& ids) : _idColumnName(idColumnName), _ids(ids) {
}

template<typename T>
bool BatchIdResultHandler<T>::handle(mysqlpp::Row& row) const {
  T id = (T)row[_idColumnName.c_str()];
  _ids.push_back(id);
  return true;
}

