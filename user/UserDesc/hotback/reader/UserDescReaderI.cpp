/*
 * =====================================================================================
 *
 *       Filename:  UserDescReaderI.cpp
 *
 *    Description:  new Service UserDesc inlude UserTime,UserContact and big UserDesc.
 *
 *        Version:  1.0
 *        Created:  2009年08月28日 18时07分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wenlong.zhang (ggh), wenlong.zhang@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "UserDescReaderI.h"
#include "user/UserDesc/hotback/share/UserDescDataI.cpp"
#include "util/cpp/String.h"
#include "UserLogicAdapter.h"
#include "util/cpp/TimeCost.h"
#include "UserTimeWriterAdapter.h"
#include "UserContactWriterAdapter.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
using namespace xce::userdesc;
using namespace com::xiaonei::xce;
using namespace mop::hi::svc::adapter;
using namespace xce::serverstate;
using namespace xce::adapter::usertime;
using namespace xce::adapter::usercontact;
void MyUtil::initialize() {
	ServiceI& service = MyUtil::ServiceI::instance();
	
	service.registerObjectCache(USER_TIME, "UT", new UserTimeFactoryI, false);
	service.registerObjectCache(USER_CONTACT, "UC", new UserContactFactoryI, false);

	service.getAdapter()->add(&UserTimeReaderI::instance(), service.createIdentity("T", ""));
	service.getAdapter()->add(&UserContactReaderI::instance(), service.createIdentity("C", ""));
	service.getAdapter()->add(&UserDescReaderI::instance(), service.createIdentity("M", ""));

	//用来setdata的task 	
    TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));

	//-----------------------------------HA---------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserDescReader.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserDescReader.Interval",5);
	
	map<string,Ice::ObjectPtr> mapNameAndService;
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("T",&UserTimeReaderI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("C",&UserContactReaderI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>("M",&UserDescReaderI::instance()));
	
	ServerStateSubscriber::instance().initialize("ControllerUserDescReader",mapNameAndService,mod,interval);

	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserDescReader.Status",0);
		ServerStateSubscriber::instance().setStatus(status,"T");
		ServerStateSubscriber::instance().setStatus(status,"C");
		ServerStateSubscriber::instance().setStatus(status,"M");
		ServerStateSubscriber::instance().setStatus(status);
		
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status;

		MCE_WARN( os.str() );
	}
}

//-----------------------------------ReaderIs------------------------
//----------------------------------UserTimeReaderI-----------------
UserTimeReaderI::UserTimeReaderI() : _isValid(false) {
}

UserTimeReaderI::~UserTimeReaderI() {
}

UserTimeDataIPtr UserTimeReaderI::locate(int id)
{
	UserTimeDataIPtr dataI = MyUtil::ServiceI::instance().findObject<UserTimeDataIPtr> (USER_TIME, id);
	if(!dataI)
	{
		dataI = MyUtil::ServiceI::instance().locateObject<UserTimeDataIPtr> (USER_TIME, id);
		if(isValid())
		{
			UserTimeWriterAdapter::instance().reload(id); 
		}
	}
	return dataI;
}

bool UserTimeReaderI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _isValid;
}


UserTimeDataPtr UserTimeReaderI::getUserTime(Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"UserTimeReaderI::getUserTime id="<<id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	UserTimeDataIPtr dataI = locate(id);
	if (!dataI) {
		MCE_WARN("UserTimeReaderI::getUserTime, Exception[NoSuchUserTime], Information[id=" << id << "]");
		return NULL;
	}else {
		return dataI->getData();
	}
}

void UserTimeReaderI::setUserTime(Ice::Int id, const MyUtil::Str2StrMap& properities, const Ice::Current & current)
{
	MCE_INFO("UserTimeReaderI::setUserTime id=" << id << " ip = " << current.con->toString().substr(current.con->toString().find("remote",25)));

	ostringstream inMsg;
	inMsg<<"UserTimeReaderI::setUserTime id="<<id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(properities).find("ID");
	if(it!=properities.end())
	{
		const_cast<Str2StrMap&>(properities).erase(it);
	}
	MyUtil::Str2StrMap filter;
	filter["id"] = boost::lexical_cast<string>(id);
	UserTimeDataIPtr result = locate(id);
	result->setProperities(properities);

}

//----------------UserContactReader--------------------
UserContactReaderI::UserContactReaderI() : _isValid(false) {
}

UserContactReaderI::~UserContactReaderI() {
}

UserContactDataIPtr UserContactReaderI::locate(int id)
{
	UserContactDataIPtr dataI = MyUtil::ServiceI::instance().findObject<UserContactDataIPtr> (USER_CONTACT, id);
	if(!dataI)
	{
		dataI = MyUtil::ServiceI::instance().locateObject<UserContactDataIPtr> (USER_CONTACT, id);
		if(isValid())
		{
			UserContactWriterAdapter::instance().reload(id); 
		}
	}
	return dataI;
}

bool UserContactReaderI::isValid()
{
	IceUtil::Mutex::Lock lock(_mutex);
	return _isValid;
}


UserContactDataPtr UserContactReaderI::getUserContact(Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"UserContactReaderI::getUserContact id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	UserContactDataIPtr dataI = locate(id);
	if (!dataI) {
		MCE_WARN("UserContactReaderI::getUserContact, Exception[NoSuchUserContact], Information[id=" << id << "]");
		return NULL;
	}else {
		UserContactDataPtr data = dataI->getData();
		return data;
	}
}

UserContactDataNPtr UserContactReaderI::getUserContactN(Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"UserContactReaderI::getUserContactN id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	UserContactDataIPtr dataI = locate(id);
	if (!dataI) {
		MCE_WARN("UserContactReaderI::getUserContactN, Exception[NoSuchUserContact], Information[id=" << id << "]");
		return NULL;
	}else {
		UserContactDataNPtr data = dataI->getDataN();
		return data;
	}
}

void UserContactReaderI::setUserContact(Ice::Int id,const MyUtil::Str2StrMap& properities, const Ice::Current & current) {
	MCE_INFO("UserContactReaderI::setUserContact id=" << id << " ip = " << current.con->toString().substr(current.con->toString().find("remote",25)));
	
	ostringstream inMsg;
	inMsg<<"UserContactReaderI::setUserContact id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(properities).find("ID");
	if(it!=properities.end())
	{
		const_cast<Str2StrMap&>(properities).erase(it);
	}
	MyUtil::Str2StrMap filter;
	filter["id"] = boost::lexical_cast<string>(id);
	UserContactDataIPtr result = locate(id);
	result->setProperities(properities);

}

//----------------UserDescReader--------------------
UserDescReaderI::UserDescReaderI() {
}

UserDescReaderI::~UserDescReaderI() {
}
UserDescDataPtr UserDescReaderI::getUserDesc(int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"UserDescReaderI::getUserDesc id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	UserDescDataPtr result = new UserDescData;
	result->userTime = UserTimeReaderI::instance().locate(id)->getData();
	result->userContact = UserContactReaderI::instance().locate(id)->getData();
	result->userFav = new UserFavData;
	result->userFav->id = result->userTime->id;
	return result;
}

UserDescDataNPtr UserDescReaderI::getUserDescN(int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"UserDescReaderI::getUserDesc id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	UserDescDataNPtr result = new UserDescDataN;
	result->userTime = UserTimeReaderI::instance().locate(id)->getData();
	result->userContact = UserContactReaderI::instance().locate(id)->getDataN();
	result->userFav = new UserFavData;
	result->userFav->id = result->userTime->id;
	return result;
}
//-----------------------------------ReaderIs end--------------------

void UserTimeReaderI::setValid(bool valid, const Ice::Current&) {
	MCE_INFO("UserTimeReaderI::setValid -> "<<valid);
	ServerStateSubscriber::instance().setStatus((int)valid,"T");
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_isValid = valid;
	}
	if(UserContactReaderI::instance().isValid())
	{
		ServerStateSubscriber::instance().setStatus((int)valid,"M");
		ServerStateSubscriber::instance().setStatus((int)valid);
	}
}
void UserTimeReaderI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
        TaskManager::instance().execute(new FillTask<UserTimeDataIPtr, UserTimeDataI>(USER_TIME, userData));
}
void UserContactReaderI::setValid(bool valid, const Ice::Current&) {
	MCE_INFO("UserContactReaderI::setValid -> "<<valid);
	ServerStateSubscriber::instance().setStatus((int)valid,"C");
	{
		IceUtil::Mutex::Lock lock(_mutex);
		_isValid = valid;
	}
	if(UserTimeReaderI::instance().isValid())
	{
		ServerStateSubscriber::instance().setStatus((int)valid,"M");
		ServerStateSubscriber::instance().setStatus((int)valid);
	}
}
void UserContactReaderI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
        TaskManager::instance().execute(new FillTask<UserContactDataIPtr, UserContactDataI>(USER_CONTACT, userData));
}

