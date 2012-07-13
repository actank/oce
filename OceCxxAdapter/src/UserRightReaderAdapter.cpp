#include "UserRightReaderAdapter.h"
#include "Date.h"
using namespace xce::adapter::userright;

using namespace MyUtil;

UserRightReaderPrx UserRightReaderAdapter::getUserRightReaderPrx(int id)
{
	return getProxy(id);
}

UserRightInfoPtr UserRightReaderAdapter::getUserRight(int id, const Ice::Context& ctx) {
	UserRightReaderPrx prx = getUserRightReaderPrx(id);
	if(prx)
	{
		UserRightDataPtr data = NULL;
		try
		{
			data = prx->getUserRight(id, ctx);
		}catch(...)
		{
			MCE_WARN("UserRightReaderAdapter::getUserRight Exception for id " << id);
			return NULL;
		}
		if(data)
		{
			UserRightInfoPtr info = new UserRightInfo;
			info->parse(data);
			return info;
		}
	}else
	{
		MCE_WARN("UserRightReaderAdapter::getUserRight no prx for id " << id);
	}
	return NULL;
}

void UserRightReaderAdapter::setUserRight(int id, const MyUtil::Str2StrMap& props) {
	if (id <= 0) {
		return;
	}
	
	std::vector<UserRightReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserRightReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setUserRight(id, props);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserRightReaderAdapter::setUserRight Exception id " << id << " " << e.what());
		}
	}
}

void UserRightReaderAdapter::setData(int id, const Ice::ObjectPtr& data) {
	if (id<=0) {
		return;
	}
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));
	std::vector<UserRightReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserRightReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserRightReaderAdapter::setData Exception id " << id << " " << e.what());
		}
	}
}
