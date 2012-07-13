#ifndef __SEARCHCACHEI_H__
#define __SEARCHCACHEI_H__

#include <SearchCache.h>
#include "SearchCacheData.h"
#include <ServiceI.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace searchcache {

const Ice::Int SEARCH_CACHE = 0;

const std::string SEARCH_CACHE_MANAGER = "M";

const string DB_SEARCH2_USERURL = "user_url";

const string DB_SEARCH2_USERBASIC = "search2_userbasic";
const string TABLE_USER_BASIC = "user_basic";
const string TABLE_USER_NETWORK_HISTORY = "network_history";

const string DB_SEARCH2_USERCONFIG = "search2_userconfig";
const string TABLE_USER_CONFIG = "user_config";

const string DB_SEARCH2_USER = "user_names";
const string TABLE_USERS = "user_names";

const int TASK_LEVEL_FILL = 1;
const int TASK_LEVEL_FILL_BASIC = 2;
const int TASK_LEVEL_FILL_CONFIG = 3;
const int TASK_LEVEL_FILL_USERS = 8;
const int TASK_LEVEL_CREATE_BASIC = 5;
const int TASK_LEVEL_CREATE_CONFIG = 6;
const int TASK_LEVEL_CREATE_USERS = 7;

const int TASK_LEVEL_FILL_DBCACHE = 8;

const int FILL_BATCH_SIZE = 3000;

//***************************************************************************

class SearchCacheManagerI: virtual public SearchCacheManager,
		public MyUtil::Singleton<SearchCacheManagerI> {
public:
	SearchCacheManagerI();
	virtual SearchCacheResult getSearchCache(::Ice::Int, const Ice::Current&);

	virtual Int2SearchCacheResultMap getSearchCacheList(const ::MyUtil::IntSeq&,
			const Ice::Current&);

	virtual void load(const ::MyUtil::IntSeq&, const Ice::Current&);

	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const Ice::Current& = Ice::Current());

	void bootstrap(int cluster, int mod, int cached);
	bool inline belongsMe(int id){
		if (_cached == 0 || _cluster==0) return true;
		return (_mod == (id % _cluster));
	}

private:
	SearchCacheDataPtr getSearchCacheData(int userId);
	MyUtil::ObjectResultPtr getSearchCacheData(const MyUtil::IntSeq& userIds);

	bool _valid;
	IceUtil::RWRecMutex _validMutex;

	int _cluster;
	int _mod;
	int _cached;
};

class FillDataToDbCacheTask: virtual public MyUtil::Task {
private:
    int _nStart;
    int _nEnd;
public:
	FillDataToDbCacheTask(int start, int end);
	virtual void handle();
};

class FillTask: virtual public MyUtil::Task {
public:
	FillTask();
	virtual void handle();
};

class FillFromDbCacheTask: virtual public MyUtil::Task {
public:
	FillFromDbCacheTask();
	virtual void handle();
};

//***************************************************************************

class SearchCacheFromDBFactoryI: public MyUtil::ServantFactory {
public:
	SearchCacheFromDBFactoryI(int cached);
	virtual Ice::ObjectPtr create(Ice::Long id);
	virtual MyUtil::ObjectResultPtr create(const MyUtil::LongSeq& ids);
private:
	int _cached;
};

//***************************************************************************

class SearchCacheFactoryI: public MyUtil::ServantFactory {
public:
	SearchCacheFactoryI();
	virtual Ice::ObjectPtr create(Ice::Long id);
	virtual MyUtil::ObjectResultPtr create(const MyUtil::LongSeq& ids);
};

//***************************************************************************

class SingleConfigResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	SingleConfigResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

class SingleUsersResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	SingleUsersResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

class SingleUserUrlResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	SingleUserUrlResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

class SingleNetworkResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	SingleNetworkResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

class SingleElementarySchoolResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	SingleElementarySchoolResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

class SingleJuniorSchoolResultHandlerI: public com::xiaonei::xce::ResultHandler{
public:
	SingleJuniorSchoolResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

class SingleHighSchoolResultHandlerI: public com::xiaonei::xce::ResultHandler{
public:
	SingleHighSchoolResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

class SingleCollegeResultHandlerI: public com::xiaonei::xce::ResultHandler{
public:
	SingleCollegeResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

class SingleUniversityResultHandlerI: public com::xiaonei::xce::ResultHandler{
public:
	SingleUniversityResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

class SingleWorkplaceResultHandlerI: public com::xiaonei::xce::ResultHandler{
public:
	SingleWorkplaceResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

class SingleRegionResultHandlerI: public com::xiaonei::xce::ResultHandler{
public:
	SingleRegionResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

class SingleBasicResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	SingleBasicResultHandlerI(SearchCacheDataPtr& result);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	SearchCacheDataPtr _result;
};

//***************************************************************************

class BatchConfigResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchConfigResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

class BatchUsersResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchUsersResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

class BatchUserUrlResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchUserUrlResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

class BatchNetworkResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchNetworkResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

class BatchElementarySchoolResultHandlerI:public com::xiaonei::xce::ResultHandler{
public:
	BatchElementarySchoolResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

class BatchJuniorSchoolResultHandlerI:public com::xiaonei::xce::ResultHandler{
public:
	BatchJuniorSchoolResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

class BatchHighSchoolResultHandlerI:public com::xiaonei::xce::ResultHandler{
public:
	BatchHighSchoolResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

class BatchCollegeResultHandlerI:public com::xiaonei::xce::ResultHandler{
public:
	BatchCollegeResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

class BatchUniversityResultHandlerI:public com::xiaonei::xce::ResultHandler{
public:
	BatchUniversityResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

class BatchWorkplaceResultHandlerI:public com::xiaonei::xce::ResultHandler{
public:
	BatchWorkplaceResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

class BatchRegionResultHandlerI:public com::xiaonei::xce::ResultHandler{
public:
	BatchRegionResultHandlerI();
	virtual bool handle(mysqlpp::Row& row) const;
};

class BatchBasicResultHandlerI: public com::xiaonei::xce::ResultHandler {
public:
	BatchBasicResultHandlerI(MyUtil::ObjectResultPtr& result, Ice::Int& maxId);
	virtual bool handle(mysqlpp::Row& row) const;
private:
	MyUtil::ObjectResultPtr _result;
	Ice::Int& _maxId;
    //boost::shared_ptr<IDbCache> _db_cache;
    //bool _initDbSusscess;
};

//***************************************************************************

class BatchConfigTask: public MyUtil::Task{
public:
	BatchConfigTask(const com::xiaonei::xce::Statement& sql,int level=TASK_LEVEL_FILL_CONFIG);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
};

class BatchUserUrlTask: public MyUtil::Task{
public:
	BatchUserUrlTask(const com::xiaonei::xce::Statement& sql,int level=TASK_LEVEL_FILL_USERS);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
};

class BatchUsersTask: public MyUtil::Task{
public:
	BatchUsersTask(const com::xiaonei::xce::Statement& sql,int level=TASK_LEVEL_FILL_USERS);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
};

class BatchNetworkTask: public MyUtil::Task{
public:
	BatchNetworkTask(const com::xiaonei::xce::Statement& sql,int level=TASK_LEVEL_FILL_BASIC);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
};

class BatchElementaryTask: public MyUtil::Task{
public:
	BatchElementaryTask(const com::xiaonei::xce::Statement& sql,int level=TASK_LEVEL_FILL_BASIC);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
};

class BatchJuniorTask: public MyUtil::Task{
public:
	BatchJuniorTask(const com::xiaonei::xce::Statement& sql,int level=TASK_LEVEL_FILL_BASIC);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
};

class BatchHighSchoolTask: public MyUtil::Task{
public:
	BatchHighSchoolTask(const com::xiaonei::xce::Statement& sql,int level=TASK_LEVEL_FILL_BASIC);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
};

class BatchCollegeTask: public MyUtil::Task{
public:
	BatchCollegeTask(const com::xiaonei::xce::Statement& sql,int level=TASK_LEVEL_FILL_BASIC);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
};

class BatchUniversityTask: public MyUtil::Task{
public:
	BatchUniversityTask(const com::xiaonei::xce::Statement& sql, int level=TASK_LEVEL_FILL_BASIC);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
};

class BatchWorkplaceTask: public MyUtil::Task{
public:
	BatchWorkplaceTask(const com::xiaonei::xce::Statement& sql,int level=TASK_LEVEL_FILL_BASIC);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
};

class BatchRegionTask: public MyUtil::Task{
public:
	BatchRegionTask(const com::xiaonei::xce::Statement& sql,int level=TASK_LEVEL_FILL_BASIC);
	virtual void handle();
private:
	com::xiaonei::xce::Statement _sql;
};

//***************************************************************************

class ServiceSchemaI: public MyUtil::Descriptor {
public:
	virtual void configure(const Ice::PropertiesPtr& props);
};

//***************************************************************************

}
}
#endif
