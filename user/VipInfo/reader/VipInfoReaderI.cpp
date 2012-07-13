#include "VipInfoReaderI.h"
#include <Monitor/client/service/ServiceMonitor.h>
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include <util/cpp/InvokeMonitor.h>

#include "user/VipInfo/share/VipInfoDataI.cpp"
#include "VipMemberDataWriterAdapter.h"
#include "Icon2UrlDataWriterAdapter.h"
#include "HatDataWriterAdapter.h"

void MyUtil::initialize() {
	MyUtil::ServiceI& service = MyUtil::ServiceI::instance();
	
	service.registerObjectCache(xce::vipinfo::CACHE_VIP_MEMBER, "VM", new xce::vipinfo::VipMemberMemDataFactoryI, false);
	service.registerObjectCache(xce::vipinfo::CACHE_ICON2URL, "IU", new xce::vipinfo::Icon2UrlDataFactoryI, false);
	service.registerObjectCache(xce::vipinfo::CACHE_HAT, "IH", new xce::vipinfo::HatDataFactoryI, false);

	service.getAdapter()->add(&xce::vipinfo::VipMemberDataReaderI::instance(), service.createIdentity("VM", ""));
	service.getAdapter()->add(&xce::vipinfo::Icon2UrlDataReaderI::instance(), service.createIdentity("IU", ""));
	service.getAdapter()->add(&xce::vipinfo::HatDataReaderI::instance(), service.createIdentity("IH", ""));
	service.getAdapter()->add(&xce::vipinfo::VipIconDataReaderI::instance(), service.createIdentity("VI", ""));
	service.getAdapter()->add(&xce::vipinfo::VipFullDataReaderI::instance(), service.createIdentity("VF", ""));

	//用来setdata的task 	
	MyUtil::TaskManager::instance().config(xce::vipinfo::TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 10));

	//-----------------------------------HA---------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("VipInfoReader.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("VipInfoReader.Interval",5);
	
	map<string,Ice::ObjectPtr> mapNameAndService;
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("VM",&xce::vipinfo::VipMemberDataReaderI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("IU",&xce::vipinfo::Icon2UrlDataReaderI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("IH",&xce::vipinfo::HatDataReaderI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("VI",&xce::vipinfo::VipIconDataReaderI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("VF",&xce::vipinfo::VipFullDataReaderI::instance()));
	
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerVipInfoReader",mapNameAndService,mod,interval);

	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("VipInfoReader.Status",0);
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status,"VM");
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status,"IU");
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status,"IH");
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status,"VI");
		xce::serverstate::ServerStateSubscriber::instance().setStatus(status,"VF");
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
//IdsFilter

void IdsFilter::filter(const MyUtil::LongSeq& ids, const MyUtil::ObjectResultPtr& data, MyUtil::LongSeq& filterIds)
{
	for(MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it)
	{
		if(data->data.find(*it) == data->data.end())
		{
			filterIds.push_back(*it);
		}
	}
}

//VipMemberDataReaderI

VipMemberDataReaderI::VipMemberDataReaderI() : _valid(false)
{
}

bool VipMemberDataReaderI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _valid;
}

VipMemberMemDataIPtr VipMemberDataReaderI::locate(int id)
{
	VipMemberMemDataIPtr dataI = MyUtil::ServiceI::instance().findObject<VipMemberMemDataIPtr> (CACHE_VIP_MEMBER, id);
	if(!dataI)
	{
		dataI = MyUtil::ServiceI::instance().locateObject<VipMemberMemDataIPtr> (CACHE_VIP_MEMBER, id);
		if(isValid())
		{
			xce::adapter::vipmemberdata::VipMemberDataWriterAdapter::instance().reload(id); 
		}
	}
	return dataI;
}

std::map<int, VipMemberDataSeq> VipMemberDataReaderI::locate(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types)
{
	MyUtil::LongSeq lIds(ids.begin(), ids.end());

	MyUtil::ObjectResultPtr sucRes = MyUtil::ServiceI::instance().getObjectCache()->findObjects(CACHE_VIP_MEMBER, lIds);
	
	if(sucRes->data.size() != lIds.size())
	{
		MyUtil::LongSeq failIds;
		IdsFilter::instance().filter(lIds, sucRes, failIds);
		if(isValid())
		{
			xce::adapter::vipmemberdata::VipMemberDataWriterAdapter::instance().reload(failIds); 
		}
	}

	std::map<int, VipMemberDataSeq> datas;
	for(MyUtil::ObjectMap::const_iterator it = sucRes->data.begin(); it != sucRes->data.end(); ++it)
	{
		datas.insert(make_pair(it->first, VipMemberMemDataIPtr::dynamicCast(it->second)->getVipMemberDataSeq(types)));
	}
	return datas;
}

std::map<int, VipMemberDataNSeq> VipMemberDataReaderI::locateN(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types)
{
	MyUtil::LongSeq lIds(ids.begin(), ids.end());

	MyUtil::ObjectResultPtr sucRes = MyUtil::ServiceI::instance().getObjectCache()->findObjects(CACHE_VIP_MEMBER, lIds);

	if(sucRes->data.size() != lIds.size())
	{
		MyUtil::LongSeq failIds;
		IdsFilter::instance().filter(lIds, sucRes, failIds);
		if(isValid())
		{
			xce::adapter::vipmemberdata::VipMemberDataWriterAdapter::instance().reload(failIds);
		}
	}

	std::map<int, VipMemberDataNSeq> datas;
	for(MyUtil::ObjectMap::const_iterator it = sucRes->data.begin(); it != sucRes->data.end(); ++it)
	{
		datas.insert(make_pair(it->first, VipMemberMemDataIPtr::dynamicCast(it->second)->getVipMemberDataNSeq(types)));
	}
	return datas;
}

VipMemberDataSeqMap VipMemberDataReaderI::getVipMemberDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg << "id size "<< ids.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);

  if(ids.empty())
  {
      return VipMemberDataSeqMap();
  }
	
  return locate(ids, types);
}

VipMemberDataNSeqMap VipMemberDataReaderI::getVipMemberDataNSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg << "id size "<< ids.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);

  if(ids.empty())
  {
      return VipMemberDataNSeqMap();
  }

  return locateN(ids, types);
}

void VipMemberDataReaderI::setVipMemberData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current & current)
{
	ostringstream inMsg;
	inMsg<<"id = "<< id;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	
	VipMemberMemDataIPtr data = locate(id);
	if(data)
	{
		data->setProps(props);
	}
}

void VipMemberDataReaderI::setValid(bool valid, const Ice::Current& current) 
{
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, boost::lexical_cast<string>(valid), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid,"VM");
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_valid = valid;
	}
	if(Icon2UrlDataReaderI::instance().isValid() == valid && HatDataReaderI::instance().isValid() == valid)
	{
		xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid, "VI");
		xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid, "VF");
		xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

void VipMemberDataReaderI::setData(const MyUtil::ObjectResultPtr& userData, const Ice::Current& current) 
{
	ostringstream inMsg;
	inMsg << "data size " << userData->data.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	MyUtil::TaskManager::instance().execute(new FillTask<VipMemberMemDataIPtr, VipMemberMemDataI>(CACHE_VIP_MEMBER, userData));
}

//Icon2UrlDataReaderI

Icon2UrlDataReaderI::Icon2UrlDataReaderI() : _valid(false)
{
}

bool Icon2UrlDataReaderI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _valid;
}

Icon2UrlDataIPtr Icon2UrlDataReaderI::locate(int id)
{
	Icon2UrlDataIPtr dataI = MyUtil::ServiceI::instance().findObject<Icon2UrlDataIPtr> (CACHE_ICON2URL, id);
	if(!dataI)
	{
		dataI = MyUtil::ServiceI::instance().locateObject<Icon2UrlDataIPtr> (CACHE_ICON2URL, id);
		if(isValid())
		{
			xce::adapter::icon2urldata::Icon2UrlDataWriterAdapter::instance().reload(id); 
		}
	}
	return dataI;
}

MyUtil::Int2StrMap Icon2UrlDataReaderI::locate(const MyUtil::IntSeq& ids)
{
	MyUtil::LongSeq lIds(ids.begin(), ids.end());

	MyUtil::ObjectResultPtr sucRes = MyUtil::ServiceI::instance().getObjectCache()->findObjects(CACHE_ICON2URL, lIds);
	
	if(sucRes->data.size() != lIds.size())
	{
		MyUtil::LongSeq failIds;
		IdsFilter::instance().filter(lIds, sucRes, failIds);
		MyUtil::ObjectResultPtr failRes = MyUtil::ServiceI::instance().getObjectCache()->locateObjects(CACHE_ICON2URL, failIds);
		for(MyUtil::ObjectMap::const_iterator it = failRes->data.begin(); it != failRes->data.end(); ++it)
		{
			sucRes->data.insert(make_pair(it->first, it->second));
		}
		if(isValid())
		{
			xce::adapter::icon2urldata::Icon2UrlDataWriterAdapter::instance().reload(failIds); 
		}
	}

	MyUtil::Int2StrMap datas;
	for(MyUtil::ObjectMap::const_iterator it = sucRes->data.begin(); it != sucRes->data.end(); ++it)
	{
		datas.insert(make_pair(it->first, Icon2UrlDataIPtr::dynamicCast(it->second)->getData()->hideUrl));
	}
	return datas;
}

MyUtil::Int2StrMap Icon2UrlDataReaderI::getIcon2UrlDataMap(const MyUtil::IntSeq& ids, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg << "id size "<< ids.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);
  
  if(ids.empty())
  {
      return MyUtil::Int2StrMap();
  }

	return locate(ids);
}

void Icon2UrlDataReaderI::setIcon2UrlData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current & current)
{
	ostringstream inMsg;
	inMsg<<"id = "<< id;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	
	Icon2UrlDataIPtr data = locate(id);
	if(data)
	{
		data->setProps(props);
	}
}

void Icon2UrlDataReaderI::setValid(bool valid, const Ice::Current& current) 
{
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, boost::lexical_cast<string>(valid), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid,"IU");
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_valid = valid;
	}
	if(VipMemberDataReaderI::instance().isValid() == valid && HatDataReaderI::instance().isValid() == valid)
	{
		xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid, "VI");
		xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid, "VF");
		xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

void Icon2UrlDataReaderI::setData(const MyUtil::ObjectResultPtr& userData, const Ice::Current& current) 
{
	ostringstream inMsg;
	inMsg << "data size " << userData->data.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	MyUtil::TaskManager::instance().execute(new FillTask<Icon2UrlDataIPtr, Icon2UrlDataI>(CACHE_ICON2URL, userData));
}

//HatDataReaderI

HatDataReaderI::HatDataReaderI() : _valid(false)
{
}

bool HatDataReaderI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _valid;
}

HatDataIPtr HatDataReaderI::locate(int id)
{
	HatDataIPtr dataI = MyUtil::ServiceI::instance().findObject<HatDataIPtr> (CACHE_HAT, id);
	if(!dataI)
	{
		dataI = MyUtil::ServiceI::instance().locateObject<HatDataIPtr> (CACHE_HAT, id);
		if(isValid())
		{
			xce::adapter::hatdata::HatDataWriterAdapter::instance().reload(id);
		}
	}
	return dataI;
}

HatDataMap HatDataReaderI::locate(const MyUtil::IntSeq& ids)
{
	MyUtil::LongSeq lIds(ids.begin(), ids.end());

	MyUtil::ObjectResultPtr sucRes = MyUtil::ServiceI::instance().getObjectCache()->findObjects(CACHE_HAT, lIds);

	if(sucRes->data.size() != lIds.size())
	{
		MyUtil::LongSeq failIds;
		IdsFilter::instance().filter(lIds, sucRes, failIds);
		MyUtil::ObjectResultPtr failRes = MyUtil::ServiceI::instance().getObjectCache()->locateObjects(CACHE_HAT, failIds);
		for(MyUtil::ObjectMap::const_iterator it = failRes->data.begin(); it != failRes->data.end(); ++it)
		{
			sucRes->data.insert(make_pair(it->first, it->second));
		}
		if(isValid())
		{
			xce::adapter::hatdata::HatDataWriterAdapter::instance().reload(failIds);
		}
	}

	HatDataMap datas;
	for(MyUtil::ObjectMap::const_iterator it = sucRes->data.begin(); it != sucRes->data.end(); ++it)
	{
		datas.insert(make_pair(it->first, HatDataIPtr::dynamicCast(it->second)->getData()));
	}
	return datas;
}

HatDataMap HatDataReaderI::getHatDataMap(const MyUtil::IntSeq& ids, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg << "id size "<< ids.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);

  if(ids.empty())
  {
      return HatDataMap();
  }

	return locate(ids);
}

void HatDataReaderI::setHatData(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current & current)
{
	ostringstream inMsg;
	inMsg<<"id = "<< id;
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);

	HatDataIPtr data = locate(id);
	if(data)
	{
		data->setProps(props);
	}
}

void HatDataReaderI::setValid(bool valid, const Ice::Current& current)
{
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, boost::lexical_cast<string>(valid), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid,"IH");
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_valid = valid;
	}
	if(VipMemberDataReaderI::instance().isValid() == valid && Icon2UrlDataReaderI::instance().isValid() == valid)
	{
		xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid, "VI");
		xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid, "VF");
		xce::serverstate::ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

void HatDataReaderI::setData(const MyUtil::ObjectResultPtr& userData, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg << "data size " << userData->data.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_INFO);
	MyUtil::TaskManager::instance().execute(new FillTask<HatDataIPtr, HatDataI>(CACHE_HAT, userData));
}

//VipIconDataReaderI

VipIconDataSeqMap VipIconDataReaderI::getVipIconDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg << "id size " << ids.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);
  
  if(ids.empty())
  {
      return VipIconDataSeqMap();
  }
	
	std::map<int, VipMemberDataSeq> memberDatas = VipMemberDataReaderI::instance().locate(ids, types);
	std::set<int> idsSet;
	for(std::map<int, VipMemberDataSeq>::const_iterator it = memberDatas.begin(); it != memberDatas.end(); ++it)
	{
		for(VipMemberDataSeq::const_iterator vIt = it->second.begin(); vIt != it->second.end(); ++vIt)
		{
			idsSet.insert((*vIt)->iconId);
		}
	}
	MyUtil::IntSeq iconIds(idsSet.begin(), idsSet.end());
	std::map<int, std::string> icon2urlDatas = Icon2UrlDataReaderI::instance().locate(iconIds);

	VipIconDataSeqMap res;
	for(std::map<int, VipMemberDataSeq>::const_iterator it = memberDatas.begin(); it != memberDatas.end(); ++it)
	{
		VipIconDataSeq dataSeq;
		for(VipMemberDataSeq::const_iterator vIt = it->second.begin(); vIt != it->second.end(); ++vIt)
		{
			std::map<int, std::string>::const_iterator findIcon2UrlIt = icon2urlDatas.find((*vIt)->iconId);
			if(findIcon2UrlIt != icon2urlDatas.end())
			{
				dataSeq.push_back(new VipIconDataI(*vIt, findIcon2UrlIt->second));
			}
		}
		if(dataSeq.empty())
		{
			continue;
		}
		res.insert(make_pair(it->first, dataSeq));
	}
	return res;
}

VipFeedDataSeqMap VipIconDataReaderI::getVipFeedDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg << "id size " << ids.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);

	if (ids.empty())
	{
		return VipFeedDataSeqMap();
	}

	std::map<int, VipMemberDataNSeq> memberDatas = VipMemberDataReaderI::instance().locateN(ids, types);
	std::set<int> iconIdsSet;
	std::set<int> hatIdsSet;
	for(std::map<int, VipMemberDataNSeq>::const_iterator it = memberDatas.begin(); it != memberDatas.end(); ++it)
	{
		for(VipMemberDataNSeq::const_iterator vIt = it->second.begin(); vIt != it->second.end(); ++vIt)
		{
			iconIdsSet.insert((*vIt)->iconId);
			hatIdsSet.insert((*vIt)->hatId);
		}
	}
	MyUtil::IntSeq iconIds(iconIdsSet.begin(), iconIdsSet.end());
	MyUtil::IntSeq hatIds(hatIdsSet.begin(), hatIdsSet.end());
	std::map<int, std::string> icon2urlDataString = Icon2UrlDataReaderI::instance().locate(iconIds);
	Icon2UrlDataMap icon2UrlDataMap;
	for(std::map<int, std::string>::const_iterator it = icon2urlDataString.begin(); it != icon2urlDataString.end(); ++it){
		Icon2UrlDataIPtr data = new Icon2UrlDataI;
		data->iconId = it->first;
		data->hideUrl = it->second;	
		icon2UrlDataMap.insert(make_pair(it->first, data));
	}
	HatDataMap hatDataMap = HatDataReaderI::instance().locate(hatIds);

	VipFeedDataSeqMap res;
	for(std::map<int, VipMemberDataNSeq>::const_iterator it = memberDatas.begin(); it != memberDatas.end(); ++it)
	{
		VipFeedDataSeq dataSeq;
		for(VipMemberDataNSeq::const_iterator vIt = it->second.begin(); vIt != it->second.end(); ++vIt)
		{
			Icon2UrlDataMap::const_iterator findIcon2UrlIt = icon2UrlDataMap.find((*vIt)->iconId);
			HatDataMap::const_iterator findHatIt = hatDataMap.find((*vIt)->hatId);
			if(findIcon2UrlIt != icon2UrlDataMap.end() && findHatIt != hatDataMap.end())
			{
				dataSeq.push_back(new VipFeedDataI(*vIt, findIcon2UrlIt->second, findHatIt->second));
			}
		}
		if(dataSeq.empty())
		{
			continue;
		}
		res.insert(make_pair(it->first, dataSeq));
	}
	return res;
}

//VipFullDataReaderI

VipFullDataSeqMap VipFullDataReaderI::getVipFullDataSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg << "id size " << ids.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);
  
  if(ids.empty())
  {
      return VipFullDataSeqMap();
  }
	
	std::map<int, VipMemberDataSeq> memberDatas = VipMemberDataReaderI::instance().locate(ids, types);
	std::set<int> idsSet;
	for(std::map<int, VipMemberDataSeq>::const_iterator it = memberDatas.begin(); it != memberDatas.end(); ++it)
	{
		for(VipMemberDataSeq::const_iterator vIt = it->second.begin(); vIt != it->second.end(); ++vIt)
		{
			idsSet.insert((*vIt)->iconId);
		}
	}
	MyUtil::IntSeq iconIds(idsSet.begin(), idsSet.end());
	std::map<int, std::string> icon2urlDatas = Icon2UrlDataReaderI::instance().locate(iconIds);

	VipFullDataSeqMap res;
	for(std::map<int, VipMemberDataSeq>::const_iterator it = memberDatas.begin(); it != memberDatas.end(); ++it)
	{
		VipFullDataSeq dataSeq;
		for(VipMemberDataSeq::const_iterator vIt = it->second.begin(); vIt != it->second.end(); ++vIt)
		{
			std::map<int, std::string>::const_iterator findIcon2UrlIt = icon2urlDatas.find((*vIt)->iconId);
			if(findIcon2UrlIt != icon2urlDatas.end())
			{
				dataSeq.push_back(new VipFullDataI(*vIt, findIcon2UrlIt->second));
			}
		}
		if(dataSeq.empty())
		{
			continue;
		}
		res.insert(make_pair(it->first, dataSeq));
	}
	return res;
}

VipFullDataNSeqMap VipFullDataReaderI::getVipFullDataNSeqMap(const MyUtil::IntSeq& ids, const MyUtil::IntSeq& types, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg << "id size " << ids.size();
	MyUtil::InvokeClient ic = MyUtil::InvokeClient::create(current, inMsg.str(), MyUtil::InvokeClient::INVOKE_LEVEL_DEBUG);

  if(ids.empty())
  {
      return VipFullDataNSeqMap();
  }

	std::map<int, VipMemberDataNSeq> memberDatas = VipMemberDataReaderI::instance().locateN(ids, types);
	std::set<int> iconIdsSet;
	std::set<int> hatIdsSet;
	for(std::map<int, VipMemberDataNSeq>::const_iterator it = memberDatas.begin(); it != memberDatas.end(); ++it)
	{
		for(VipMemberDataNSeq::const_iterator vIt = it->second.begin(); vIt != it->second.end(); ++vIt)
		{
			iconIdsSet.insert((*vIt)->iconId);
			hatIdsSet.insert((*vIt)->hatId);
		}
	}
	MyUtil::IntSeq iconIds(iconIdsSet.begin(), iconIdsSet.end());
	MyUtil::IntSeq hatIds(hatIdsSet.begin(), hatIdsSet.end());
	std::map<int, std::string> icon2urlDatas = Icon2UrlDataReaderI::instance().locate(iconIds);
	HatDataMap hatDatas = HatDataReaderI::instance().locate(hatIds);

	VipFullDataNSeqMap res;
	for(std::map<int, VipMemberDataNSeq>::const_iterator it = memberDatas.begin(); it != memberDatas.end(); ++it)
	{
		VipFullDataNSeq dataSeq;
		for(VipMemberDataNSeq::const_iterator vIt = it->second.begin(); vIt != it->second.end(); ++vIt)
		{
			std::map<int, std::string>::const_iterator findIcon2UrlIt = icon2urlDatas.find((*vIt)->iconId);
			HatDataMap::const_iterator findHatIt = hatDatas.find((*vIt)->hatId);
			if(findIcon2UrlIt != icon2urlDatas.end() && findHatIt != hatDatas.end())
			{
				dataSeq.push_back(new VipFullDataNI(*vIt, findIcon2UrlIt->second, findHatIt->second->hideUrl, findHatIt->second->status));
			}
		}
		if(dataSeq.empty())
		{
			continue;
		}
		res.insert(make_pair(it->first, dataSeq));
	}
	return res;
}

//FillTask
template<class T, class P>
FillTask<T, P>::FillTask(const Ice::Int identifier, const ::MyUtil::ObjectResultPtr& userData) : _identifier(identifier), _userData(userData)
{
}

template<class T, class P>
void FillTask<T, P>::handle()
{
	MCE_INFO("FillTask::handle " << _identifier << "  set data size : " << _userData->data.size());
	MyUtil::ObjectMap res;
	for(MyUtil::ObjectMap::iterator it = _userData->data.begin();it != _userData->data.end();++it)
	{
		T obj = new P;
		obj->parse(it->second);
		res.insert(make_pair(it->first, obj));
	}
	MyUtil::ServiceI::instance().getObjectCache()->addObjects(_identifier , res);
}

}
}
