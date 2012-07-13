#ifndef _USER_DESC_CACHE_ICE
#define _USER_DESC_CACHE_ICE

#include <Util.ice>
#include <ObjectCache.ice>

module xce
{
module userdesccache
{
class UserTimeData
    {
	int id;
	int registerTime;
	int activateTime;
        int lastLoginTime;
        int lastLogoutTime;	
    };

class UserContactData
    {
	int id;
	string msn;
        string mobilePhone;
        string fixPhone;
        int qq;
        string homepage;
    };

interface UserTimeManager
    {
	UserTimeData getUserTime(int userId);
    };

interface UserContactManager
    {
	UserContactData getUserContact(int userId);
    };

class UserDescData
    {
        int id;
	int registerTime;
	int activateTime;
        int lastLoginTime;
        int lastLogoutTime;	
	
	string msn;
        string mobilePhone;
        string fixPhone;
        int qq;
        string homepage;
/*
	UserTimeData userTime;
	UserContactData userContact;
	UserFavData userFav;
*/
    };
dictionary<int, UserDescData> UserDescDataMap;

class UserDescUpdateOldData 
   { 
       int userid; 
       byte table; 
       MyUtil::Str2StrMap data; 
   }; 
sequence<UserDescUpdateOldData> UserDescUpdateOldDataSeq; 
 
dictionary<byte, string> Byte2StrMap; 
class UserDescUpdateNewData 
   { 
       int userid; 
       Byte2StrMap data; 
   }; 
sequence<UserDescUpdateNewData> UserDescUpdateNewDataSeq; 


interface UserDescSlaveManager
   {
	UserDescDataMap getUserDescMap(MyUtil::IntSeq userIds);
	UserDescData getUserDesc(int userId);
	void setData(MyUtil::ObjectResult userData);
	bool isValid();
	void setValid(bool newState);
	void update(UserDescUpdateNewDataSeq data);
    };

interface UserDescReloader
    {
        void reload(MyUtil::IntSeq userIds);
        void update(UserDescUpdateOldDataSeq data);
    };


};
};

#endif
