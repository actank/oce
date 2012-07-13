#include "UserTimeReaderPreloader.h"
#include <user/UserDesc/src/UserDescDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::usertimepreloader;
using namespace xce::userdesc;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserTimePreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserDescReader.Controller", "ControllerUserDescReader");
        int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
        int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserDescReader.Proxy.Timeout", 300);

		int cluster = 100;

		GeneralPreloader<UserTimeReaderPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserTimePreloaderFactory,
                ctlEndpoint, 
				cluster,
				subInterval,
                timeout,
				PRX_DATAQUEUE_MAX_SIZE ,
                PRODUCE_BATCH_SIZE ,
                CONSUME_BATCH_SIZE ,
				WRITE_BATCH_SIZE ,
                CONSUME_BATCH_THREAD_SIZE,
				"T",true);
}


ObjectResultPtr UserTimePreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_INFO("UserTimePreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserTimePreloaderResultHandler handler(results );
	Statement sql;
	sql << "SELECT id, registertime, activatetime, lastlogintime, lastlogouttime FROM " << TABLE_USER_TIME << " WHERE id >= "<< minId <<" AND id <= "<< maxId;
	QueryRunner(DB_USER_TIME, CDbRServer ).query(sql, handler);
	
	return results;
}

ObjectResultPtr UserTimePreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("UserTimePreloader::create group -> id size: "<<ids.size());
        
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserTimePreloaderResultHandler handler(results );
	
	Statement sql;
	sql << "SELECT id, registertime, activatetime, lastlogintime, lastlogouttime FROM " << TABLE_USER_TIME << "WHERE id IN (  ";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";

	QueryRunner(DB_USER_TIME,CDbRServer).query(sql, handler);

	return results;
}


BatchUserTimePreloaderResultHandler::BatchUserTimePreloaderResultHandler(
                MyUtil::ObjectResultPtr& results) :
        _results(results) {
}

bool BatchUserTimePreloaderResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["id"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
	{
		UserTimeDataIPtr rs = new UserTimeDataI;
		rs->parse(row);
		_results->data.insert(make_pair(id, rs));
    } else 
	{
		MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
    }
    return true;
}

