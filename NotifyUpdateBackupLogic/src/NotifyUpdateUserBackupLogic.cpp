#include "NotifyUpdateUserBackupLogic.h"

#include "LogWrapper.h"

#include "time.h"

#include "util/cpp/TimeCost.h"
using namespace xce::notifyupdatebackuplogic;
using namespace MyUtil;


void NotifyUpdateUserBackupLogicI::notifyUpdateUser( Ice::Int userId, const string& table, const MyUtil::Str2StrMap& props, const Ice::Current& current)
{
	MyUtil::TaskManager::instance().execute( new NotifyUpdateUserTask(userId, table, props));
}

