#ifndef __SCORE_CACHE_DATA_FACTORY_H__
#define __SCORE_CACHE_DATA_FACTORY_H__

#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
#include <ServiceI.h>
#include "UtilCxx/src/Singleton.h"
#include "ReplicaCluster/CacheLoader/src/CacheLoaderFactory.h"

namespace xce {
namespace scorecache {

/*****************************************************************************/

class ScoreCacheDataNFactory: virtual public xce::cacheloader::MinMaxFactory, virtual public MyUtil::Singleton<ScoreCacheDataNFactory>  {
protected:
        friend class MyUtil::Singleton<ScoreCacheDataNFactory>;
public:
        Ice::ObjectPtr create(Ice::Int id);
        virtual MyUtil::ObjectResultPtr create(Ice::Int minId,Ice::Int maxId);
        virtual MyUtil::ObjectResultPtr create(const MyUtil::IntSeq& ids);
public:
        MyUtil::ObjectResultPtr createByDays(const vector<int>& ids,int days,long zerotime);
};

/*****************************************************************************/

class ScoreLoginResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        ScoreLoginResultHandler(const  MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~ScoreLoginResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class ScoreLoginByDaysResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        ScoreLoginByDaysResultHandler(const  MyUtil::ObjectResultPtr& result,MyUtil::IntSeq& ids, Ice::Int days,long zerotime):_result(result),_ids(ids),_days(days),_zerotime(zerotime){}
        virtual ~ScoreLoginByDaysResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
        MyUtil::IntSeq& _ids;
	Ice::Int _days;
        long _zerotime;
};

class ScoreResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        ScoreResultHandler(const  MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~ScoreResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class ScoreAwardCountResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        ScoreAwardCountResultHandler(const  MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~ScoreAwardCountResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class ScoreSignResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        ScoreSignResultHandler(const  MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~ScoreSignResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

class ScoreVipResultHandler : virtual public com::xiaonei::xce::ResultHandler {
public:
        ScoreVipResultHandler (const  MyUtil::ObjectResultPtr& result):_result(result){}
        virtual ~ScoreVipResultHandler(){}
protected:
        virtual bool handle(mysqlpp::Row& res) const;
private:
        MyUtil::ObjectResultPtr _result;
};

/*****************************************************************************/

}
}

#endif
