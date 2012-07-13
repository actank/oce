#include "UserBaseI.h"
#include "LogWrapper.h"
#include "BatchUpdaterAdapter.h"
#include "UserLogicAdapter.h"
#include "Date.h"
#include "QueryRunner.h"

#include "user/UserBase/hotback/data/UserBaseDataI.cpp"

#include "time.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "util/cpp/TimeCost.h"
#include "Monitor/client/service/ServiceMonitor.h"
#include "boost/lexical_cast.hpp"

#include "UserPassportReaderAdapter.h"
#include "UserNameReaderAdapter.h"
#include "UserStateReaderAdapter.h"
#include "UserRightReaderAdapter.h"
#include "UserLoginCountReaderAdapter.h"
#include "UserUrlReaderAdapter.h"
#include "UserBornReaderAdapter.h"
#include "UserStageReaderAdapter.h"
#include "UserDoingReaderAdapter.h"

using namespace com::xiaonei::xce;
using namespace mop::hi::svc::adapter;
using namespace xce::userbase;
using namespace MyUtil;
using namespace xce::serverstate;

using xce::adapter::userpassport::UserPassportReaderAdapter;
using xce::adapter::username::UserNameReaderAdapter;
using xce::adapter::userstate::UserStateReaderAdapter;
using xce::adapter::userright::UserRightReaderAdapter;
using xce::adapter::userlogincount::UserLoginCountReaderAdapter;
using xce::adapter::userurl::UserUrlReaderAdapter;
using xce::adapter::userborn::UserBornReaderAdapter;
using xce::adapter::userstage::UserStageReaderAdapter;
using xce::adapter::userdoing::UserDoingReaderAdapter;
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
	TimeCost tc = TimeCost::create(inMsg.str());

	
	int id;
	Str2StrMap::const_iterator it = props.find(CUPID);
	if(it!=props.end())
	{
		id=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		MCE_WARN("UserPassportManagerI::createUserPassport CreateUserPassportException::Can find userID in props");
		return;
	}

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, id);
	
		QueryRunner( DB_USER_PASSPORT , CDbWServer ).execute( Statement::buildInsert( TABLE_USER_PASSPORT, props ) );
	
	
		ostringstream idMsg;
		idMsg<<" id="<<id;
		tc.step(idMsg.str());
	
		MCE_INFO("UserPassportManagerI::createUserPassport Loading " << id);
		
		UserPassportDataIPtr rs = new UserPassportDataI ;
		Statement sql;
		sql << "SELECT id, account, domain, password_md5, status, safe_status FROM user_passport WHERE id = '"<<id<<"'";
		BatchPassportResHandlerI handler(rs);
		size_t resultSize = QueryRunner(DB_USER_PASSPORT, CDbWServer ).query(sql, handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserPassportManagerI::createUserPassport CreateUserPassportException::Can not load new added user ,userid  [" << id << "]");
			return ;
		}
		
		if (rs->uID != id) {
			MCE_WARN("UserPassportManagerI::createUserPassport CreateUserPassportException::UserId error,userid = " << id << ", resultID = "<< rs->uID);
			return ;
		}
	
		MCE_INFO( "UserPassportManagerI::createUserPassport Loading " << id << " done.");
	
		UserPassportReaderAdapter::instance().setData(id, rs);
	}
	
	UserLogicAdapter::instance().insertUserPassport(id, props);
}

void UserPassportManagerI::setUserPassport(Ice::Int uid,
		                                   const MyUtil::Str2StrMap& props, 
										   const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(props).find("ID");
	if(it!=props.end())
	{
		const_cast<Str2StrMap&>(props).erase(it);
	}

	Str2StrMap filter;
	filter[CUPID] = boost::lexical_cast<string>(uid);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, uid);
		QueryRunner( DB_USER_PASSPORT, CDbWServer ).execute( Statement::buildUpdate( TABLE_USER_PASSPORT , filter ,  props ) ); 

		UserPassportReaderAdapter::instance().setUserPassport(uid, props);
	}

	UserLogicAdapter::instance().updateUserPassport(uid, props, current.ctx);
}

//reload for us
void UserPassportManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);
		UserPassportDataIPtr rs = new UserPassportDataI ;
		Statement sql;
		sql << "SELECT id, account, domain, password_md5, status, safe_status FROM user_passport WHERE id = '"<<userId<<"'";
		BatchPassportResHandlerI handler(rs);
		size_t resultSize = QueryRunner(DB_USER_PASSPORT, CDbRServer ).query(sql, handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserPassportManagerI::reload CreateUserPassportException::Can not load new added user ,userid  [" << userId << "]");
			return ;
		}
		
		if (rs->uID != userId) {
			MCE_WARN("UserPassportManagerI::reload CreateUserPassportException::UserId error,userid = " << userId << ", resultID = "<< rs->uID);
			return ;
		}
	
		UserPassportReaderAdapter::instance().setData(userId, rs);
	}

}

//-------------------------UserNameManagerI-------------------------------

void UserNameManagerI::createUserName(const Str2StrMap& props,
		                              const Ice::Current& current)
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
	
	ostringstream inMsg;
	inMsg<<"UserNameManagerI::createUserName" <<  " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	
	int id;
	Str2StrMap::const_iterator it = props.find(CUNID);
	if(it!=props.end())
	{
		id=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		MCE_WARN("UserNameManagerI::createUserName CreateUserNameException::Can find userID in props");
		return;
	}

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, id);
		QueryRunner( DB_USER_NAME , CDbWServer ).execute( Statement::buildInsert( TABLE_USER_NAME, props ) );
		
		ostringstream idMsg;
		idMsg<<" id="<<id;
		tc.step(idMsg.str());
		
		MCE_INFO("UserNameManagerI::createUserName Loading " << id);
		
		
		UserNameDataIPtr rs = new UserNameDataI ;
		BatchNameResHandlerI handler(rs);
		Statement sql;
		sql << "SELECT id, name, nickname FROM user_names WHERE id = '"<<id<<"'";
		size_t resultSize = QueryRunner(DB_USER_NAME, CDbWServer ).query(sql,handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserNameManagerI::createUserName CreateUserNameException::Can not load new added user ,userid  [" << id << "]");
			return ;
		}
		if (rs->uID != id) {
			MCE_WARN("UserNameManagerI::createUserName CreateUserNameException::UserId error,userid = " << id << ", resultID = "<< rs->uID);
			return ;
		}
	
		MCE_INFO( "UserNameManagerI::createUserName Loading " << id << " done.");
	
		UserNameReaderAdapter::instance().setData(id, rs);
	}
	UserLogicAdapter::instance().insertUserNames(id, props);
}

void UserNameManagerI::setUserName(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(props).find("ID");
	if(it!=props.end())
	{
		const_cast<Str2StrMap&>(props).erase(it);
	}

	Str2StrMap filter;
	filter[CUNID] = boost::lexical_cast<string>(uid);
	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, uid);
		QueryRunner( DB_USER_NAME, CDbWServer ).execute( Statement::buildUpdate( TABLE_USER_NAME , filter ,  props ) );

		UserNameReaderAdapter::instance().setUserName(uid, props);
	}

	
	UserLogicAdapter::instance().updateUserNames(uid, props, current.ctx);
}

//reload for us
void UserNameManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);
		UserNameDataIPtr rs = new UserNameDataI ;
		BatchNameResHandlerI handler(rs);
		Statement sql;
		sql << "SELECT id, name, nickname FROM user_names WHERE id = '"<<userId<<"'";
		size_t resultSize = QueryRunner(DB_USER_NAME, CDbRServer ).query(sql,handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserNameManagerI::reload CreateUserNameException::Can not load new added user ,userid  [" << userId << "]");
			return ;
		}
		if (rs->uID != userId) {
			MCE_WARN("UserNameManagerI::reload CreateUserNameException::UserId error,userid = " << userId << ", resultID = "<< rs->uID);
			return ;
		}
	
		UserNameReaderAdapter::instance().setData(userId, rs);
	}

}

//-------------------------UserStateManagerI-------------------------------

//--------------------------------------------------------------------------------

void UserStateManagerI::createUserState(const Str2StrMap& props,
		                              const Ice::Current& current)
{
	InvokeClient ic = InvokeClient::create(current,"",InvokeClient::INVOKE_LEVEL_INFO);
	
	ostringstream inMsg;
	inMsg<<"UserStateManagerI::createUserState" <<  " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	
	
	int id;
	Str2StrMap::const_iterator it = props.find(CUSID);
	if(it!=props.end())
	{
		id=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		MCE_WARN("UserStateManagerI::createUserState CreateUserStateException::Can find userID in props");
		return;
	}

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, id);
		QueryRunner( DB_USER_STATE , CDbWServer ).execute( Statement::buildInsert( TABLE_USER_STATE, props ) );
		
		ostringstream idMsg;
		idMsg<<" id="<<id;
		tc.step(idMsg.str());
		
		MCE_INFO("UserStateManagerI::createUserState Loading " << id);
		
		UserStateDataIPtr rs = new UserStateDataI ;
		BatchStateResHandlerI handler(rs);
		Statement sql;
		sql << "SELECT id, state, star, level FROM user_state WHERE id = '"<<id<<"'";
		size_t resultSize = QueryRunner(DB_USER_STATE, CDbWServer ).query(sql, handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserStateManagerI::createUserState CreateUserStateException::Can not load new added user ,userid  [" << id << "]");
			return ;
		}
		if (rs->id != id) {
			MCE_WARN("UserStateManagerI::createUserState CreateUserStateException::UserId error,userid = " << id << ", resultID = "<< rs->id);
			return ;
		}
	
		MCE_INFO( "UserStateManagerI::createUserState Loading " << id << " done.");

		UserStateReaderAdapter::instance().setData(id, rs);
	}
	UserLogicAdapter::instance().insertUserState(id, props);
}

void UserStateManagerI::setUserState(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(props).find("ID");
	if(it!=props.end())
	{
		const_cast<Str2StrMap&>(props).erase(it);
	}
	
	Str2StrMap filter;
	filter[CUSID] = boost::lexical_cast<string>(uid);
	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, uid);
		QueryRunner( DB_USER_STATE, CDbWServer ).execute( Statement::buildUpdate( TABLE_USER_STATE , filter ,  props ) );

		UserStateReaderAdapter::instance().setUserState(uid, props);
	}

	
	UserLogicAdapter::instance().updateUserState(uid, props, current.ctx);
}

//reload for us
void UserStateManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		UserStateDataIPtr rs = new UserStateDataI ;
		BatchStateResHandlerI handler(rs);
		Statement sql;
		sql << "SELECT id, state, star, level FROM user_state WHERE id = '"<<userId<<"'";
		size_t resultSize = QueryRunner(DB_USER_STATE, CDbRServer ).query(sql, handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserStateManagerI::reload CreateUserStateException::Can not load new added user ,userid  [" << userId << "]");
			return ;
		}
		if (rs->id != userId) {
			MCE_WARN("UserStateManagerI::reload CreateUserStateException::UserId error,userid = " << userId << ", resultID = "<< rs->id);
			return ;
		}
	
		UserStateReaderAdapter::instance().setData(userId, rs);
	}

}

void UserStateManagerI::setStateOn(Ice::Int userId, Ice::Int pos, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);
		Statement sql;
		sql << "UPDATE user_state SET state = state | " << (1 << pos) << " WHERE " << CUSID << " = '"<<userId<<"' LIMIT 1";
		QueryRunner(DB_USER_STATE, CDbWServer).execute(sql);

		UserStateReaderAdapter::instance().setStateOn(userId, pos);
	}
	
	Str2StrMap props;
	props[CUSSTATE] = boost::lexical_cast<string>(UserStateReaderAdapter::instance().getUserState(userId)->state());
	
	UserLogicAdapter::instance().updateUserState(userId,props);
}

void UserStateManagerI::setStateOff(Ice::Int userId, Ice::Int pos, const Ice::Current& current) 
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);

		Statement sql;
		sql << "UPDATE user_state SET state = state & (~" << (1 << pos) << ") WHERE " << CUSID << " = '"<<userId<<"' LIMIT 1";
		QueryRunner(DB_USER_STATE, CDbWServer).execute(sql);
		
		UserStateReaderAdapter::instance().setStateOff(userId, pos);
	}
	
	Str2StrMap props;
	props[CUSSTATE] = boost::lexical_cast<string>(UserStateReaderAdapter::instance().getUserState(userId)->state());

	UserLogicAdapter::instance().updateUserState(userId,props);
}

//-------------------------UserRightManagerI-------------------------------

//--------------------------------------------------------------------------------

void UserRightManagerI::createUserRight(const Str2StrMap& props,
		                              const Ice::Current& current)
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
	
	ostringstream inMsg;
	inMsg<<"UserRightManagerI::createUserRight" <<  " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	
	int id;
	Str2StrMap::const_iterator it = props.find(CURID);
	if(it!=props.end())
	{
		id=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		MCE_WARN("UserRightManagerI::createUserRight CreateUserRightException::Can find userID in props");
		return;
	}

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, id);
		QueryRunner( DB_USER_RIGHT , CDbWServer ).execute( Statement::buildInsert( TABLE_USER_RIGHT, props ) );
		
		ostringstream idMsg;
		idMsg<<" id="<<id;
		tc.step(idMsg.str());
	
		MCE_INFO("UserRightManagerI::createUserRight Loading " << id);
		
		UserRightDataIPtr rs = new UserRightDataI ;
		BatchRightResHandlerI handler(rs);
		Statement sql;
		sql << "SELECT id, auth, common_rights FROM user_right WHERE id = '"<<id <<"'";
		size_t resultSize = QueryRunner(DB_USER_RIGHT, CDbWServer ).query(sql,handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserRightManagerI::createUserRight CreateUserRightException::Can not load new added user ,userid  [" << id << "]");
			return ;
		}
		
		if (rs->uID != id) {
			MCE_WARN("UserRightManagerI::createUserRight CreateUserRightException::UserId error,userid = " << id << ", resultID = "<< rs->uID);
			return ;
		}
	
		MCE_INFO( "UserRightManagerI::createUserRight Loading " << id << " done.");

		UserRightReaderAdapter::instance().setData(id, rs);
	}
	UserLogicAdapter::instance().insertUserRight(id, props);
}

void UserRightManagerI::setUserRight(Ice::Int uid,
		                           const MyUtil::Str2StrMap& props, 
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(props).find("ID");
	if(it!=props.end())
	{
		const_cast<Str2StrMap&>(props).erase(it);
	}
	
	Str2StrMap filter;
	filter[CURID] = boost::lexical_cast<string>(uid);
	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, uid);
		QueryRunner( DB_USER_RIGHT, CDbWServer ).execute( Statement::buildUpdate( TABLE_USER_RIGHT , filter ,  props ) );

		UserRightReaderAdapter::instance().setUserRight(uid, props);
	}

	
	UserLogicAdapter::instance().updateUserRight(uid, props, current.ctx);
}

void UserRightManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);
		UserRightDataIPtr rs = new UserRightDataI ;
		BatchRightResHandlerI handler(rs);
		Statement sql;
		sql << "SELECT id, auth, common_rights FROM user_right WHERE id = '"<<userId<<"'";
		size_t resultSize = QueryRunner(DB_USER_RIGHT, CDbRServer ).query(sql,handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserRightManagerI::reload CreateUserRightException::Can not load new added user ,userid  [" << userId << "]");
			return ;
		}
		
		if (rs->uID != userId) {
			MCE_WARN("UserRightManagerI::reload CreateUserRightException::UserId error,userid = " << userId << ", resultID = "<< rs->uID);
			return ;
		}
	
		UserRightReaderAdapter::instance().setData(userId, rs);
	}

}

//-------------------------UserLoginCountManagerI-------------------------------

//--------------------------------------------------------------------------------

void UserLoginCountManagerI::createUserLoginCount(const Str2StrMap& props,
		                              const Ice::Current& current)
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
	
	ostringstream inMsg;
	inMsg<<"UserLoginCountManagerI::createUserLoginCount" <<  " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	
	
	int id;
	Str2StrMap::const_iterator it = props.find(CULCID);
	if(it!=props.end())
	{
		id=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		MCE_WARN("UserLoginCountManagerI::createUserLoginCount CreateUserLoginCountException::Can find userID in props");
		return;
	}

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, id);
		QueryRunner( DB_USER_LOGINCOUNT , CDbWServer ).execute( Statement::buildInsert( TABLE_USER_LOGINCOUNT, props ) );
		
		ostringstream idMsg;
		idMsg<<" id="<<id;
		tc.step(idMsg.str());
		
		MCE_INFO("UserLoginCountManagerI::createUserLoginCount Loading " << id);
		
		UserLoginCountDataIPtr rs = new UserLoginCountDataI ;
		BatchLoginCountResHandlerI handler(rs);
		Statement sql;
		sql << "SELECT id, logincount FROM user_logincount WHERE id = '"<<id<<"'";
		size_t resultSize = QueryRunner(DB_USER_LOGINCOUNT, CDbWServer ).query(sql,handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserLoginCountManagerI::createUserLoginCount CreateUserLoginCountException::Can not load new added user ,userid  [" << id << "]");
			rs->uID = id;
			rs->logincount = 0;
			return ;
		}
		if (rs->uID != id) {
			MCE_WARN("UserLoginCountManagerI::createUserLoginCount CreateUserLoginCountException::UserId error,userid = " << id << ", resultID = "<< rs->uID);
			rs->uID = id;
			rs->logincount = 0;
		}
	
	
		MCE_INFO( "UserLoginCountManagerI::createUserLoginCount Loading " << id << " done.");
		
		UserLoginCountReaderAdapter::instance().setData(id, rs);
	}
}

void UserLoginCountManagerI::incUserLoginCount(Ice::Int uid,
							       const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<uid;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	mop::hi::svc::adapter::BatchUpdaterAdapter::instance().incLoginCount(uid,1);
	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, uid);

		UserLoginCountReaderAdapter::instance().incUserLoginCount(uid);
	}
}


//reload for us
void UserLoginCountManagerI::reload(Ice::Int userId, const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);
		UserLoginCountDataIPtr rs = new UserLoginCountDataI ;
		BatchLoginCountResHandlerI handler(rs);
		Statement sql;
		sql << "SELECT id, logincount FROM user_logincount WHERE id = '"<<userId<<"'";
		size_t resultSize = QueryRunner(DB_USER_LOGINCOUNT, CDbRServer ).query(sql,handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserLoginCountManagerI::reload CreateUserLoginCountException::Can not load new added user ,userid  [" << userId << "]");
			rs->uID = userId;
			rs->logincount = 0;
			return ;
		}
		if (rs->uID != userId) {
			MCE_WARN("UserLoginCountManagerI::reload CreateUserLoginCountException::UserId error,userid = " << userId << ", resultID = "<< rs->uID);
			rs->uID = userId;
			rs->logincount = 0;
		}
	
		UserLoginCountReaderAdapter::instance().setData(userId, rs);
	}

}


//----------------UserUrlManager--------------------

void UserUrlManagerI::setUserUrl(Ice::Int id, const MyUtil::Str2StrMap& props, const Ice::Current & current) {
	ostringstream inMsg;
	inMsg<<"id="<<id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(props).find("ID");
	if(it!=props.end())
	{
		const_cast<Str2StrMap&>(props).erase(it);
	}
	
	MyUtil::Str2StrMap filter;
	filter["id"] = boost::lexical_cast<string>(id);
	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, id);
		
		QueryRunner(DB_USER_URL,CDbWServer).execute(Statement::buildUpdate(TABLE_USER_URL, filter, props));

		UserUrlReaderAdapter::instance().setUserUrl(id, props);
	}
	UserLogicAdapter::instance().updateUserUrl(id, props, current.ctx);
}

void UserUrlManagerI::createUserUrl(const MyUtil::Str2StrMap& props, const Ice::Current & current) 
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
	
	ostringstream inMsg;
	inMsg<<"UserUrlManagerI::createUserUrl" <<  " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	int id;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		id=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		MCE_WARN("UserUrlManagerI::createUserUrl CreateUserUrlException::Can find userID in props");
		return;
	}

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, id);
		QueryRunner(DB_USER_URL,CDbWServer).execute(Statement::buildInsert(TABLE_USER_URL, props));
		ostringstream idMsg;
		idMsg<<" id="<<id;
		tc.step(idMsg.str());
		
		MCE_INFO("UserUrlManagerI::createUserUrl Loading " << id);
		UserUrlDataIPtr result = new UserUrlDataI;
		Statement sql;
		sql << "SELECT id, largeurl, mainurl, headurl, tinyurl FROM user_url WHERE id = '" << id << "'";
		UserUrlResultHandlerI handler(result);
		size_t resultSize = QueryRunner(DB_USER_URL, CDbWServer).query(sql, handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserUrlManagerI::createUserUrl CreateUserUrlException::Can not load new added user ,userid  [" << id << "]");
			return ;
		}
		if (id != result->id) {
			MCE_WARN("UserUrlManagerI::createUserUrl CreateUserUrlException::UserId error,userid = " << id << ", resultID = "<< result->id);
			return ;
		}
		MCE_INFO("UserUrlManagerI::createUserUrl Loading  " << id << " done.");

		UserUrlReaderAdapter::instance().setData(id, result);
	}
	UserLogicAdapter::instance().insertUserUrl(id, props);
}

void UserUrlManagerI::reload(Ice::Int id, const Ice::Current & current) {
	ostringstream inMsg;
	inMsg<<"id="<<id;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, id);
		
		UserUrlDataIPtr result = new UserUrlDataI;
		Statement sql;
		sql << "SELECT id, largeurl, mainurl, headurl, tinyurl FROM user_url WHERE id = '" << id << "'";
		UserUrlResultHandlerI handler(result);
		size_t resultSize = QueryRunner(DB_USER_URL, CDbRServer).query(sql, handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserUrlManagerI::relaod CreateUserUrlException::Can not load new added user ,userid  [" << id << "]");
			return ;
		}
		if (id != result->id) {
			MCE_WARN("UserUrlManagerI::reload CreateUserUrlException::UserId error,userid = " << id << ", resultID = "<< result->id);
			return ;
		}

		UserUrlReaderAdapter::instance().setData(id, result);
	}

}

//-------------------------UserBorn---------------------------

void UserBornManagerI::createUserBorn(const Str2StrMap& props, const Ice::Current& current )
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
	
	ostringstream inMsg;
	inMsg<<"UserBornManagerI::createUserBorn" <<  " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());
	
	int id;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		id=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		MCE_WARN("UserBornManagerI::createUserBorn CreateUserBornException::Can find userID in props");
		return;
	}

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, id);
		QueryRunner(DB_USER_BORN, CDbWServer).execute(Statement::buildInsert(TABLE_USER_BORN, props));
		
		ostringstream idMsg;
		idMsg<<" id="<<id;
		tc.step(idMsg.str());
		
		MCE_INFO("UserBornManagerI::createUserBorn Loading " << id);
		Str2StrMap filter;
		filter[COLUMN_ID] = boost::lexical_cast<string>(id);
		UserBornDataIPtr rs = new UserBornDataI ;
		UserBornResultHandlerI handler(rs);
		size_t resultSize = QueryRunner(DB_USER_BORN, CDbWServer).query(Statement::buildSelect(TABLE_USER_BORN, filter), handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserBornManagerI::createUserBorn CreateUserBornException::Can not load new added user ,userid  [" << id << "]");
			return ;
		}
		if (id != rs->id) {
			MCE_WARN("UserBornManagerI::createUserBorn CreateUserBornException::UserId error,userid = " << id << ", resultID = "<< rs->id);
			return ;
		}
		MCE_INFO("UserBornManagerI::createUserBorn Loading " << id << " done.");

		UserBornReaderAdapter::instance().setData(id, rs);
	}
	UserLogicAdapter::instance().insertUserBorn(id, props);
}

void UserBornManagerI::reloadUserBorn(Ice::Int userId,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);
		Str2StrMap filter;
		filter[COLUMN_ID] = boost::lexical_cast<string>(userId);
		UserBornDataIPtr rs = new UserBornDataI ;
		UserBornResultHandlerI handler(rs);
		size_t resultSize = QueryRunner(DB_USER_BORN, CDbRServer).query(Statement::buildSelect(TABLE_USER_BORN, filter), handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserBornManagerI::reload CreateUserBornException::Can not load new added user ,userid  [" << userId << "]");
			return ;
		}
		if (userId != rs->id) {
			MCE_WARN("UserBornManagerI::reload CreateUserBornException::UserId error,userid = " << userId << ", resultID = "<< rs->id);
			return ;
		}

		UserBornReaderAdapter::instance().setData(userId, rs);
	}
}

void UserBornManagerI::setUserBorn(Ice::Int userId,const Str2StrMap& props,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(props).find("ID");
	if(it!=props.end())
	{
		const_cast<Str2StrMap&>(props).erase(it);
	}
	
	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);
		Str2StrMap filter;
		filter[COLUMN_ID] = boost::lexical_cast<string>(userId);
		QueryRunner(DB_USER_BORN, CDbWServer).execute(Statement::buildUpdate(TABLE_USER_BORN, filter, props));

		UserBornReaderAdapter::instance().setUserBorn(userId, props);
	}
	UserLogicAdapter::instance().updateUserBorn(userId, props,current.ctx);
}

//-------------------------UserStage---------------------------

void UserStageManagerI::createUserStage(const Str2StrMap& props, const Ice::Current& current )
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
	
	ostringstream inMsg;
	inMsg<<"UserStageManagerI::createUserStage" <<  " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	int id;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		id=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		MCE_WARN("UserStageManagerI::createUserStage CreateUserStageException::Can find userID in props");
		return;
	}

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, id);
		QueryRunner(DB_USER_STAGE, CDbWServer).execute(Statement::buildInsert(TABLE_USER_STAGE, props));
		
		ostringstream idMsg;
		idMsg<<" id="<<id;
		tc.step(idMsg.str());
		
		MCE_INFO("UserStageManagerI::createUserStage Loading " << id);
		Str2StrMap filter;
		filter[COLUMN_ID] = boost::lexical_cast<string>(id);
		UserStageDataIPtr rs = new UserStageDataI ;
		UserStageResultHandlerI handler(rs);
		size_t resultSize = QueryRunner(DB_USER_STAGE, CDbWServer).query(Statement::buildSelect(TABLE_USER_STAGE, filter), handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserStageManagerI::createUserStage CreateUserStageException::Can not load new added user ,userid  [" << id << "]");
			return ;
		}
		if (id != rs->id) {
			MCE_WARN("UserStageManagerI::createUserStage CreateUserStageException::UserId error,userid = " << id << ", resultID = "<< rs->id);
			return ;
		}
		MCE_INFO("UserStageManagerI::createUserStage Loading " << id << " done.");

		UserStageReaderAdapter::instance().setData(id, rs);
	}
	UserLogicAdapter::instance().insertUserStage(id, props);
}

void UserStageManagerI::reloadUserStage(Ice::Int userId,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);
		Str2StrMap filter;
		filter[COLUMN_ID] = boost::lexical_cast<string>(userId);
		UserStageDataIPtr rs = new UserStageDataI ;
		UserStageResultHandlerI handler(rs);
		size_t resultSize = QueryRunner(DB_USER_STAGE, CDbRServer).query(Statement::buildSelect(TABLE_USER_STAGE, filter), handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserStageManagerI::reload CreateUserStageException::Can not load new added user ,userid  [" << userId << "]");
			return ;
		}
		if (userId != rs->id) {
			MCE_WARN("UserStageManagerI::reload CreateUserStageException::UserId error,userid = " << userId << ", resultID = "<< rs->id);
			return ;
		}

		UserStageReaderAdapter::instance().setData(userId, rs);
	}
}

void UserStageManagerI::setUserStage(Ice::Int userId,const Str2StrMap& props,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(props).find("ID");
	if(it!=props.end())
	{
		const_cast<Str2StrMap&>(props).erase(it);
	}
	
	Str2StrMap filter;
	filter[COLUMN_ID] = boost::lexical_cast<string>(userId);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);
		QueryRunner(DB_USER_STAGE, CDbWServer).execute(Statement::buildUpdate(TABLE_USER_STAGE, filter, props));

		UserStageReaderAdapter::instance().setUserStage(userId, props);
	}
	UserLogicAdapter::instance().updateUserStage(userId, props,current.ctx);
}

//-------------------------UserDoing---------------------------

void UserDoingManagerI::createUserDoing(const Str2StrMap& props, const Ice::Current& current )
{
	InvokeClient ic = InvokeClient::create(current, "", InvokeClient::INVOKE_LEVEL_INFO);
	
	ostringstream inMsg;
	inMsg<<"UserDoingManagerI::createUserDoing" <<  " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(inMsg.str());

	int id;
	Str2StrMap::const_iterator it = props.find("ID");
	if(it!=props.end())
	{
		id=boost::lexical_cast<Ice::Int>(it->second);
	}else
	{
		MCE_WARN("UserDoingManagerI::createUserDoing CreateUserDoingException::Can find userID in props");
		return;
	}

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, id);
		QueryRunner(DB_USER_DOING, CDbWServer).execute(Statement::buildInsert(TABLE_USER_DOING, props));
		ostringstream idMsg;
		idMsg<<" id="<<id;
		tc.step(idMsg.str());
		
		MCE_INFO("UserDoingManagerI::createUserDoing Loading " << id);
		Str2StrMap filter;
		filter[COLUMN_ID] = boost::lexical_cast<string>(id);
		UserDoingDataIPtr rs = new UserDoingDataI ;
		UserDoingResultHandlerI handler(rs);
		size_t resultSize = QueryRunner(DB_USER_DOING, CDbWServer).query(Statement::buildSelect(TABLE_USER_DOING, filter), handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserDoingManagerI::createUserDoing CreateUserDoingException::Can not load new added user ,userid  [" << id << "]");
			return ;
		}
		if (id != rs->id) {
			MCE_WARN("UserDoingManagerI::createUserDoing CreateUserDoingException::UserId error,userid = " << id << ", resultID = "<< rs->id);
			return ;
		}
		MCE_INFO("UserDoingManagerI::createUserDoing Loading " << id << " done.");
		UserDoingReaderAdapter::instance().setData(id, rs);
	}
	UserLogicAdapter::instance().insertUserDoing(id, props);
}

void UserDoingManagerI::reloadUserDoing(Ice::Int userId,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);

	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);
		Str2StrMap filter;
		filter[COLUMN_ID] = boost::lexical_cast<string>(userId);
		UserDoingDataIPtr rs = new UserDoingDataI ;
		UserDoingResultHandlerI handler(rs);
		size_t resultSize = QueryRunner(DB_USER_DOING, CDbRServer).query(Statement::buildSelect(TABLE_USER_DOING, filter), handler);
		
		if (resultSize == 0) {
			MCE_WARN("UserDoingManagerI::reload CreateUserDoingException::Can not load new added user ,userid  [" << userId << "]");
			return ;
		}
		if (userId != rs->id) {
			MCE_WARN("UserDoingManagerI::reload CreateUserDoingException::UserId error,userid = " << userId << ", resultID = "<< rs->id);
			return ;
		}
	
		UserDoingReaderAdapter::instance().setData(userId, rs);
	}
}

void UserDoingManagerI::setUserDoing(Ice::Int userId,const Str2StrMap& props,const Ice::Current& current)
{
	ostringstream inMsg;
	inMsg<<"id="<<userId;
	InvokeClient ic = InvokeClient::create(current, inMsg.str(), InvokeClient::INVOKE_LEVEL_INFO);
	
	//去掉map中的用户id
	Str2StrMap::iterator it = const_cast<Str2StrMap&>(props).find("ID");
	if(it!=props.end())
	{
		const_cast<Str2StrMap&>(props).erase(it);
	}
	
	Str2StrMap filter;
	filter[COLUMN_ID] = boost::lexical_cast<string>(userId);
	{
		xce::tempmutex::Lock lock(tempMutexManagerPtr_, userId);
		QueryRunner(DB_USER_DOING, CDbWServer).execute(Statement::buildUpdate(TABLE_USER_DOING, filter, props));
		UserDoingReaderAdapter::instance().setUserDoing(userId, props);
	}
	UserLogicAdapter::instance().updateUserDoing(userId, props,current.ctx);
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

