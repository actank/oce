/*
 * =====================================================================================
 *
 *       Filename:  UserDescWriterI.h
 *
 *    Description:  new UserDesc include UserTime,UserContact,a big Object UserDesc.
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

#ifndef __USERDESC_WRITER_I_H__
#define __USERDESC_WRITER_I_H__

#include "UserDesc.h"
#include "TaskManager.h"
#include "ServiceI.h"

namespace xce {
namespace userdesc {
//-----------------------UserTime---------------------------------

class UserTimeWriterI : virtual public UserTimeWriter, public MyUtil::Singleton<UserTimeWriterI>{
public:
	UserTimeWriterI();
	virtual ~UserTimeWriterI();
	virtual void setUserTime(Ice::Int id, const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void createUserTime(const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void reload(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
};

//-----------------------UserContact---------------------------------
class UserContactWriterI : virtual public UserContactWriter, public MyUtil::Singleton<UserContactWriterI>{
public:
	UserContactWriterI();
	virtual ~UserContactWriterI();
	virtual void setUserContact(Ice::Int id, const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void createUserContact(const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void reload(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());

};

};
};


#endif

