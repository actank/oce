#include "UserNameReaderAdapter.h"
#include "Date.h"
using namespace xce::adapter::username;
using namespace MyUtil;

UserNameReaderPrx UserNameReaderAdapter::getUserNameReaderPrx(int id)
{
	return getProxy(id);
}

UserNameInfoPtr UserNameReaderAdapter::getUserName(int id, const Ice::Context& ctx) {
	UserNameReaderPrx prx = getUserNameReaderPrx(id);
	if(prx)
	{
		UserNameDataPtr data = NULL;
		try
		{
			data = prx->getUserName(id, ctx);
		}catch(...)
		{
			MCE_WARN("UserNameReaderAdapter::getUserName Exception for id " << id);
			return NULL;
		}
		if(data) 
		{
			UserNameInfoPtr info = new UserNameInfo;
			info->parse(data);
			return info;
		}else {
			return NULL;
		}
	}else
	{
		MCE_WARN("UserNameReaderAdapter::getUserName no prx for id " << id);
	}
	return NULL;
}

void UserNameReaderAdapter::setUserName(int id, const MyUtil::Str2StrMap& props) {
	if (id <= 0) {
		return;
	}
	
	std::vector<UserNameReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserNameReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setUserName(id, props);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserNameReaderAdapter::setUserName Exception id " << id << " " << e.what());
		}
	}
}

void UserNameReaderAdapter::setData(int id, const Ice::ObjectPtr& data) {
	if (id<=0) {
		return;
	}
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));
	std::vector<UserNameReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserNameReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserNameReaderAdapter::setData Exception id " << id << " " << e.what());
		}
	}
}
