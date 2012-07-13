/*
 * =====================================================================================
 *
 *       Filename:  UserDescDataI.h
 *
 *    Description:  Common code
 *
 *        Version:  1.0
 *        Created:  2009年09月02日 18时50分31秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */


#ifndef __USERDESCDATAI_H__
#define __USERDESCDATAI_H__

#include "UserDesc.h"
#include <Ice/Ice.h>
#include <TaskManager.h>
#include <ServiceI.h>
#include <QueryRunner.h>
#include <IceUtil/IceUtil.h>
#include <ObjectCacheI.h>

//----------------------------------DataIs---------------------------
namespace xce{
namespace userdesc{

const std::string DB_USER_TIME = "user_time";
const std::string DB_USER_CONTACT = "user_contact";

const std::string TABLE_USER_TIME = "user_time";
const std::string TABLE_USER_CONTACT = "user_contact";

class UserTimeDataI : virtual public UserTimeData {
public:
	void setProperities(const MyUtil::Str2StrMap& properities);
	UserTimeDataPtr getData();
	void parse(mysqlpp::Row& row);
	void parse(Ice::ObjectPtr& obj);
private:
	IceUtil::Mutex _mutex;
};
typedef ::IceUtil::Handle<UserTimeDataI> UserTimeDataIPtr;

class UserContactDataI: virtual public UserContactDataN {
public:
	void setProperities(const MyUtil::Str2StrMap& properities);
	UserContactDataPtr getData();
	UserContactDataNPtr getDataN();
	void parse(mysqlpp::Row& row);
	void parse(Ice::ObjectPtr& obj);
private:
	IceUtil::Mutex _mutex;
};
typedef ::IceUtil::Handle<UserContactDataI> UserContactDataIPtr;

//----------------------------ResultHandlers-------------------------
class UserTimeResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	UserTimeResultHandlerI(const UserTimeDataIPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	UserTimeDataIPtr _result;	
};


class UserContactResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	UserContactResultHandlerI(const UserContactDataIPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	UserContactDataIPtr _result;	
};

//----------------------------Factorys-------------------------

class UserTimeFactoryI : public MyUtil::ServantFactory {
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};

class UserContactFactoryI : public MyUtil::ServantFactory{
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};
};
};

#endif
