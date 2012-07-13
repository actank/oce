#ifndef __USERAPICACHEPRELOADER_H__
#define __USERAPICACHEPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <IceUtil/Thread.h>
#include <UserApiCache.h>

namespace xce{
namespace userapicachepreloader{

const std::string DB_BOOTSTRAP = "userapicache_bootstrap";

const std::string TABLE_USER_BORN = "user_born";
const std::string DB_USER_BORN = "userapicache_user_born";
const std::string TABLE_USER_STAGE = "user_stage";
const std::string DB_USER_STAGE = "userapicache_user_stage";
const std::string TABLE_USER_BASIC = "user_basic";
const std::string DB_USER_BASIC = "userapicache_user_basic";
const std::string TABLE_USER_CONFIG = "user_config";
const std::string DB_USER_CONFIG = "userapicache_user_config";
const std::string TABLE_USER_URL = "user_url";
const std::string DB_USER_URL = "userapicache_user_url";
const std::string TABLE_USER_RIGHT = "user_right";
const std::string DB_USER_RIGHT = "userapicache_user_right";
const std::string TABLE_USER_NAME = "user_names";
const std::string DB_USER_NAME = "userapicache_user_name";
const std::string TABLE_USER_PASSPORT = "user_passport";
const std::string DB_USER_PASSPORT = "userapicache_user_passport";
const std::string TABLE_USER_STATE = "user_state";
const std::string DB_USER_STATE = "userapicache_user_state";

const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 5;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserApiCachePreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserApiCachePreloader> {
public:
        UserApiCachePreloader () {}
        virtual ~UserApiCachePreloader () {}
};

class UserApiCachePreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserApiCachePreloaderFactory() {}
        virtual ~UserApiCachePreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class BatchUserStageResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStageResultHandler(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserBornResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserBornResultHandler(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserUrlResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserUrlResultHandler(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserConfigResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserConfigResultHandler(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserPassportResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserPassportResultHandler(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserNameResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserNameResultHandler(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserRightResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserRightResultHandler(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserStateResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStateResultHandler(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

}
}

#endif
