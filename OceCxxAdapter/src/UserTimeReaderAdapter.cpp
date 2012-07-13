/*
 * =====================================================================================
 *
 *       Filename:  UserTimeReaderAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年09月16日 12时09分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wenlong.zhang, wenlong.zhang@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#include "UserTimeReaderAdapter.h"
#include "Date.h"
using namespace xce::adapter::usertime;

UserTimeReaderPrx UserTimeReaderAdapter::getUserTimeReader(int id)
{
	return getProxy(id);
}

UserTimeInfoPtr UserTimeReaderAdapter::getUserTime(int userId, const Ice::Context& ctx) {
	UserTimeReaderPrx prx = getUserTimeReader(userId);
	if(prx)
	{
		UserTimeInfoPtr info = new UserTimeInfo;
		UserTimeDataPtr data = prx->getUserTime(userId, ctx);
		if(data)
		{
			info->parse(data);
			return info;
		}
		return NULL;
	}
	return NULL;
}

UserTimeDataPtr UserTimeReaderAdapter::getUserTimeData(int userId, const Ice::Context& ctx) {
	UserTimeReaderPrx prx = getUserTimeReader(userId);
	if(prx)
	{
		UserTimeDataPtr data = prx->getUserTime(userId, ctx);
		return data;
	} else {
		return NULL;
	}
}

void UserTimeReaderAdapter::setUserTime(int userid, const MyUtil::Str2StrMap& properities, const Ice::Context& ctx) {
	if(userid <= 0) {
		return;
	}
	std::vector<UserTimeReaderPrx> allProxies = getAllProxySeq(userid);
	for(std::vector<UserTimeReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setUserTime(userid, properities);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserTimeReaderAdapter::setUserTime Exception id = " << userid << " " << e.what());
		}
	}
}

void UserTimeReaderAdapter::setData(int userid, const Ice::ObjectPtr& data)
{
	if(!data) {
		return;
	}
	
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(userid, data));

	std::vector<UserTimeReaderPrx> allProxies = getAllProxySeq(userid);
	for(std::vector<UserTimeReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserTimeReaderAdapter::setData Exception id = " << userid << " " << e.what());
		}
	}
}
