#include "IdCacheDataI.h"
#include "UserPassportAdapter.h"
#include "UserStateAdapter.h"

namespace idcache
{

//UserId2RelatedIdInfoI

UserId2RelatedIdInfoI::UserId2RelatedIdInfoI()
{
	doubleAccountId = 0;
}

void UserId2RelatedIdInfoI::parse(const Ice::ObjectPtr& data)
{
	UserId2RelatedIdInfoPtr newData = UserId2RelatedIdInfoPtr::dynamicCast(data);
	doubleAccountId = newData->doubleAccountId;
	virtualIds = newData->virtualIds;
}

UserId2RelatedIdInfoIPtr UserId2RelatedIdInfoI::getData()
{
	IceUtil::Mutex::Lock lock(_mutex);
	UserId2RelatedIdInfoIPtr newData = new UserId2RelatedIdInfoI;
	newData->doubleAccountId = doubleAccountId;
	newData->virtualIds = virtualIds;
	return newData;
}

void UserId2RelatedIdInfoI::setDoubleAccountId(int id)
{
	IceUtil::Mutex::Lock lock(_mutex);
	doubleAccountId = id;
}

void UserId2RelatedIdInfoI::addVirtualIdInfo(VirtualIdInfoIPtr vi)
{
	IceUtil::Mutex::Lock lock(_mutex);
	for(VirtualIdInfoSeq::iterator it = virtualIds.begin(); it != virtualIds.end(); ++it)
	{
		if((*it)->virtualId == vi->virtualId)
		{
			return;
		}
	}
	virtualIds.push_back(vi);
}

void UserId2RelatedIdInfoI::deleteVirtualId(int id)
{
	IceUtil::Mutex::Lock lock(_mutex);
	for(VirtualIdInfoSeq::iterator it = virtualIds.begin(); it != virtualIds.end(); ++it)
	{
		if((*it)->virtualId == id)
		{
			virtualIds.erase(it);
			break;
		}
	}
}

//VirtualIdInfoI

VirtualIdInfoIPtr VirtualIdInfoI::getData()
{
	IceUtil::Mutex::Lock lock(_mutex);
	VirtualIdInfoIPtr newData = new VirtualIdInfoI;
	newData->virtualId = virtualId;
	newData->userId = userId;
	newData->transId = transId;
	return newData;
}

int VirtualIdInfoI::getVirtualId()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return virtualId;
}

int VirtualIdInfoI::getUserId()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return userId;
}

int VirtualIdInfoI::getTransId()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return transId;
}

void VirtualIdInfoI::parse(mysqlpp::Row& row)
{
	virtualId = (int)row["virtual_id"];
	userId = (int)row["user_id"];
	transId = (int)row["page_id"];
}

void VirtualIdInfoI::parse(const Ice::ObjectPtr& data)
{
	VirtualIdInfoPtr newData = VirtualIdInfoPtr::dynamicCast(data);
	virtualId = newData->virtualId;
	userId = newData->userId;
	transId = newData->transId;
}

Ice::ObjectPtr UserId2RelatedIdInfoFactoryI::create(Ice::Int id)
{
	return load(id);
}

UserId2RelatedIdInfoIPtr UserId2RelatedIdInfoFactoryI::load(int id)
{
	UserId2RelatedIdInfoIPtr res = new UserId2RelatedIdInfoI;

	VirtualIdInfoSeq virtualIds;
	BatchVirtualIdResultHandlerI handler(virtualIds);
	com::xiaonei::xce::Statement sql;
	sql << "SELECT virtual_id, page_id, user_id, state FROM " << TABLE_VIRTUAL_ID << " WHERE user_id = '" << id << "'";
	com::xiaonei::xce::QueryRunner(DB_VIRTUAL_ID, com::xiaonei::xce::CDbRServer ).query(sql,handler);
	
	res->virtualIds = virtualIds;

	try
	{
		xce::adapter::userstate::UserStateInfoPtr us = xce::adapter::userstate::UserStateAdapter::instance().getUserState(id);
		if(!us)
		{
			return NULL;
		}
		if(!us->isDoubleAccount())
		{
			res->doubleAccountId = 0;
		}else
		{ 
			xce::adapter::userpassport::UserPassportInfoPtr up = xce::adapter::userpassport::UserPassportAdapter::instance().getUserPassport(id);
			if(!up)
			{
				return NULL;
			}
			int doubleAccountId = 0;
			UserIdResultHandlerI handler(id, doubleAccountId);
			com::xiaonei::xce::Statement sql;
			sql << "SELECT id FROM " << TABLE_USER_PASSPORT << " WHERE account = '" << up->account() << "'";
			com::xiaonei::xce::QueryRunner(DB_USER_PASSPORT, com::xiaonei::xce::CDbRServer ).query(sql,handler);
			res->doubleAccountId = doubleAccountId;
		}
	}catch(Ice::Exception& e)
	{
		MCE_WARN("UserId2RelatedIdInfoFactoryI::create getUserPassport id = " << id << " Exception " << e);
		return NULL;
	}

	if(res->doubleAccountId == 0 && res->virtualIds.empty())
	{
		return NULL;
	}

	return res;
}

Ice::ObjectPtr VirtualIdInfoFactoryI::create(Ice::Int id)
{
	return load(id);
}

VirtualIdInfoIPtr VirtualIdInfoFactoryI::load(int id)
{
	VirtualIdInfoIPtr res = new VirtualIdInfoI;
	res->virtualId = id;

	VirtualIdInfoResultHandlerI handler(res);
	com::xiaonei::xce::Statement sql;
	sql << "SELECT user_id, page_id, state FROM " << TABLE_VIRTUAL_ID << " WHERE virtual_id = '" << id << "'";
	if(com::xiaonei::xce::QueryRunner(DB_VIRTUAL_ID, com::xiaonei::xce::CDbRServer ).query(sql,handler))
	{
		if(res->userId > 0)
		{
			return res;
		}
	}
	return NULL;
}

VirtualIdInfoResultHandlerI::VirtualIdInfoResultHandlerI(VirtualIdInfoIPtr& result) : m_result(result)
{
}

bool VirtualIdInfoResultHandlerI::handle(mysqlpp::Row& row) const
{
	int state = (int)row["state"];
	if(state != 0)
	{
		m_result->userId = -1;
		m_result->transId = -1;
		return true;
	}
	int userId = (int)row["user_id"];
	int pageId = (int)row["page_id"];
	m_result->userId = userId;
	m_result->transId = pageId;
	return true;
}

BatchVirtualIdResultHandlerI::BatchVirtualIdResultHandlerI(VirtualIdInfoSeq& result) : m_result(result)
{
}

bool BatchVirtualIdResultHandlerI::handle(mysqlpp::Row& row) const
{
	int state = (int)row["state"];
	if(state != 0)
	{
		return true;
	}
	int virtualId = (int)row["virtual_id"];
	int userId = (int)row["user_id"];
	int pageId = (int)row["page_id"];

	VirtualIdInfoIPtr data = new VirtualIdInfoI;
	data->virtualId = virtualId;
	data->userId = userId;
	data->transId = pageId;

	m_result.push_back(data);
	return true;
}

UserIdResultHandlerI::UserIdResultHandlerI(const int id, int& doubleAccountId) : m_id(id), m_doubleAccountId(doubleAccountId)
{
}

bool UserIdResultHandlerI::handle(mysqlpp::Row& row) const
{
	int userId = (int)row["id"];
	if(userId != m_id)
	{
		m_doubleAccountId = userId;
	}
	return true;
}

}
