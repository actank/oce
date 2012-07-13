#ifndef __USERADSCACHEI_H__
#define __USERADSCACHEI_H__

#include <UserAdsCache.h>
#include <ServiceI.h>
#include <Allocator.h>
#include <Allocator.h>
namespace xce {
namespace useradscache {

const std::string DB_USER_BASIC = "useradscache_user_basic";
const std::string DB_USER_PASSPORT = "useradscache_user_passport";
const std::string DB_USER_BORN = "useradscache_user_born";
const std::string DB_USER_STAGE = "useradscache_user_stage";
const std::string DB_USER_URL = "useradscache_user_url";
const std::string DB_REGION_INFO = "useradscache_region_info";
const std::string DB_WORKPLACE_INFO = "useradscache_workplace_info";
const std::string DB_NETWORK_HISTORY = "useradscache_network_history";
const std::string DB_ELEMENTARY_SCHOOL_INFO = "useradscache_elementary_school_info";
const std::string DB_JUNIOR_HIGH_SCHOOL_INFO = "useradscache_junior_high_school_info";
const std::string DB_HIGH_SCHOOL_INFO = "useradscache_high_school_info";
const std::string DB_COLLEGE_INFO = "useradscache_college_info";
const std::string DB_UNIVERSITY_INFO = "useradscache_university_info";

const ::Ice::Int USER_ADS_CACHE_URL = 0;
const ::std::string USER_ADS_CACHE_URL_MANAGER = "M";
const int TASK_LEVEL_FILL = 1;

class UserAdsCacheManagerI: virtual public UserAdsCacheManager,
                public ::MyUtil::Singleton<UserAdsCacheManagerI> {
public:
	virtual UserAdsCacheUrlDataPtr getUserAdsCacheUrl(::Ice::Int userId, const ::Ice::Current& = ::Ice::Current());
	virtual Int2UserAdsCacheUrlDataMap getUserAdsCacheUrlList(const ::MyUtil::IntSeq& userIds, const ::Ice::Current& = ::Ice::Current());

	virtual UserAdsCacheDataPtr getUserAdsCache(::Ice::Int userId, const ::Ice::Current& = ::Ice::Current());
	virtual Int2UserAdsCacheDataMap getUserAdsCacheList(const ::MyUtil::IntSeq& userIds, const ::Ice::Current& = ::Ice::Current());
	virtual bool isValid(const Ice::Current& = ::Ice::Current());
        virtual void setValid(bool valid, const ::Ice::Current& = ::Ice::Current());
	virtual void setData(const ::MyUtil::ObjectResultPtr& userData, const ::Ice::Current& = ::Ice::Current());

	int mod() const {return _mod;}
	void set_mod(int mod) {_mod = mod;}
private:
	UserAdsCacheManagerI();
        UserAdsCacheUrlDataPtr getUserAdsCacheUrlData(int userId);
        ::MyUtil::ObjectResultPtr getUserAdsCacheUrlData(const ::MyUtil::IntSeq& userIds);

       UserAdsCacheDataPtr getUserAdsCacheData(int userId);
        ::MyUtil::ObjectResultPtr getUserAdsCacheData(const ::MyUtil::IntSeq& userIds);
	UserAdsCacheDataPtr toOld(const UserAdsCacheUrlDataPtr& obj);
	friend class MyUtil::Singleton<UserAdsCacheManagerI>;
	bool _valid;
	IceUtil::RWRecMutex _validMutex;
        int _mod;
};

class FillTask: virtual public ::MyUtil::Task {
public:
        FillTask(const ::MyUtil::ObjectResultPtr& userData);
        virtual void handle();
private:
        ::MyUtil::ObjectResultPtr _userData;
};

class UserAdsCacheFactoryI: public ::MyUtil::ServantFactory {
public:
        virtual ::Ice::ObjectPtr create(::Ice::Long id);
        virtual ::MyUtil::ObjectResultPtr create(const ::MyUtil::LongSeq& ids);
};

class ServiceSchemaI: public ::MyUtil::Descriptor {
public:
        virtual void configure(const ::Ice::PropertiesPtr& props);
};

//---------------------------------Thread-----------------------------

class BatchUserStageThread: public IceUtil::Thread {
public:
	BatchUserStageThread(const std::string& ids, const MyUtil::ObjectResultPtr& result);
	virtual ~BatchUserStageThread() {}
	virtual void run();
private:
	com::xiaonei::xce::Statement _sql;
	MyUtil::ObjectResultPtr _result;
};
class BatchUserBornThread: public IceUtil::Thread {
public:
	BatchUserBornThread(const std::string& ids, const MyUtil::ObjectResultPtr& result);
	virtual ~BatchUserBornThread() {}
	virtual void run();
private:
	com::xiaonei::xce::Statement _sql;
	MyUtil::ObjectResultPtr _result;
};
class BatchUserUrlThread: public IceUtil::Thread {
public:
	BatchUserUrlThread(const std::string& ids, const MyUtil::ObjectResultPtr& result);
	virtual ~BatchUserUrlThread() {}
	virtual void run();
private:
	com::xiaonei::xce::Statement _sql;
	MyUtil::ObjectResultPtr _result;
};

class BatchRegionInfoThread: public IceUtil::Thread {
public:
	BatchRegionInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result);
	virtual ~BatchRegionInfoThread() {}
	virtual void run();
private:
	com::xiaonei::xce::Statement _sql;
	MyUtil::ObjectResultPtr _result;
};

class BatchNetworkHistoryThread: public IceUtil::Thread {
public:
	BatchNetworkHistoryThread(const std::string& ids, const MyUtil::ObjectResultPtr& result);
	virtual ~BatchNetworkHistoryThread() {}
	virtual void run();
private:
	com::xiaonei::xce::Statement _sql;
	MyUtil::ObjectResultPtr _result;
};

class BatchWorkplaceInfoThread: public IceUtil::Thread {
public:
	BatchWorkplaceInfoThread(const string& ids, const MyUtil::ObjectResultPtr& result);
	virtual ~BatchWorkplaceInfoThread() {}
	virtual void run();
private:
	com::xiaonei::xce::Statement _sql;
	MyUtil::ObjectResultPtr _result;
};

class BatchElementarySchoolInfoThread: public IceUtil::Thread {
public:
	BatchElementarySchoolInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result);
	virtual ~BatchElementarySchoolInfoThread() {}
	virtual void run();
private:
	com::xiaonei::xce::Statement _sql;
	MyUtil::ObjectResultPtr _result;
};

class BatchJuniorHighSchoolInfoThread: public IceUtil::Thread {
public:
	BatchJuniorHighSchoolInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result);
	virtual ~BatchJuniorHighSchoolInfoThread() {}
	virtual void run();
private:
	com::xiaonei::xce::Statement _sql;
	MyUtil::ObjectResultPtr _result;
};

class BatchHighSchoolInfoThread: public IceUtil::Thread {
public:
	BatchHighSchoolInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result);
	virtual ~BatchHighSchoolInfoThread() {}
	virtual void run();
private:
	com::xiaonei::xce::Statement _sql;
	MyUtil::ObjectResultPtr _result;
};

class BatchCollegeInfoThread: public IceUtil::Thread {
public:
	BatchCollegeInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result);
	virtual ~BatchCollegeInfoThread() {}
	virtual void run();
private:
	com::xiaonei::xce::Statement _sql;
	MyUtil::ObjectResultPtr _result;
};

class BatchUniversityInfoThread: public IceUtil::Thread {
public:
	BatchUniversityInfoThread(const std::string& ids, const MyUtil::ObjectResultPtr& result);
	virtual ~BatchUniversityInfoThread() {}
	virtual void run();
private:
	com::xiaonei::xce::Statement _sql;
	MyUtil::ObjectResultPtr _result;
};

//---------------------------------BatchResultHandler-----------------------------

class BatchUserPassportResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserPassportResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};
class BatchUserBornResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserBornResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};
class BatchUserStageResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStageResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserUrlResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserUrlResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchRegionResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchRegionResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchWorkplaceResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchWorkplaceResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchNetworkResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchNetworkResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchElementaryResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchElementaryResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchJuniorResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchJuniorResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchSeniorResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchSeniorResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchCollegeResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchCollegeResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUniversityResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
        BatchUniversityResultHandlerI(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};
//---------------------------------BatchResultHandler end-----------------------------


}
}

#endif
