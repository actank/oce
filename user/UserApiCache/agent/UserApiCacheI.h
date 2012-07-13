#ifndef __USERAPICACHEI_H__
#define __USERAPICACHEI_H__

#include <UserApiCache.h>
#include <ServiceI.h>

namespace xce {
namespace userapicache {

const int USER_API_CACHE = 0;
const int TASK_LEVEL_FILL = 10;
const int TASK_LEVEL_UPDATE = 11;

class UserApiCacheManagerI: virtual public UserApiCacheManager,
                public ::MyUtil::Singleton<UserApiCacheManagerI> {
public:
	UserApiCacheManagerI();
	virtual UserApiCacheDataPtr getUserApiCache(::Ice::Int userId, const ::Ice::Current& = ::Ice::Current());
	virtual Int2UserApiCacheDataMap getUserApiCacheList(const ::MyUtil::IntSeq& userIds, const ::Ice::Current& = ::Ice::Current());
	virtual Int2UserApiCacheData1Map getUserApiCache1List(const ::MyUtil::IntSeq& userIds, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const Ice::Current& = ::Ice::Current());
        virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());
	virtual void update(const UserApiCacheUpdateNewDataPtr& data, const ::Ice::Current& = ::Ice::Current());

	void set_mod(int mod) {mod_ = mod;MCE_INFO("UserApiCacheManagerI::set_mod -> "<<mod_);}
	int mod() const {return mod_;}
	void set_retention(const MyUtil::ObjectRetentionPtr& retention) {retention_ = retention;}
	MyUtil::ObjectRetentionPtr retention() {return retention_;}
private:
        ::MyUtil::ObjectResultPtr getUserApiCacheData(const ::MyUtil::IntSeq& userIds);

	bool _valid;
	MyUtil::ObjectRetentionPtr retention_;
        int mod_;
	IceUtil::RWRecMutex _validMutex;
};

class UserApiCacheEntry;

typedef IceUtil::Handle<UserApiCacheEntry> UserApiCacheEntryPtr;

class UserApiCacheEntry : public Ice::Object {
public:
	UserApiCacheEntry(const UserApiCacheEntryPtr& entry);
	UserApiCacheEntry(const UserApiCacheDataPtr& data);
	UserApiCacheEntry(const UserApiCacheData1Ptr& data);

	UserApiCacheData1Ptr GetUserApiCacheData1() const;
	UserApiCacheDataPtr GetUserApiCacheData() const;

	void set_id(int id) {id_ = id;};
	void set_status(int status) {status_ = status;}
	void set_name(const string& name) {name_ = name;}
	void set_state(int state) {state = state_;}
	void set_level(int level) {level = level_;}
	void set_auth(int auth) {auth_ = auth;}
	void set_statusconfig(int statusconfig) {statusconfig_ = statusconfig;}
	void set_basicconfig(int basicconfig) {basicconfig_ = basicconfig;}
	void set_profileprivacy(const string& profileprivacy) {profileprivacy_ = profileprivacy;}
	void set_stage(int stage) {stage_ = stage;}
	void set_gender(const string& gender) {gender_ = gender;}
	void set_univ(int univ) {univ_ = univ;}
	void set_birthyear(int birthyear) {birthyear_ = birthyear;}
	void set_birthmonth(int birthmonth) {birthmonth_ = birthmonth;}
	void set_birthday(int birthday) {birthday_ = birthday;}
	void set_homeprovince(const string& homeprovince) {homeprovince_ = homeprovince;}
	void set_homecity(const string&homecity) {homecity_ = homecity;}
	void set_mainurl(const string& mainurl) {mainurl_ = mainurl;}
	void set_headurl(const string& headurl) {headurl_ = headurl;}
	void set_tinyurl(const string& tinyurl) {tinyurl_ = tinyurl;}
	
private:
	int id_;
	int status_;
	string name_;
	int state_;
	int level_;
	int auth_;
	int statusconfig_;
	int basicconfig_;
	string profileprivacy_;
	int stage_;
	string gender_;
	int univ_;
	int birthyear_;
	int birthmonth_;
	int birthday_;
	string homeprovince_;
	string homecity_;
	string mainurl_;
	string headurl_;
	string tinyurl_;
};

class FillTask: virtual public ::MyUtil::Task {
public:
        FillTask(const ::MyUtil::ObjectResultPtr& userData) : _userData(userData) {}
        virtual void handle();
private:
        ::MyUtil::ObjectResultPtr _userData;
};

class UpdateTask: public MyUtil::Task {
public:
        UpdateTask(const UserApiCacheUpdateNewDataPtr& data, const Ice::Current& current):Task(TASK_LEVEL_UPDATE),data_(data), current_(current){}
        virtual void handle();
private:
        void UpdateData(int id, const Byte2StrMap& data);
        void UpdateData(UserApiCacheEntryPtr& data, const Byte2StrMap&);
        UserApiCacheUpdateNewDataSeq data_;
        Ice::Current current_;
};

class UserApiCacheFactoryI: public ::MyUtil::ServantFactory {
public:
        virtual ::Ice::ObjectPtr create(::Ice::Long id);
        virtual ::MyUtil::ObjectResultPtr create(const ::MyUtil::LongSeq& ids);
};

}
}

#endif
