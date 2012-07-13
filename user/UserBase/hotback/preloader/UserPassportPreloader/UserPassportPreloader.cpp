#include "UserPassportPreloader.h"
#include <user/UserBase/hotback/data/UserBaseDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::userpassportpreloader;
using namespace xce::userbase;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserPassportPreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserBaseReader.Controller", "ControllerUserBaseReader");
        int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
        int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBaseReader.Proxy.Timeout", 300);

		int cluster = 100;
		GeneralPreloader<UserPassportReaderPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserPassportPreloaderFactory,
                ctlEndpoint, 
				cluster,
				subInterval,
                timeout,
				PRX_DATAQUEUE_MAX_SIZE ,
                PRODUCE_BATCH_SIZE ,
                CONSUME_BATCH_SIZE ,
				WRITE_BATCH_SIZE ,
                CONSUME_BATCH_THREAD_SIZE,
				"UPM",true);
}


ObjectResultPtr UserPassportPreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_INFO("UserPassportPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserPassportPreloaderResultHandler handler(results );
	Statement sql;
	sql << "SELECT id, account, domain, password_md5, status,safe_status FROM " << TABLE_USER_PASSPORT << " WHERE id >= "<< minId <<" AND id <= "<< maxId;
	QueryRunner(DB_USER_PASSPORT, CDbRServer ).query(sql, handler);
	
	return results;
}

ObjectResultPtr UserPassportPreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("UserPassportPreloader::create group -> id size: "<<ids.size());
        
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserPassportPreloaderResultHandler handler(results );
	
	Statement sql;
	sql << "SELECT id, account, domain, password_md5, status,safe_status FROM " << TABLE_USER_PASSPORT << "WHERE id IN (  ";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";

	QueryRunner(DB_USER_PASSPORT,CDbRServer).query(sql, handler);

	return results;
}


BatchUserPassportPreloaderResultHandler::BatchUserPassportPreloaderResultHandler(
                MyUtil::ObjectResultPtr& results) :
        _results(results) {
}

bool BatchUserPassportPreloaderResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["id"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
	{
		UserPassportDataIPtr rs = new UserPassportDataI;
		rs->parse(row);
		_results->data.insert(make_pair(id, rs));
    } else 
	{
		MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
    }
    return true;
}

