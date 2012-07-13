#ifndef __USERNETWORKPRELOADER_H__
#define __USERNETWORKPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include "user/UserNetwork/data/UserNetworkDataI.h"
#include <UserNetwork.h>

namespace xce{
namespace usernetworkpreloader{

//database table name
const string DB_NETWORK ="network_preloader";
//database table name
const string TABLE_NETWORK ="network_history";
const string TABLE_HIGH_SCHOOL_INFO = "high_school_info";
const string TABLE_UNIVERSITY_INFO = "university_info";
const string TABLE_WORKPLACE_INFO = "workplace_info";
const string TABLE_COLLEGE_INFO = "college_info";
const string TABLE_ELEMENTARY_SCHOOL_INFO = "elementary_school_info";
const string TABLE_JUNIOR_HIGH_SCHOOL_INFO = "junior_high_school_info";
const string TABLE_REGION_INFO = "region_info";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserNetworkPreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserNetworkPreloader> {
public:
        UserNetworkPreloader () {}
        virtual ~UserNetworkPreloader () {}
};

class UserNetworkPreloaderFactory : virtual public xce::generalpreloader::IdListFactory {
public:
        UserNetworkPreloaderFactory() {}
        virtual ~UserNetworkPreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
};

class BatchUserNetworkResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserNetworkResultHandler (MyUtil::ObjectResultPtr& results) : _results(results){};
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};

class BatchUserHighSchoolResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserHighSchoolResultHandler (MyUtil::ObjectResultPtr& results) : _results(results){};
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};

class BatchUserUniversityResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserUniversityResultHandler (MyUtil::ObjectResultPtr& results) : _results(results){};
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};

class BatchUserWorkplaceResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserWorkplaceResultHandler (MyUtil::ObjectResultPtr& results) : _results(results){};
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};

class BatchUserCollegeResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserCollegeResultHandler (MyUtil::ObjectResultPtr& results) : _results(results){};
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};

class BatchUserElementarySchoolResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserElementarySchoolResultHandler (MyUtil::ObjectResultPtr& results) : _results(results){};
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};

class BatchUserJuniorHighSchoolResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserJuniorHighSchoolResultHandler (MyUtil::ObjectResultPtr& results) : _results(results){};
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};

class BatchUserRegionResultHandler : public com::xiaonei::xce::ResultHandler {
public:
        BatchUserRegionResultHandler (MyUtil::ObjectResultPtr& results) : _results(results){};
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _results;
};
}
}

#endif
