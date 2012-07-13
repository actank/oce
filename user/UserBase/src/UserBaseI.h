#ifndef __USERBASEI_H__
#define __USERBASEI_H__

#include "UserBase.h"
#include <Ice/Ice.h>
#include <IceUtil/AbstractMutex.h>
#include <IceUtil/Mutex.h>
#include <IceUtil/RWRecMutex.h>
#include "UserBaseDataI.h"
#include "TaskManager.h"

#include "ServiceI.h"
#include "QueryRunner.h"

namespace xce
{
namespace userbase
{

const int TASK_LEVEL_FILL = 1;
const int TASK_LEVEL_LOAD = 10;

const int LOAD_TYPE_USERPASSPORT = 1;
const int LOAD_TYPE_USERRIGHT = 2;
const int LOAD_TYPE_USERNAMES = 3;
const int LOAD_TYPE_USERSTATE = 4;
const int LOAD_TYPE_USERURL = 5;
const int LOAD_TYPE_USERLOGINCOUNT = 6;
const int LOAD_TYPE_USERBORN = 7;
const int LOAD_TYPE_USERSTAGE = 8;
const int LOAD_TYPE_USERDOING = 9;
const int LOAD_TYPE_USERBASIC = 10;
const int LOAD_TYPE_USERBASECOMMONVIEW = 11;
const int LOAD_TYPE_USERBASEFULLVIEW = 12;

//cache ID
const Ice::Int USER_PASSPORT   = 0;
const Ice::Int USER_NAME       = 1;
const Ice::Int USER_RIGHT      = 2;
const Ice::Int USER_LOGINCOUNT = 3;
const Ice::Int USER_STATE = 4;
const Ice::Int USER_URL = 5;
const Ice::Int USER_BORN  = 7;
const Ice::Int USER_STAGE    = 8;
const Ice::Int USER_DOING = 9;
//service name
const string USER_BASEVIEW_MANAGER   = "UBVM";
const string USER_PASSPORT_MANAGER   = "UPM";
const string USER_NAME_MANAGER       = "UNM";
const string USER_RIGHT_MANAGER      = "URM";
const string USER_STATE_MANAGER      = "USM";
const string USER_LOGINCOUNT_MANAGER = "ULCM";
const string USER_URL_MANAGER        = "U";
const string USER_BASIC_MANAGER      = "UBM";

const string USER_BORN_MANAGER       = "UBOM";
const string USER_STAGE_MANAGER      = "USGM";
const string USER_DOING_MANAGER      = "UDM";
//-------------------------UserPassportManagerI-------------------------------
class UserPassportManagerI : virtual public UserPassportManager, public MyUtil::Singleton<UserPassportManagerI>
{
public:
	UserPassportManagerI() : valid_(false) {}
	
    virtual void createUserPassport(
		const Str2StrMap& props,
		const Ice::Current& = Ice::Current());

    virtual UserPassportDataPtr getUserPassport(
		Ice::Int uid,
		const Ice::Current& = Ice::Current());

    virtual void setUserPassport(
		Ice::Int uid,
		const MyUtil::Str2StrMap& props,
		const Ice::Current& = Ice::Current());

	virtual void reload(Ice::Int userId,const Ice::Current& =Ice::Current());

	virtual Ice::Int getUserIdByEmail(const string& email,const Ice::Current& =Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
private:
	UserPassportDataIPtr m_getUserPassport(int userId);
	bool valid_;
	IceUtil::RWRecMutex validMutex_;
};




//-------------------------UserNameManagerI-------------------------------
class UserNameManagerI : virtual public UserNameManager, public MyUtil::Singleton<UserNameManagerI>
{
public:
	UserNameManagerI() : valid_(false) {}
	
    virtual void createUserName(
		const Str2StrMap& props,
		const Ice::Current& = Ice::Current());

    virtual UserNameDataPtr getUserName(
		Ice::Int uid,
		const Ice::Current& = Ice::Current());

    virtual void setUserName(
		Ice::Int uid,
		const MyUtil::Str2StrMap& props,
		const Ice::Current& = Ice::Current());

	virtual void reload(Ice::Int userId,const Ice::Current& =Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
private:
	UserNameDataIPtr m_getUserName(int userId);
	bool valid_;
	IceUtil::RWRecMutex validMutex_;
};


//-------------------------UserRightManagerI-------------------------------
class UserRightManagerI : virtual public UserRightManager, public MyUtil::Singleton<UserRightManagerI>
{
public:
	UserRightManagerI() : valid_(false) {}
	
    virtual void createUserRight(
		const Str2StrMap& props,
		const Ice::Current& = Ice::Current());

    virtual UserRightDataPtr getUserRight(
		Ice::Int uid,
		const Ice::Current& = Ice::Current());

    virtual void setUserRight(
		Ice::Int uid,
		const MyUtil::Str2StrMap& props,
		const Ice::Current& = Ice::Current());

	virtual void reload(Ice::Int userId,const Ice::Current& =Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
private:
	UserRightDataIPtr m_getUserRight(int userId);
	bool valid_;
	IceUtil::RWRecMutex validMutex_;
};

//-------------------------UserStateManagerI-------------------------------
class UserStateManagerI : virtual public UserStateManager, public MyUtil::Singleton<UserStateManagerI>
{
public:
	UserStateManagerI() : valid_(false) {}
	
    virtual void createUserState(
		const Str2StrMap& props,
		const Ice::Current& = Ice::Current());

    virtual UserStateDataPtr getUserState(
		Ice::Int uid,
		const Ice::Current& = Ice::Current());

    virtual void setUserState(
		Ice::Int uid,
		const MyUtil::Str2StrMap& props,
		const Ice::Current& = Ice::Current());

	virtual void reload(Ice::Int userId,const Ice::Current& =Ice::Current());
	virtual void setStateOn(Ice::Int userId, Ice::Int pos, const Ice::Current& = Ice::Current());
        virtual void setStateOff(Ice::Int userId, Ice::Int pos, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
private:
	UserStateDataIPtr m_getUserState(int userId);
	bool valid_;
	IceUtil::RWRecMutex validMutex_;
};


//-------------------------UserLoginCountManagerI-------------------------------
class UserLoginCountManagerI : virtual public UserLoginCountManager, public MyUtil::Singleton<UserLoginCountManagerI>
{
public:
	UserLoginCountManagerI() : valid_(false) {}
	
    virtual void createUserLoginCount(
		const Str2StrMap& props,
		const Ice::Current& = Ice::Current());

    virtual int getUserLoginCount(
		Ice::Int uid,
		const Ice::Current& = Ice::Current());

    virtual void incUserLoginCount(
		Ice::Int uid,
		const Ice::Current& = Ice::Current());

	virtual void reload(Ice::Int userId,const Ice::Current& =Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
private:
	UserLoginCountDataIPtr m_getUserLoginCount(int userId);
	bool valid_;
	IceUtil::RWRecMutex validMutex_;
};

//----------------------------------------------------------------------------
class UserUrlManagerI : virtual public UserUrlManager, public MyUtil::Singleton<UserUrlManagerI>{
public:
	UserUrlManagerI() : valid_ (false) {}
	virtual UserUrlDataPtr getUserUrl(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setUserUrl(Ice::Int id, const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void createUserUrl(const MyUtil::Str2StrMap& content, const Ice::Current& = Ice::Current());
	virtual void reload(Ice::Int id, const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
private:
	UserUrlDataIPtr m_getUserUrl(int userId);
	bool valid_;
	IceUtil::RWRecMutex validMutex_;
};

//----------------------------------------------------------------------------


class UserBasicManagerI : virtual public UserBasicManager ,  public MyUtil::Singleton<UserBasicManagerI>
{
public:
	UserBasicManagerI() : valid_(false){}
	virtual UserBasicDataPtr getUserBasic(Ice::Int userId,	const Ice::Current& = Ice::Current());
	void setValid(bool valid);
	void setValidForce(bool valid, const Ice::Current& = Ice::Current());

	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
private:
	UserBasicDataIPtr m_getUserBasic(int userId);
	
public:
	void parseData(UserBasicDataIPtr& desData, const UserBornDataPtr& bornData, 
		           const UserStageDataPtr& stageData, const UserDoingDataPtr& doingData,
				   const UserNameDataIPtr& nameData, const UserUrlDataPtr&  urlData, 
				   const UserPassportDataIPtr&  passportData, const UserStateDataIPtr& stateData );
	bool valid_;
	IceUtil::RWRecMutex validMutex_;
};

//----------------------------------------------------------------------------

class UserBornManagerI : virtual public UserBornManager ,  public MyUtil::Singleton<UserBornManagerI>
{
public:
	UserBornManagerI() : valid_(false){}
	virtual void createUserBorn(const Str2StrMap& props,const Ice::Current& = Ice::Current());
	virtual void reloadUserBorn(Ice::Int userId,const Ice::Current& = Ice::Current());
	virtual UserBornDataPtr getUserBorn(Ice::Int userId,	const Ice::Current& = Ice::Current());
	virtual void setUserBorn(Ice::Int userId, const Str2StrMap& props,const Ice::Current& = Ice::Current());
	
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
private:
	UserBornDataIPtr m_getUserBorn(int userId);
	bool valid_;
	IceUtil::RWRecMutex validMutex_;
};


//----------------------------------------------------------------------------


class UserStageManagerI : virtual public UserStageManager ,  public MyUtil::Singleton<UserStageManagerI>
{
public:
	UserStageManagerI() : valid_ (false){}
	virtual void createUserStage(const Str2StrMap& props,const Ice::Current& = Ice::Current());
	virtual void reloadUserStage(Ice::Int userId,const Ice::Current& = Ice::Current());
	virtual UserStageDataPtr getUserStage(Ice::Int userId,	const Ice::Current& = Ice::Current());
	virtual void setUserStage(Ice::Int userId, const Str2StrMap& props,const Ice::Current& = Ice::Current());

	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
private:
	UserStageDataIPtr m_getUserStage(int userId);
	bool valid_;
	IceUtil::RWRecMutex validMutex_;
};




//----------------------------------------------------------------------------


class UserDoingManagerI : virtual public UserDoingManager ,  public MyUtil::Singleton<UserDoingManagerI>
{
public:
	UserDoingManagerI() : valid_ (false){}
	virtual void createUserDoing(const Str2StrMap& props,const Ice::Current& = Ice::Current());
	virtual void reloadUserDoing(Ice::Int userId,const Ice::Current& = Ice::Current());
	virtual UserDoingDataPtr getUserDoing(Ice::Int userId,	const Ice::Current& = Ice::Current());
	virtual void setUserDoing(Ice::Int userId, const Str2StrMap& props,const Ice::Current& = Ice::Current());
	
	virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
private:
	UserDoingDataIPtr m_getUserDoing(int userId);
	bool valid_;
	IceUtil::RWRecMutex validMutex_;
};



//-------------------------UserBaseViewManagerI-------------------------------
class UserBaseViewManagerI : virtual public UserBaseViewManager, public MyUtil::Singleton<UserBaseViewManagerI>
{
public:
	UserBaseViewManagerI() : valid_ (false) {}
	
    virtual UserBaseViewDataPtr getUserBaseView( Ice::Int uid,
                                        	  const Ice::Current& = Ice::Current());
	virtual UserCommonViewDataPtr getUserCommonView(int uid, 
			                                  const Ice::Current& = Ice::Current());

	virtual bool isValid(const ::Ice::Current& = ::Ice::Current());
	
	void setValid(bool valid);
	
	void setValidForce(bool valid, const Ice::Current& = Ice::Current());
private:
	UserBasicDataIPtr parseToUserBasicData( const int uid,
			                            const UserNameDataIPtr& nameData, 
										const UserUrlDataPtr&  urlData, 
										const UserPassportDataIPtr&  passportData, 
										const UserStateDataIPtr& stateData );
	bool valid_;
	IceUtil::RWRecMutex validMutex_;

};


template <class T,class P> class FillTask: virtual public ::MyUtil::Task {
public:
        FillTask(const Ice::Int identifier, const ::MyUtil::ObjectResultPtr& userData): MyUtil::Task(TASK_LEVEL_FILL),
			     _identifier(identifier),_userData(userData)
	{
	}
        virtual void handle()
		{
			MCE_INFO("[FillTask::handle]" << _identifier << "  set Data size : " << _userData->data.size());
			for(MyUtil::ObjectMap::iterator it = _userData->data.begin();it != _userData->data.end();++it)
			{
				T obj = new P;
				obj->parse(it->second);
				MyUtil::ServiceI::instance().getObjectCache()->addObject(_identifier , it->first, obj);
			}
		}

private:
		Ice::Int _identifier;
        ::MyUtil::ObjectResultPtr _userData;
};

class LoadTask : virtual public ::MyUtil::Task {
public:
	LoadTask(int type, long userid) : MyUtil::Task(TASK_LEVEL_LOAD), type_(type), userid_(userid) {}
	virtual void handle();
private:
	void LoadUserPassport();
	void LoadUserRight();
	void LoadUserState();
	void LoadUserNames();
	void LoadUserLoginCount();
	void LoadUserUrl();
	void LoadUserBorn();
	void LoadUserStage();
	void LoadUserDoing();
	void LoadUserBasic();
	void LoadUserBaseCommonView();
	void LoadUserBaseFullView();
	int type_;
	long userid_;
};


};
};
#endif

