#include "UpdateUserBackupLogic.h"

#include "LogWrapper.h"

#include "time.h"

#include "util/cpp/TimeCost.h"

#include "UserLogicAdapter.h"

#include "ObjectCacheAdapter.h"

#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

using namespace xce::updatebackuplogic;
using namespace MyUtil;
using namespace mop::hi::svc::adapter;


void UpdateUserBackupLogicI::updateUser( Ice::Int userId, const string& table, const MyUtil::Str2StrMap& props, const Ice::Current& current)
{
	MyUtil::TaskManager::instance().execute( new UpdateUserTask(userId, table, props, _oceChannel, _userChannel));
}

void UpdateUserTask::handle()
{
	if( _table == TABLE_USERPASSPORT )
	{
		UserLogicAdapter::instance().updateUserPassport( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 0, "UserBase", _userChannel);
	}
	if( _table == TABLE_USERNAMES )
	{
		UserLogicAdapter::instance().updateUserNames( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 1, "UserBase", _userChannel);
	}
	if( _table == TABLE_USERRIGHT )
	{
		UserLogicAdapter::instance().updateUserRight( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 2, "UserBase", _userChannel);
	}
	/*if( _table == TABLE_USERLOGINCOUNT )
	{
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 3, "UserBase", _userChannel);
	}*/
	if( _table == TABLE_USERSTATE )
	{
		UserLogicAdapter::instance().updateUserState( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 4, "UserBase", _userChannel);
	}
	if( _table == TABLE_USERURL )
	{
		UserLogicAdapter::instance().updateUserUrl( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 5, "UserBase", _userChannel);
	}
	if( _table == TABLE_USERBORN )
	{
		UserLogicAdapter::instance().updateUserBorn( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 6, "UserBase", _userChannel);
	}
	if( _table == TABLE_USERSTAGE )
	{
		UserLogicAdapter::instance().updateUserStage( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 7, "UserBase", _userChannel);
	}
	if( _table == TABLE_USERDOING )
	{
		UserLogicAdapter::instance().updateUserDoing( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 8, "UserBase", _userChannel);
	}
	if( _table == TABLE_USERTIME )
	{
		UserLogicAdapter::instance().updateUserTime( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 0, "UserDesc", _userChannel);
	}
	if( _table == TABLE_USERCONTACT )
	{
		UserLogicAdapter::instance().updateUserContact( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 1, "UserDesc", _userChannel);
	}
	if( _table == TABLE_USERCONFIG )
	{
		UserLogicAdapter::instance().updateUserConfig( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 100, 0, "UserConfig", _userChannel);
	}
	if( _table == TABLE_HIGH_SCHOOL_INFO )
	{
		UserLogicAdapter::instance().updateUserHighSchool( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 10, 0, "UserNetworkN", _oceChannel);
	}
	if( _table == TABLE_UNIVERSITY_INFO )
	{
		UserLogicAdapter::instance().updateUserUniversity( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 10, 0, "UserNetworkN", _oceChannel);
	}
	if( _table == TABLE_WORKPLACE_INFO )
	{
		UserLogicAdapter::instance().updateUserWorkplace( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 10, 0, "UserNetworkN", _oceChannel);
	}
	if( _table == TABLE_COLLEGE_INFO )
	{
		UserLogicAdapter::instance().updateUserCollege( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 10, 0, "UserNetworkN", _oceChannel);
	}
	if( _table == TABLE_ELEMENTARY_SCHOOL_INFO )
	{
		UserLogicAdapter::instance().updateUserElementarySchool( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 10, 0, "UserNetworkN", _oceChannel);
	}
	if( _table == TABLE_JUNIOR_HIGH_SCHOOL_INFO )
	{
		UserLogicAdapter::instance().updateUserJuniorHighSchool( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 10, 0, "UserNetworkN", _oceChannel);
	}
	if( _table == TABLE_REGION_INFO )
	{
		UserLogicAdapter::instance().updateUserRegion( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 10, 0, "UserNetworkN", _oceChannel);
	}
	/*if( _table == TABLE_USERCOUNT )
	{
		UserLogicAdapter::instance().updateUserRegion( _userId, _props );
		xce::adapter::objectcache::ObjectCacheAdapter::reload(_userId, 10, 0, "UserCount", _oceChannel);
	}*/
}


