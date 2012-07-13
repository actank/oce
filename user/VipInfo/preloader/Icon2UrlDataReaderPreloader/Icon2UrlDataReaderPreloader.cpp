#include "Icon2UrlDataReaderPreloader.h"
#include <user/VipInfo/share/VipInfoDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

void MyUtil::initialize() {
	MyUtil::ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&xce::icon2urldatareaderpreloader::Icon2UrlDataReaderPreloader::instance(), service.createIdentity("M", ""));
	std::string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("VipInfoReader.Controller", "ControllerVipInfoReader");
	int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
	int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("VipInfoReader.Proxy.Timeout", 300);

	int cluster = 10;
	xce::generalpreloader::GeneralPreloader<xce::vipinfo::Icon2UrlDataReaderPrx>::instance().initialize(
                new xce::icon2urldatareaderpreloader::BatchIconsIdProducer,
                new xce::icon2urldatareaderpreloader::Icon2UrlDataReaderPreloaderFactory,
                ctlEndpoint, 
				cluster,
				subInterval,
                timeout,
				xce::icon2urldatareaderpreloader::PRX_DATAQUEUE_MAX_SIZE ,
                xce::icon2urldatareaderpreloader::PRODUCE_BATCH_SIZE ,
                xce::icon2urldatareaderpreloader::CONSUME_BATCH_SIZE ,
				xce::icon2urldatareaderpreloader::WRITE_BATCH_SIZE ,
                xce::icon2urldatareaderpreloader::CONSUME_BATCH_THREAD_SIZE,
				"IU",true);
}

namespace xce
{
namespace icon2urldatareaderpreloader
{

MyUtil::ObjectResultPtr Icon2UrlDataReaderPreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_INFO("Icon2UrlDataReaderPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	
	xce::vipinfo::BatchIcon2UrlDataResultHandlerI handler(result);

	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_ICON_URL << " WHERE id >= "<< minId <<" AND id <= "<< maxId;
	
	com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbRServer ).query(sql, handler);
	
	return result;
}

MyUtil::ObjectResultPtr Icon2UrlDataReaderPreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_INFO("Icon2UrlDataReaderPreloaderFactory::create group -> id size: "<<ids.size());
        
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	
	xce::vipinfo::BatchIcon2UrlDataResultHandlerI handler(result);

	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_ICON_URL << " WHERE id IN (  ";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";
	
	com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbRServer ).query(sql, handler);
	
	return result;
}

std::vector<int> BatchIconsIdProducer::produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod)
{
	std::vector<int> results;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT id FROM " << TABLE_ICON_URL << " WHERE id >= " << beginId
		<< " ORDER BY id LIMIT " << load_batch_size;
	xce::generalpreloader::BatchUsersIdHandlerI handler( results, maxId );
	com::xiaonei::xce::QueryRunner( DB_MEMBERS, com::xiaonei::xce::CDbRServer ).query( sql, handler );
	return results;
}

}
}

