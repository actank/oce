#include "UserBornPreloader.h"
#include <user/UserBase/hotback/data/UserBaseDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::userbornpreloader;
using namespace xce::userbase;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserBornPreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserBaseReader.Controller", "ControllerUserBaseReader");
    int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
    int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBaseReader.Proxy.Timeout", 300);
    int cluster = 100 ;
		
		GeneralPreloader<UserBornReaderPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserBornPreloaderFactory,
                ctlEndpoint,
				cluster,
                subInterval,
                timeout,
				PRX_DATAQUEUE_MAX_SIZE ,
                PRODUCE_BATCH_SIZE ,
                CONSUME_BATCH_SIZE ,
				WRITE_BATCH_SIZE ,
                CONSUME_BATCH_THREAD_SIZE,
				"UBOM",true);
}


ObjectResultPtr UserBornPreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_INFO("UserBornPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserBornPreloaderResultHandler handler(results );
	Statement sql;
	sql << "SELECT id, gender, birth_day, birth_month, birth_year, is_lunar_birth, home_province, home_city, city_code, astrology FROM " << TABLE_USER_BORN << " WHERE id >= "<< minId <<" AND id <= "<< maxId;
	QueryRunner(DB_USER_BORN, CDbRServer ).query(sql, handler);
	
	return results;
}

ObjectResultPtr UserBornPreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("UserBornPreloader::create group -> id size: "<<ids.size());
        
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserBornPreloaderResultHandler handler(results );
	
	Statement sql;
	sql << "SELECT id, gender, birth_day, birth_month, birth_year, is_lunar_birth, home_province, home_city, city_code, astrology FROM " << TABLE_USER_BORN << "WHERE id IN (  ";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";

	QueryRunner(DB_USER_BORN,CDbRServer).query(sql, handler);

	return results;
}


BatchUserBornPreloaderResultHandler::BatchUserBornPreloaderResultHandler(
                MyUtil::ObjectResultPtr& results) :
        _results(results) {
}

bool BatchUserBornPreloaderResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["id"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
	{
		UserBornDataIPtr rs = new UserBornDataI;
		rs->parse(row);
		_results->data.insert(make_pair(id, rs));
    } else 
	{
		MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
    }
    return true;
}

