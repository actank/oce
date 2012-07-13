#include "UserBaseI.h"
#include "LogWrapper.h"
#include "BatchUpdaterAdapter.h"
#include "Date.h"
#include "UserBaseDataI.h"
#include "QueryRunner.h"
#include <UserPassportAdapter.h>
#include <UserRightAdapter.h>
#include <UserStateAdapter.h>
#include <UserNameAdapter.h>
#include <UserUrlAdapter.h>
#include <UserStageAdapter.h>
#include <UserBornAdapter.h>
#include <UserDoingAdapter.h>
#include <UserBaseAdapter.h>
#include <UserBasicAdapter.h>


#include "time.h"

//#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"
using namespace com::xiaonei::xce;
using namespace mop::hi::svc::adapter;
using namespace xce::userbase;
using namespace MyUtil;
using namespace xce::serverstate;
using namespace xce::adapter::userpassport;
using namespace xce::adapter::userright;
using namespace xce::adapter::userdoing;
using namespace xce::adapter::userstage;
using namespace xce::adapter::userborn;
using namespace xce::adapter::userstate;
using namespace xce::adapter::username;
using namespace xce::adapter::userbase;
using namespace xce::adapter::userurl;
using namespace xce::adapter::userbasic;
using MyUtil::Str2StrMap;
//-------------------------UserPassportManagerI-------------------------------
/* the new instialize
 */
void MyUtil::initialize()
{
	ServiceI& service=ServiceI::instance();


	service.registerObjectCache(USER_PASSPORT, "UP", new UserPassportFactoryI, false);
	service.registerObjectCache(USER_NAME, "UN", new UserNameFactoryI, false);
	service.registerObjectCache(USER_RIGHT, "UR", new UserRightFactoryI, false);
	service.registerObjectCache(USER_LOGINCOUNT, "ULC", new UserLoginCountFactoryI, false);
	service.registerObjectCache(USER_STATE, "US", new UserStateFactoryI, false);
	service.registerObjectCache(USER_URL, "UU", new UserUrlFactoryI, false);
	service.registerObjectCache(USER_BORN, "UBO", new UserBornFactoryI, false);
	service.registerObjectCache(USER_STAGE, "USG", new UserStageFactoryI, false);
	service.registerObjectCache(USER_DOING, "UD", new UserDoingFactoryI, false);
	
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

	service.getAdapter()->add(&UserBaseViewManagerI::instance(), service.createIdentity(
			USER_BASEVIEW_MANAGER, ""));
	
	service.getAdapter()->add(&UserUrlManagerI::instance(), service.createIdentity(USER_URL_MANAGER, ""));
	service.getAdapter()->add(&UserBasicManagerI::instance(),service.createIdentity(USER_BASIC_MANAGER, ""));
	
	service.getAdapter()->add(&UserBornManagerI::instance(),service.createIdentity(USER_BORN_MANAGER, ""));
	service.getAdapter()->add(&UserStageManagerI::instance(),service.createIdentity(USER_STAGE_MANAGER, ""));
	service.getAdapter()->add(&UserDoingManagerI::instance(),service.createIdentity(USER_DOING_MANAGER, ""));

//用来setdata的task 	
    TaskManager::instance().config(TASK_LEVEL_FILL, MyUtil::ThreadPoolConfig(1, 100));
    TaskManager::instance().config(TASK_LEVEL_LOAD, MyUtil::ThreadPoolConfig(1, 10));
	
//-----------------------------------HA---------------------------------------------
	//int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserBase.Mod");
	//int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBase.Interval",5);
	
	map<string,Ice::ObjectPtr> mapNameAndService;
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_PASSPORT_MANAGER,&UserPassportManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_NAME_MANAGER,&UserNameManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_RIGHT_MANAGER,&UserRightManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_STATE_MANAGER,&UserStateManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_LOGINCOUNT_MANAGER,&UserLoginCountManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_URL_MANAGER,&UserUrlManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_BASIC_MANAGER,&UserBasicManagerI::instance()));
	
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_BORN_MANAGER,&UserBornManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_STAGE_MANAGER,&UserStageManagerI::instance()));
	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_DOING_MANAGER,&UserDoingManagerI::instance()));

	mapNameAndService.insert(make_pair<string,Ice::ObjectPtr>(USER_BASEVIEW_MANAGER,&UserBaseViewManagerI::instance()));
	

	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBase.Status",0);
		
		
	//	ServerStateSubscriber::instance().setStatus(status);
		
		std::ostringstream os;
		os<<"MyUtil::initialize set status : "<<status;

		MCE_WARN( os.str() );
	}
}
void UserBasicManager::setValidForce(bool, const Ice::Current&) {
}
//--------------------------------------------------------------------------------

void UserPassportManagerI::createUserPassport(const MyUtil::Str2StrMap& props,
		                                      const Ice::Current& current)
{
	MCE_INFO("UserPassportManagerI::createUserPassport" << " " << current.con->toString().substr(current.con->toString().find("remote",25)));
  
}

UserPassportDataPtr UserPassportManagerI::getUserPassport(Ice::Int uid,
                                            		   const Ice::Current& current)
{
  return UserPassportAdapter::instance().getUserPassportManager(uid)->getUserPassport(uid);
}

void UserPassportManagerI::setUserPassport(Ice::Int uid,
		                                   const MyUtil::Str2StrMap& props, 
										   const Ice::Current& current)
{
  UserPassportAdapter::instance().getUserPassportManager(uid)->setUserPassport(uid,props);
}

//reload for us
void UserPassportManagerI::reload(Ice::Int userId, const Ice::Current& current)
{

}

Ice::Int UserPassportManagerI::getUserIdByEmail(const string& email, const Ice::Current& current)
{
  return 0;
}


//-------------------------UserNameManagerI-------------------------------

void UserNameManagerI::createUserName(const MyUtil::Str2StrMap& props,
		                              const Ice::Current& current)
{
	MCE_INFO("UserNameManagerI::createUserName" << " " << current.con->toString().substr(current.con->toString().find("remote",25)));
}

UserNameDataPtr UserNameManagerI::getUserName(Ice::Int uid,
                                           const Ice::Current& current)
{
  return UserNameAdapter::instance().getUserNameManager(uid)->getUserName(uid);
}

void UserNameManagerI::setUserName(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
  UserNameAdapter::instance().getUserNameManager(uid)->setUserName(uid,props);
}

//reload for us
void UserNameManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
}

//-------------------------UserStateManagerI-------------------------------

//--------------------------------------------------------------------------------

void UserStateManagerI::createUserState(const MyUtil::Str2StrMap& props,
		                              const Ice::Current& current)
{
	MCE_INFO("UserStateManagerI::createUserState" << " " << current.con->toString().substr(current.con->toString().find("remote",25)));
}

UserStateDataPtr UserStateManagerI::getUserState(Ice::Int uid,
                                           const Ice::Current& current)
{
  return UserStateAdapter::instance().getUserStateManager(uid)->getUserState(uid);
}

void UserStateManagerI::setUserState(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
  UserStateAdapter::instance().getUserStateManager(uid)->setUserState(uid, props);
}
//reload for us
void UserStateManagerI::reload(Ice::Int userId, const Ice::Current& current)
{

}

void UserStateManagerI::setStateOn(Ice::Int userId, Ice::Int pos, const Ice::Current& current)
{
}

void UserStateManagerI::setStateOff(Ice::Int userId, Ice::Int pos, const Ice::Current& current) 
{
}


//-------------------------UserRightManagerI-------------------------------

//--------------------------------------------------------------------------------

void UserRightManagerI::createUserRight(const MyUtil::Str2StrMap& props,
		                              const Ice::Current& current)
{
	MCE_INFO("UserRightManagerI::createUserRight" << " " << current.con->toString().substr(current.con->toString().find("remote",25)));
}

UserRightDataPtr UserRightManagerI::getUserRight(Ice::Int uid,
                                           const Ice::Current& current)
{
  return UserRightAdapter::instance().getUserRightManager(uid)->getUserRight(uid);
}

void UserRightManagerI::setUserRight(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
  UserRightAdapter::instance().getUserRightManager(uid)->setUserRight(uid, props);
}

void UserRightManagerI::reload(Ice::Int userId, const Ice::Current& current)
{

}


//-------------------------UserLoginCountManagerI-------------------------------

//--------------------------------------------------------------------------------

void UserLoginCountManagerI::createUserLoginCount(const MyUtil::Str2StrMap& props,
		                              const Ice::Current& current)
{
}

int UserLoginCountManagerI::getUserLoginCount(Ice::Int uid,
                                           const Ice::Current& current)
{
  return 0;
}

void UserLoginCountManagerI::incUserLoginCount(Ice::Int uid,
							       const Ice::Current& current)
{
}


//reload for us
void UserLoginCountManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
}


//----------------UserUrlManager--------------------
UserUrlDataPtr UserUrlManagerI::getUserUrl(Ice::Int id, const Ice::Current& current) {
  return UserUrlAdapter::instance().getUserUrlManager(id)->getUserUrl(id);
}

void UserUrlManagerI::setUserUrl(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current & current) {
  UserUrlAdapter::instance().getUserUrlManager(id)->setUserUrl(id,props);
}

void UserUrlManagerI::createUserUrl(const MyUtil::Str2StrMap& props, const Ice::Current & current) 
{
	MCE_INFO("UserUrlManagerI::createUserUrl" << " " << current.con->toString().substr(current.con->toString().find("remote",25)));
}

void UserUrlManagerI::reload(Ice::Int id, const Ice::Current & current) {

}

//-------------------------UserBasic---------------------------

UserBasicDataPtr UserBasicManagerI::getUserBasic(Ice::Int userId, const Ice::Current& current)
{
  MCE_INFO("UserBasicManagerI::getUserBasic -> id: " << userId <<" "  << current.con->toString().substr(current.con->toString().find("remote",25)));
  return UserBasicAdapter::instance().getUserBasicManager(userId)->getUserBasic(userId);
}

//-------------------------UserBorn---------------------------

void UserBornManagerI::createUserBorn(const MyUtil::Str2StrMap& props, const Ice::Current& current )
{
	MCE_INFO("UserBornManagerI::createUserBorn" << " " << current.con->toString().substr(current.con->toString().find("remote",25)));
	
}

void UserBornManagerI::reloadUserBorn(Ice::Int userId,const Ice::Current& current)
{
}

UserBornDataPtr UserBornManagerI::getUserBorn(Ice::Int userId, const Ice::Current& current)
{
  return UserBornAdapter::instance().getUserBornManager(userId)->getUserBorn(userId);
}

void UserBornManagerI::setUserBorn(Ice::Int userId,const MyUtil::Str2StrMap& props,const Ice::Current& current)
 
{
  UserBornAdapter::instance().getUserBornManager(userId)->setUserBorn(userId, props);
}

//-------------------------UserStage---------------------------

void UserStageManagerI::createUserStage(const MyUtil::Str2StrMap& props, const Ice::Current& current )
{
	MCE_INFO("UserPassportManagerI::createUserStage" << " " << current.con->toString().substr(current.con->toString().find("remote",25)));
  int id;
  MyUtil::Str2StrMap::const_iterator it = props.find("ID");
  if(it!=props.end()) {
    id=boost::lexical_cast<Ice::Int>(it->second);
  } else {
    MCE_WARN("UserStageManagerI::createUserStage CreateUserStageException::Can find userID in props");
    return;
  }
  UserStageAdapter::instance().getUserStageManager(id)->createUserStage(props);
}

void UserStageManagerI::reloadUserStage(Ice::Int userId,const Ice::Current& current)
{
}

UserStageDataPtr UserStageManagerI::getUserStage(Ice::Int userId, const Ice::Current& current)
{
  return UserStageAdapter::instance().getUserStageManager(userId)->getUserStage(userId);
}

void UserStageManagerI::setUserStage(Ice::Int userId,const MyUtil::Str2StrMap& props,const Ice::Current& current)
 
{
  UserStageAdapter::instance().getUserStageManager(userId)->setUserStage(userId, props);
}

//-------------------------UserDoing---------------------------

void UserDoingManagerI::createUserDoing(const MyUtil::Str2StrMap& props, const Ice::Current& current )
{
	MCE_INFO("UserPassportManagerI::createUserDoing" << " " << current.con->toString().substr(current.con->toString().find("remote",25)));
  int id;
  MyUtil::Str2StrMap::const_iterator it = props.find("ID");
  if(it!=props.end()) {
    id=boost::lexical_cast<Ice::Int>(it->second);
  } else {
    MCE_WARN("UserDoingManagerI::createUserDoing CreateUserDoingException::Can find userID in props");
    return;
  }
  UserDoingAdapter::instance().getUserDoingManager(id)->createUserDoing(props);
}

void UserDoingManagerI::reloadUserDoing(Ice::Int userId,const Ice::Current& current)
{
}

UserDoingDataPtr UserDoingManagerI::getUserDoing(Ice::Int userId, const Ice::Current& current)
{
  return UserDoingAdapter::instance().getUserDoingManager(userId)->getUserDoing(userId);
}

void UserDoingManagerI::setUserDoing(Ice::Int userId,const MyUtil::Str2StrMap& props,const Ice::Current& current)
{
  UserDoingAdapter::instance().getUserDoingManager(userId)->setUserDoing(userId, props);
}




//-----------------------------------------------------------------------
UserBaseViewDataPtr UserBaseViewManagerI::getUserBaseView( Ice::Int uid,
		                                               const Ice::Current& current) 
{
  return UserBaseAdapter::instance().getUserBaseViewManager(uid)->getUserBaseView(uid);
}



UserCommonViewDataPtr UserBaseViewManagerI::getUserCommonView(int uid, const Ice::Current& current) 
{
  MCE_INFO("UserBaseViewManagerI::getUserCommonView -> id: " << uid <<" "  << current.con->toString().substr(current.con->toString().find("remote",25)));
  return UserBaseAdapter::instance().getUserBaseViewManager(uid)->getUserCommonView(uid);
}


//--------------------------------------------------------------------------------


void UserPassportManagerI::setValid(bool valid, const Ice::Current& current) {
}
void UserPassportManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
}
bool UserPassportManagerI::isValid(const Ice::Current& current) {
  return valid_;
}

void UserStateManagerI::setValid(bool valid, const Ice::Current& current) {
}
void UserStateManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
}
bool UserStateManagerI::isValid(const Ice::Current& current) {
  return valid_;
}

void UserLoginCountManagerI::setValid(bool valid, const Ice::Current& current) {
}
void UserLoginCountManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
}
bool UserLoginCountManagerI::isValid(const Ice::Current& current) {
	return valid_;
}

void UserRightManagerI::setValid(bool valid, const Ice::Current& current) {
}
void UserRightManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
}
bool UserRightManagerI::isValid(const Ice::Current& current) {
  return valid_;
}

void UserUrlManagerI::setValid(bool valid, const Ice::Current& current) {
}
void UserUrlManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
}
bool UserUrlManagerI::isValid(const Ice::Current& current) {
  return valid_;
}

void UserNameManagerI::setValid(bool valid, const Ice::Current& current) {
}
void UserNameManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
}
bool UserNameManagerI::isValid(const Ice::Current& current) {
  return valid_;
}
void xce::userbase::UserBaseViewManager::setValidForce(bool, const Ice::Current&) {
}

void UserBornManagerI::setValid(bool valid, const Ice::Current& current) {
}
void UserBornManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
}
bool UserBornManagerI::isValid(const Ice::Current& current) {
  return valid_;
}

void UserStageManagerI::setValid(bool valid, const Ice::Current& current) {
}
void UserStageManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
}
bool UserStageManagerI::isValid(const Ice::Current& current) {
  return valid_;
}

void UserDoingManagerI::setValid(bool valid, const Ice::Current& current) {
}
void UserDoingManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
}
bool UserDoingManagerI::isValid(const Ice::Current& current) {
  return valid_;
}
bool UserBaseViewManagerI::isValid(const Ice::Current& current) {
  return true;
}
bool UserBasicManagerI::isValid(const ::Ice::Current&) {
  return valid_;
}
void UserBaseViewManagerI::setValidForce(bool, Ice::Current const&) {
}

void UserBasicManagerI::setValidForce(bool, Ice::Current const&) {
}
