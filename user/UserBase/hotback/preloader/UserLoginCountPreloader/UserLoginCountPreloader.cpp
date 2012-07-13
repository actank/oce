#include "UserLoginCountPreloader.h"
#include <user/UserBase/hotback/data/UserBaseDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::userlogincountpreloader;
using namespace xce::userbase;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserLoginCountPreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserBaseReader.Controller", "ControllerUserBaseReader");
        int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
        int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBaseReader.Proxy.Timeout", 300);

		int cluster = 100;
		
		GeneralPreloader<UserLoginCountReaderPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserLoginCountPreloaderFactory,
                ctlEndpoint, 
				cluster,
				subInterval,
                timeout,
				PRX_DATAQUEUE_MAX_SIZE ,
                PRODUCE_BATCH_SIZE ,
                CONSUME_BATCH_SIZE ,
				WRITE_BATCH_SIZE ,
                CONSUME_BATCH_THREAD_SIZE,
				"ULCM",true);
}


ObjectResultPtr UserLoginCountPreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_INFO("UserLoginCountPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserLoginCountPreloaderResultHandler handler(results );
	Statement sql;
	sql << "SELECT id, logincount FROM " << TABLE_USER_LOGINCOUNT << " WHERE id >= "<< minId <<" AND id <= "<< maxId;
	QueryRunner(DB_USER_LOGINCOUNT, CDbRServer ).query(sql, handler);
	
	return results;
}

ObjectResultPtr UserLoginCountPreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("UserLoginCountPreloader::create group -> id size: "<<ids.size());
        
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserLoginCountPreloaderResultHandler handler(results );
	
	Statement sql;
	sql << "SELECT id, logincount FROM " << TABLE_USER_LOGINCOUNT << "WHERE id IN (  ";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";

	QueryRunner(DB_USER_LOGINCOUNT,CDbRServer).query(sql, handler);

	return results;
}


BatchUserLoginCountPreloaderResultHandler::BatchUserLoginCountPreloaderResultHandler(
                MyUtil::ObjectResultPtr& results) :
        _results(results) {
}

bool BatchUserLoginCountPreloaderResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["id"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
	{
		UserLoginCountDataIPtr rs = new UserLoginCountDataI;
		rs->parse(row);
		_results->data.insert(make_pair(id, rs));
    } else 
	{
		MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
    }
    return true;
}

