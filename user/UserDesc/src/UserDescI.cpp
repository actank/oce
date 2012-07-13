/*
 * =====================================================================================
 *
 *       Filename:  UserDescI.cpp
 *
 *    Description:  new Service UserDesc inlude UserTime,UserContact and big UserDesc.
 *
 *        Version:  1.0
 *        Created:  2009年08月28日 18时07分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "UserDescI.h"
#include "util/cpp/String.h"
#include "UserLogicAdapter.h"
#include "util/cpp/TimeCost.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
using namespace xce::userdesc;
using namespace com::xiaonei::xce;
using namespace mop::hi::svc::adapter;
using namespace xce::serverstate;
void MyUtil::initialize() {
	ServiceI& service = MyUtil::ServiceI::instance();
	
	service.registerObjectCache(USER_TIME, "UT", new UserTimeFactoryI, false);
	service.registerObjectCache(USER_CONTACT, "UC", new UserContactFactoryI, false);

	service.getAdapter()->add(&UserTimeManagerI::instance(), service.createIdentity("T", ""));
	service.getAdapter()->add(&UserContactManagerI::instance(), service.createIdentity("C", ""));
	service.getAdapter()->add(&UserFavManagerI::instance(), service.createIdentity("F", ""));
	service.getAdapter()->add(&UserDescManagerI::instance(), service.createIdentity("M", ""));

	//用来setdata的task 	
    TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));

	//-----------------------------------HA---------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserDesc.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserDesc.Interval",5);
	
	map<string,Ice::ObjectPtr> mapNameAndService;
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("T",&UserTimeManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("C",&UserContactManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("M",&UserDescManagerI::instance()));
	
	ServerStateSubscriber::instance().initialize("ControllerUserDesc",mapNameAndService,mod,interval);

	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserDesc.Status",0);
		ServerStateSubscriber::instance().setStatus(status,"T");
		ServerStateSubscriber::instance().setStatus(status,"C");
		ServerStateSubscriber::instance().setStatus(status);
		
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status;

		MCE_WARN( os.str() );
	}
}

//-----------------------------------ManagerIs------------------------
//----------------------------------UserTimeManagerI-----------------
UserTimeManagerI::UserTimeManagerI() {
}

UserTimeManagerI::~UserTimeManagerI() {
}

UserTimeDataPtr UserTimeManagerI::getUserTime(Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"UserTimeManagerI::getUserTime id="<<id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	UserTimeDataIPtr dataI = MyUtil::ServiceI::instance().locateObject<UserTimeDataIPtr> (USER_TIME, id);
	if (!dataI) {
		MCE_WARN("UserTimeManagerI::getUserTime, Exception[NoSuchUserTime], Information[id=" << id << "]");
		return NULL;
	}else {
		return dataI->getData();
	}
}

void UserTimeManagerI::setUserTime(Ice::Int id, const MyUtil::Str2StrMap& properities, const Ice::Current & current)
{
	MCE_INFO("UserTimeManagerI::setUserTime id=" << id << " ip = " << current.con->toString().substr(current.con->toString().find("remote",25)));

	ostringstream inMsg;
	inMsg<<"UserTimeManagerI::setUserTime id="<<id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(properities).find("ID");
	if(it!=properities.end())
	{
		const_cast<Str2StrMap&>(properities).erase(it);
	}

	MyUtil::Str2StrMap filter;
	filter["id"] = boost::lexical_cast<string>(id);
	QueryRunner(DB_USER_TIME,CDbWServer).schedule(Statement::buildUpdate(TABLE_USER_TIME, filter, properities));
	UserTimeDataIPtr result = MyUtil::ServiceI::instance().locateObject<UserTimeDataIPtr> (USER_TIME, id);
	result->setProperities(properities);
	UserLogicAdapter::instance().updateUserTime(id, properities, current.ctx);
}

void UserTimeManagerI::createUserTime(const MyUtil::Str2StrMap& properities, const Ice::Current & current)
{
	
	MCE_INFO("UserTimeManagerI::createUserTime " << " ip = " << current.con->toString().substr(current.con->toString().find("remote",25)));

	ostringstream inMsg;
	inMsg<<"UserTimeManagerI::createUserTime " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	MyUtil::Str2StrMap::const_iterator itr = properities.find("ID");
	int id;
	if (itr == properities.end()) {
		MCE_WARN("UserTimeManagerI::createUserTime, Exception[NoId]");
		return;
	} else {
		id = boost::lexical_cast<int>(itr->second);
	}

        ostringstream idMsg;
        idMsg<<" id="<<id;
        tc.step(idMsg.str());

	QueryRunner(DB_USER_TIME,CDbWServer).execute(Statement::buildInsert(TABLE_USER_TIME, properities));
	UserTimeDataIPtr result = new UserTimeDataI;
        Statement sql;
        sql << "SELECT id, registertime, activatetime, lastlogintime, lastlogouttime FROM user_time WHERE id = '" << id << "'";
        UserTimeResultHandlerI handler(result);
        QueryRunner(DB_USER_TIME, CDbWServer).query(sql, handler);
        if (id != result->id) {
		MCE_WARN("UserTimeManagerI::createUserTime, Exception[NoSuchUserTime], Information[id=" << id << "]");
                return;
        }
	ServiceI::instance().addObject(result,USER_TIME,result->id);
}

void UserTimeManagerI::reload(Ice::Int id, const Ice::Current & current) {
        ostringstream inMsg;
        inMsg<<"UserTimeManagerI::reload id=" << " " << current.con->toString().substr(current.con->toString().find("remote",25));
        TimeCost tc = TimeCost::create(inMsg.str());

	MyUtil::ServiceI::instance().reloadObject(USER_TIME, id);

}
//----------------UserContactManager--------------------
UserContactManagerI::UserContactManagerI() {
}

UserContactManagerI::~UserContactManagerI() {
}

UserContactDataPtr UserContactManagerI::getUserContact(Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"UserContactManagerI::getUserContact id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	UserContactDataIPtr dataI = MyUtil::ServiceI::instance().locateObject<UserContactDataIPtr> (USER_CONTACT, id);
	if (!dataI) {
		MCE_WARN("UserContactManagerI::getUserContact, Exception[NoSuchUserContact], Information[id=" << id << "]");
		return NULL;
	}else {
		UserContactDataPtr data = dataI->getData();
		return data;
	}
}

void UserContactManagerI::setUserContact(Ice::Int id,const MyUtil::Str2StrMap& properities, const Ice::Current & current) {
	MCE_INFO("UserContactManagerI::setUserContact id=" << id << " ip = " << current.con->toString().substr(current.con->toString().find("remote",25)));
	
	ostringstream inMsg;
	inMsg<<"UserContactManagerI::setUserContact id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(properities).find("ID");
	if(it!=properities.end())
	{
		const_cast<Str2StrMap&>(properities).erase(it);
	}

	MyUtil::Str2StrMap filter;
	filter["id"] = boost::lexical_cast<string>(id);
	QueryRunner(DB_USER_CONTACT,CDbWServer).schedule(Statement::buildUpdate(TABLE_USER_CONTACT, filter, properities));
	UserContactDataIPtr result = MyUtil::ServiceI::instance().locateObject<UserContactDataIPtr> (USER_CONTACT, id);
	result->setProperities(properities);
	UserLogicAdapter::instance().updateUserContact(id, properities, current.ctx);
}

void UserContactManagerI::createUserContact(const MyUtil::Str2StrMap& properities, const Ice::Current & current)
{
	MCE_INFO("UserContactManagerI::createUserContact " << " ip = " << current.con->toString().substr(current.con->toString().find("remote",25)));

	ostringstream inMsg;
	inMsg<<"UserContactManagerI::createUserContact " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	MyUtil::Str2StrMap::const_iterator itr = properities.find("ID");
	int id;
	if (itr == properities.end()) {
		MCE_WARN("UserContactManagerI::createUserContact, Exception[NoId]");
		return;
	} else {
		id = boost::lexical_cast<int>(itr->second);
	}
        ostringstream idMsg;
        idMsg<<" id ="<<id;
        tc.step(idMsg.str());

	QueryRunner(DB_USER_CONTACT,CDbWServer).execute(Statement::buildInsert(TABLE_USER_CONTACT,properities));
        UserContactDataIPtr result = new UserContactDataI;
        Statement sql;
        sql << "SELECT id, msn, homepage, mobilephone, fixphone, qq FROM user_contact WHERE id = '" << id << "'";
        UserContactResultHandlerI handler(result);
        QueryRunner(DB_USER_CONTACT, CDbWServer).query(sql, handler);
        if (id != result->id) {
		MCE_WARN("UserContactManagerI::createUserContact, Exception[NoSuchUserContact], Information[id=" << id << "]");
                return;
        }
	ServiceI::instance().addObject(result,USER_CONTACT,result->id);
}

void UserContactManagerI::reload(Ice::Int id, const Ice::Current & current) {
	ostringstream inMsg;
	inMsg<<"UserContactManagerI::reload id = " << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	MyUtil::ServiceI::instance().reloadObject(USER_CONTACT, id);

}

//----------------UserFavManager--------------------
UserFavManagerI::UserFavManagerI() {
}

UserFavManagerI::~UserFavManagerI() {
}

UserFavDataPtr UserFavManagerI::getUserFav(Ice::Int id, const Ice::Current& current) {
	MCE_WARN("UserFavManagerI::getUserFav id=" << id << " ip = "<<current.con->toString());
	return new UserFavData;
}

void UserFavManagerI::setUserFav(Ice::Int id, const MyUtil::Str2StrMap& properities, const Ice::Current & current) {
	MCE_WARN("UserFavManagerI::setUserFav id=" << id << " ip = "<<current.con->toString());

}

void UserFavManagerI::createUserFav(const MyUtil::Str2StrMap& properities, const Ice::Current & current) {
	MCE_WARN("UserFavManagerI::createUserFav ip = "<<current.con->toString());
}

void UserFavManagerI::reload(Ice::Int id, const Ice::Current & current) {
	MCE_WARN("UserFavManagerI::reload id=" << id << " ip = "<<current.con->toString());
}


//----------------UserDescManager--------------------
UserDescManagerI::UserDescManagerI() {
}

UserDescManagerI::~UserDescManagerI() {
}
UserDescDataPtr UserDescManagerI::getUserDesc(int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"UserDescManagerI::getUserDesc id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	UserDescDataPtr result = new UserDescData;
	result->userTime = ServiceI::instance().locateObject<UserTimeDataIPtr>(USER_TIME, id)->getData();
	result->userContact = ServiceI::instance().locateObject<UserContactDataIPtr>(USER_CONTACT, id)->getData();
	result->userFav = new UserFavData;
	result->userFav->id = result->userTime->id;
	return result;
}
//-----------------------------------ManagerIs end--------------------

void UserTimeManagerI::setValid(bool valid, const Ice::Current&) {
	MCE_INFO("UserTimeManagerI::setValid -> "<<valid);
	ServerStateSubscriber::instance().setStatus((int)valid,"T");
}
void UserTimeManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
        TaskManager::instance().execute(new FillTask<UserTimeDataIPtr, UserTimeDataI>(USER_TIME, userData));
}
void UserFavManagerI::setValid(bool valid, const Ice::Current&) {
	MCE_WARN("UserFavManagerI::setValid -> "<<valid);
}
void UserFavManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
}
void UserContactManagerI::setValid(bool valid, const Ice::Current&) {
	MCE_INFO("UserContactManagerI::setValid -> "<<valid);
	ServerStateSubscriber::instance().setStatus((int)valid,"C");
}
void UserContactManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
        TaskManager::instance().execute(new FillTask<UserContactDataIPtr, UserContactDataI>(USER_CONTACT, userData));
}

