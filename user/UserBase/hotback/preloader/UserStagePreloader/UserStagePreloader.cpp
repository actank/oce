#include "UserStagePreloader.h"
#include <user/UserBase/hotback/data/UserBaseDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::userstagepreloader;
using namespace xce::userbase;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserStagePreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserBaseReader.Controller", "ControllerUserBaseReader");
    int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
    int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBaseReader.Proxy.Timeout", 300);
    int cluster = 100 ;
		
		GeneralPreloader<UserStageReaderPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserStagePreloaderFactory,
                ctlEndpoint,
				cluster,
                subInterval,
                timeout,
				PRX_DATAQUEUE_MAX_SIZE ,
                PRODUCE_BATCH_SIZE ,
                CONSUME_BATCH_SIZE ,
				WRITE_BATCH_SIZE ,
                CONSUME_BATCH_THREAD_SIZE,
				"USGM",true);
}


ObjectResultPtr UserStagePreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_INFO("UserStagePreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserStagePreloaderResultHandler handler(results );
	Statement sql;
	sql << "SELECT id, univ, stage FROM " << TABLE_USER_STAGE << " WHERE id >= "<< minId <<" AND id <= "<< maxId;
	QueryRunner(DB_USER_STAGE, CDbRServer ).query(sql, handler);
	
	return results;
}

ObjectResultPtr UserStagePreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("UserStagePreloader::create group -> id size: "<<ids.size());
        
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserStagePreloaderResultHandler handler(results );
	
	Statement sql;
	sql << "SELECT id, univ, stage FROM " << TABLE_USER_STAGE << "WHERE id IN (  ";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";

	QueryRunner(DB_USER_STAGE,CDbRServer).query(sql, handler);

	return results;
}


BatchUserStagePreloaderResultHandler::BatchUserStagePreloaderResultHandler(
                MyUtil::ObjectResultPtr& results) :
        _results(results) {
}

bool BatchUserStagePreloaderResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["id"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
	{
		UserStageDataIPtr rs = new UserStageDataI;
		rs->parse(row);
		_results->data.insert(make_pair(id, rs));
    } else 
	{
		MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
    }
    return true;
}

