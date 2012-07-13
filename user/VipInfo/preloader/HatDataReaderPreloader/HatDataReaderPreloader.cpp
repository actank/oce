#include "HatDataReaderPreloader.h"
#include <user/VipInfo/share/VipInfoDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

void MyUtil::initialize() {
	MyUtil::ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&xce::hatdatareaderpreloader::HatDataReaderPreloader::instance(), service.createIdentity("M", ""));
	std::string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("VipInfoReader.Controller", "ControllerVipInfoReader");
	int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
	int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("VipInfoReader.Proxy.Timeout", 300);

	int cluster = 10;
	xce::generalpreloader::GeneralPreloader<xce::vipinfo::HatDataReaderPrx>::instance().initialize(
                new xce::hatdatareaderpreloader::BatchHatsIdProducer,
                new xce::hatdatareaderpreloader::HatDataReaderPreloaderFactory,
                ctlEndpoint, 
				cluster,
				subInterval,
                timeout,
				xce::hatdatareaderpreloader::PRX_DATAQUEUE_MAX_SIZE ,
                xce::hatdatareaderpreloader::PRODUCE_BATCH_SIZE ,
                xce::hatdatareaderpreloader::CONSUME_BATCH_SIZE ,
				xce::hatdatareaderpreloader::WRITE_BATCH_SIZE ,
                xce::hatdatareaderpreloader::CONSUME_BATCH_THREAD_SIZE,
				"IH",true);
}

namespace xce
{
namespace hatdatareaderpreloader
{

MyUtil::ObjectResultPtr HatDataReaderPreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_INFO("HatDataReaderPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	
	xce::vipinfo::BatchHatDataResultHandlerI handler(result);

	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_HAT << " WHERE id >= "<< minId <<" AND id <= "<< maxId;
	
	com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbRServer ).query(sql, handler);
	
	return result;
}

MyUtil::ObjectResultPtr HatDataReaderPreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_INFO("HatDataReaderPreloaderFactory::create group -> id size: "<<ids.size());
        
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	
	xce::vipinfo::BatchHatDataResultHandlerI handler(result);

	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_HAT << " WHERE id IN (  ";
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

std::vector<int> BatchHatsIdProducer::produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod)
{
	std::vector<int> results;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT id FROM " << TABLE_HAT << " WHERE id >= " << beginId
		<< " ORDER BY id LIMIT " << load_batch_size;
	xce::generalpreloader::BatchUsersIdHandlerI handler( results, maxId );
	com::xiaonei::xce::QueryRunner( DB_MEMBERS, com::xiaonei::xce::CDbRServer ).query( sql, handler );
	return results;
}

}
}

