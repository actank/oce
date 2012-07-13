#include "UserPassportReaderAdapter.h"
#include "Date.h"
using namespace xce::adapter::userpassport;
using namespace MyUtil;

UserPassportReaderPrx UserPassportReaderAdapter::getUserPassportReaderPrx(int id)
{
	return getProxy(id);
}

UserPassportInfoPtr UserPassportReaderAdapter::getUserPassport(int id, const Ice::Context& ctx){
	UserPassportReaderPrx prx = getUserPassportReaderPrx(id);
	if(prx) {
		UserPassportDataPtr data = NULL; 
		try {
			data = prx->getUserPassport(id, ctx);
		}catch(...) {
			MCE_WARN("UserPassportReaderAdapter::getUserPassport Exception for id " << id);
			return NULL;
		}
		if(data) {
			UserPassportInfoPtr ptr = new UserPassportInfo;
			ptr->parse(data);
			return ptr;
		}
	}else {
		MCE_WARN("UserPassportReaderAdapter::getUserPassport no prx for id " << id);
	}
	return NULL;
}

void UserPassportReaderAdapter::setUserPassport(int id, const Str2StrMap& props) {
	if(id <= 0) {
		return;
	}
	std::vector<UserPassportReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserPassportReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setUserPassport(id, props);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserPassportReaderAdapter::setUserPassport Exception id " << id << " " << e.what());
		}
	}
}

void UserPassportReaderAdapter::setData(int id, const Ice::ObjectPtr& data) {
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(id, data));
	std::vector<UserPassportReaderPrx> allProxies = getAllProxySeq(id);
	for(std::vector<UserPassportReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserPassportReaderAdapter::setData Exception id " << id << " " << e.what());
		}
	}
}

