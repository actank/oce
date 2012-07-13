#include "VirtualId2RelatedIdPreloader.h"
#include <user/IdCache/share/IdCacheDataI.cpp>
#include "GeneralPreloader/src/GeneralPreloader.h"

void MyUtil::initialize() {
	MyUtil::ServiceI& service = MyUtil::ServiceI::instance();
	service.getAdapter()->add(&idcache::VirtualId2RelatedIdPreloader::instance(), service.createIdentity("M", ""));
	std::string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("IdCache.Controller", "ControllerIdCacheReader");
	int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
	int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("IdCache.Proxy.Timeout", 300);

	int cluster = 10;
	xce::generalpreloader::GeneralPreloader<idcache::VirtualId2RelatedIdSetterPrx>::instance().initialize(
                new idcache::BatchUserIdProducer,
                new idcache::VirtualId2RelatedIdPreloaderFactory,
                ctlEndpoint, 
				cluster,
				subInterval,
                timeout,
				idcache::PRX_DATAQUEUE_MAX_SIZE ,
                idcache::PRODUCE_BATCH_SIZE ,
                idcache::CONSUME_BATCH_SIZE ,
				idcache::WRITE_BATCH_SIZE ,
                idcache::CONSUME_BATCH_THREAD_SIZE,
				"VS",true);
}

namespace idcache
{

MyUtil::ObjectResultPtr VirtualId2RelatedIdPreloaderFactory::create(Ice::Int minId,Ice::Int maxId) 
{
	MCE_INFO("VirtualId2RelatedIdPreloaderFactory::create group -> id from: "<<minId <<" to: "<<maxId);
	
	return NULL;
}

MyUtil::ObjectResultPtr VirtualId2RelatedIdPreloaderFactory::create(const MyUtil::IntSeq& ids) 
{
	MCE_INFO("DoubleAccountId2RelatedIdPreloaderFactory::create group -> id size: "<<ids.size());
    
	MyUtil::Int2IntMap idsMap;
	for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it)
	{
		idsMap.insert(make_pair(*it, *it));
	}
	
	BatchFilterDoubleAccountIdInfoHandlerI bdHandler(idsMap);

	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_MULTI_IDS << " WHERE userid IN (  ";
	for (MyUtil::Int2IntMap::const_iterator it = idsMap.begin(); it != idsMap.end(); ++it) {
		if (it != idsMap.begin()) {
			sql << ",";
		}
		sql << it->first;
	}
	sql << ")";
	
	com::xiaonei::xce::QueryRunner(DB_MULTI_IDS, com::xiaonei::xce::CDbRServer ).query(sql, bdHandler);

	MyUtil::ObjectResultPtr finalRes = new MyUtil::ObjectResult;

	BatchVirtualIdInfoHandlerI bvHandler(finalRes);

	com::xiaonei::xce::Statement sql1;
	sql1 << "SELECT virtual_id, page_id, user_id, state FROM " << TABLE_VIRTUAL_ID << " WHERE user_id IN ( ";
	for (MyUtil::Int2IntMap::const_iterator it = idsMap.begin(); it != idsMap.end(); ++it) {
		if (it != idsMap.begin()) {
			sql1 << ",";
		}
		sql1 << it->first;
	}
	sql1 << ")";
	
	com::xiaonei::xce::QueryRunner(DB_VIRTUAL_ID_PRELOADER, com::xiaonei::xce::CDbRServer ).query(sql1, bvHandler);

	return finalRes;
}

std::vector<int> BatchUserIdProducer::produce(const int beginId, const int load_batch_size, int& maxId ,int& tableMod)
{
	std::vector<int> results;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT user_id, state FROM " << TABLE_VIRTUAL_ID << " WHERE user_id > " << beginId
		<< " ORDER BY user_id LIMIT " << load_batch_size;
	BatchUserIdHandlerI handler( results, maxId );
	com::xiaonei::xce::QueryRunner( DB_VIRTUAL_ID_PRELOADER, com::xiaonei::xce::CDbRServer ).query( sql, handler );
	
	return results;
}

BatchUserIdHandlerI::BatchUserIdHandlerI( std::vector<int>& results, Ice::Int& maxId ) : _results(results), _maxId(maxId)
{
}

bool BatchUserIdHandlerI::handle(mysqlpp::Row& row) const 
{
	int id = (int)row["user_id"];
	int state = (int)row["state"];
	_maxId = id > _maxId ? id : _maxId;
	if(state != 0)
	{
		return true;
	}
	_results.push_back( id );
	return true;
}


BatchFilterDoubleAccountIdInfoHandlerI::BatchFilterDoubleAccountIdInfoHandlerI(MyUtil::Int2IntMap& results) : _results(results)
{
}

bool BatchFilterDoubleAccountIdInfoHandlerI::handle(mysqlpp::Row& row) const
{
	int userId = (int)row["userid"];

	_results.erase(userId);

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
		return true;
	}

	int userId = (int)row["user_id"];
	int transId = (int)row["page_id"];
	
	VirtualIdInfoPtr vd = new VirtualIdInfo;
	vd->virtualId = virtualId;
	vd->userId = userId;
	vd->transId = transId;

	MyUtil::ObjectMap::const_iterator findIt = _results->data.find(userId);
	if(findIt != _results->data.end())
	{
		UserId2RelatedIdInfoPtr data = UserId2RelatedIdInfoPtr::dynamicCast(findIt->second);
		data->virtualIds.push_back(vd);
	}else
	{
		UserId2RelatedIdInfoPtr data = new UserId2RelatedIdInfo;
		data->doubleAccountId = 0;
		data->virtualIds.push_back(vd);
		_results->data.insert(make_pair(userId, data));
	}
	return true;
}

}

