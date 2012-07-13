#include "UserLoginCountReaderAdapter.h"
#include "Date.h"
#include <boost/lexical_cast.hpp>
using namespace xce::adapter::userlogincount;
using namespace MyUtil;

UserLoginCountReaderPrx UserLoginCountReaderAdapter::getUserLoginCountReaderPrx(int id)
{
	return getProxy(id);
}

int UserLoginCountReaderAdapter::getUserLoginCount(int id, const Ice::Context& ctx) {
	UserLoginCountReaderPrx prx = getUserLoginCountReaderPrx(id);
	if(prx)
	{
		try
		{
			return prx->getUserLoginCount(id, ctx);
		}catch(...)
		{
			MCE_WARN("UserLoginCountReaderAdapter::getUserLoginCount Exception for id " << id);
			return -1;
		}
	}else
	{
		MCE_WARN("UserLoginCountReaderAdapter::getUserLoginCount no prx for id " << id);
	}
	return -1;
}

void UserLoginCountReaderAdapter::incUserLoginCount(int id) {
	if (id <= 0) {
		return;
	}
	
	std::vector<UserLoginCountReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserLoginCountReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->incUserLoginCount(id);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserLoginCountReaderAdapter::incUserLoginCount Exception id " << id << " " << e.what());
		}
	}
}

void UserLoginCountReaderAdapter::setData(int id, const Ice::ObjectPtr& data) {
	if (id<=0) {
		return;
	}
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));
	std::vector<UserLoginCountReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserLoginCountReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserLoginCountReaderAdapter::setData Exception id " << id << " " << e.what());
		}
	}
}
