#ifndef _NOTIFYUPDATEBACKUPLOGIC_ICE
#define _NOTIFYUPDATEBACKUPLOGIC_ICE

#include <Util.ice>

#include <ObjectCache.ice>
module xce
{
module notifyupdatebackuplogic
{

	class NotifyUpdateUserBackupLogic
	{
		void notifyUpdateUser(int userId, string table, MyUtil::Str2StrMap props);
	};
};  
};  
#endif
