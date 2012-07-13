#include "IdCacheReaderI.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include <util/cpp/InvokeMonitor.h>
#include "user/IdCache/share/IdCacheDataI.cpp"
#include "IdCacheWriterAdapter.h"

void MyUtil::initialize()
{
	MyUtil::ServiceI& service = MyUtil::ServiceI::instance();


	service.registerObjectCache(idcache::USERID_RELATED, "UR", new idcache::UserId2RelatedIdInfoFactoryI, false);
	service.registerObjectCache(idcache::VIRTUAL_ID, "VI", new idcache::VirtualIdInfoFactoryI, false);
	
	service.getAdapter()->add(&idcache::DoubleAccountId2RelatedIdSetterI::instance(), service.createIdentity(
			idcache::DOUBLEACCOUNTID2RELATEDID_SETTER, ""));
	service.getAdapter()->add(&idcache::VirtualId2RelatedIdSetterI::instance(), service.createIdentity(
			idcache::VIRTUALID2RELATEDID_SETTER, ""));
	service.getAdapter()->add(&idcache::IdCacheReaderI::instance(), service.createIdentity(
			idcache::ID_CACHE, ""));
	
//-----------------------------------HA---------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("IdCache.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("IdCache.Interval",5);
	
	map<string,Ice::ObjectPtr> mapNameAndService;
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(idcache::DOUBLEACCOUNTID2RELATEDID_SETTER,&idcache::DoubleAccountId2RelatedIdSetterI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(idcache::VIRTUALID2RELATEDID_SETTER,&idcache::VirtualId2RelatedIdSetterI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(idcache::ID_CACHE,&idcache::IdCacheReaderI::instance()));
	
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerIdCacheReader",mapNameAndService,mod,interval);

	idcache::IdFilterFillThread::instance().start();

	MyUtil::ServiceMonitorManager::instance().start();
    
	MyUtil::TaskManager::instance().config(idcache::TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 10));
}

namespace idcache
{

IdCacheReaderI::IdCacheReaderI() : _isValid(false)
{
}

IdRelationInfoPtr IdCacheReaderI::getIdRelationInfo(::Ice::Int id, const ::Ice::Current& current)
{
	std::ostringstream os;
	os<<id;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);

	int userId = id;

	if(IdFilter::instance().isMaybeVirtualId(id) && !IdFilter::instance().isFilterId(id))
	{
		VirtualIdInfoIPtr viInfo = locateVirtualIdInfo(id);
		if(!viInfo)
		{
			return NULL;
		}
		userId = viInfo->getUserId();
	}

	IdRelationInfoPtr res = new IdRelationInfo;
	res->directId = userId;

	UserId2RelatedIdInfoIPtr u2rInfo = locateUserId2RelatedIdInfo(userId)->getData();

	MyUtil::IntSeq ids;
	for(VirtualIdInfoSeq::const_iterator it = u2rInfo->virtualIds.begin(); it != u2rInfo->virtualIds.end(); ++it)
	{
		ids.push_back((*it)->virtualId);
	}
	res->relatedIds.insert(make_pair(userId, ids));

	if(u2rInfo->doubleAccountId != 0)
	{
		UserId2RelatedIdInfoPtr dU2rInfo = locateUserId2RelatedIdInfo(u2rInfo->doubleAccountId)->getData();
		
		MyUtil::IntSeq ids;
		for(VirtualIdInfoSeq::const_iterator it = dU2rInfo->virtualIds.begin(); it != dU2rInfo->virtualIds.end(); ++it)
		{
			ids.push_back((*it)->virtualId);
		}
		res->relatedIds.insert(make_pair(u2rInfo->doubleAccountId, ids));
	}
	return res;
}

int IdCacheReaderI::getRelatedUserId(::Ice::Int virtualId, const ::Ice::Current& current)
{
	std::ostringstream os;
	os<<virtualId;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);

	VirtualIdInfoIPtr data = locateVirtualIdInfo(virtualId);
	if(data)
	{
		return data->getUserId();
	}
	return -1;
}

int IdCacheReaderI::getRelatedTransId(::Ice::Int virtualId, const ::Ice::Current& current)
{
	std::ostringstream os;
	os<<virtualId;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);

	VirtualIdInfoIPtr data = locateVirtualIdInfo(virtualId);
	if(data)
	{
		return data->getTransId();
	}
	return -1;
}

void IdCacheReaderI::createIdRelation(::Ice::Int id, ::Ice::Int transId, ::Ice::Int virtualId, const ::Ice::Current& current)
{
	std::ostringstream os;
	os<<" id = "<<id<<" virtualId = " <<virtualId;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	VirtualIdInfoIPtr obj = new VirtualIdInfoI;
	obj->virtualId = virtualId;
	obj->userId = id;
	obj->transId = transId; 
	
	UserId2RelatedIdInfoIPtr u2rInfo = locateUserId2RelatedIdInfo(id);
	if(u2rInfo)
	{
		u2rInfo->addVirtualIdInfo(obj);
	}
	
	MyUtil::ServiceI::instance().getObjectCache()->addObject(VIRTUAL_ID, virtualId, obj);


}

void IdCacheReaderI::deleteIdRelation(::Ice::Int id, ::Ice::Int virtualId, const ::Ice::Current& current)
{
	std::ostringstream os;
	os<<" id = "<<id<<" virtualId = " <<virtualId;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	UserId2RelatedIdInfoIPtr u2rInfo = locateUserId2RelatedIdInfo(id);
	if(u2rInfo)
	{
		u2rInfo->deleteVirtualId(virtualId);
		UserId2RelatedIdInfoIPtr newData = u2rInfo->getData();
		if(newData->doubleAccountId == 0 && newData->virtualIds.empty())
		{
			MyUtil::ServiceI::instance().removeObject(USERID_RELATED, id);
		}
	}
	
	MyUtil::ServiceI::instance().removeObject(VIRTUAL_ID, virtualId);
}


UserId2RelatedIdInfoIPtr IdCacheReaderI::locateUserId2RelatedIdInfo(int id)
{
	try
	{
		if (isValid()) {
			UserId2RelatedIdInfoIPtr res = MyUtil::ServiceI::instance().findObject<UserId2RelatedIdInfoIPtr>(USERID_RELATED, id);
			if (res) {
				return res;
			} else {
				res = MyUtil::ServiceI::instance().locateObject<UserId2RelatedIdInfoIPtr>(USERID_RELATED, id);
				IdCacheWriterAdapter::instance().reload(id);
				MCE_WARN("IdCacheReaderI::locate NoSuchIdRelationInfoPtr id = " << id << "");
				return res;
			}
		} else {
			return MyUtil::ServiceI::instance().locateObject<UserId2RelatedIdInfoIPtr>(USERID_RELATED, id);
		}
	}catch(...)
	{
		MCE_WARN("IdCacheReaderI::locateUserId2RelatedIdInfo id = " << id << " Exception!");
		return NULL;
	}
}

VirtualIdInfoIPtr IdCacheReaderI::locateVirtualIdInfo(int id)
{
	try
	{
		if (isValid()) {
			VirtualIdInfoIPtr res = MyUtil::ServiceI::instance().findObject<VirtualIdInfoIPtr>(VIRTUAL_ID, id);
			if (res) {
				return res;
			} else {
				res = MyUtil::ServiceI::instance().locateObject<VirtualIdInfoIPtr>(VIRTUAL_ID, id);
				IdCacheWriterAdapter::instance().reload(id);
				MCE_WARN("IdCacheReaderI::locate NoSuchVirtualIdInfoPtr id = " << id << "");
				return res;
			}
		} else {
			return MyUtil::ServiceI::instance().locateObject<VirtualIdInfoIPtr>(VIRTUAL_ID, id);
		}
	}catch(...)
	{
		MCE_WARN("IdCacheReaderI::locateVirtualIdInfo id = " << id << " Exception!");
		return NULL;
	}
}

bool IdCacheReaderI::isVirtualId(::Ice::Int id, const ::Ice::Current& current)
{
	std::ostringstream os;
	os<<" id = "<<id;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);

	return IdFilter::instance().isMaybeVirtualId(id) && !(IdFilter::instance().isFilterId(id));
}

void IdCacheReaderI::setData(const MyUtil::ObjectResultPtr& data, const Ice::Current& current)
{
	std::ostringstream os;
	os<<" data size = "<<data->data.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);
	MyUtil::TaskManager::instance().execute(new FillTask(data));
}

void IdCacheReaderI::setValid(bool flag)
{
	if(flag == DoubleAccountId2RelatedIdSetterI::instance().isValid() && flag == VirtualId2RelatedIdSetterI::instance().isValid() && flag == IdFilter::instance().isValid())
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_isValid = flag;
		xce::serverstate::ServerStateSubscriber::instance().setStatus((int)flag,ID_CACHE);
	}
}

bool IdCacheReaderI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _isValid;
}


void DoubleAccountId2RelatedIdSetterI::setData(const MyUtil::ObjectResultPtr& data, const Ice::Current& current)
{
	std::ostringstream os;
	os<<" data size = "<<data->data.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);

	IdCacheReaderI::instance().setData(data, current);
}

void DoubleAccountId2RelatedIdSetterI::setValid(bool flag, const ::Ice::Current& current)
{
	std::ostringstream os;
	os<<" flag = "<<flag;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_isValid = flag;
	}
	xce::serverstate::ServerStateSubscriber::instance().setStatus((int)flag,DOUBLEACCOUNTID2RELATEDID_SETTER);
	IdCacheReaderI::instance().setValid(flag);
}

bool DoubleAccountId2RelatedIdSetterI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _isValid;
}

void VirtualId2RelatedIdSetterI::setData(const MyUtil::ObjectResultPtr& data, const Ice::Current& current)
{
	std::ostringstream os;
	os<<" data size = "<<data->data.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);
	
	IdCacheReaderI::instance().setData(data, current);
}

bool VirtualId2RelatedIdSetterI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _isValid;
}

void VirtualId2RelatedIdSetterI::setValid(bool flag, const ::Ice::Current& current)
{
	std::ostringstream os;
	os<<" flag = "<<flag;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, os.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_isValid = flag;
	}
	xce::serverstate::ServerStateSubscriber::instance().setStatus((int)flag,VIRTUALID2RELATEDID_SETTER);
	IdCacheReaderI::instance().setValid(flag);
}

IdFilter::IdFilter() : _isValid(false)
{
}

void IdFilter::setValid(bool flag)
{
	MCE_INFO("IdFilter::setValid flag = " << flag);
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_isValid = flag;
	}
	IdCacheReaderI::instance().setValid(flag);
}

bool IdFilter::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _isValid;
}

bool IdFilter::isFilterId(int id)
{
	IceUtil::Mutex::Lock lock(_mutex);
	if(_isValid)
	{
		return _idSet.find(id) != _idSet.end();
	}else
	{
		MyUtil::IntSeq ids;
		int maxId;

		com::xiaonei::xce::Statement sql;
		sql << "SELECT id from user_passport WHERE id = " << id;
		BatchUserIdHandler handle(ids, maxId);
		
		com::xiaonei::xce::QueryRunner( DB_USER_PASSPORT ).query(sql, handle);

		return !(ids.empty());
	}
}

bool IdFilter::isMaybeVirtualId(int id)
{
	return id > MIN_FILTER_ID && id < MAX_FILTER_ID;
}

void IdFilter::add(MyUtil::IntSeq ids)
{
	IceUtil::Mutex::Lock lock(_mutex);
	_idSet.insert(ids.begin(), ids.end());
}

void IdFilterFillThread::run()
{
	int beginId = MIN_FILTER_ID;
	int maxId = beginId;
	
	while(true)
	{
		MyUtil::IntSeq ids;

		com::xiaonei::xce::Statement sql;
		sql << "SELECT id from user_passport WHERE id >= " << beginId
			<< " ORDER BY id LIMIT " << 1000;
		BatchUserIdHandler handle(ids, maxId);
		try
		{
			com::xiaonei::xce::QueryRunner( DB_USER_PASSPORT ).query(sql, handle);
		}catch(...)
		{
			maxId = beginId;
			continue;
		}
		{
			IdFilter::instance().add(ids);
		}

		if(maxId == beginId | maxId > MAX_FILTER_ID)
		{
			break;
		}

		beginId = maxId;
	}
	IdFilter::instance().setValid(true);
}

BatchUserIdHandler::BatchUserIdHandler(MyUtil::IntSeq& ids, int& maxId) : _ids(ids), _maxId(maxId)
{
}

bool BatchUserIdHandler::handle(mysqlpp::Row& row) const
{
	int id = (int)row["id"];
	_maxId = id > _maxId ? id : _maxId;
	_ids.push_back(id);
	return true;
}

FillTask::FillTask(const MyUtil::ObjectResultPtr& userData) : _userData(userData)
{
}

void FillTask::handle()
{
	for(MyUtil::ObjectMap::const_iterator it = _userData->data.begin(); it != _userData->data.end(); ++it)
	{
		UserId2RelatedIdInfoIPtr obj = new UserId2RelatedIdInfoI;
		obj->parse(it->second);
		MyUtil::ServiceI::instance().getObjectCache()->addObject(USERID_RELATED, it->first, obj);
		for(VirtualIdInfoSeq::const_iterator iIt = obj->virtualIds.begin(); iIt != obj->virtualIds.end(); ++iIt)
		{
			VirtualIdInfoIPtr newData = new VirtualIdInfoI;
			newData->parse(*iIt);
			MyUtil::ServiceI::instance().getObjectCache()->addObject(VIRTUAL_ID, (*iIt)->virtualId, newData);
		}
	}
}

}
