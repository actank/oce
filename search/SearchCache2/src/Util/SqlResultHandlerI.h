#ifndef __SQLRESULTHANDLER_H__
#define __SQLRESULTHANDLER_H__

#include <ServiceI.h>
#include <IceUtil/IceUtil.h>
#include <SearchCache.h>
#include "../Util/SearchCacheData.h"
#include <ServiceI.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>
namespace xce {
namespace searchcache {

//***************************************************************************
class SingleResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	SingleResultHandlerI(SearchCacheDataPtr& result,void (SearchCacheData::*p)(mysqlpp::Row& row) );
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
    void (SearchCacheData::*_pFunc)(mysqlpp::Row& row);
};

//***************************************************************************
class BatchResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchResultHandlerI(std::string id, void (SearchCacheData::*p)(mysqlpp::Row& row), int cacheid=0);
	virtual bool handle(mysqlpp::Row& row) const;
    std::string _id;
    void (SearchCacheData::*_pFunc)(mysqlpp::Row& row);
private:
    const int _cacheid;
};

class BatchBasicResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchBasicResultHandlerI(MyUtil::ObjectResultPtr& result, Ice::Int& maxId, int cacheid=0);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::ObjectResultPtr _result;
	Ice::Int& _maxId;
    const int _cacheid;
    //boost::shared_ptr<IDbCache> _db_cache;
    //bool _initDbSusscess;
};

//***************************************************************************
class BatchTask: public MyUtil::Task{
public:
	BatchTask(std::string id, std::string db, void (SearchCacheData::*p)(mysqlpp::Row& row),
        const com::xiaonei::xce::Statement& sql,int level);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
    std::string _db;
    std::string _id;
    void (SearchCacheData::*_pFunc)(mysqlpp::Row& row);
};

};
};
#endif
