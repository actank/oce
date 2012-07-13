#include "UserConfigPreloader.h"
#include <OceCommon/src/UserConfigCommon.h>
#include "GeneralPreloader/src/GeneralPreloader.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::userconfigpreloader;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserConfigPreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserConfig.Controller", "ControllerUserConfigR");
        int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
        int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserConfig.Proxy.Timeout", 300);
		
        int cluster= 100;
		
		GeneralPreloader<mop::hi::svc::model::UserConfigManagerPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserConfigPreloaderFactory,
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


ObjectResultPtr UserConfigPreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_INFO("UserConfigPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserConfigPreloaderResultHandler handler(results );
	Statement sql;
	sql << "SELECT id, email_config, basic_config, poke_config, request_friend_config," 
		<< 	" photo_config, message_config, browse_config, display_menu_list, more_menu_list," 
		<<  " want_see_css, send_feed_config, recv_feed_config, profile_privacy, status_config" 
		<<  " FROM " << TABLE_USER_CONFIG << " WHERE id >= "<< minId <<" AND id <= "<< maxId;
	QueryRunner(DB_USER_CONFIG, CDbRServer ).query(sql, handler);
	
	return results;
}

ObjectResultPtr UserConfigPreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("UserConfigPreloader::create group -> id size: "<<ids.size());
        
	ObjectResultPtr results = new ObjectResult;
	
	BatchUserConfigPreloaderResultHandler handler(results );
	
	Statement sql;
	sql << "SELECT id, email_config, basic_config, poke_config, request_friend_config," 
		<< 	" photo_config, message_config, browse_config, display_menu_list, more_menu_list," 
		<<  " want_see_css, send_feed_config, recv_feed_config, profile_privacy, status_config" 
		<<  " FROM " << TABLE_USER_CONFIG << "WHERE id IN (  ";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";

	QueryRunner(DB_USER_CONFIG,CDbRServer).query(sql, handler);

	return results;
}


BatchUserConfigPreloaderResultHandler::BatchUserConfigPreloaderResultHandler(
                MyUtil::ObjectResultPtr& results) :
        _results(results) {
}

bool BatchUserConfigPreloaderResultHandler::handle(mysqlpp::Row& row) const {
	
	int id = (int) row["id"];
    MyUtil::ObjectMap::iterator it = _results->data.find(id);
    if (it == _results->data.end())
	{
		mop::hi::svc::model::UserConfigDataIPtr rs = new mop::hi::svc::model::UserConfigDataI;
		rs->parse(row);
		_results->data.insert(make_pair(id, rs));
    } else 
	{
		MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
    }
    return true;
}

