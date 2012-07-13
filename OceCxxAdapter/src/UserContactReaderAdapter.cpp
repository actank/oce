/*
 * =====================================================================================
 *
 *       Filename:  UserContactReaderAdapter.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年09月16日 12时09分27秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wenlong.zhang@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#include "UserContactReaderAdapter.h"
#include "Date.h"
using namespace xce::adapter::usercontact;

UserContactReaderPrx UserContactReaderAdapter::getUserContactReader(int id)
{
	return getProxy(id);
}

UserContactDataPtr UserContactReaderAdapter::getUserContact(int userId, const Ice::Context& ctx) {
	UserContactReaderPrx prx = getProxy(userId);
	if(prx)
	{
		return getProxy(userId)->getUserContact(userId, ctx);
	}
	return NULL;
}

UserContactDataNPtr UserContactReaderAdapter::getUserContactN(int userId, const Ice::Context& ctx) {
	UserContactReaderPrx prx = getProxy(userId);
	if(prx)
	{
		return getProxy(userId)->getUserContactN(userId, ctx);
	}
	return NULL;
}

void UserContactReaderAdapter::setUserContact(int userid, const MyUtil::Str2StrMap& properities) {
	if(userid <= 0) {
		return;
	}
	std::vector<UserContactReaderPrx> allProxies = getAllProxySeq(userid);
	for(std::vector<UserContactReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setUserContact(userid, properities);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserContactReaderAdapter::setUserContact Exception id = " << userid << " " << e.what());
		}
	}
}

void UserContactReaderAdapter::setData(int userid, const Ice::ObjectPtr& data)
{
	if(!data) {
		return;
	}
	
	MyUtil::ObjectResultPtr res = new MyUtil::ObjectResult;
	res->data.insert(make_pair(userid, data));

	std::vector<UserContactReaderPrx> allProxies = getAllProxySeq(userid);
	for(std::vector<UserContactReaderPrx>::const_iterator it = allProxies.begin(); it != allProxies.end(); ++it )
	{
		try
		{
			(*it)->ice_timeout(10000)->setData(res);
		}catch(Ice::Exception& e)
		{
			MCE_WARN("UserContactReaderAdapter::setData Exception id = " << userid << " " << e.what());
		}
	}
}
