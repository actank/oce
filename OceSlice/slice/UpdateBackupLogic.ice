#ifndef _UPDATEBACKUPLOGIC_ICE
#define _UPDATEBACKUPLOGIC_ICE

#include <Util.ice>

#include <ObjectCache.ice>
module xce
{
module updatebackuplogic
{

	class UpdateUserBackupLogic
	{
		void updateUser(int userId, string table, MyUtil::Str2StrMap props);
	};
};  
};  
#endif
