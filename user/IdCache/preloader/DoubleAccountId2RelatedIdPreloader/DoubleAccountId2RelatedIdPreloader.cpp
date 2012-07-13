#include "DoubleAccountId2RelatedIdPreloader.h"
#include <user/IdCache/share/IdCacheDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

void MyUtil::initialize() {
	MyUtil::ServiceI& service = MyUtil::ServiceI::instance();
	service.getAdapter()->add(&idcache::DoubleAccountId2RelatedIdPreloader::instance(), service.createIdentity("M", ""));
	std::string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("IdCache.Controller", "ControllerIdCacheReader");
	int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
	int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("IdCache.Proxy.Timeout", 300);

	int cluster = 10;
	xce::generalpreloader::GeneralPreloader<idcache::DoubleAccountId2RelatedIdSetterPrx>::instance().initialize(
                new idcache::BatchGroupIdProducer,
                new idcache::DoubleAccountId2RelatedIdPreloaderFactory,
                ctlEndpoint, 
				cluster,
				subInterval,
                timeout,
				idcache::PRX_DATAQUEUE_MAX_SIZE ,
                idcache::PRODUCE_BATCH_SIZE ,
                idcache::CONSUME_BATCH_SIZE ,
				idcache::WRITE_BATCH_SIZE ,
                idcache::CONSUME_BATCH_THREAD_SIZE,
				"DS",true);
}

namespace idcache
{

MyUtil::ObjectResultPtr DoubleAccountId2RelatedIdPreloaderFactory::create(Ice::Int minId,Ice::Int maxId) 
{
	MCE_INFO("DoubleAccountId2RelatedIdPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	
	BatchDoubleAccountIdInfoHandlerI bdHandler(result);

	com::xiaonei::xce::Statement sql;
	sql << "SELECT userid, group_id FROM " << TABLE_MULTI_IDS << " WHERE group_id >= "<< minId <<" AND group_id <= "<< maxId;
	
	com::xiaonei::xce::QueryRunner(DB_MULTI_IDS_PRELOADER, com::xiaonei::xce::CDbRServer ).query(sql, bdHandler);

	MyUtil::ObjectResultPtr finalRes = new MyUtil::ObjectResult;

	MyUtil::IntSeq newIds;

	for(MyUtil::ObjectMap::const_iterator it = result->data.begin(); it != result->data.end(); ++it)
	{
		DoubleAccountIdInfoPtr oldData = DoubleAccountIdInfoPtr::dynamicCast(it->second);

		UserId2RelatedIdInfoPtr newData0 = new UserId2RelatedIdInfo;
		newData0->doubleAccountId = oldData->userId1;

		finalRes->data.insert(make_pair(oldData->userId0, newData0));
		
		UserId2RelatedIdInfoPtr newData1 = new UserId2RelatedIdInfo;
		newData1->doubleAccountId = oldData->userId0;

		finalRes->data.insert(make_pair(oldData->userId1, newData1));

		newIds.push_back(oldData->userId0);
		newIds.push_back(oldData->userId1);
	}
	
	BatchVirtualIdInfoHandlerI bvHandler(finalRes);

	com::xiaonei::xce::Statement sql1;
	sql1 << "SELECT virtual_id, page_id, user_id, state FROM " << TABLE_VIRTUAL_ID << " WHERE user_id IN ( ";
	for (MyUtil::IntSeq::const_iterator it = newIds.begin(); it != newIds.end(); ++it) {
		if (it != newIds.begin()) {
			sql1 << ",";
		}
		sql1 << *it;
	}
	sql1 << ")";
	
	com::xiaonei::xce::QueryRunner(DB_VIRTUAL_ID_PRELOADER, com::xiaonei::xce::CDbRServer ).query(sql1, bvHandler);

	return finalRes;
}

MyUtil::ObjectResultPtr DoubleAccountId2RelatedIdPreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_INFO("DoubleAccountId2RelatedIdPreloaderFactory::create group -> id size: "<<ids.size());
        
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	
	BatchDoubleAccountIdInfoHandlerI bdHandler(result);

	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_MULTI_IDS << "WHERE group_id IN (  ";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";
	
	com::xiaonei::xce::QueryRunner(DB_MULTI_IDS_PRELOADER, com::xiaonei::xce::CDbRServer ).query(sql, bdHandler);

	MyUtil::ObjectResultPtr finalRes = new MyUtil::ObjectResult;

	MyUtil::IntSeq newIds;

	for(MyUtil::ObjectMap::const_iterator it = result->data.begin(); it != result->data.end(); ++it)
	{
		DoubleAccountIdInfoPtr oldData = DoubleAccountIdInfoPtr::dynamicCast(it->second);

		UserId2RelatedIdInfoPtr newData0 = new UserId2RelatedIdInfo;
		newData0->doubleAccountId = oldData->userId1;

		finalRes->data.insert(make_pair(oldData->userId0, newData0));
		
		UserId2RelatedIdInfoPtr newData1 = new UserId2RelatedIdInfo;
		newData1->doubleAccountId = oldData->userId0;

		finalRes->data.insert(make_pair(oldData->userId1, newData1));

		newIds.push_back(oldData->userId0);
		newIds.push_back(oldData->userId1);
	}
	
	BatchVirtualIdInfoHandlerI bvHandler(finalRes);

	com::xiaonei::xce::Statement sql1;
	sql1 << "SELECT virtual_id, page_id, user_id, state FROM " << TABLE_VIRTUAL_ID << " WHERE user_id IN ( ";
	for (MyUtil::IntSeq::const_iterator it = newIds.begin(); it != newIds.end(); ++it) {
		if (it != newIds.begin()) {
			sql1 << ",";
		}
		sql1 << *it;
	}
	sql1 << ")";
	
	com::xiaonei::xce::QueryRunner(DB_VIRTUAL_ID_PRELOADER, com::xiaonei::xce::CDbRServer ).query(sql1, bvHandler);
	
	return finalRes;
}

std::vector<int> BatchGroupIdProducer::produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod)
{
	std::vector<int> results;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT group_id FROM " << TABLE_MULTI_IDS << " WHERE group_id > " << beginId
		<< " ORDER BY group_id LIMIT " << load_batch_size;
	BatchGroupIdHandlerI handler( results, maxId );
	com::xiaonei::xce::QueryRunner( DB_MULTI_IDS_PRELOADER, com::xiaonei::xce::CDbRServer ).query( sql, handler );
	
	return results;
}

BatchGroupIdHandlerI::BatchGroupIdHandlerI( std::vector<int>& results, Ice::Int& maxId ) : _results(results), _maxId(maxId)
{
}

bool BatchGroupIdHandlerI::handle(mysqlpp::Row& row) const 
{
	int id = (int) row["group_id"];
	_maxId = id > _maxId ? id : _maxId;
	_results.push_back( id );
	return true;
}


BatchDoubleAccountIdInfoHandlerI::BatchDoubleAccountIdInfoHandlerI(MyUtil::ObjectResultPtr results) : _results(results)
{
}

bool BatchDoubleAccountIdInfoHandlerI::handle(mysqlpp::Row& row) const
{
	int groupId = (int)row["group_id"];
	int userId = (int)row["userid"];

	MyUtil::ObjectMap::const_iterator findIt = _results->data.find(groupId);
	if(findIt != _results->data.end())
	{
		DoubleAccountIdInfoPtr data = DoubleAccountIdInfoPtr::dynamicCast(findIt->second);
		data->userId1 = userId;
	}else
	{
		DoubleAccountIdInfoPtr data = new DoubleAccountIdInfo;
		data->userId0 = userId;
		_results->data.insert(make_pair(groupId, data));
	}

	return true;
}

BatchVirtualIdInfoHandlerI::BatchVirtualIdInfoHandlerI(MyUtil::ObjectResultPtr results) : _results(results)
{
}

bool BatchVirtualIdInfoHandlerI::handle(mysqlpp::Row& row) const
{
	int state = (int)row["state"];
	if(state != 0)
	{
		return true;
	}
	int virtualId = 0;
	try
	{
		virtualId = (int)row["virtual_id"];
	}catch(...)
	{
	}
	int userId = (int)row["user_id"];
	int transId = (int)row["page_id"];
	VirtualIdInfoPtr vd = new VirtualIdInfo;
	vd->virtualId = virtualId;
	vd->userId = userId;
	vd->transId = transId;

	UserId2RelatedIdInfoPtr data = UserId2RelatedIdInfoPtr::dynamicCast(_results->data.find(userId)->second);
	data->virtualIds.push_back(vd);
	return true;
}

}

