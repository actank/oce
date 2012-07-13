#include "UserDoingPreloader.h"
#include <user/UserBase/src/UserBaseDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::userdoingpreloader;
using namespace xce::userbase;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserDoingPreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserBase.Controller", "ControllerUserBase");
    int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
    int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBase.Proxy.Timeout", 300);
    int cluster = 100 ;
		
		GeneralPreloader<UserDoingManagerPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserDoingPreloaderFactory,
                ctlEndpoint,
				cluster,
                subInterval,
                timeout,
				PRX_DATAQUEUE_MAX_SIZE ,
                PRODUCE_BATCH_SIZE ,
                CONSUME_BATCH_SIZE ,
				WRITE_BATCH_SIZE ,
                CONSUME_BATCH_THREAD_SIZE,
				"UDM",true);
}


ObjectResultPtr UserDoingPreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_INFO("UserDoingPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserDoingPreloaderResultHandler handler(results );
	Statement sql;
	sql << "SELECT id, doing, doingtime FROM " << TABLE_USER_DOING << " WHERE id >= "<< minId <<" AND id <= "<< maxId;
	QueryRunner(DB_USER_DOING, CDbRServer ).query(sql, handler);
	
	return results;
}

ObjectResultPtr UserDoingPreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("UserDoingPreloader::create group -> id size: "<<ids.size());
        
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserDoingPreloaderResultHandler handler(results );
	
	Statement sql;
	sql << "SELECT id, doing, doingtime FROM " << TABLE_USER_DOING << "WHERE id IN (  ";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";

	QueryRunner(DB_USER_DOING,CDbRServer).query(sql, handler);

	return results;
}


BatchUserDoingPreloaderResultHandler::BatchUserDoingPreloaderResultHandler(
                MyUtil::ObjectResultPtr& results) :
        _results(results) {
}

bool BatchUserDoingPreloaderResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["id"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
	{
		UserDoingDataIPtr rs = new UserDoingDataI;
		rs->parse(row);
		_results->data.insert(make_pair(id, rs));
    } else 
	{
		MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
    }
    return true;
}

