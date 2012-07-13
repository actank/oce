#include "UserStateReaderAdapter.h"
#include "Date.h"
using namespace xce::adapter::userstate;

void UserStateInfo::parse(const UserStateDataPtr& data) {
  id_ = data->id;
  state_ = data->state;
  star_ = data->star;
  level_ = data->level;
}

UserStateReaderPrx UserStateReaderAdapter::getUserStateReaderPrx(int id)
{
	return getProxy(id);
}

void UserStateReaderAdapter::setStateOn(int id, int pos) {
	if (id <= 0) {
		return;
	}
	
	std::vector<UserStateReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserStateReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setStateOn(id, pos);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserStateReaderAdapter::setStateOn Exception id " << id << " pos " << pos << " " << e.what());
		}
	}
}

void UserStateReaderAdapter::setStateOff(int id, int pos) {
	if (id <= 0) {
		return;
	}
	
	std::vector<UserStateReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserStateReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setStateOff(id, pos);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserStateReaderAdapter::setStateOff Exception id " << id << " pos " << pos << " " << e.what());
		}
	}
}

UserStateInfoPtr UserStateReaderAdapter::getUserState(int id, const Ice::Context& ctx) {
	UserStateReaderPrx prx = getUserStateReaderPrx(id);
	if(prx)
	{
		UserStateDataPtr data = NULL;
		try
		{
			data = prx->getUserState(id, ctx);
		}catch(...)
		{
			MCE_WARN("UserStateReaderAdapter::getUserState Exception for id " << id);
			return NULL;
		}
		if(data)
		{
			UserStateInfoPtr info = new UserStateInfo;
			info->parse(data);
			return info;
		}
	}else
	{
		MCE_WARN("UserStateReaderAdapter::getUserState no prx for id " << id);
	}
	return NULL;
}

void UserStateReaderAdapter::setUserState(int id, const MyUtil::Str2StrMap& props) {
	if (id <= 0) {
		return;
	}
	
	std::vector<UserStateReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserStateReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setUserState(id, props);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserStateReaderAdapter::setUserState Exception id " << id << " " << e.what());
		}
	}
}

void UserStateReaderAdapter::setData(int id, const Ice::ObjectPtr& data) {
	if (id <= 0) {
		return;
	}

	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));

	std::vector<UserStateReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserStateReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserStateReaderAdapter::setData Exception id " << id << " " << e.what());
		}
	}
}

