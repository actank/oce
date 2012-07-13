#include "UserDoingReaderAdapter.h"
#include "Date.h"
using namespace xce::adapter::userdoing;

using namespace MyUtil;

UserDoingReaderPrx UserDoingReaderAdapter::getUserDoingReaderPrx(int id)
{
	return getProxy(id);
}

UserDoingInfoPtr UserDoingReaderAdapter::getUserDoing(int id, const Ice::Context& ctx) {
	UserDoingReaderPrx prx = getUserDoingReaderPrx(id);
	if(prx)
	{
		UserDoingDataPtr data = NULL;
		try
		{
			data = prx->getUserDoing(id, ctx);
		}catch(...)
		{
			MCE_WARN("UserDoingReaderAdapter::getUserDoing Exception for id " << id);
			return NULL;
		}
		if(data)
		{
			UserDoingInfoPtr info = new UserDoingInfo;
			info->parse(data);
			return info;
		}else
		{
			return NULL;
		}
	}else
	{
		MCE_WARN("UserDoingReaderAdapter::getUserDoing no prx for id " << id);
	}
	return NULL;
}

void UserDoingReaderAdapter::setUserDoing(int id, const MyUtil::Str2StrMap& props) {
	if (id <= 0) {
		return;
	}
	
	std::vector<UserDoingReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserDoingReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setUserDoing(id, props);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserDoingReaderAdapter::setUserDoing Exception id " << id << " " << e.what());
		}
	}
}

void UserDoingReaderAdapter::setData(int id, const Ice::ObjectPtr& data) {
	if (id<=0) {
		return;
	}
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));
	std::vector<UserDoingReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserDoingReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserDoingReaderAdapter::setData Exception id " << id << " " << e.what());
		}
	}
}
