#ifndef __NOTIFYUPDATEUSERBACKUPLOGIC_H__
#define __NOTIFYUPDATEUSERBACKUPLOGIC_H__

#include "NotifyUpdateBackupLogic.h"
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
namespace notifyupdatebackuplogic
{

const int TASK_LEVEL_NOTIFYUPDATE = 10;


//----------------------------------------------------------------------------
class NotifyUpdateUserBackupLogicI : virtual public NotifyUpdateUserBackupLogic, public MyUtil::Singleton<NotifyUpdateUserBackupLogicI>
{
public:
    void notifyUpdateUser( Ice::Int userId, const string& table, const MyUtil::Str2StrMap& props, const Ice::Current& = Ice::Current());
};


class NotifyUpdateUserTask: virtual public ::MyUtil::Task 
{
	public:
		
		NotifyUpdateUserTask(const int userId, const string table, const MyUtil::Str2StrMap props):
			           MyUtil::Task(TASK_LEVEL_NOTIFYUPDATE), _userId(userId), _table(table), _props(props)
		{
		}
       
		virtual void handle()
		{
			MyUtil::BackupChannel bc;

			try
			{
				bc.uncheckedCast<xce::updatebackuplogic::UpdateUserBackupLogicPrx>(bc.locate("UU", "UpdateBackupLogic",MyUtil::TWO_WAY, 300))->updateUser(_userId, _table, _props);
			}catch(...)
			{
				return;
			}
		}

private:
		const int _userId;
		const string _table;
		MyUtil::Str2StrMap _props;
};

};
};
#endif

