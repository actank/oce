#ifndef __USERAPICACHE_LOADER_I_H__
#define __USERAPICACHE_LOADER_I_H__

#include <ServiceI.h>
#include <UserApiCache.h>
#include <QueryRunner.h>
#include "user/UserApiCache/share/UserApiCacheDataI.cpp"
#include "ReplicaCluster/CacheLoader/src/CacheLoaderFactory.h"

namespace xce {
namespace userapicache {

const std::string DB_BOOTSTRAP = "userapicache_bootstrap";

const std::string TABLE_USER_BORN = "user_born";
const std::string DB_USER_BORN = "userapicache_user_born";
const std::string DB_USER_BORN_PRELOADER = "userapicache_user_born_preloader";
const std::string TABLE_USER_STAGE = "user_stage";
const std::string DB_USER_STAGE = "userapicache_user_stage";
const std::string DB_USER_STAGE_PRELOADER = "userapicache_user_stage_preloader";
const std::string TABLE_USER_CONFIG = "user_config";
const std::string DB_USER_CONFIG = "userapicache_user_config";
const std::string DB_USER_CONFIG_PRELOADER = "userapicache_user_config_preloader";
const std::string TABLE_USER_URL = "user_url";
const std::string DB_USER_URL = "userapicache_user_url";
const std::string DB_USER_URL_PRELOADER = "userapicache_user_url_preloader";
const std::string TABLE_USER_RIGHT = "user_right";
const std::string DB_USER_RIGHT = "userapicache_user_right";
const std::string DB_USER_RIGHT_PRELOADER = "userapicache_user_right_preloader";
const std::string TABLE_USER_NAME = "user_names";
const std::string DB_USER_NAME = "userapicache_user_name";
const std::string DB_USER_NAME_PRELOADER = "userapicache_user_name_preloader";
const std::string TABLE_USER_PASSPORT = "user_passport";
const std::string DB_USER_PASSPORT = "userapicache_user_passport";
const std::string DB_USER_PASSPORT_PRELOADER = "userapicache_user_passport_preloader";
const std::string TABLE_USER_STATE = "user_state";
const std::string DB_USER_STATE = "userapicache_user_state";
const std::string DB_USER_STATE_PRELOADER = "userapicache_user_state_preloader";

const int PRODUCE_BATCH_SIZE = 3000;
const int CONSUME_BATCH_SIZE = 3000;
const int WRITE_BATCH_SIZE = 3000;

class UserApiCacheFactory : virtual public xce::cacheloader::MinMaxFactory {
public:
        UserApiCacheFactory() {}
        virtual ~UserApiCacheFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};

class UserApiCacheLoaderI : public UserApiCacheLoader,  virtual public MyUtil::Singleton<UserApiCacheLoaderI> {
public:
        UserApiCacheLoaderI() {}
        virtual ~UserApiCacheLoaderI() {}
        virtual void load(const MyUtil::IntSeq& ids, const Ice::Current&);
};

class BatchUserBornResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserBornResultHandler(MyUtil::ObjectResultPtr& result);
        virtual bool handle(mysqlpp::Row& row) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserStageResultHandler: public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStageResultHandler(MyUtil::ObjectResultPtr& result);
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
