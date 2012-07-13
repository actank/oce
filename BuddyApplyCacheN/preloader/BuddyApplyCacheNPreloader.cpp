#include "BuddyApplyCacheNPreloader.h"
#include "GeneralPreloader/src/GeneralPreloader.h"

using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace ::xce::buddyapplycache;
using namespace ::MyUtil;
using namespace ::com::xiaonei::xce;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&BuddyApplyCacheNPreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("BuddyApplyCacheN.Controller", "ControllerBuddyApplyCacheN");
	int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
	int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("BuddyApplyCacheN.Proxy.Timeout", 300);
	int cluster= 10;
	GeneralPreloader<xce::buddyapplycache::BuddyApplyCacheManagerNPrx>::instance().initialize(
			new BatchUsersIdProducer,
			new BuddyApplyCacheNPreloaderFactory,
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

ObjectResultPtr BuddyApplyCacheNPreloaderFactory::create(const MyUtil::IntSeq& ids) {

	MCE_INFO("BuddyApplyCacheNPreloaderFactory::create group -> id size: "<<ids.size());
	int minId = ids.front();
	int maxId = ids[ids.size()-1];
	MCE_INFO("BuddyApplyCacheNPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);

	ObjectResultPtr results = new ObjectResult;

	for(std::vector<int>::const_iterator it = ids.begin(); it != ids.end() ; ++it)
	{
		int id = *it;
		BuddyApplyCacheNPtr buddyApplyCacheResult = new BuddyApplyCacheN();
		buddyApplyCacheResult->ownerId = id;
		BuddyApplyCacheResultHandlerI handler(buddyApplyCacheResult);
		Statement sql;
		sql
		<< "SELECT applicant,UNIX_TIMESTAMP(time) AS unix_time,why,stat_from,group_name"
		<< " FROM buddyapply_accepter_" << abs(id % 100) << " WHERE accepter = '" << id << "'"
		<< " ORDER BY time DESC"
		<< " LIMIT 100";
		ostringstream pattern;
		pattern << "buddyapply_accepter_" << id % 100;
		QueryRunner("buddyapplycache_accepter", CDbRServer,pattern.str()).query(sql, handler);
	
		results->data.insert(make_pair(*it, buddyApplyCacheResult));
	}

	MCE_INFO("BuddyApplyCacheNPreloaderFactory::create group -> data.size: "<< results->data.size());
	return results;

}
