/*
 * =====================================================================================
 *
 *       Filename:  UserDescWriterI.cpp
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

#include "UserDescWriterI.h"
#include "user/UserDesc/hotback/share/UserDescDataI.cpp"
#include "util/cpp/String.h"
#include "UserLogicAdapter.h"
#include "util/cpp/TimeCost.h"
#include "UserTimeReaderAdapter.h"
#include "UserContactReaderAdapter.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
using namespace xce::userdesc;
using namespace com::xiaonei::xce;
using namespace mop::hi::svc::adapter;
using namespace xce::serverstate;
using namespace xce::tempmutex;
using namespace xce::adapter::usertime;
using namespace xce::adapter::usercontact;

void MyUtil::initialize() {
	ServiceI& service = MyUtil::ServiceI::instance();
	

	service.getAdapter()->add(&UserTimeWriterI::instance(), service.createIdentity("T", ""));
	service.getAdapter()->add(&UserContactWriterI::instance(), service.createIdentity("C", ""));

	//-----------------------------------HA---------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserDescWriter.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserDescWriter.Interval",5);
	
	map<string,Ice::ObjectPtr> mapNameAndService;
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("T",&UserTimeWriterI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("C",&UserContactWriterI::instance()));
	
	ServerStateSubscriber::instance().initialize("ControllerUserDescWriter",mapNameAndService,mod,interval);

	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserDescWriter.Status",0);
		ServerStateSubscriber::instance().setStatus(status,"T");
		ServerStateSubscriber::instance().setStatus(status,"C");
		ServerStateSubscriber::instance().setStatus(status);
		
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status;

		MCE_WARN( os.str() );
	}
}

//----------------------------------UserTimeWriterI-----------------
UserTimeWriterI::UserTimeWriterI() {
	_tempMutexManager = new TempMutexManager();
}

UserTimeWriterI::~UserTimeWriterI() {
}


void UserTimeWriterI::setUserTime(Ice::Int id, const MyUtil::Str2StrMap& properities, const Ice::Current & current)
{
	MCE_INFO("UserTimeWriterI::setUserTime id=" << id << " ip = " << current.con->toString().substr(current.con->toString().find("remote",25)));

	ostringstream inMsg;
	inMsg<<"UserTimeWriterI::setUserTime id="<<id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(properities).find("ID");
	if(it!=properities.end())
	{
		const_cast<Str2StrMap&>(properities).erase(it);
	}

	MyUtil::Str2StrMap filter;
	filter["id"] = boost::lexical_cast<string>(id);

	{
		Lock lock(_tempMutexManager,id);
		QueryRunner(DB_USER_TIME,CDbWServer).execute(Statement::buildUpdate(TABLE_USER_TIME, filter, properities));
		UserTimeReaderAdapter::instance().setUserTime(id,properities);
	}
	UserLogicAdapter::instance().updateUserTime(id, properities, current.ctx);
}

void UserTimeWriterI::createUserTime(const MyUtil::Str2StrMap& properities, const Ice::Current & current)
{
	MCE_INFO("UserTimeWriterI::createUserTime " << " ip = " << current.con->toString().substr(current.con->toString().find("remote",25)));

	ostringstream inMsg;
	inMsg<<"UserTimeWriterI::createUserTime " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	MyUtil::Str2StrMap::const_iterator itr = properities.find("ID");
	int id;
	if (itr == properities.end()) {
		MCE_WARN("UserTimeWriterI::createUserTime, Exception[NoId]");
		return;
	} else {
		id = boost::lexical_cast<int>(itr->second);
	}

	ostringstream idMsg;
	idMsg<<" id="<<id;
	tc.step(idMsg.str());

	Lock lock(_tempMutexManager,id);
	QueryRunner(DB_USER_TIME,CDbWServer).execute(Statement::buildInsert(TABLE_USER_TIME, properities));
	UserTimeDataIPtr result = new UserTimeDataI;
	Statement sql;
	sql << "SELECT id, registertime, activatetime, lastlogintime, lastlogouttime FROM user_time WHERE id = '" << id << "'";
	UserTimeResultHandlerI handler(result);
	QueryRunner(DB_USER_TIME, CDbWServer).query(sql, handler);
	if (id != result->id) {
		MCE_WARN("UserTimeWriterI::createUserTime, Exception[NoSuchUserTime], Information[id=" << id << "]");
		return;
	} 
	
	UserTimeReaderAdapter::instance().setData(id, result);

}

void UserTimeWriterI::reload(Ice::Int id, const Ice::Current & current) {
	
	ostringstream inMsg;
	inMsg<<"UserTimeWriterI::reload id=" << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	
	UserTimeDataIPtr result = new UserTimeDataI;
	Statement sql;
	sql << "SELECT id, registertime, activatetime, lastlogintime, lastlogouttime FROM user_time WHERE id = '" << id << "'";
	UserTimeResultHandlerI handler(result);
	
	Lock lock(_tempMutexManager, id);
	QueryRunner(DB_USER_TIME, CDbWServer).query(sql, handler);
	if (id != result->id) {
		MCE_WARN("UserTimeWriterI::createUserTime, Exception[NoSuchUserTime], Information[id=" << id << "]");
		return;
	} 
	
	UserTimeReaderAdapter::instance().setData(id, result);
}

//----------------UserContactWriter--------------------
UserContactWriterI::UserContactWriterI() {
	_tempMutexManager = new TempMutexManager();
}

UserContactWriterI::~UserContactWriterI() {
}

void UserContactWriterI::setUserContact(Ice::Int id,const MyUtil::Str2StrMap& properities, const Ice::Current & current) {
	MCE_INFO("UserContactWriterI::setUserContact id=" << id << " ip = " << current.con->toString().substr(current.con->toString().find("remote",25)));
	
	ostringstream inMsg;
	inMsg<<"UserContactWriterI::setUserContact id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(properities).find("ID");
	if(it!=properities.end())
	{
		const_cast<Str2StrMap&>(properities).erase(it);
	}

	MyUtil::Str2StrMap filter;
	filter["id"] = boost::lexical_cast<string>(id);
	QueryRunner(DB_USER_CONTACT,CDbWServer).execute(Statement::buildUpdate(TABLE_USER_CONTACT, filter, properities));

	{
		Lock lock(_tempMutexManager, id);
		UserContactReaderAdapter::instance().setUserContact(id,properities);
	}
	
	UserLogicAdapter::instance().updateUserContact(id, properities, current.ctx);
}

void UserContactWriterI::createUserContact(const MyUtil::Str2StrMap& properities, const Ice::Current & current)
{ 
	MCE_INFO("UserContactWriterI::createUserContact " << " ip = " << current.con->toString().substr(current.con->toString().find("remote",25)));

	ostringstream inMsg;
	inMsg<<"UserContactWriterI::createUserContact " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	MyUtil::Str2StrMap::const_iterator itr = properities.find("ID");
	int id;
	if (itr == properities.end()) {
		MCE_WARN("UserContactWriterI::createUserContact, Exception[NoId]");
		return;
	} else {
		id = boost::lexical_cast<int>(itr->second);
	}
	
	ostringstream idMsg;
	idMsg<<" id ="<<id;
	tc.step(idMsg.str());

	Lock lock(_tempMutexManager, id);
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

	UserContactReaderAdapter::instance().setData(id, result);
}

void UserContactWriterI::reload(Ice::Int id, const Ice::Current & current) {
	UserContactDataIPtr result = new UserContactDataI;
	Statement sql;
	sql << "SELECT id, msn, homepage, mobilephone, fixphone, qq FROM user_contact WHERE id = '" << id << "'";
	UserContactResultHandlerI handler(result);
	Lock lock(_tempMutexManager, id);
	QueryRunner(DB_USER_CONTACT, CDbWServer).query(sql, handler);
	if (id != result->id) {
		MCE_WARN("UserContactManagerI::reload, Exception[NoSuchUserContact], Information[id=" << id << "]");
		return;
	}

	UserContactReaderAdapter::instance().setData(id, result);
}


void UserTimeWriterI::setValid(bool valid, const Ice::Current&) {
	MCE_INFO("UserTimeWriterI::setValid -> "<<valid);
	ServerStateSubscriber::instance().setStatus((int)valid,"T");
		ServerStateSubscriber::instance().setStatus((int)valid);
}
void UserContactWriterI::setValid(bool valid, const Ice::Current&) {
	MCE_INFO("UserContactWriterI::setValid -> "<<valid);
	ServerStateSubscriber::instance().setStatus((int)valid);
}
