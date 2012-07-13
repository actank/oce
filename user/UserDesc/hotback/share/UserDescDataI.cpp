/*
 * =====================================================================================
 *
 *       Filename:  UserDescDataI.cpp
 *
 *    Description:  common data
 *
 *        Version:  1.0
 *        Created:  2009年09月02日 19时14分54秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "UserDescDataI.h"
#include "Date.h"

using namespace xce::userdesc;
using namespace com::xiaonei::xce;

//-----------------------------------FactoryIs------------------------
//-------------------------------UserTimeFactoryI---------------------
Ice::ObjectPtr UserTimeFactoryI::create(Ice::Int id) {
	UserTimeDataIPtr result = new UserTimeDataI;
	Statement sql;
	sql << "SELECT id, registertime, activatetime, lastlogintime, lastlogouttime FROM user_time WHERE id = '" << id << "'";
	UserTimeResultHandlerI handler(result);
	QueryRunner(DB_USER_TIME, CDbRServer).query(sql, handler);
	if (id != result->id) {
		MCE_WARN("UserTimeFactoryI::create, Exception[NoSuchUserTime],Information[id=" << id << "]");
		return NULL;
	}
	return result;
}
//-------------------------------UserContactFactoryI---------------------
Ice::ObjectPtr UserContactFactoryI::create(Ice::Int id) {
	UserContactDataIPtr result = new UserContactDataI;
	Statement sql;
	sql << "SELECT id, msn, homepage, mobilephone, fixphone, qq FROM user_contact WHERE id = '" << id << "'";
	UserContactResultHandlerI handler(result);
	QueryRunner(DB_USER_CONTACT, CDbRServer).query(sql, handler);
	if (id != result->id) {
		MCE_WARN("UserTimeFactoryI::create, Exception[NoSuchUserContact],Information[id=" << id << "]");
		return NULL;
	}
	return result;
}
//-----------------------------------FactoryIs end--------------------

//----------------------------------------ResultHandlers--------------
//-------------------------------UserTimeResultHandlerI---------------------
UserTimeResultHandlerI::UserTimeResultHandlerI(const UserTimeDataIPtr& result) :
	_result(result) {
}
bool UserTimeResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->id = (int) row["id"];
	_result->registerTime = (time_t) mysqlpp::DateTime(row["registertime"]);
	_result->activateTime = (time_t) mysqlpp::DateTime(row["activatetime"]);
	_result->lastLoginTime = (time_t) mysqlpp::DateTime(row["lastLogintime"]);
	_result->lastLogoutTime = (time_t) mysqlpp::DateTime(row["lastLogouttime"]);
	return true;
}

//-------------------------------UserContactResultHandlerI---------------------

UserContactResultHandlerI::UserContactResultHandlerI(
		const UserContactDataIPtr& result) :
	_result(result) {
}
bool UserContactResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->id = (int) row["id"];
	_result->msn = (string) row["msn"];
	_result->homepage = (string) row["homepage"];
	_result->mobilePhone = (string) row["mobilephone"];
	_result->fixPhone = (string) row["fixphone"];
	_result->qq = (long) row["qq"];
	return true;
}
//-------------------------------ResultHandlerIs end---------------------


//--------------------------------SetProperties getDatas----------------------
//-------------------------------UserTime---------------------
void UserTimeDataI::setProperities(const MyUtil::Str2StrMap& properities) {
	IceUtil::Mutex::Lock lock(_mutex);
	for (MyUtil::Str2StrMap::const_iterator it = properities.begin(); it != properities.end(); ++it) {
			if ((*it).first == "REGISTERTIME") {
				registerTime = (time_t) mysqlpp::DateTime((*it).second);
			}else if ((*it).first == "ACTIVATETIME") {
				activateTime = (time_t) mysqlpp::DateTime((*it).second);
			}else if ((*it).first == "LASTLOGINTIME") {
				lastLoginTime = (time_t) mysqlpp::DateTime((*it).second);
			}else if ((*it).first == "LASTLOGOUTTIME") {
				lastLogoutTime = (time_t) mysqlpp::DateTime((*it).second);
			}
	}
}

void UserTimeDataI::parse(Ice::ObjectPtr& obj)
{
	UserTimeDataPtr data = UserTimeDataPtr::dynamicCast(obj);
	id = data->id ;
	registerTime = data->registerTime ;
	activateTime = data->activateTime ;
	lastLoginTime = data->lastLoginTime ;
	lastLogoutTime = data->lastLogoutTime ;

}
void UserTimeDataI::parse(mysqlpp::Row& row)
{ 
	id = (int) row["id"];
	registerTime = (time_t) mysqlpp::DateTime(row["registertime"]);
	activateTime = (time_t) mysqlpp::DateTime(row["activatetime"]);
	lastLoginTime = (time_t) mysqlpp::DateTime(row["lastLogintime"]);
	lastLogoutTime = (time_t) mysqlpp::DateTime(row["lastLogouttime"]);
}

UserTimeDataPtr UserTimeDataI::getData() {
	IceUtil::Mutex::Lock lock(_mutex);
	UserTimeDataPtr newData = new UserTimeData;
	newData->id = id;
	newData->registerTime = registerTime;
	newData->activateTime = activateTime;
	newData->lastLoginTime = lastLoginTime;
	newData->lastLogoutTime = lastLogoutTime;
	return newData;
}
//-------------------------------UserContact---------------------

void UserContactDataI::parse(Ice::ObjectPtr& obj)
{
	if(obj->ice_isA("::xce::userdesc::UserContactData")){
		UserContactDataPtr data = UserContactDataPtr::dynamicCast(obj);
		id = data->id ;
		msn = data->msn ;
		homepage = data->homepage ;
		mobilePhone = data->mobilePhone ;
		fixPhone = data->fixPhone ;
		qq = data->qq ;
	}else if(obj->ice_isA("::xce::userdesc::UserContactDataN")){
		UserContactDataNPtr data = UserContactDataNPtr::dynamicCast(obj);
		id = data->id ;
		msn = data->msn ;
		homepage = data->homepage ;
		mobilePhone = data->mobilePhone ;
		fixPhone = data->fixPhone ;
		qq = data->qq ;
	}else{
		MCE_WARN("UserContactDataI::parse Unknown type");
	}
}

void UserContactDataI::parse(mysqlpp::Row& row)
{ 
	id = (int) row["id"];
	msn = (string) row["msn"];
	homepage = (string) row["homepage"];
	mobilePhone = (string) row["mobilephone"];
	fixPhone = (string) row["fixphone"];
	qq = (long) row["qq"];
}

void UserContactDataI::setProperities(const MyUtil::Str2StrMap& properities) {
	IceUtil::Mutex::Lock lock(_mutex);
	for (MyUtil::Str2StrMap::const_iterator it = properities.begin(); it != properities.end(); ++it) {
			if ((*it).first == "MSN"){
				msn = (*it).second;
			}else if ((*it).first == "MOBILEPHONE") {
				mobilePhone = (*it).second;
			}else if ((*it).first == "FIXPHONE") {
				fixPhone = (*it).second;
			}else if ((*it).first == "QQ") {
				qq = boost::lexical_cast<long>((*it).second);
			}else if ((*it).first == "HOMEPAGE") {
				homepage = (*it).second;
			}
	}
}

UserContactDataPtr UserContactDataI::getData() {
	IceUtil::Mutex::Lock lock(_mutex);
	UserContactDataPtr newData = new UserContactData;
	newData->id = id;
	newData->msn = msn;
	newData->mobilePhone = mobilePhone;
	newData->fixPhone = fixPhone;
	newData->qq = (int)qq;
	newData->homepage = homepage;
	return newData;
}

UserContactDataNPtr UserContactDataI::getDataN() {
	IceUtil::Mutex::Lock lock(_mutex);
	UserContactDataNPtr newData = new UserContactDataN;
	newData->id = id;
	newData->msn = msn;
	newData->mobilePhone = mobilePhone;
	newData->fixPhone = fixPhone;
	newData->qq = qq;
	newData->homepage = homepage;
	return newData;
}
//--------------------------------setProperities getDatas end---------------------------
