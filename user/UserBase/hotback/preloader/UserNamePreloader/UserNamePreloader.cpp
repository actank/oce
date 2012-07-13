#include "UserNamePreloader.h"
#include <user/UserBase/hotback/data/UserBaseDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::usernamepreloader;
using namespace xce::userbase;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserNamePreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserBaseReader.Controller", "ControllerUserBaseReader");
    int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
    int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBaseReader.Proxy.Timeout", 300);
    int cluster = 100 ;
		
		GeneralPreloader<UserNameReaderPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserNamePreloaderFactory,
                ctlEndpoint,
				cluster,
                subInterval,
                timeout,
				PRX_DATAQUEUE_MAX_SIZE ,
                PRODUCE_BATCH_SIZE ,
                CONSUME_BATCH_SIZE ,
				WRITE_BATCH_SIZE ,
                CONSUME_BATCH_THREAD_SIZE,
				"UNM",true);
}


ObjectResultPtr UserNamePreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_INFO("UserNamePreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserNamePreloaderResultHandler handler(results );
	Statement sql;
	sql << "SELECT id, name, nickname FROM " << TABLE_USER_NAME << " WHERE id >= "<< minId <<" AND id <= "<< maxId;
	QueryRunner(DB_USER_NAME, CDbRServer ).query(sql, handler);
	
	return results;
}

ObjectResultPtr UserNamePreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("UserNamePreloader::create group -> id size: "<<ids.size());
        
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserNamePreloaderResultHandler handler(results );
	
	Statement sql;
	sql << "SELECT id, name, nickname FROM " << TABLE_USER_NAME << "WHERE id IN (  ";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";

	QueryRunner(DB_USER_NAME,CDbRServer).query(sql, handler);

	return results;
}


BatchUserNamePreloaderResultHandler::BatchUserNamePreloaderResultHandler(
                MyUtil::ObjectResultPtr& results) :
        _results(results) {
}

bool BatchUserNamePreloaderResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["id"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
	{
		UserNameDataIPtr rs = new UserNameDataI;
		rs->parse(row);
		_results->data.insert(make_pair(id, rs));
    } else 
	{
		MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
    }
    return true;
}

