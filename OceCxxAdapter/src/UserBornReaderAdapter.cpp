#include "UserBornReaderAdapter.h"
#include "Date.h"
using namespace xce::adapter::userborn;

using namespace MyUtil;

UserBornReaderPrx UserBornReaderAdapter::getUserBornReaderPrx(int id)
{
	return getProxy(id);
}

UserBornInfoPtr UserBornReaderAdapter::getUserBorn(int id, const Ice::Context& ctx) {
	UserBornReaderPrx prx = getUserBornReaderPrx(id);
	if(prx)
	{
		UserBornDataNPtr data = NULL;
		try
		{
			data = prx->getUserBorn(id, ctx);
		}catch(...)
		{
			MCE_WARN("UserBornReaderAdapter::getUserBorn Exception for id " << id);
			return NULL;
		}
		if(data)
		{
			UserBornInfoPtr info = new UserBornInfo;
			info->parse(data);
			return info;
		}else 
		{
			return NULL;
		}
	}else
	{
		MCE_WARN("UserBornReaderAdapter::getUserBorn no prx for id " << id);
	}
	return NULL;
}

void UserBornReaderAdapter::setUserBorn(int id, const MyUtil::Str2StrMap& props) {
	if (id <= 0) {
		return;
	}
	
	std::vector<UserBornReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserBornReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setUserBorn(id, props);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserBornReaderAdapter::setUserBorn Exception id " << id << " " << e.what());
		}
	}
}

void UserBornReaderAdapter::setData(int id, const Ice::ObjectPtr& data) {
	if (id<=0) {
		return;
	}
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));
	std::vector<UserBornReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserBornReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserBornReaderAdapter::setData Exception id " << id << " " << e.what());
		}
	}
}
