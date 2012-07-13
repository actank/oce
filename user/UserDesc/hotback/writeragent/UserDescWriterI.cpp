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
#include "util/cpp/String.h"
#include "util/cpp/TimeCost.h"
#include "UserContactAdapter.h"
#include "UserTimeAdapter.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
using namespace xce::userdesc;
using namespace com::xiaonei::xce;
using namespace xce::serverstate;
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
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserDescWriter.Status",1);
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
}

UserTimeWriterI::~UserTimeWriterI() {
}


void UserTimeWriterI::setUserTime(Ice::Int id, const MyUtil::Str2StrMap& properities, const Ice::Current & current)
{
	ostringstream inMsg;
	inMsg<<"UserTimeWriterI::setUserTime id="<<id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	
	UserTimeAdapter::instance().setUserTime(id, properities);
}

void UserTimeWriterI::createUserTime(const MyUtil::Str2StrMap& properities, const Ice::Current & current)
{
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
	
	UserTimeAdapter::instance().createUserTime(id, properities);

}

void UserTimeWriterI::reload(Ice::Int id, const Ice::Current & current) {
	
	ostringstream inMsg;
	inMsg<<"UserTimeWriterI::reload id=" << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	
	UserTimeAdapter::instance().reload(id);
}

//----------------UserContactWriter--------------------
UserContactWriterI::UserContactWriterI() {
}

UserContactWriterI::~UserContactWriterI() {
}

void UserContactWriterI::setUserContact(Ice::Int id,const MyUtil::Str2StrMap& properities, const Ice::Current & current) {
	ostringstream inMsg;
	inMsg<<"UserContactWriterI::setUserContact id=" << id << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	UserContactAdapter::instance().setUserContact(id, properities);
}

void UserContactWriterI::createUserContact(const MyUtil::Str2StrMap& properities, const Ice::Current & current)
{ 
	ostringstream inMsg;
	inMsg<<"UserContactWriterI::createUserContact " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	
	MyUtil::Str2StrMap::const_iterator itr = properities.find("ID");
	int id;
	if (itr == properities.end()) {
		MCE_WARN("UserContactWriterI::createUserTime, Exception[NoId]");
		return;
	} else {
		id = boost::lexical_cast<int>(itr->second);
	}
	UserContactAdapter::instance().createUserContact(id, properities);
}

void UserContactWriterI::reload(Ice::Int id, const Ice::Current & current) {
	ostringstream inMsg;
	inMsg<<"UserTimeWriterI::reload id=" << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	
	UserContactAdapter::instance().reload(id);
}


void UserTimeWriterI::setValid(bool valid, const Ice::Current&) {
	MCE_INFO("UserTimeWriterI::setValid -> "<<valid);
	ServerStateSubscriber::instance().setStatus((int)valid,"T");
	ServerStateSubscriber::instance().setStatus((int)valid);
}
void UserContactWriterI::setValid(bool valid, const Ice::Current&) {
	MCE_INFO("UserContactWriterI::setValid -> "<<valid);
	ServerStateSubscriber::instance().setStatus((int)valid,"C");
	ServerStateSubscriber::instance().setStatus((int)valid);
}
