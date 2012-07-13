#include "PageFansPreloader.h"

using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace ::xce::buddy;
using namespace ::MyUtil;
using namespace ::com::xiaonei::xce;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&PageFansPreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("PageFansCache.Controller", "ControllerPageFansCache");
	int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
	int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("PageFansCache.Proxy.Timeout", 3000);
	int cluster= 1;
	GeneralPreloader<PageFansManagerPrx>::instance().initialize(
			new BatchPageIdProducer,
			new PageFansPreloaderFactory,
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

ObjectResultPtr PageFansPreloaderFactory::create(const MyUtil::IntSeq& ids) {

	MCE_INFO("PageFansPreloaderFactory::create group -> id size: "<<ids.size());
	int minId = ids.front();
	int maxId = ids[ids.size()-1];
	MCE_INFO("PageFansPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);

	ObjectResultPtr results = new ObjectResult;

	for(std::vector<int>::const_iterator it = ids.begin(); it != ids.end() ; ++it)
	{

			int id=*it;
			PageFansObjPtr obj = new PageFansObj();

			{
				//	PageFansResultHandler handler(results );
					Statement sql;

					sql << "SELECT id, user_id, state , UNIX_TIMESTAMP(add_time) As  add_time , operator , page_id  FROM " <<
							PageFansHelper::getTableName("page_fans", id) << " WHERE page_id = '" << id
							<<"' ORDER BY user_id ";
				//	QueryRunner( DB_PAGE_FANS , CDbRServer ).query(sql, handler);
			}

			{
				//	PageEntryBlockResultHandler handler(results );
					Statement sql;

					sql << "SELECT id, userid, pageid , executor ,UNIX_TIMESTAMP(timestamp) As  timestamp  FROM " <<  TABLE_PAGE_FANS_BLOCK << " WHERE pageid = '" << id
							<<"' ORDER BY userid ";

				//	QueryRunner( DB_PAGE_FANS_BLOCK , CDbRServer ).query(sql, handler);

			}

		    results->data.insert(make_pair(id , obj));

	}

	MCE_INFO("PageFansPreloaderFactory::create group -> data.size: "<< results->data.size());

	return results;

}
