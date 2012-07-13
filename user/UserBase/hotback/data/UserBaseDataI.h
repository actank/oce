#ifndef __USERBASEDATAI_H__
#define __USERBASEDATAI_H__

#include "UserBase.h"
#include "boost/lexical_cast.hpp"
#include "mysql++/mysql++.h"

#include <TaskManager.h>
#include <QueryRunner.h>
#include <Ice/Ice.h>

#include "ObjectCacheI.h"
#include "DataSet.h"
namespace xce
{
namespace userbase
{
using namespace MyUtil;

using namespace std;

const string COLUMN_ID = "ID";
//database db name
const string DB_USER_PASSPORT   = "user_passport";
const string DB_USER_NAME       = "user_names";
const string DB_USER_RIGHT      = "user_right";
const string DB_USER_LOGINCOUNT = "user_logincount";
const string DB_USER_STATE	= "user_state";

const string DB_USER_BASIC   = "user_basic";
const string DB_USER_URL   = "user_url";

const string DB_USER_BORN    = "user_born";
const string DB_USER_STAGE      = "user_stage";
const string DB_USER_DOING   = "user_doing";
//database table name
const string TABLE_USER_PASSPORT   = "user_passport";
const string TABLE_USER_NAME       = "user_names";
const string TABLE_USER_RIGHT      = "user_right";
const string TABLE_USER_LOGINCOUNT = "user_logincount";
const string TABLE_USER_STATE      = "user_state";

const string TABLE_USER_BASIC = "user_basic";
const string TABLE_USER_URL = "user_url";

const string TABLE_USER_BORN  = "user_born";
const string TABLE_USER_STAGE    = "user_stage";
const string TABLE_USER_DOING = "user_doing";
//----------------------------------------------------------------------------
class UserPassportDataI;
typedef IceUtil::Handle<UserPassportDataI> UserPassportDataIPtr;
class UserPassportDataI : public UserPassportData
{
public:
        UserPassportDataIPtr getData();
        void setProperty(const string& key, const string& value);
        void setProperties(const Str2StrMap& properties);

        void parse(mysqlpp::Row& row);
        void parse(Ice::ObjectPtr& obj);
private:
		IceUtil::Mutex m_mutex;
};

class BatchPassportResHandlerI : public com::xiaonei::xce::ResultHandler
{
	public:
		BatchPassportResHandlerI( UserPassportDataIPtr& rs ) : m_rs(rs){};
		virtual bool handle(mysqlpp::Row& row) const ;
	private:
		UserPassportDataIPtr m_rs; 
};

class UserEmailResultHandlerI : public com::xiaonei::xce::ResultHandler
{
	public:
		UserEmailResultHandlerI( Ice::Int& rs );
		virtual bool handle(mysqlpp::Row& row) const ;
	private:
		Ice::Int& m_rs; 
};


class UserPassportFactoryI : public ServantFactory {
public:
    virtual Ice::ObjectPtr create(Ice::Int id);
};

//----------------------------------------------------------------------------
class UserNameDataI;
typedef IceUtil::Handle<UserNameDataI> UserNameDataIPtr;
class UserNameDataI : public UserNameData
{
public:
        UserNameDataIPtr getData();
        void setProperty(const string& key, const string& value);
        void setProperties(const Str2StrMap& properties);

        void parse(mysqlpp::Row& row);
        void parse(Ice::ObjectPtr& obj);
private:
		IceUtil::Mutex m_mutex;
};


class BatchNameResHandlerI : public com::xiaonei::xce::ResultHandler
{
	public:
		BatchNameResHandlerI( UserNameDataIPtr& rs ) : m_rs(rs){};
		virtual bool handle(mysqlpp::Row& row) const ;
	private:
		UserNameDataIPtr m_rs; 
};

class UserNameFactoryI : public ServantFactory {
public:
    virtual Ice::ObjectPtr create(Ice::Int id);
};

//----------------------------------------------------------------------------

class UserStateDataI;
typedef IceUtil::Handle<UserStateDataI> UserStateDataIPtr;
class UserStateDataI : public UserStateData
{
public:
        UserStateDataIPtr getData();
        void setProperty(const string& key, const string& value);
        void setProperties(const Str2StrMap& properties);
		void setStateOn(uint32_t pos) {
			IceUtil::Mutex::Lock lock(m_mutex);
			state |= (1 << pos);
        }
        void setStateOff(uint32_t pos) {
			IceUtil::Mutex::Lock lock(m_mutex);
			state &= (~(1 << pos));
        }

		int getState()
		{
			IceUtil::Mutex::Lock lock(m_mutex);
			return state;
        }
        
		void parse(mysqlpp::Row& row);
        void parse(Ice::ObjectPtr& obj);
private:
		IceUtil::Mutex m_mutex;
};


class BatchStateResHandlerI : public com::xiaonei::xce::ResultHandler
{
	public:
		BatchStateResHandlerI( UserStateDataIPtr& rs ) : m_rs(rs){};
		virtual bool handle(mysqlpp::Row& row) const ;
	private:
		UserStateDataIPtr m_rs; 
};

class UserStateFactoryI : public ServantFactory {
public:
    virtual Ice::ObjectPtr create(Ice::Int id);
};


//----------------------------------------------------------------------------
class UserRightDataI;
typedef IceUtil::Handle<UserRightDataI> UserRightDataIPtr;
class UserRightDataI : public UserRightData
{
public:
        UserRightDataIPtr getData();
        void setProperty(const string& key, const string& value);
        void setProperties(const Str2StrMap& properties);

        void parse(mysqlpp::Row& row);
        void parse(Ice::ObjectPtr& obj);
private:
		IceUtil::Mutex m_mutex;
};


class BatchRightResHandlerI : public com::xiaonei::xce::ResultHandler
{
	public:
		BatchRightResHandlerI( UserRightDataIPtr& rs ) : m_rs(rs){};
		virtual bool handle(mysqlpp::Row& row) const ;
	private:
		UserRightDataIPtr m_rs; 
};


class UserRightFactoryI : public ServantFactory {
public:
    virtual Ice::ObjectPtr create(Ice::Int id);
};

//----------------------------------------------------------------------------
class UserLoginCountDataI;
typedef IceUtil::Handle<UserLoginCountDataI> UserLoginCountDataIPtr;
class UserLoginCountDataI : public UserLoginCountData
{
public:
        UserLoginCountDataIPtr getData();
		int getLogincount();
        void setProperty(const string& key, const string& value);
        void setProperties(const Str2StrMap& properties);

        void incLogincount();
        
		void parse(mysqlpp::Row& row);
        void parse(Ice::ObjectPtr& obj);
private:
		IceUtil::Mutex m_mutex;
};


class BatchLoginCountResHandlerI : public com::xiaonei::xce::ResultHandler
{
	public:
		BatchLoginCountResHandlerI( UserLoginCountDataIPtr& rs ) : m_rs(rs){};
		virtual bool handle(mysqlpp::Row& row) const ;
	private:
		UserLoginCountDataIPtr m_rs; 
};

class UserLoginCountFactoryI : public ServantFactory {
public:
    virtual Ice::ObjectPtr create(Ice::Int id);
};

//----------------------------------------------------------------------------
class UserUrlDataI;
typedef ::IceUtil::Handle<UserUrlDataI> UserUrlDataIPtr;
class UserUrlDataI: virtual public UserUrlData {
public:
	void setProperities(const MyUtil::Str2StrMap& properities);
	UserUrlDataIPtr getData();
    void parse(mysqlpp::Row& row);
	void parse(Ice::ObjectPtr& obj);
private:
	IceUtil::RWRecMutex _mutex;
};

class UserUrlResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	UserUrlResultHandlerI(const UserUrlDataIPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	UserUrlDataIPtr _result;	
};

class UserUrlFactoryI : public MyUtil::ServantFactory{
public:
	virtual Ice::ObjectPtr create(Ice::Int id);
};


//----------------------------------------------------------------------------

class UserBasicDataI : virtual public UserBasicDataN
{
public:
    
	void setProperty(const string& key, const string& value);
	void setProperties(const Str2StrMap& props);
    void parse(mysqlpp::Row& row);
	void parse(Ice::ObjectPtr& obj);
	UserBasicDataNPtr getData();
private:
	IceUtil::Mutex m_mutex;
};

typedef IceUtil::Handle<UserBasicDataI> UserBasicDataIPtr;

class UserBasicResultHandlerI: public com::xiaonei::xce::ResultHandler
{
public:
	UserBasicResultHandlerI(const UserBasicDataIPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	UserBasicDataIPtr m_result;
};

class UserBasicFactoryI : public MyUtil::ServantFactory
{
public:
	Ice::ObjectPtr create(Ice::Int id);
};
//----------------------------------------------------------------------------

class UserBornDataI;

typedef IceUtil::Handle<UserBornDataI> UserBornDataIPtr;

class UserBornDataI : virtual public UserBornDataN
{
public:
    
	void setProperty(const string& key, const string& value);
	void setProperties(const Str2StrMap& props);
    void parse(mysqlpp::Row& row);
	void parse(Ice::ObjectPtr& obj);
	UserBornDataIPtr getData();
private:
	IceUtil::Mutex m_mutex;
};


class UserBornResultHandlerI: public com::xiaonei::xce::ResultHandler
{
public:
	UserBornResultHandlerI(const UserBornDataIPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	UserBornDataIPtr m_result;
};

class UserBornFactoryI : public MyUtil::ServantFactory
{
public:
	Ice::ObjectPtr create(Ice::Int id);
};
//----------------------------------------------------------------------------
class UserStageDataI;

typedef IceUtil::Handle<UserStageDataI> UserStageDataIPtr;
class UserStageDataI : virtual public UserStageData
{
public:
    
	void setProperty(const string& key, const string& value);
	void setProperties(const Str2StrMap& props);
    void parse(mysqlpp::Row& row);
	void parse(Ice::ObjectPtr& obj);
	UserStageDataIPtr getData();
private:
	IceUtil::Mutex m_mutex;
};


class UserStageResultHandlerI: public com::xiaonei::xce::ResultHandler
{
public:
	UserStageResultHandlerI(const UserStageDataIPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	UserStageDataIPtr m_result;
};

class UserStageFactoryI : public MyUtil::ServantFactory
{
public:
	Ice::ObjectPtr create(Ice::Int id);
};
//----------------------------------------------------------------------------
class UserDoingDataI;
typedef IceUtil::Handle<UserDoingDataI> UserDoingDataIPtr;
class UserDoingDataI : virtual public UserDoingData
{
public:
    
	void setProperty(const string& key, const string& value);
	void setProperties(const Str2StrMap& props);
    void parse(mysqlpp::Row& row);
	void parse(Ice::ObjectPtr& obj);
	UserDoingDataIPtr getData();
private:
	IceUtil::Mutex m_mutex;
};


class UserDoingResultHandlerI: public com::xiaonei::xce::ResultHandler
{
public:
	UserDoingResultHandlerI(const UserDoingDataIPtr& result);
	virtual bool handle(mysqlpp::Row&) const;
private:
	UserDoingDataIPtr m_result;
};

class UserDoingFactoryI : public MyUtil::ServantFactory
{
public:
	Ice::ObjectPtr create(Ice::Int id);
};
//----------------------------------------------------------------------------

class UserBaseViewDataI;
typedef IceUtil::Handle<UserBaseViewDataI> UserBaseViewDataIPtr;
class UserBaseViewDataI : public UserBaseViewDataN
{
public:

        UserBaseViewDataI(const UserPassportDataIPtr& updi, 
				          const UserNameDataIPtr& undi,
				          const UserRightDataIPtr& urdi,
	                	  const UserLoginCountDataIPtr& ulcdi, const UserStateDataIPtr& usdi, 
						  const  UserUrlDataPtr& uurldi, const UserBasicDataNPtr& ubdi )
		{
			upd = updi;

			und = undi;

			urd = urdi;

			ulcd = ulcdi;
			
			usd = usdi;

			userurl= uurldi;

			userbasic = ubdi;

		};
};

};
};
#endif

