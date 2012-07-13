#include "VipInfoWriterI.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include "user/VipInfo/share/VipInfoDataI.cpp"
#include "util/cpp/InvokeMonitor.h"
#include "VipMemberDataReaderAdapter.h"
#include "Icon2UrlDataReaderAdapter.h"
#include "HatDataReaderAdapter.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

void MyUtil::initialize() {
	MyUtil::ServiceI& service = MyUtil::ServiceI::instance();

	service.getAdapter()->add(&xce::vipinfo::VipMemberDataWriterI::instance(), service.createIdentity("VM", ""));
	service.getAdapter()->add(&xce::vipinfo::Icon2UrlDataWriterI::instance(), service.createIdentity("IU", ""));
	service.getAdapter()->add(&xce::vipinfo::HatDataWriterI::instance(), service.createIdentity("IH", ""));

	//-----------------------------------HA---------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("VipInfoWriter.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("VipInfoWriter.Interval",5);
	
	map<string,Ice::ObjectPtr> mapNameAndService;
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("VM",&xce::vipinfo::VipMemberDataWriterI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("IU",&xce::vipinfo::Icon2UrlDataWriterI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("IH",&xce::vipinfo::HatDataWriterI::instance()));
	
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerVipInfoWriter",mapNameAndService,mod,interval);

	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("VipInfoWriter.Status",0);
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status,"VM");
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status,"IU");
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status,"IH");
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status);
		
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status;

		MCE_WARN( os.str() );
	}
	MyUtil::ServiceMonitorManager::instance().start();
}

namespace xce
{
namespace vipinfo
{

//VipMemberDataWriterI

VipMemberDataWriterI::VipMemberDataWriterI() : _valid(false)
{
	_tempMutexManager = new xce::tempmutex::TempMutexManager();
}

void VipMemberDataWriterI::createVipMemberData(const MyUtil::Str2StrMap& props, const Ice::Current & current)
{
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "", MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	MyUtil::Str2StrMap::const_iterator it = props.find("UID");
	int id;
	if (it == props.end())
	{
		MCE_WARN("VipMemberDataWriterI::createVipMemberData , Exception no id!");
		return;
	} else {
		id = boost::lexical_cast<int>(it->second);
	}

	it = props.find("TYPE");
	if (it == props.end())
	{
		MCE_WARN("VipMemberDataWriterI::createVipMemberData , Exception no type!");
		return;
	} 

	{
		xce::tempmutex::Lock lock(_tempMutexManager,id);
		
		com::xiaonei::xce::QueryRunner(DB_MEMBERS,com::xiaonei::xce::CDbWServer).execute(com::xiaonei::xce::Statement::buildInsert(TABLE_VIP_MEMBER, props));
		
		VipMemberMemDataIPtr result = new VipMemberMemDataI;
	
		com::xiaonei::xce::Statement sql;
		sql << "SELECT * FROM " << TABLE_VIP_MEMBER << " WHERE uid = '" << id << "'";
		VipMemberMemDataResultHandlerI handler(result);
		com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbWServer).query(sql, handler);
		if (result->data.empty()) 
		{
			MCE_WARN("VipMemberDataWriterI::createVipMemberData , Exception NoSuchVipMemberData id = " << id);
			return;
		} 
		
		xce::adapter::vipmemberdata::VipMemberDataReaderAdapter::instance().setData(id, result);
	}
	

}

void VipMemberDataWriterI::setVipMemberData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current & current)
{
	std::ostringstream inMsg;
	inMsg<<"id = "<<id;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	//去掉map中的用户id
	MyUtil::Str2StrMap::iterator it = const_cast<MyUtil::Str2StrMap&>(props).find("UID");
	if(it!=props.end())
	{
		const_cast<MyUtil::Str2StrMap&>(props).erase(it);
	}

	MyUtil::Str2StrMap::const_iterator tIt = props.find("TYPE");
	if(tIt == props.end())
	{
		MCE_WARN("VipMemberDataWriterI::setVipMemberData Exception no type in props for id " << boost::lexical_cast<std::string>(id));
		return;
	}

	MyUtil::Str2StrMap filter;
	filter["uid"] = boost::lexical_cast<string>(id);
	filter["type"] = tIt->second;
	{
		xce::tempmutex::Lock lock(_tempMutexManager,id);
		com::xiaonei::xce::QueryRunner(DB_MEMBERS,com::xiaonei::xce::CDbWServer).execute(com::xiaonei::xce::Statement::buildUpdate(TABLE_VIP_MEMBER, filter, props));
		xce::adapter::vipmemberdata::VipMemberDataReaderAdapter::instance().setVipMemberData(id,props);
	}
}

void VipMemberDataWriterI::reload(const MyUtil::IntSeq& ids, const Ice::Current & current) {
	
	ostringstream inMsg;
	inMsg<<"id size = " << ids.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_VIP_MEMBER << " WHERE uid IN ( ";
	for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it)
	{
		if(it != ids.begin())
		{
			sql << ",";
		}
		sql << *it;
	}
	sql << " )";
	
	BatchVipMemberMemDataResultHandlerI handler(result);

	{
		com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbRServer).query(sql, handler);
		xce::adapter::vipmemberdata::VipMemberDataReaderAdapter::instance().setData(result);
	}
}

void VipMemberDataWriterI::setValid(bool valid, const Ice::Current & current)
{
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, boost::lexical_cast<string>(valid), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	xce::serverstate::ServerStateSubscriber::instance().setStatus(boost::lexical_cast<int>(valid),"VM");

	{
		IceUtil::Mutex::Lock lock(_mutex);
		_valid = valid;
	}

	if( Icon2UrlDataWriterI::instance().isValid() == valid && HatDataWriterI::instance().isValid() == valid )
	{
		xce::serverstate::ServerStateSubscriber::instance().setStatus(boost::lexical_cast<int>(valid));
	}

}

bool VipMemberDataWriterI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _valid;
}

//Icon2UrlDataWriterI

Icon2UrlDataWriterI::Icon2UrlDataWriterI() : _valid(false)
{
	_tempMutexManager = new xce::tempmutex::TempMutexManager();
}

void Icon2UrlDataWriterI::createIcon2UrlData(const MyUtil::Str2StrMap& props, const Ice::Current & current)
{
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "", MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	MyUtil::Str2StrMap::const_iterator it = props.find("ID");
	int id;
	if (it == props.end())
	{
		MCE_WARN("Icon2UrlDataWriterI::createIcon2UrlData , Exception no id!");
		return;
	} else {
		id = boost::lexical_cast<int>(it->second);
	}

	{
		xce::tempmutex::Lock lock(_tempMutexManager,id);
		
		com::xiaonei::xce::QueryRunner(DB_MEMBERS,com::xiaonei::xce::CDbWServer).execute(com::xiaonei::xce::Statement::buildInsert(TABLE_ICON_URL, props));
		
		Icon2UrlDataIPtr result = new Icon2UrlDataI;
	
		com::xiaonei::xce::Statement sql;
		sql << "SELECT * FROM " << TABLE_ICON_URL << " WHERE id = '" << id << "'";
		Icon2UrlDataResultHandlerI handler(result);
		com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbWServer).query(sql, handler);
		if (id != result->iconId) 
		{
			MCE_WARN("Icon2UrlDataWriterI::createIcon2UrlData , Exception NoSuchIcon2UrlData id = " << id);
			return;
		} 
		
		xce::adapter::icon2urldata::Icon2UrlDataReaderAdapter::instance().setData(id, result);
	}
	

}

void Icon2UrlDataWriterI::setIcon2UrlData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current & current)
{
	std::ostringstream inMsg;
	inMsg<<"id = "<<id;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	//去掉map中的用户id
	MyUtil::Str2StrMap::iterator it = const_cast<MyUtil::Str2StrMap&>(props).find("ID");
	if(it!=props.end())
	{
		const_cast<MyUtil::Str2StrMap&>(props).erase(it);
	}

	MyUtil::Str2StrMap filter;
	filter["id"] = boost::lexical_cast<string>(id);
	{
		xce::tempmutex::Lock lock(_tempMutexManager,id);
		com::xiaonei::xce::QueryRunner(DB_MEMBERS,com::xiaonei::xce::CDbWServer).execute(com::xiaonei::xce::Statement::buildUpdate(TABLE_ICON_URL, filter, props));
		xce::adapter::icon2urldata::Icon2UrlDataReaderAdapter::instance().setIcon2UrlData(id,props);
	}
}

void Icon2UrlDataWriterI::reload(const MyUtil::IntSeq& ids, const Ice::Current & current) {
	
	ostringstream inMsg;
	inMsg<<"id size = " << ids.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_ICON_URL << " WHERE id IN ( ";
	for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it)
	{
		if(it != ids.begin())
		{
			sql << ",";
		}
		sql << *it;
	}
	sql << " )";
	
	BatchIcon2UrlDataResultHandlerI handler(result);

	{
		com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbRServer).query(sql, handler);
		xce::adapter::icon2urldata::Icon2UrlDataReaderAdapter::instance().setData(result);
	}
}

void Icon2UrlDataWriterI::setValid(bool valid, const Ice::Current & current)
{
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, boost::lexical_cast<string>(valid), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	xce::serverstate::ServerStateSubscriber::instance().setStatus(boost::lexical_cast<int>(valid),"IU");

	{
		IceUtil::Mutex::Lock lock(_mutex);
		_valid = valid;
	}

	if(VipMemberDataWriterI::instance().isValid() == valid && HatDataWriterI::instance().isValid() == valid)
	{
		xce::serverstate::ServerStateSubscriber::instance().setStatus(boost::lexical_cast<int>(valid));
	}

}

bool Icon2UrlDataWriterI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _valid;
}

//HatDataWriterI

HatDataWriterI::HatDataWriterI() : _valid(false)
{
	_tempMutexManager = new xce::tempmutex::TempMutexManager();
}

void HatDataWriterI::createHatData(const MyUtil::Str2StrMap& props, const Ice::Current & current)
{
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, "", MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	MyUtil::Str2StrMap::const_iterator it = props.find("ID");
	int id;
	if (it == props.end())
	{
		MCE_WARN("HatDataWriterI::createHatData , Exception no id!");
		return;
	} else {
		id = boost::lexical_cast<int>(it->second);
	}

	{
		xce::tempmutex::Lock lock(_tempMutexManager,id);

		com::xiaonei::xce::QueryRunner(DB_MEMBERS,com::xiaonei::xce::CDbWServer).execute(com::xiaonei::xce::Statement::buildInsert(TABLE_HAT, props));

		HatDataIPtr result = new HatDataI;

		com::xiaonei::xce::Statement sql;
		sql << "SELECT * FROM " << TABLE_HAT << " WHERE id = '" << id << "'";
		HatDataResultHandlerI handler(result);
		com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbWServer).query(sql, handler);
		if (id != result->hatId)
		{
			MCE_WARN("HatDataWriterI::createHatData , Exception NoSuchHatData id = " << id);
			return;
		}

		xce::adapter::hatdata::HatDataReaderAdapter::instance().setData(id, result);
	}


}

void HatDataWriterI::setHatData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current & current)
{
	std::ostringstream inMsg;
	inMsg<<"id = "<<id;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	//去掉map中的用户id
	MyUtil::Str2StrMap::iterator it = const_cast<MyUtil::Str2StrMap&>(props).find("ID");
	if(it!=props.end())
	{
		const_cast<MyUtil::Str2StrMap&>(props).erase(it);
	}

	MyUtil::Str2StrMap filter;
	filter["id"] = boost::lexical_cast<string>(id);
	{
		xce::tempmutex::Lock lock(_tempMutexManager,id);
		com::xiaonei::xce::QueryRunner(DB_MEMBERS,com::xiaonei::xce::CDbWServer).execute(com::xiaonei::xce::Statement::buildUpdate(TABLE_HAT, filter, props));
		xce::adapter::hatdata::HatDataReaderAdapter::instance().setHatData(id,props);
	}
}

void HatDataWriterI::reload(const MyUtil::IntSeq& ids, const Ice::Current & current) {

	ostringstream inMsg;
	inMsg<<"id size = " << ids.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	com::xiaonei::xce::Statement sql;
	sql << "SELECT * FROM " << TABLE_HAT << " WHERE id IN ( ";
	for(MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it)
	{
		if(it != ids.begin())
		{
			sql << ",";
		}
		sql << *it;
	}
	sql << " )";

	BatchHatDataResultHandlerI handler(result);

	{
		com::xiaonei::xce::QueryRunner(DB_MEMBERS, com::xiaonei::xce::CDbRServer).query(sql, handler);
		xce::adapter::hatdata::HatDataReaderAdapter::instance().setData(result);
	}
}

void HatDataWriterI::setValid(bool valid, const Ice::Current & current)
{
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, boost::lexical_cast<string>(valid), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	xce::serverstate::ServerStateSubscriber::instance().setStatus(boost::lexical_cast<int>(valid),"IH");

	{
		IceUtil::Mutex::Lock lock(_mutex);
		_valid = valid;
	}

	if(VipMemberDataWriterI::instance().isValid() == valid && Icon2UrlDataWriterI::instance().isValid() == valid)
	{
		xce::serverstate::ServerStateSubscriber::instance().setStatus(boost::lexical_cast<int>(valid));
	}

}

bool HatDataWriterI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _valid;
}

}
}
