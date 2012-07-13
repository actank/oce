#include "UserUrlReaderAdapter.h"
#include "Date.h"
using namespace xce::adapter::userurl;

using namespace MyUtil;

UserUrlReaderPrx UserUrlReaderAdapter::getUserUrlReaderPrx(int id)
{
	return getProxy(id);
}

UserUrlInfoPtr UserUrlReaderAdapter::getUserUrl(int id, const Ice::Context& ctx) {
	UserUrlReaderPrx prx = getUserUrlReaderPrx(id);
	if(prx)
	{
		UserUrlDataPtr data = NULL;
		try
		{
			data = prx->getUserUrl(id, ctx);
		}catch(...)
		{
			MCE_WARN("UserUrlReaderAdapter::getUserUrl Exception for id " << id);
			return NULL;
		}
		if(data)
		{
			UserUrlInfoPtr info = new UserUrlInfo;
			info->parse(data);
			return info;
		}
	}else
	{
		MCE_WARN("UserUrlReaderAdapter::getUserUrl no prx for id " << id);
	}
	return NULL;
}

void UserUrlReaderAdapter::setUserUrl(int id, const MyUtil::Str2StrMap& props) {
	if (id <= 0) {
		return;
	}
	
	std::vector<UserUrlReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserUrlReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setUserUrl(id, props);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserUrlReaderAdapter::setUserUrl Exception id " << id << " " << e.what());
		}
	}
}

void UserUrlReaderAdapter::setData(int id, const Ice::ObjectPtr& data) {
	if (id<=0) {
		return;
	}
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));
	std::vector<UserUrlReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserUrlReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserUrlReaderAdapter::setData Exception id " << id << " " << e.what());
		}
	}
}
