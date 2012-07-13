#include "UserStageReaderAdapter.h"
#include "Date.h"
using namespace xce::adapter::userstage;

using namespace MyUtil;

UserStageReaderPrx UserStageReaderAdapter::getUserStageReaderPrx(int id)
{
	return getProxy(id);
}

UserStageInfoPtr UserStageReaderAdapter::getUserStage(int id, const Ice::Context& ctx) {
	UserStageReaderPrx prx = getUserStageReaderPrx(id);
	if(prx)
	{
		UserStageDataPtr data = NULL;
		try
		{
			data = prx->getUserStage(id, ctx);
		}catch(...)
		{
			MCE_WARN("UserStageReaderAdapter::getUserStage Exception for id " << id);
			return NULL;
		}
		if(data)
		{
			UserStageInfoPtr info = new UserStageInfo;
			info->parse(data);
			return info;
		}
	}else
	{
		MCE_WARN("UserStageReaderAdapter::getUserStage no prx for id " << id);
	}
	return NULL;
}

void UserStageReaderAdapter::setUserStage(int id, const MyUtil::Str2StrMap& props) {
	if (id <= 0) {
		return;
	}
	
	std::vector<UserStageReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserStageReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setUserStage(id, props);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserStageReaderAdapter::setUserStage Exception id " << id << " " << e.what());
		}
	}
}

void UserStageReaderAdapter::setData(int id, const Ice::ObjectPtr& data) {
	if (id<=0) {
		return;
	}
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));
	std::vector<UserStageReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserStageReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserStageReaderAdapter::setData Exception id " << id << " " << e.what());
		}
	}
}
