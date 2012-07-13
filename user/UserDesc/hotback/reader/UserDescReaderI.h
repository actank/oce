/*
 * =====================================================================================
 *
 *       Filename:  UserDescReaderI.h
 *
 *    Description:  new UserDesc include UserTime,UserContact,a big Object UserDesc.
 *
 *        Version:  1.0
 *        Created:  2009年08月28日 16时26分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  wenlong.zhang (ggh), wenlong.zhang@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#ifndef __USERDESC_READER_I_H__
#define __USERDESC_READER_I_H__

#include "UserDesc.h"
#include "user/UserDesc/hotback/share/UserDescDataI.h"
#include "TaskManager.h"
#include "ServiceI.h"


namespace xce {
namespace userdesc {
//-----------------------UserTime---------------------------------

const int TASK_LEVEL_FILL = 1;

const Ice::Int USER_TIME = 0;
const Ice::Int USER_CONTACT = 1;

class UserTimeReaderI : virtual public UserTimeReader, public MyUtil::Singleton<UserTimeReaderI>{
public:
	UserTimeReaderI();
	virtual ~UserTimeReaderI();
	virtual UserTimeDataPtr getUserTime(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setUserTime(Ice::Int id, const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
public:
	UserTimeDataIPtr locate(int id);
	bool isValid();
private:

	IceUtil::Mutex _mutex;
	bool _isValid;
};

//-----------------------UserContact---------------------------------
class UserContactReaderI : virtual public UserContactReader, public MyUtil::Singleton<UserContactReaderI>{
public:
	UserContactReaderI();
	virtual ~UserContactReaderI();
	virtual UserContactDataPtr getUserContact(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual UserContactDataNPtr getUserContactN(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setUserContact(Ice::Int id, const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
public:
	UserContactDataIPtr locate(int id);
	bool isValid();
private:

	IceUtil::Mutex _mutex;
	bool _isValid;
};

//-----------------------UserDesc---------------------------------

class UserDescReaderI : virtual public UserDescReader, public MyUtil::Singleton<UserDescReaderI>{
public:
	UserDescReaderI();
	virtual ~UserDescReaderI();
	virtual UserDescDataPtr getUserDesc(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual UserDescDataNPtr getUserDescN(Ice::Int id, const Ice::Current& = Ice::Current());
};

template <class T,class P> class FillTask: virtual public ::MyUtil::Task {
public:
        FillTask(const Ice::Int identifier, const ::MyUtil::ObjectResultPtr& userData):
			     _identifier(identifier),_userData(userData)
	{
	}
        virtual void handle()
		{
			std::ostringstream os;
			os<<"[FillTask::handle]" << _identifier << "  set Data size : " << _userData->data.size()<< " id = ";
			
			for(MyUtil::ObjectMap::iterator it = _userData->data.begin();it != _userData->data.end();++it)
			{
				os<<it->first<<" ";
				T obj = new P;
				obj->parse(it->second);
				MyUtil::ServiceI::instance().getObjectCache()->addObject(_identifier , it->first, obj);
			}
			MCE_INFO(os.str());
		}

private:
		Ice::Int _identifier;
        ::MyUtil::ObjectResultPtr _userData;
};

};
};


#endif

