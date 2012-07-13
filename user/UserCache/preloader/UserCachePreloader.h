#ifndef __USERCACHEPRELOADER_H__
#define __USERCACHEPRELOADER_H__

#include <ServiceI.h>
#include <QueryRunner.h>
#include "GeneralPreloader/src/GeneralPreloaderFactory.h"
#include <IceUtil/Thread.h>

namespace xce{
namespace usercachepreloader{


const int PRODUCE_BATCH_SIZE = 1000;
const int CONSUME_BATCH_SIZE = 2000;
const int CONSUME_BATCH_THREAD_SIZE = 3;
const int WRITE_BATCH_SIZE = 3000;
const int PRX_DATAQUEUE_MAX_SIZE = 100000;

class UserCachePreloader : virtual public Ice::Object, virtual public MyUtil::Singleton<UserCachePreloader> {
public:
        UserCachePreloader () {}
        virtual ~UserCachePreloader () {}
};

class UserCachePreloaderFactory : virtual public xce::generalpreloader::MinMaxFactory {
public:
        UserCachePreloaderFactory() {}
        virtual ~UserCachePreloaderFactory() {}
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
	virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
};


class BatchUserPassportResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserPassportResultHandler(const  MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserPassportResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserBornResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserBornResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserBornResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserStageResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStageResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserStageResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserUrlResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserUrlResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserUrlResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserNamesResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserNamesResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserNamesResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class BatchUserStateResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        BatchUserStateResultHandler(const MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~BatchUserStateResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

}
}

#endif
