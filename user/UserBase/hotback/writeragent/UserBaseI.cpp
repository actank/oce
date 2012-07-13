#include "UserBaseI.h"
#include "LogWrapper.h"
#include "Date.h"
#include "time.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "Monitor/client/service/ServiceMonitor.h"
#include "boost/lexical_cast.hpp"

#include "UserPassportAdapter.h"
#include "UserNameAdapter.h"
#include "UserStateAdapter.h"
#include "UserRightAdapter.h"
#include "UserLoginCountAdapter.h"
#include "UserUrlAdapter.h"
#include "UserBornAdapter.h"
#include "UserStageAdapter.h"
#include "UserDoingAdapter.h"

using namespace com::xiaonei::xce;
using namespace xce::userbase;
using namespace MyUtil;
using namespace xce::serverstate;

using xce::adapter::userpassport::UserPassportAdapter;
using xce::adapter::username::UserNameAdapter;
using xce::adapter::userstate::UserStateAdapter;
using xce::adapter::userright::UserRightAdapter;
using xce::adapter::userlogincount::UserLoginCountAdapter;
using xce::adapter::userurl::UserUrlAdapter;
using xce::adapter::userborn::UserBornAdapter;
using xce::adapter::userstage::UserStageAdapter;
using xce::adapter::userdoing::UserDoingAdapter;
//-------------------------UserPassportManagerI-------------------------------
/* the new instialize
 */
void MyUtil::initialize()
{
	ServiceI& service=ServiceI::instance();

	service.getAdapter()->add(&UserPassportManagerI::instance(), service.createIdentity(
			USER_PASSPORT_MANAGER, ""));
	service.getAdapter()->add(&UserNameManagerI::instance(), service.createIdentity(
			USER_NAME_MANAGER, ""));
	service.getAdapter()->add(&UserRightManagerI::instance(), service.createIdentity(
			USER_RIGHT_MANAGER, ""));
	service.getAdapter()->add(&UserStateManagerI::instance(), service.createIdentity(
			USER_STATE_MANAGER, ""));
	service.getAdapter()->add(&UserLoginCountManagerI::instance(), service.createIdentity(
			USER_LOGINCOUNT_MANAGER, ""));

	
	service.getAdapter()->add(&UserUrlManagerI::instance(), service.createIdentity(USER_URL_MANAGER, ""));
	
	service.getAdapter()->add(&UserBornManagerI::instance(),service.createIdentity(USER_BORN_MANAGER, ""));
	service.getAdapter()->add(&UserStageManagerI::instance(),service.createIdentity(USER_STAGE_MANAGER, ""));
	service.getAdapter()->add(&UserDoingManagerI::instance(),service.createIdentity(USER_DOING_MANAGER, ""));

//-----------------------------------HA---------------------------------------------
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserBaseWriter.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBaseWriter.Interval",5);
	
	map<string,Ice::ObjectPtr> mapNameAndService;
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_PASSPORT_MANAGER,&UserPassportManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_NAME_MANAGER,&UserNameManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_RIGHT_MANAGER,&UserRightManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_STATE_MANAGER,&UserStateManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_LOGINCOUNT_MANAGER,&UserLoginCountManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_URL_MANAGER,&UserUrlManagerI::instance()));
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_BORN_MANAGER,&UserBornManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_STAGE_MANAGER,&UserStageManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_DOING_MANAGER,&UserDoingManagerI::instance()));

	
	ServerStateSubscriber::instance().initialize("ControllerUserBaseWriter",mapNameAndService,mod,interval);

	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBaseWriter.Status",0);
		
		ServerStateSubscriber::instance().setStatus(status,USER_PASSPORT_MANAGER);
		ServerStateSubscriber::instance().setStatus(status,USER_NAME_MANAGER);
		ServerStateSubscriber::instance().setStatus(status,USER_RIGHT_MANAGER);
		ServerStateSubscriber::instance().setStatus(status,USER_STATE_MANAGER);
		ServerStateSubscriber::instance().setStatus(status,USER_LOGINCOUNT_MANAGER);
		ServerStateSubscriber::instance().setStatus(status,USER_URL_MANAGER);
		ServerStateSubscriber::instance().setStatus(status,USER_BASIC_MANAGER);
		ServerStateSubscriber::instance().setStatus(status,USER_BORN_MANAGER);
		ServerStateSubscriber::instance().setStatus(status,USER_STAGE_MANAGER);
		ServerStateSubscriber::instance().setStatus(status,USER_DOING_MANAGER);
		
		ServerStateSubscriber::instance().setStatus(status);
		
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status;

		MCE_WARN( os.str() );
	}
  ServiceMonitorManager::instance().start();
}

//--------------------------------------------------------------------------------

void UserPassportManagerI::createUserPassport(const Str2StrMap& props,
		                                      const Ice::Current& current)
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
	ostringstream inMsg;
	inMsg<<"UserPassportManagerI::createUserPassport" << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	
	int userId;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		userId=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		return;
	}
	
	UserPassportAdapter::instance().getProxy(userId)->createUserPassport(props);
}

void UserPassportManagerI::setUserPassport(Ice::Int uid,
		                                   const MyUtil::Str2StrMap& props, 
										   const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	UserPassportAdapter::instance().getProxy(uid)->setUserPassport(uid, props);
}

//reload for us
void UserPassportManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserPassportAdapter::instance().getProxy(userId)->reload(userId);
}

//-------------------------UserNameManagerI-------------------------------

void UserNameManagerI::createUserName(const Str2StrMap& props,
		                              const Ice::Current& current)
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	int userId;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		userId=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		return;
	}
	
	UserNameAdapter::instance().getProxy(userId)->createUserName(props);
}

void UserNameManagerI::setUserName(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	UserNameAdapter::instance().getProxy(uid)->setUserName(uid, props);
}

//reload for us
void UserNameManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserNameAdapter::instance().getProxy(userId)->reload(userId);
}

//-------------------------UserStateManagerI-------------------------------

//--------------------------------------------------------------------------------

void UserStateManagerI::createUserState(const Str2StrMap& props,
		                              const Ice::Current& current)
{
	InvokeClient ic = InvokeClient::create(current,"",InvokeClient::INVOKE_LEVEL_INFO);

	int userId;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		userId=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		return;
	}

	UserStateAdapter::instance().getProxy(userId)->createUserState(props);
}

void UserStateManagerI::setUserState(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	UserStateAdapter::instance().getProxy(uid)->setUserState(uid, props);
}

//reload for us
void UserStateManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserStateAdapter::instance().getProxy(userId)->reload(userId);
}

void UserStateManagerI::setStateOn(Ice::Int userId, Ice::Int pos, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	UserStateAdapter::instance().getProxy(userId)->setStateOn(userId, pos);
}

void UserStateManagerI::setStateOff(Ice::Int userId, Ice::Int pos, const Ice::Current& current) 
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	UserStateAdapter::instance().getProxy(userId)->setStateOff(userId, pos);
}

//-------------------------UserRightManagerI-------------------------------

//--------------------------------------------------------------------------------

void UserRightManagerI::createUserRight(const Str2StrMap& props,
		                              const Ice::Current& current)
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	int userId;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		userId=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		return;
	}

	UserRightAdapter::instance().getProxy(userId)->createUserRight(props);
}

void UserRightManagerI::setUserRight(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	UserRightAdapter::instance().getProxy(uid)->setUserRight(uid, props);
}

void UserRightManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserRightAdapter::instance().getProxy(userId)->reload(userId);
}

//-------------------------UserLoginCountManagerI-------------------------------

//--------------------------------------------------------------------------------

void UserLoginCountManagerI::createUserLoginCount(const Str2StrMap& props,
		                              const Ice::Current& current)
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	int userId;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		userId=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		return;
	}

	UserLoginCountAdapter::instance().getProxy(userId)->createUserLoginCount(props);
}

void UserLoginCountManagerI::incUserLoginCount(Ice::Int uid,
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	UserLoginCountAdapter::instance().getProxy(uid)->incUserLoginCount(uid);
}


//reload for us
void UserLoginCountManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserLoginCountAdapter::instance().getProxy(userId)->reload(userId);
}


//----------------UserUrlManager--------------------

void UserUrlManagerI::setUserUrl(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current & current) {
	ostringstream inMsg;
	inMsg<<"id="<<id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserUrlAdapter::instance().getProxy(id)->setUserUrl(id, props);
}

void UserUrlManagerI::createUserUrl(const MyUtil::Str2StrMap& props, const Ice::Current & current) 
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	int userId;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		userId=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		return;
	}
	
	UserUrlAdapter::instance().getProxy(userId)->createUserUrl(props);
}

void UserUrlManagerI::reload(Ice::Int id, const Ice::Current & current) {
	ostringstream inMsg;
	inMsg<<"id="<<id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserUrlAdapter::instance().getProxy(id)->reload(id);
}

//-------------------------UserBorn---------------------------

void UserBornManagerI::createUserBorn(const Str2StrMap& props, const Ice::Current& current )
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	int userId;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		userId=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		return;
	}

	UserBornAdapter::instance().getProxy(userId)->createUserBorn(props);
}

void UserBornManagerI::reloadUserBorn(Ice::Int userId,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserBornAdapter::instance().getProxy(userId)->reloadUserBorn(userId);
}

void UserBornManagerI::setUserBorn(Ice::Int userId,const Str2StrMap& props,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	UserBornAdapter::instance().getProxy(userId)->setUserBorn(userId, props);
}

//-------------------------UserStage---------------------------

void UserStageManagerI::createUserStage(const Str2StrMap& props, const Ice::Current& current )
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	int userId;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		userId=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		return;
	}

	UserStageAdapter::instance().getProxy(userId)->createUserStage(props);
}

void UserStageManagerI::reloadUserStage(Ice::Int userId,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	UserStageAdapter::instance().getProxy(userId)->reloadUserStage(userId);
}

void UserStageManagerI::setUserStage(Ice::Int userId,const Str2StrMap& props,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	UserStageAdapter::instance().getProxy(userId)->setUserStage(userId, props);
}

//-------------------------UserDoing---------------------------

void UserDoingManagerI::createUserDoing(const Str2StrMap& props, const Ice::Current& current )
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);

	int userId;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		userId=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		return;
	}

	UserDoingAdapter::instance().getProxy(userId)->createUserDoing(props);
}

void UserDoingManagerI::reloadUserDoing(Ice::Int userId,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	UserDoingAdapter::instance().getProxy(userId)->reloadUserDoing(userId);
}

void UserDoingManagerI::setUserDoing(Ice::Int userId,const Str2StrMap& props,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	UserDoingAdapter::instance().getProxy(userId)->setUserDoing(userId, props);
}

//--------------------------------------------------------------------------------


void UserPassportManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		ServerStateSubscriber::instance().setStatus((int)valid,USER_PASSPORT_MANAGER);
		ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

void UserStateManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		ServerStateSubscriber::instance().setStatus((int)valid,USER_STATE_MANAGER);
		ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

void UserLoginCountManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		ServerStateSubscriber::instance().setStatus((int)valid,USER_LOGINCOUNT_MANAGER);
		ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

void UserRightManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		ServerStateSubscriber::instance().setStatus((int)valid,USER_RIGHT_MANAGER);
		ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

void UserUrlManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		ServerStateSubscriber::instance().setStatus((int)valid,USER_URL_MANAGER);
		ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

void UserNameManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		ServerStateSubscriber::instance().setStatus((int)valid,USER_NAME_MANAGER);
		ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

void UserBornManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);

	{
		ServerStateSubscriber::instance().setStatus((int)valid,USER_BORN_MANAGER);
		ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

void UserStageManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		ServerStateSubscriber::instance().setStatus((int)valid,USER_STAGE_MANAGER);
		ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

void UserDoingManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		ServerStateSubscriber::instance().setStatus((int)valid,USER_DOING_MANAGER);
		ServerStateSubscriber::instance().setStatus((int)valid);
	}
}

