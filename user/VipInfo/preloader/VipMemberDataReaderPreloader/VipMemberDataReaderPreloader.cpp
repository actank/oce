#include "VipMemberDataReaderPreloader.h"
#include <user/VipInfo/share/VipInfoDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

void MyUtil::initialize() {
	MyUtil::ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&xce::vipmemberdatareaderpreloader::VipMemberDataReaderPreloader::instance(), service.createIdentity("M", ""));
	std::string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("VipInfoReader.Controller", "ControllerVipInfoReader");
	int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
	int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("VipInfoReader.Proxy.Timeout", 300);

	int cluster = 10;
	xce::generalpreloader::GeneralPreloader<xce::vipinfo::VipMemberDataReaderPrx>::instance().initialize(
                new xce::vipmemberdatareaderpreloader::BatchVipMemberIdProducer,
                new xce::vipmemberdatareaderpreloader::VipMemberDataReaderPreloaderFactory,
                ctlEndpoint, 
				cluster,
				subInterval,
                timeout,
				xce::vipmemberdatareaderpreloader::PRX_DATAQUEUE_MAX_SIZE ,
                xce::vipmemberdatareaderpreloader::PRODUCE_BATCH_SIZE ,
                xce::vipmemberdatareaderpreloader::CONSUME_BATCH_SIZE ,
				xce::vipmemberdatareaderpreloader::WRITE_BATCH_SIZE ,
                xce::vipmemberdatareaderpreloader::CONSUME_BATCH_THREAD_SIZE,
				"VM",true);
}

namespace xce
{
namespace vipmemberdatareaderpreloader
{

MyUtil::ObjectResultPtr VipMemberDataReaderPreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_INFO("VipMemberDataReaderPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	
	xce::vipinfo::BatchVipMemberMemDataResultHandlerI handler(result);

	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_VIP_MEMBER << " WHERE uid >= "<< minId <<" AND uid <= "<< maxId;
	
	com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbRServer ).query(sql, handler);
	
	return result;
}

MyUtil::ObjectResultPtr VipMemberDataReaderPreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_INFO("VipMemberDataReaderPreloaderFactory::create group -> id size: "<<ids.size());
        
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	
	xce::vipinfo::BatchVipMemberMemDataResultHandlerI handler(result);

	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_VIP_MEMBER << "WHERE uid IN (  ";
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

std::vector<int> BatchVipMemberIdProducer::produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod)
{
	std::vector<int> results;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT uid FROM user_member_info WHERE uid > " << beginId
		<< " ORDER BY uid LIMIT " << load_batch_size;
	BatchVipMemberIdHandlerI handler( results, maxId );
	com::xiaonei::xce::QueryRunner( DB_MEMBERS, com::xiaonei::xce::CDbRServer ).query( sql, handler );
	return results;
}

BatchVipMemberIdHandlerI::BatchVipMemberIdHandlerI( std::vector<int>& results, Ice::Int& maxId ) : _results(results), _maxId(maxId)
{
}

bool BatchVipMemberIdHandlerI::handle(mysqlpp::Row& row) const 
{
	int id = (int) row["uid"];
	_maxId = id > _maxId ? id : _maxId;
	_results.push_back( id );
	return true;
}

}
}

