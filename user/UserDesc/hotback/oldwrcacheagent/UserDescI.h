/*
 * =====================================================================================
 *
 *       Filename:  UserDescI.h
 *
 *    Description:  new UserDesc include UserTime,UserContact,UserFav
 *    		    and a big Object UserDesc.
 *
 *        Version:  1.0
 *        Created:  2009年08月28日 16时26分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __USERDESCI_H__
#define __USERDESCI_H__

#include "../share/UserDescDataI.cpp"
#include "TaskManager.h"
#include "ServiceI.h"
#include "TempMutex.h"


namespace xce {
namespace userdesc {
//-----------------------UserTime---------------------------------
class UserTimeManagerI : virtual public UserTimeManager, public MyUtil::Singleton<UserTimeManagerI>{
public:
	UserTimeManagerI();
	virtual ~UserTimeManagerI();
	virtual UserTimeDataPtr getUserTime(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setUserTime(Ice::Int id, const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void createUserTime(const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void reload(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
public:
	bool isValid();
private:

	IceUtil::Mutex _mutex;
	bool _isValid;
	xce::tempmutex::TempMutexManagerPtr _tempMutexManager;
};

//-----------------------UserContact---------------------------------
class UserContactManagerI : virtual public UserContactManager, public MyUtil::Singleton<UserContactManagerI>{
public:
	UserContactManagerI();
	virtual ~UserContactManagerI();
	virtual UserContactDataPtr getUserContact(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setUserContact(Ice::Int id, const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void createUserContact(const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void reload(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

public:
	bool isValid();
private:

	IceUtil::Mutex _mutex;
	bool _isValid;
	xce::tempmutex::TempMutexManagerPtr _tempMutexManager;
};

//-----------------------UserFav---------------------------------

class UserFavManagerI : virtual public UserFavManager, public MyUtil::Singleton<UserFavManagerI>{
public:
	UserFavManagerI();
	virtual ~UserFavManagerI();
	virtual UserFavDataPtr getUserFav(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setUserFav(Ice::Int id, const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void createUserFav(const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void reload(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
};

//-----------------------UserDesc---------------------------------

class UserDescManagerI : virtual public UserDescManager, public MyUtil::Singleton<UserDescManagerI>{
public:
	UserDescManagerI();
	virtual ~UserDescManagerI();
	virtual UserDescDataPtr getUserDesc(Ice::Int id, const Ice::Current& = Ice::Current());
};

};
};


#endif

