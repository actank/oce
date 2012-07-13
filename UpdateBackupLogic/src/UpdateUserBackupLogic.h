#ifndef __UPDATEUSERBACKUPLOGIC_H__
#define __UPDATEUSERBACKUPLOGIC_H__

#include "UpdateBackupLogic.h"
#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include "TaskManager.h"
#include "ObjectCacheI.h"

#include "ServiceI.h"
#include "QueryRunner.h"

#include "Channel.h"

namespace xce
{
namespace updatebackuplogic
{

const int TASK_LEVEL_UPDATE = 10;

const string TABLE_USERDOING = "user_doing";
const string TABLE_USERSTAGE = "user_stage";
const string TABLE_USERBORN = "user_born";
const string TABLE_USERSTATE = "user_state";
const string TABLE_USERNAMES = "user_names";
const string TABLE_USERPASSPORT = "user_passport";
const string TABLE_USERCONFIG = "user_config";
const string TABLE_USERURL = "user_url";
const string TABLE_USERTIME = "user_time";
const string TABLE_USERCONTACT = "user_contact";
const string TABLE_USERRIGHT = "user_right";

const string TABLE_HIGH_SCHOOL_INFO = "high_school_info";
const string TABLE_UNIVERSITY_INFO = "university_info";
const string TABLE_WORKPLACE_INFO = "workplace_info";
const string TABLE_COLLEGE_INFO = "college_info";
const string TABLE_ELEMENTARY_SCHOOL_INFO = "elementary_school_info";
const string TABLE_JUNIOR_HIGH_SCHOOL_INFO = "junior_high_school_info";
const string TABLE_REGION_INFO = "region_info";

//const string TABLE_USERLOGINCOUNT = "user_logincount";
//const string TABLE_USERCOUNT = "user_count";
//----------------------------------------------------------------------------
class UpdateUserBackupLogicI : virtual public UpdateUserBackupLogic, public MyUtil::Singleton<UpdateUserBackupLogicI>
{
	public:
	
		UpdateUserBackupLogicI()
		{
			_oceChannel = new MyUtil::OceChannel;
			_userChannel = new MyUtil::UserChannel;
		}

		void updateUser( Ice::Int userId, const string& table, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
	
	private:
		
		MyUtil::ChannelPtr _oceChannel;
		MyUtil::ChannelPtr _userChannel;
};


class UpdateUserTask: virtual public ::MyUtil::Task 
{
	public:
		
		UpdateUserTask(const int userId, const string table, const MyUtil::Str2StrMap props, const MyUtil::ChannelPtr& oceChannel, const MyUtil::ChannelPtr& userChannel):
			           MyUtil::Task(TASK_LEVEL_UPDATE), _userId(userId), _table(table), _props(props), _oceChannel(oceChannel), _userChannel(userChannel)
		{
		}
       
		virtual void handle();

	private:
	
		const int _userId;
		const string _table;
		MyUtil::Str2StrMap _props;
		MyUtil::ChannelPtr _oceChannel;
		MyUtil::ChannelPtr _userChannel;
};

};
};
#endif

