#ifndef _USERDESC_ICE
#define _USERDESC_ICE

#include <Util.ice>

#include <ObjectCache.ice>
module xce
{
module userdesc
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

class UserContactDataN
    {
	int id;
	string msn;
	string mobilePhone;
	string fixPhone;
	long qq;
	string homepage;
    };

class UserFavData
    {
	int id;
        string fav;
	int favFlag;
    };
class UserDescData
    {
	UserTimeData userTime;
	UserContactData userContact;
	UserFavData userFav;
    };
class UserDescDataN
    {
	UserTimeData userTime;
	UserContactDataN userContact;
	UserFavData userFav;
    };


	class UserTimeReader
    {
	UserTimeData getUserTime(int userId);
	void setUserTime(int userId, MyUtil::Str2StrMap properties);
	void setValid(bool newState);
	void setData(MyUtil::ObjectResult userData);
    };

	class UserContactReader
    {
	UserContactData getUserContact(int userId);
	UserContactDataN getUserContactN(int userId);
	void setUserContact(int userId, MyUtil::Str2StrMap properties);
	void setValid(bool newState);
	void setData(MyUtil::ObjectResult userData);
    };


	class UserDescReader
    {
		UserDescData getUserDesc(int userId);
		UserDescDataN getUserDescN(int userId);
    };

	class UserTimeWriter
    {
	void createUserTime(MyUtil::Str2StrMap properties);
	void reload(int userId);
	void setUserTime(int userId, MyUtil::Str2StrMap properties);
	void setValid(bool newState);
    };

	class UserContactWriter
    {
	void createUserContact(MyUtil::Str2StrMap properties);
	void reload(int userId);
	void setUserContact(int userId, MyUtil::Str2StrMap properties);
	void setValid(bool newState);
    };



class UserTimeManager
    {
	void createUserTime(MyUtil::Str2StrMap properties);
	void reload(int userId);
	UserTimeData getUserTime(int userId);
	void setUserTime(int userId, MyUtil::Str2StrMap properties);
		void setValid(bool newState);
    };

	class UserContactManager
    {
	void createUserContact(MyUtil::Str2StrMap properties);
	void reload(int userId);
	UserContactData getUserContact(int userId);
	void setUserContact(int userId, MyUtil::Str2StrMap properties);
		void setValid(bool newState);
    };


	class UserFavManager
    {
	void createUserFav(MyUtil::Str2StrMap properties);
	void reload(int userId);
	UserFavData getUserFav(int userId);
	void setUserFav(int userId, MyUtil::Str2StrMap properties);
		void setValid(bool newState);
    };

class UserDescManager
    {
	UserDescData getUserDesc(int userId);
    };

};
};

#endif
