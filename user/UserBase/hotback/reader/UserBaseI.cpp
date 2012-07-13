#include "UserBaseI.h"
#include "LogWrapper.h"
#include "BatchUpdaterAdapter.h"
#include "UserLogicAdapter.h"
#include "Date.h"
#include "QueryRunner.h"
#include "RpcFilterManager.h"

#include "user/UserBase/hotback/data/UserBaseDataI.cpp"

#include "time.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "boost/lexical_cast.hpp"

#include "UserPassportWriterAdapter.h"
#include "UserNameWriterAdapter.h"
#include "UserStateWriterAdapter.h"
#include "UserRightWriterAdapter.h"
#include "UserLoginCountWriterAdapter.h"
#include "UserUrlWriterAdapter.h"
#include "UserBornWriterAdapter.h"
#include "UserStageWriterAdapter.h"
#include "UserDoingWriterAdapter.h"

using namespace com::xiaonei::xce;
using namespace mop::hi::svc::adapter;
using namespace xce::userbase;
using namespace MyUtil;
using namespace xce::serverstate;

using xce::adapter::userpassport::UserPassportWriterAdapter;
using xce::adapter::username::UserNameWriterAdapter;
using xce::adapter::userstate::UserStateWriterAdapter;
using xce::adapter::userright::UserRightWriterAdapter;
using xce::adapter::userlogincount::UserLoginCountWriterAdapter;
using xce::adapter::userurl::UserUrlWriterAdapter;
using xce::adapter::userborn::UserBornWriterAdapter;
using xce::adapter::userstage::UserStageWriterAdapter;
using xce::adapter::userdoing::UserDoingWriterAdapter;
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
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserBaseReader.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBaseReader.Interval",5);
	
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
	
	ServerStateSubscriber::instance().initialize("ControllerUserBaseReader",mapNameAndService,mod,interval);

	{
		int status = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserBaseReader.Status",0);
		
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
  MyUtil::RpcFilterManager::instance().initialize("UserBaseReader");
  ServiceMonitorManager::instance().start();
}

//--------------------------------------------------------------------------------

UserPassportDataPtr UserPassportManagerI::getUserPassport(Ice::Int uid,
                                            		   const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
  
	bool flag = MyUtil::RpcFilterManager::instance().filter(current, "user_passport");

	if (!flag) {
	  MCE_WARN("UserPassportManagerI::getUserPassport filter account and passwordmd5");
	  UserPassportDataPtr data = (m_getUserPassport(uid))->getData();
	  data->account = "";
	  data->passwordmd5 = "";
	  return data;
	} else {
		return (m_getUserPassport(uid))->getData();
	}
}

void UserPassportManagerI::setUserPassport(Ice::Int uid,
		                                   const MyUtil::Str2StrMap& props, 
										   const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	m_getUserPassport(uid)->setProperties(props);
	
}

Ice::Int UserPassportManagerI::getUserIdByEmail(const string& email, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"email: " << email;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	int result = -1;
	Statement sql;
	sql << "SELECT id FROM user_passport WHERE account = " << mysqlpp::quote << email;
	UserEmailResultHandlerI handler(result);
	QueryRunner(DB_USER_PASSPORT, CDbWServer).query(sql, handler);
	MCE_INFO("UserPassportManagerI::getUserIdByEmail(" << email<< ") -> id: "<<result);
	return result;

}


UserPassportDataIPtr UserPassportManagerI::m_getUserPassport(int userId)
{
	if (isValid()) {
		UserPassportDataIPtr res = ServiceI::instance().findObject<UserPassportDataIPtr>(USER_PASSPORT, userId);
		if (res) {
			return res;
		} else {
			TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERPASSPORT, userId));
			MCE_WARN("UserPassportManagerI::m_getUserPassport NoSuchUserPassportException:: reason - cache miss [" << userId << "]");
			throw NoSuchObjectException();
		}
	} else {
		try{
			return ServiceI::instance().locateObject<UserPassportDataIPtr>(USER_PASSPORT, userId);
		} catch(const MyUtil::NoSuchObjectException& e) {
			MCE_WARN("UserPassportManagerI::m_getUserPassport NoSuchUserPassportException:: reason - NoSuchUserPassportException [" << userId << "]");
			throw NoSuchObjectException();
		}
	}
}

//-------------------------UserNameManagerI-------------------------------

UserNameDataPtr UserNameManagerI::getUserName(Ice::Int uid,
                                           const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	return (m_getUserName(uid))->getData();
}

void UserNameManagerI::setUserName(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	m_getUserName(uid)->setProperties(props);
	
}

UserNameDataIPtr UserNameManagerI::m_getUserName(int userId)
{
	if (isValid()) {
		UserNameDataIPtr res = ServiceI::instance().findObject<UserNameDataIPtr>(USER_NAME, userId);
		if (res) {
			return res;
		} else {
			TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERNAMES, userId));
			MCE_WARN("UserNameManagerI::m_getUserName NoSuchUserNameException:: reason - cache miss [" << userId << "]");
			throw NoSuchObjectException();
		}
	} else {
		try{
			return ServiceI::instance().locateObject<UserNameDataIPtr>(USER_NAME, userId);
		} catch(const MyUtil::NoSuchObjectException& e) {
			MCE_WARN("UserNameManagerI::m_getUserName NoSuchUserNameException:: reason - NoSuchUserNameException [" << userId << "]");
			throw NoSuchObjectException();
		}
	}
}

//-------------------------UserStateManagerI-------------------------------

UserStateDataPtr UserStateManagerI::getUserState(Ice::Int uid,
                                           const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	return (m_getUserState(uid))->getData();
}

void UserStateManagerI::setUserState(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	m_getUserState(uid)->setProperties(props);
}

void UserStateManagerI::setStateOn(Ice::Int userId, Ice::Int pos, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
		
	m_getUserState(userId)->setStateOn(pos);
}

void UserStateManagerI::setStateOff(Ice::Int userId, Ice::Int pos, const Ice::Current& current) 
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
		
	m_getUserState(userId)->setStateOff(pos);
}

UserStateDataIPtr UserStateManagerI::m_getUserState(int userId)
{
	if (isValid()) {
		UserStateDataIPtr res = ServiceI::instance().findObject<UserStateDataIPtr>(USER_STATE, userId);
		if (res) {
			return res;
		} else {
			TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERSTATE, userId));
			MCE_WARN("UserStateManagerI::m_getUserState NoSuchUserStateException:: reason - cache miss [" << userId << "]");
			throw NoSuchObjectException();
		}
	} else {
		try{
			return ServiceI::instance().locateObject<UserStateDataIPtr>(USER_STATE, userId);
		} catch(const MyUtil::NoSuchObjectException& e) {
			MCE_WARN("UserStateManagerI::m_getUserState NoSuchUserStateException:: reason - NoSuchUserStateException [" << userId << "]");
			throw NoSuchObjectException();
		}
	}
}

//-------------------------UserRightManagerI-------------------------------

UserRightDataPtr UserRightManagerI::getUserRight(Ice::Int uid,
                                           const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	return (m_getUserRight(uid))->getData();
}

void UserRightManagerI::setUserRight(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
		
	m_getUserRight(uid)->setProperties(props);
}

UserRightDataIPtr UserRightManagerI::m_getUserRight(int userId)
{
	if (isValid()) {
		UserRightDataIPtr res = ServiceI::instance().findObject<UserRightDataIPtr>(USER_RIGHT, userId);
		if (res) {
			return res;
		} else {
			TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERRIGHT, userId));
			MCE_WARN("UserRightManagerI::m_getUserRight NoSuchUserRightException:: reason - cache miss [" << userId << "]");
			throw NoSuchObjectException();
		}
	} else {
		try{
			return ServiceI::instance().locateObject<UserRightDataIPtr>(USER_RIGHT, userId);
		} catch(const MyUtil::NoSuchObjectException& e) {
			MCE_WARN("UserRightManagerI::m_getUserRight NoSuchUserRightException:: reason - NoSuchUserRightException [" << userId << "]");
			throw NoSuchObjectException();
		}
	}
}


//-------------------------UserLoginCountManagerI-------------------------------

int UserLoginCountManagerI::getUserLoginCount(Ice::Int uid,
                                           const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return (m_getUserLoginCount(uid))->getLogincount();
}

void UserLoginCountManagerI::incUserLoginCount(Ice::Int uid,
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	m_getUserLoginCount(uid)->incLogincount();
}

UserLoginCountDataIPtr UserLoginCountManagerI::m_getUserLoginCount(int userId)
{
	if (isValid()) {
		UserLoginCountDataIPtr res = ServiceI::instance().findObject<UserLoginCountDataIPtr>(USER_LOGINCOUNT, userId);
		if (res) {
			return res;
		} else {
			TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERLOGINCOUNT, userId));
			MCE_WARN("UserLoginCountManagerI::m_getUserLoginCount NoSuchUserLoginCountException:: reason - cache miss [" << userId << "]");
			throw NoSuchObjectException();
		}
	} else {
		try{
			return ServiceI::instance().locateObject<UserLoginCountDataIPtr>(USER_LOGINCOUNT, userId);
		} catch(const MyUtil::NoSuchObjectException& e) {
			MCE_WARN("UserLoginCountManagerI::m_getUserLoginCount NoSuchUserLoginCountException:: reason - NoSuchUserLoginCountException [" << userId << "]");
			throw NoSuchObjectException();
		}
	}
}



//----------------UserUrlManager--------------------
UserUrlDataPtr UserUrlManagerI::getUserUrl(Ice::Int id, const Ice::Current& current) {
	ostringstream inMsg;
	inMsg<<"id="<<id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	return m_getUserUrl(id)->getData();
}

void UserUrlManagerI::setUserUrl(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current & current) {
	ostringstream inMsg;
	inMsg<<"id="<<id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
		
	UserUrlDataIPtr result = m_getUserUrl(id);
	result->setProperities(props);
}

UserUrlDataIPtr UserUrlManagerI::m_getUserUrl(int userId)
{
	if (isValid()) {
		UserUrlDataIPtr res = ServiceI::instance().findObject<UserUrlDataIPtr>(USER_URL, userId);
		if (res) {
			return res;
		} else {
			TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERURL, userId));
			MCE_WARN("UserUrlManagerI::m_getUserUrl NoSuchUserUrlException:: reason - cache miss [" << userId << "]");
			throw NoSuchObjectException();
		}
	} else {
		try{
			return ServiceI::instance().locateObject<UserUrlDataIPtr>(USER_URL, userId);
		} catch(const MyUtil::NoSuchObjectException& e) {
			MCE_WARN("UserUrlManagerI::m_getUserUrl NoSuchUserUrlException:: reason - NoSuchUserUrlException [" << userId << "]");
			throw NoSuchObjectException();
		}
	}
}
//-------------------------UserBasic---------------------------

UserBasicDataNPtr UserBasicManagerI::getUserBasic(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	return m_getUserBasic(userId)->getData();
}

UserBasicDataIPtr UserBasicManagerI::m_getUserBasic(int userId) { 
	
	UserBasicDataIPtr result = new UserBasicDataI;
	
	UserPassportDataIPtr  upd;
	UserStateDataIPtr     usd; 
	UserUrlDataIPtr       uud;
	UserNameDataIPtr      und;
	UserBornDataIPtr      ubd;
	UserStageDataIPtr     usgd; 
	UserDoingDataIPtr     udd;

	if (isValid()) {
		upd = ServiceI::instance().findObject<UserPassportDataIPtr> (USER_PASSPORT, userId); 
		usd = ServiceI::instance().findObject<UserStateDataIPtr> (USER_STATE, userId); 
		und = ServiceI::instance().findObject<UserNameDataIPtr> (USER_NAME, userId); 
		uud = ServiceI::instance().findObject<UserUrlDataIPtr> (USER_URL, userId); 
		ubd  = ServiceI::instance().findObject<UserBornDataIPtr> (USER_BORN, userId); 
		usgd = ServiceI::instance().findObject<UserStageDataIPtr> (USER_STAGE, userId); 
		udd = ServiceI::instance().findObject<UserDoingDataIPtr> (USER_DOING, userId);
		if (upd && usd && und && uud && ubd && usgd && udd) {
			upd = upd->getData();
			usd = usd->getData();
			und = und->getData();
			uud = uud->getData();
			ubd = ubd->getData();
			usgd = usgd->getData();
			udd = udd->getData();
		} else {
			TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERBASIC, userId));
			MCE_WARN("UserBasicManagerI::m_getUserBasic NoSuchUserBasicException:: reason - cache miss [" << userId << "]");
			throw NoSuchObjectException();
		}
	} else {
		try {
			upd = ServiceI::instance().locateObject<UserPassportDataIPtr> (USER_PASSPORT, userId)->getData(); 
		} catch(...) {
			MCE_WARN("UserBasicManagerI::m_getUserBasic NoSuchUserPassportException:: reason - NoSuchUserPassportException [" << userId << "]");
			throw NoSuchObjectException();
	}

		try {
			usd = ServiceI::instance().locateObject<UserStateDataIPtr> (USER_STATE, userId)->getData(); 
		} catch(...) {
			MCE_WARN("UserBasicManagerI::m_getUserBasic NoSuchUserStateException:: reason - NoSuchUserStateException [" << userId << "]");
			throw NoSuchObjectException();
		}
		try {
			und = ServiceI::instance().locateObject<UserNameDataIPtr> (USER_NAME, userId)->getData(); 
		} catch(...) {
			MCE_WARN("UserBasicManagerI::m_getUserBasic NoSuchUserNameException:: reason - NoSuchUserNameException [" << userId << "]");
			throw NoSuchObjectException();
		}
		
		try {	
			uud = ServiceI::instance().locateObject<UserUrlDataIPtr> (USER_URL, userId)->getData(); 
		} catch(...) {
			MCE_WARN("UserBasicManagerI::m_getUserBasic NoSuchUserUrlException:: reason - NoSuchUserUrlException [" << userId << "]");
			throw NoSuchObjectException();
		}
		
		try {
			ubd  = ServiceI::instance().locateObject<UserBornDataIPtr> (USER_BORN, userId)->getData(); 
		} catch(...) {
			MCE_WARN("UserBasicManagerI::m_getUserBasic NoSuchUserBornException:: reason - NoSuchUserBornException [" << userId << "]");
			throw NoSuchObjectException();
		}

		try {
			usgd = ServiceI::instance().locateObject<UserStageDataIPtr> (USER_STAGE, userId)->getData(); 
		} catch(...) {
			MCE_WARN("UserBasicManagerI::m_getUserBasic NoSuchUserStageException:: reason - NoSuchUserStageException [" << userId << "]");
			throw NoSuchObjectException();
		}
	
		try {
			udd = ServiceI::instance().locateObject<UserDoingDataIPtr> (USER_DOING, userId)->getData(); 
		} catch(...) {
			MCE_WARN("UserBasicManagerI::m_getUserBasic NoSuchUserDoingException:: reason - NoSuchUserDoingException [" << userId << "]");
			throw NoSuchObjectException();
		}
	}

	parseData(result,ubd,usgd,udd,und,uud,upd,usd);
		
	return result;
}

void UserBasicManagerI::parseData(UserBasicDataIPtr& desData, const UserBornDataNPtr& bornData, 
		                          const UserStageDataPtr& stageData, const UserDoingDataPtr& doingData,
								  const UserNameDataIPtr& nameData, const UserUrlDataPtr&  urlData, 
								  const UserPassportDataIPtr&  passportData, const UserStateDataIPtr& stateData )
{
	//将userborn中的数据添加到userbasic对象中
	if(bornData)
	{
       desData->id = bornData->id ;
       desData->gender = bornData->gender ;
       desData->birthDay = bornData->birthDay ;
       desData->birthMonth = bornData->birthMonth ;
       desData->birthYear = bornData->birthYear ;
       desData->isLunarBirth = bornData->isLunarBirth;
       desData->homeProvince = bornData->homeProvince ;
       desData->homeCity = bornData->homeCity ;
       desData->cityCode = bornData->cityCode ;
       desData->astrology = bornData->astrology ;
	}
	
	//将userstage中的数据添加到userbasic对象中
	if(stageData)
	{
		desData->univ = stageData->univ ;
		desData->stage = stageData->stage ;
	}
	
	//将userdoing中的数据添加到userbasic对象中
	if(doingData)
	{
		desData->doing = doingData->doing;
		desData->doingTime = doingData->doingTime;
	}
	//将username中的数据添加到userbasic对象中
	if(nameData)
	{
       desData->name= nameData->name;
	}
	//将userurl中的数据添加到userbasic对象中
	if(urlData)
	{
       desData->headUrl = urlData->headUrl;
       desData->tinyUrl = urlData->tinyUrl;
	}
	//将userpassport中的数据添加到userbasic对象中
	if(passportData)
	{
       desData->status = passportData->status;
	}
	//将userstate中的数据添加到userbasic对象中
	if(stateData)
	{
       desData->star = stateData->star;
	}
}
//-------------------------UserBorn---------------------------

UserBornDataNPtr UserBornManagerI::getUserBorn(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	return m_getUserBorn(userId)->getData();
}

void UserBornManagerI::setUserBorn(Ice::Int userId,const Str2StrMap& props,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	m_getUserBorn(userId)->setProperties(props);
}

UserBornDataIPtr UserBornManagerI::m_getUserBorn(int userId) {
	if (isValid()) {
		UserBornDataIPtr res = ServiceI::instance().findObject<UserBornDataIPtr> (USER_BORN, userId);
		if (res) {
			return res;
		} else {
			TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERBORN, userId));
			MCE_WARN("UserBornManagerI::m_getUserBorn NoSuchUserBornException:: reason - cache miss [" << userId << "]");
			throw NoSuchObjectException();
		}
	} else {
		try{
			return ServiceI::instance().locateObject<UserBornDataIPtr> (USER_BORN, userId);
		} catch(const MyUtil::NoSuchObjectException& e) {
			MCE_WARN("UserBornManagerI::m_getUserBorn NoSuchUserBornException:: reason - NoSuchUserBornException [" << userId << "]");
			throw NoSuchObjectException();
		}
	}
}

//-------------------------UserStage---------------------------

UserStageDataPtr UserStageManagerI::getUserStage(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	return m_getUserStage(userId)->getData();
}

void UserStageManagerI::setUserStage(Ice::Int userId,const Str2StrMap& props,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	m_getUserStage(userId)->setProperties(props);
}

UserStageDataIPtr UserStageManagerI::m_getUserStage(int userId) {
	if (isValid()) {
		UserStageDataIPtr res = ServiceI::instance().findObject<UserStageDataIPtr> (USER_STAGE, userId);
		if (res) {
			return res;
		} else {
			TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERSTAGE, userId));
			MCE_WARN("UserStageManagerI::m_getUserStage NoSuchUserStageException:: reason - cache miss [" << userId << "]");
			throw NoSuchObjectException();
		}
	} else {
		try{
			return ServiceI::instance().locateObject<UserStageDataIPtr> (USER_STAGE, userId);
		} catch(const MyUtil::NoSuchObjectException& e) {
			MCE_WARN("UserStageManagerI::m_getUserStage NoSuchUserStageException:: reason - NoSuchUserStageException [" << userId << "]");
			throw NoSuchObjectException();
		}
	}
}

//-------------------------UserDoing---------------------------

UserDoingDataPtr UserDoingManagerI::getUserDoing(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);

	return m_getUserDoing(userId)->getData();
}

void UserDoingManagerI::setUserDoing(Ice::Int userId,const Str2StrMap& props,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
		
	m_getUserDoing(userId)->setProperties(props);
}

UserDoingDataIPtr UserDoingManagerI::m_getUserDoing(int userId) {
	if (isValid()) {
		UserDoingDataIPtr res = ServiceI::instance().findObject<UserDoingDataIPtr> (USER_DOING, userId);
		if (res) {
			return res;
		} else {
			TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERDOING, userId));
			MCE_WARN("UserDoingManagerI::m_getUserDoing NoSuchUserDoingException:: reason - cache miss [" << userId << "]");
			throw NoSuchObjectException();
		}
	} else {
		try{
			return ServiceI::instance().locateObject<UserDoingDataIPtr> (USER_DOING, userId);
		} catch(const MyUtil::NoSuchObjectException& e) {
			MCE_WARN("UserDoingManagerI::m_getUserDoing NoSuchUserDoingException:: reason - NoSuchUserDoingException [" << userId << "]");
			throw NoSuchObjectException();
		}
	}
}
//-------------------------UserBaseVeiwManagerI---------------------------

UserBaseViewDataNPtr UserBaseViewManagerI::getUserBaseView( Ice::Int uid,
		                                               const Ice::Current& current) 
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	bool flag = MyUtil::RpcFilterManager::instance().filter(current, "user_passport");
	
	UserPassportDataIPtr    upd ;
	UserNameDataIPtr        und ;
	UserRightDataIPtr      urd  ;
	UserLoginCountDataIPtr ulcd  ;
	UserStateDataIPtr usd  ;
	UserUrlDataIPtr uurld  ;

	UserBasicDataIPtr  ubd ;
	
	if (isValid()) {
			upd = ServiceI::instance().findObject<UserPassportDataIPtr>(USER_PASSPORT, uid);
			und = ServiceI::instance().findObject<UserNameDataIPtr>(USER_NAME, uid);
			urd = ServiceI::instance().findObject<UserRightDataIPtr>(USER_RIGHT, uid);
			ulcd = ServiceI::instance().findObject<UserLoginCountDataIPtr>(USER_LOGINCOUNT, uid);
			usd = ServiceI::instance().findObject<UserStateDataIPtr>(USER_STATE, uid);
			uurld = ServiceI::instance().findObject<UserUrlDataIPtr> (USER_URL, uid);

			//拆分userbasic表之后，为原来userbasic对象作的兼容，将拆分后的对象拼成原来的userbasic对象
			ubd = parseToUserBasicData(uid, und, uurld, upd, usd);

			if (upd && und && urd && ulcd && usd && uurld && ubd) {
				upd = upd->getData();
				und = und->getData();
				urd = urd->getData();
				ulcd = ulcd->getData();
				usd = usd->getData();
				uurld = uurld->getData();
			} else {
				TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERBASEFULLVIEW, uid));
				MCE_WARN("UserBaseViewManagerI::getUserBaseView NoSuchUserBaseViewException:: reason - cache miss [" << uid << "]");
				throw NoSuchObjectException();
			}
	} else {
		try {
			upd = ServiceI::instance().locateObject<UserPassportDataIPtr>(USER_PASSPORT, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserBaseView NoSuchUserBaseViewException:: reason - NoSuchUserPassportException [" << uid << "]");
			throw NoSuchObjectException();
		}

		try {
			und = ServiceI::instance().locateObject<UserNameDataIPtr>(USER_NAME, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserBaseView NoSuchUserBaseViewException:: reason - NoSuchUserNameException [" << uid << "]");
			throw NoSuchObjectException();
		}
	
		try {
			urd = ServiceI::instance().locateObject<UserRightDataIPtr>(USER_RIGHT, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserBaseView NoSuchUserBaseViewException:: reason - NoSuchUserRightException [" << uid << "]");
			throw NoSuchObjectException();
		}
	
		try {
			ulcd = ServiceI::instance().locateObject<UserLoginCountDataIPtr>(USER_LOGINCOUNT, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserBaseView NoSuchUserBaseViewException:: reason - NoSuchUserLoginCountException [" << uid << "]");
			throw NoSuchObjectException();
		}
	
		try {
			usd = ServiceI::instance().locateObject<UserStateDataIPtr>(USER_STATE, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserBaseView NoSuchUserBaseViewException:: reason - NoSuchUserStateException [" << uid << "]");
			throw NoSuchObjectException();
		}
	
		try {
			uurld = ServiceI::instance().locateObject<UserUrlDataIPtr> (USER_URL, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserBaseView NoSuchUserBaseViewException:: reason - NoSuchUserUrlException [" << uid << "]");
			throw NoSuchObjectException();
		}
	

		//拆分userbasic表之后，为原来userbasic对象作的兼容，将拆分后的对象拼成原来的userbasic对象
		ubd = parseToUserBasicData(uid, und, uurld, upd, usd);
	}
	

	UserBaseViewDataIPtr res = new UserBaseViewDataI(upd, und, urd, ulcd, usd,uurld,ubd);
  if (!flag) {
    res->upd->account="";
    res->upd->passwordmd5="";
  }
	return res; 
}


UserBasicDataIPtr UserBaseViewManagerI::parseToUserBasicData(const int uid,
		                                                 const UserNameDataIPtr& nameData, 
		                                                 const UserUrlDataPtr&  urlData, 
														 const UserPassportDataIPtr&  passportData, 
														 const UserStateDataIPtr& stateData )
{
	UserBasicDataIPtr res = new UserBasicDataI;

	UserBornDataIPtr      ubod;
	UserStageDataIPtr     usgd; 
	UserDoingDataIPtr     udd;
	
	if (isValid()) {
		ubod  = ServiceI::instance().findObject<UserBornDataIPtr> (USER_BORN, uid); 
		usgd = ServiceI::instance().findObject<UserStageDataIPtr> (USER_STAGE, uid); 
		udd = ServiceI::instance().findObject<UserDoingDataIPtr> (USER_DOING, uid);
		if (ubod && usgd && udd) {
			ubod = ubod->getData();
			usgd = usgd->getData();
			udd = udd->getData();
		} else {
			return 0;
		}
	} else {
		try {
			ubod  = ServiceI::instance().locateObject<UserBornDataIPtr> (USER_BORN, uid)->getData(); 
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserBaseView(parseToUserBasicData) NoSuchUserBaseViewException:: reason - NoSuchUserBornException [" << uid << "]");
			throw NoSuchObjectException();
		}

		try {
			usgd = ServiceI::instance().locateObject<UserStageDataIPtr> (USER_STAGE, uid)->getData(); 
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserBaseView(parseToUserBasicData) NoSuchUserBaseViewException:: reason - NoSuchUserStageException [" << uid << "]");
			throw NoSuchObjectException();
		}
	
		try {
			udd = ServiceI::instance().locateObject<UserDoingDataIPtr> (USER_DOING, uid)->getData(); 
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserBaseView(parseToUserBasicData) NoSuchUserBaseViewException:: reason - NoSuchUserDoingException [" << uid << "]");
			throw NoSuchObjectException();
		}
	}

	//将userborn中的数据添加到userbasic对象中
	if(ubod)
	{
       res->id = ubod->id ;
       res->gender = ubod->gender ;
       res->birthDay = ubod->birthDay ;
       res->birthMonth = ubod->birthMonth ;
       res->birthYear = ubod->birthYear ;
       res->isLunarBirth = ubod->isLunarBirth;
       res->homeProvince = ubod->homeProvince ;
       res->homeCity = ubod->homeCity ;
       res->cityCode = ubod->cityCode ;
       res->astrology = ubod->astrology ;
	}
	
	//将userstage中的数据添加到userbasic对象中
	if(usgd)
	{
		res->univ = usgd->univ ;
		res->stage = usgd->stage ;
	}
	
	//将userdoing中的数据添加到userbasic对象中
	if(udd)
	{
		res->doing = udd->doing;
		res->doingTime = udd->doingTime;
	}
	//将username中的数据添加到userbasic对象中
	if(nameData)
	{
       res->name= nameData->name;
	}
	//将userurl中的数据添加到userbasic对象中
	if(urlData)
	{
       res->headUrl = urlData->headUrl;
       res->tinyUrl = urlData->tinyUrl;
	}
	//将userpassport中的数据添加到userbasic对象中
	if(passportData)
	{
       res->status = passportData->status;
	}
	//将userstate中的数据添加到userbasic对象中
	if(stateData)
	{
       res->star = stateData->star;
	}

	return res;
}

UserCommonViewDataPtr UserBaseViewManagerI::getUserCommonView(int uid, const Ice::Current& current) 
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_DEBUG);
	
	bool flag = MyUtil::RpcFilterManager::instance().filter(current, "user_passport");

	UserPassportDataIPtr    upd ;
	UserNameDataIPtr        und ;
	UserRightDataIPtr      urd  ;
	UserLoginCountDataIPtr ulcd  ;
	UserStateDataIPtr usd  ;
	UserUrlDataIPtr uurld  ;
	UserBornDataIPtr  ubod ;
	UserStageDataIPtr usgd ;

	if (isValid()) {
			upd = ServiceI::instance().findObject<UserPassportDataIPtr>(USER_PASSPORT, uid);
			und = ServiceI::instance().findObject<UserNameDataIPtr>(USER_NAME, uid);
			urd = ServiceI::instance().findObject<UserRightDataIPtr>(USER_RIGHT, uid);
			ulcd = ServiceI::instance().findObject<UserLoginCountDataIPtr>(USER_LOGINCOUNT, uid);
			usd = ServiceI::instance().findObject<UserStateDataIPtr>(USER_STATE, uid);
			uurld = ServiceI::instance().findObject<UserUrlDataIPtr> (USER_URL, uid);
			ubod = ServiceI::instance().findObject<UserBornDataIPtr> (USER_BORN, uid);
			usgd = ServiceI::instance().findObject<UserStageDataIPtr> (USER_STAGE, uid);

			if (upd && und && urd && ulcd && usd && uurld && ubod && usgd) {
				upd = upd->getData();
				und = und->getData();
				urd = urd->getData();
				ulcd = ulcd->getData();
				uurld = uurld->getData();
				ubod = ubod->getData();
				usgd = usgd->getData();
			} else {
				TaskManager::instance().execute(new LoadTask(LOAD_TYPE_USERBASECOMMONVIEW, uid));
				MCE_WARN("UserBaseViewManagerI::getUserCommonView NoSuchUserCommonViewException:: reason - cache miss [" << uid << "]");
				throw NoSuchObjectException();
			}
	} else {
		try{
			upd = ServiceI::instance().locateObject<UserPassportDataIPtr>(USER_PASSPORT, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserCommonView NoSuchUserCommonViewException:: reason - NoSuchUserPassportException [" << uid << "]");
			throw NoSuchObjectException();
		}

		try{
			und = ServiceI::instance().locateObject<UserNameDataIPtr>(USER_NAME, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserCommonView NoSuchUserCommonViewException:: reason - NoSuchUserNameException [" << uid << "]");
			throw NoSuchObjectException();
		}
	
		try{
			urd = ServiceI::instance().locateObject<UserRightDataIPtr>(USER_RIGHT, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserCommonView NoSuchUserCommonViewException:: reason - NoSuchUserRightException [" << uid << "]");
			throw NoSuchObjectException();
		}
	
		try{
			ulcd = ServiceI::instance().locateObject<UserLoginCountDataIPtr>(USER_LOGINCOUNT, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserCommonView NoSuchUserCommonViewException:: reason - NoSuchUserLoginCountException [" << uid << "]");
			throw NoSuchObjectException();
		}
	
		try{
			usd = ServiceI::instance().locateObject<UserStateDataIPtr>(USER_STATE, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserCommonView NoSuchUserCommonViewException:: reason - NoSuchUserStateException [" << uid << "]");
			throw NoSuchObjectException();
		}
	
		try{
			uurld = ServiceI::instance().locateObject<UserUrlDataIPtr> (USER_URL, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserCommonView NoSuchUserCommonViewException:: reason - NoSuchUserUrlException [" << uid << "]");
			throw NoSuchObjectException();
		}
	
		try{
			ubod = ServiceI::instance().locateObject<UserBornDataIPtr> (USER_BORN, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserCommonView NoSuchUserCommonViewException:: reason - NoSuchUserBornException [" << uid << "]");
			throw NoSuchObjectException();
		}
	
		try{
			usgd = ServiceI::instance().locateObject<UserStageDataIPtr> (USER_STAGE, uid)->getData();
		} catch(...) {
			MCE_WARN("UserBaseViewManagerI::getUserCommonView NoSuchUserCommonViewException:: reason - NoSuchUserStageException [" << uid << "]");
			throw NoSuchObjectException();
		}
	}
	
	UserCommonViewDataPtr res = new UserCommonViewData();
	res->userpassport = upd;
	res->userright = urd;
	res->username = und;
	res->userlogincount = ulcd;
	res->userstate = usd;
	res->userurl = uurld;

	res->stage=usgd->stage;
	res->univ=usgd->univ;
	res->gender=ubod->gender;

  if (!flag) {
    res->userpassport->account = "";
    res->userpassport->passwordmd5 = "";
  }
	return res; 
}


//--------------------------------------------------------------------------------


void UserPassportManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		IceUtil::RWRecMutex::WLock lock(validMutex_);
		valid_ = valid;
		ServerStateSubscriber::instance().setStatus((int)valid,USER_PASSPORT_MANAGER);
	}
	UserBasicManagerI::instance().setValid(valid);
	UserBaseViewManagerI::instance().setValid(valid);
}
void UserPassportManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);
	TaskManager::instance().execute(new FillTask<UserPassportDataIPtr, UserPassportDataI>(USER_PASSPORT , userData));
}
bool UserPassportManagerI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);

	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}

void UserStateManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		IceUtil::RWRecMutex::WLock lock(validMutex_);
		valid_ = valid;
		ServerStateSubscriber::instance().setStatus((int)valid,USER_STATE_MANAGER);
	}
	UserBasicManagerI::instance().setValid(valid);
	UserBaseViewManagerI::instance().setValid(valid);
}
void UserStateManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);
	TaskManager::instance().execute(new FillTask<UserStateDataIPtr, UserStateDataI>(USER_STATE, userData));
}
bool UserStateManagerI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);

	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}

void UserLoginCountManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		IceUtil::RWRecMutex::WLock lock(validMutex_);
		valid_ = valid;
		ServerStateSubscriber::instance().setStatus((int)valid,USER_LOGINCOUNT_MANAGER);
	}
	UserBaseViewManagerI::instance().setValid(valid);
}
void UserLoginCountManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);
	TaskManager::instance().execute(new FillTask<UserLoginCountDataIPtr, UserLoginCountDataI>(USER_LOGINCOUNT, userData));
}
bool UserLoginCountManagerI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);

	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}

void UserRightManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		IceUtil::RWRecMutex::WLock lock(validMutex_);
		valid_ = valid;
		ServerStateSubscriber::instance().setStatus((int)valid,USER_RIGHT_MANAGER);
	}
	UserBaseViewManagerI::instance().setValid(valid);
}
void UserRightManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);
	TaskManager::instance().execute(new FillTask<UserRightDataIPtr, UserRightDataI>(USER_RIGHT, userData));
}
bool UserRightManagerI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);

	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}

void UserUrlManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		IceUtil::RWRecMutex::WLock lock(validMutex_);
		valid_ = valid;
		ServerStateSubscriber::instance().setStatus((int)valid,USER_URL_MANAGER);
	}
	UserBasicManagerI::instance().setValid(valid);
	UserBaseViewManagerI::instance().setValid(valid);
}
void UserUrlManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);
	TaskManager::instance().execute(new FillTask<UserUrlDataIPtr, UserUrlDataI>(USER_URL, userData));
}
bool UserUrlManagerI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);

	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}

void UserNameManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		IceUtil::RWRecMutex::WLock lock(validMutex_);
		valid_ = valid;
		ServerStateSubscriber::instance().setStatus((int)valid,USER_NAME_MANAGER);
	}
	UserBasicManagerI::instance().setValid(valid);
	UserBaseViewManagerI::instance().setValid(valid);
}
void UserNameManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);
	TaskManager::instance().execute(new FillTask<UserNameDataIPtr, UserNameDataI>(USER_NAME, userData));
}
bool UserNameManagerI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);

	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}

void UserBornManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);

	{
		IceUtil::RWRecMutex::WLock lock(validMutex_);
		valid_ = valid;
		ServerStateSubscriber::instance().setStatus((int)valid,USER_BORN_MANAGER);
	}
	UserBasicManagerI::instance().setValid(valid);
	UserBaseViewManagerI::instance().setValid(valid);
}
void UserBornManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);
	TaskManager::instance().execute(new FillTask<UserBornDataIPtr, UserBornDataI>(USER_BORN, userData));
}
bool UserBornManagerI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);

	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}

void UserStageManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		IceUtil::RWRecMutex::WLock lock(validMutex_);
		valid_ = valid;
		ServerStateSubscriber::instance().setStatus((int)valid,USER_STAGE_MANAGER);
	}
	UserBasicManagerI::instance().setValid(valid);
	UserBaseViewManagerI::instance().setValid(valid);
}
void UserStageManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);
	TaskManager::instance().execute(new FillTask<UserStageDataIPtr, UserStageDataI>(USER_STAGE, userData));
}
bool UserStageManagerI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);

	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}

void UserDoingManagerI::setValid(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		IceUtil::RWRecMutex::WLock lock(validMutex_);
		valid_ = valid;
		ServerStateSubscriber::instance().setStatus((int)valid,USER_DOING_MANAGER);
	}
	UserBasicManagerI::instance().setValid(valid);
	UserBaseViewManagerI::instance().setValid(valid);
}
void UserDoingManagerI::setData(const ObjectResultPtr& userData, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);
	TaskManager::instance().execute(new FillTask<UserDoingDataIPtr, UserDoingDataI>(USER_DOING, userData));
}
bool UserDoingManagerI::isValid(const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);

	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}

void UserBasicManagerI::setValidForce(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);

	{
		IceUtil::RWRecMutex::WLock lock(validMutex_);
		valid_ = valid;
		ServerStateSubscriber::instance().setStatus((int)valid,USER_BASIC_MANAGER);
	}
}

void UserBasicManagerI::setValid(bool valid) {
	IceUtil::RWRecMutex::WLock lock(validMutex_);
	if (UserPassportManagerI::instance().isValid() 
		&& UserStateManagerI::instance().isValid() 
		&& UserUrlManagerI::instance().isValid()
		&& UserNameManagerI::instance().isValid()
		&& UserBornManagerI::instance().isValid()
		&& UserStageManagerI::instance().isValid()
		&& UserDoingManagerI::instance().isValid()) {
		valid_ = true;
	} else {
		valid_ = false;
	}
}

bool UserBasicManagerI::isValid(const ::Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);

	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}

void UserBaseViewManagerI::setValidForce(bool valid, const Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current, boost::lexical_cast<string>(valid), InvokeClient::INVOKE_LEVEL_INFO);
	
	{
		IceUtil::RWRecMutex::WLock lock(validMutex_);
		valid_ = valid;
		ServerStateSubscriber::instance().setStatus((int)valid,USER_BASEVIEW_MANAGER);
	}
}

void UserBaseViewManagerI::setValid(bool valid) {
	IceUtil::RWRecMutex::WLock lock(validMutex_);
	UserPassportDataIPtr    upd ;
	UserNameDataIPtr        und ;
	UserRightDataIPtr      urd  ;
	UserLoginCountDataIPtr ulcd  ;
	UserStateDataIPtr usd  ;
	UserUrlDataPtr uurld  ;
	UserBornDataNPtr  ubod ;
	UserStageDataPtr usgd ;
	if (UserPassportManagerI::instance().isValid()
		&& UserNameManagerI::instance().isValid()
		&& UserRightManagerI::instance().isValid()
		&& UserLoginCountManagerI::instance().isValid()
		&& UserStateManagerI::instance().isValid()
		&& UserUrlManagerI::instance().isValid()
		&& UserBornManagerI::instance().isValid()
		&& UserStageManagerI::instance().isValid()
		&& UserDoingManagerI::instance().isValid()) {
		valid_ = true;
	} else {
		valid_ = false;
	}
}
bool UserBaseViewManagerI::isValid(const ::Ice::Current& current) {
	InvokeClient ic = InvokeClient::create(current);

	IceUtil::RWRecMutex::RLock lock(validMutex_);
	return valid_;
}

void LoadTask::handle() {
	switch (type_) {
		case LOAD_TYPE_USERPASSPORT:
			LoadUserPassport();
			break;
		case LOAD_TYPE_USERRIGHT:
			LoadUserRight();
			break;
		case LOAD_TYPE_USERSTATE:
			LoadUserState();
			break;
		case LOAD_TYPE_USERNAMES:
			LoadUserNames();
			break;
		case LOAD_TYPE_USERLOGINCOUNT:
			LoadUserLoginCount();
			break;
		case LOAD_TYPE_USERURL:
			LoadUserUrl();
			break;
		case LOAD_TYPE_USERBORN:
			LoadUserBorn();
			break;
		case LOAD_TYPE_USERSTAGE:
			LoadUserStage();
			break;
		case LOAD_TYPE_USERDOING:
			LoadUserDoing();
			break;
		case LOAD_TYPE_USERBASIC:
			LoadUserBasic();
			break;
		case LOAD_TYPE_USERBASECOMMONVIEW:
			LoadUserBaseCommonView();
			break;
		case LOAD_TYPE_USERBASEFULLVIEW:
			LoadUserBaseFullView();
			break;
		default:
			MCE_WARN("LoadTask::handle Unknown type: "<<type_);
	}
}

void LoadTask::LoadUserPassport() {
	try {
		ServiceI::instance().locateObject<UserPassportDataIPtr>(USER_PASSPORT, userid_);
		UserPassportWriterAdapter::instance().reload(userid_);
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("LoadTask::LoadUserPassport -> NoSuchUserPassportException ["<< userid_<<"]");
	}
}

void LoadTask::LoadUserRight() {
	try {
		ServiceI::instance().locateObject<UserRightDataIPtr>(USER_RIGHT, userid_);
		UserRightWriterAdapter::instance().reload(userid_);
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("LoadTask::LoadUserRight -> NoSuchUserRightException ["<<userid_<<"]");
	}
}

void LoadTask::LoadUserState() {
	try {
		ServiceI::instance().locateObject<UserStateDataIPtr>(USER_STATE, userid_);
		UserStateWriterAdapter::instance().reload(userid_);
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("LoadTask::LoadUserState -> NoSuchUserStateException ["<<userid_<<"]");
	}
}

void LoadTask::LoadUserNames() {
	try {
		ServiceI::instance().locateObject<UserNameDataIPtr>(USER_NAME, userid_);
		UserNameWriterAdapter::instance().reload(userid_);
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("LoadTask::LoadUserNames -> NoSuchUserNamesException ["<<userid_<<"]");
	}
}

void LoadTask::LoadUserLoginCount() {
	try {
		ServiceI::instance().locateObject<UserLoginCountDataIPtr>(USER_LOGINCOUNT, userid_);
		UserLoginCountWriterAdapter::instance().reload(userid_);
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("LoadTask::LoadUserLoginCount -> NoSuchUserLoginCountException ["<<userid_<<"]");
	}
}

void LoadTask::LoadUserUrl() {
	try {
		ServiceI::instance().locateObject<UserUrlDataIPtr>(USER_URL, userid_);
		UserUrlWriterAdapter::instance().reload(userid_);
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("LoadTask::LoadUserUrl -> NoSuchUserUrlException ["<<userid_<<"]");
	}
}

void LoadTask::LoadUserBorn() {
	try {
		ServiceI::instance().locateObject<UserBornDataIPtr> (USER_BORN, userid_);
		UserBornWriterAdapter::instance().reload(userid_);
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("LoadTask::LoadUserBorn -> NoSuchUserBornException ["<<userid_<<"]");
	}
}

void LoadTask::LoadUserStage() {
	try {
		ServiceI::instance().locateObject<UserStageDataIPtr> (USER_STAGE, userid_);
		UserStageWriterAdapter::instance().reload(userid_);
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("LoadTask::LoadUserStage -> NoSuchUserStageException ["<<userid_<<"]");
	}
}

void LoadTask::LoadUserDoing() {
	try {
		ServiceI::instance().locateObject<UserDoingDataIPtr> (USER_DOING, userid_);
		UserDoingWriterAdapter::instance().reload(userid_);
	} catch (const MyUtil::NoSuchObjectException& e) {
		MCE_WARN("LoadTask::LoadUserDoing -> NoSuchUserDoingException ["<<userid_<<"]");
	}
}

void LoadTask::LoadUserBasic() {
	LoadUserPassport();
	LoadUserState();
	LoadUserUrl();
	LoadUserNames();
	LoadUserBorn();
	LoadUserStage();
	LoadUserDoing();
}

void LoadTask::LoadUserBaseCommonView() {
	LoadUserPassport();
	LoadUserNames();
	LoadUserRight();
	LoadUserLoginCount();
	LoadUserState();
	LoadUserUrl();
	LoadUserBorn();
	LoadUserStage();
}

void LoadTask::LoadUserBaseFullView() {
	LoadUserPassport();
	LoadUserNames();
	LoadUserRight();
	LoadUserLoginCount();
	LoadUserState();
	LoadUserUrl();
	LoadUserBorn();
	LoadUserStage();
	LoadUserDoing();
}


