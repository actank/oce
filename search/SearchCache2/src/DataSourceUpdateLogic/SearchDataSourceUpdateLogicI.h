#ifndef __SEARCHDATASOURCEUPDATELOGICI_H__
#define __SEARCHDATASOURCEUPDATELOGICI_H__

#include <SearchCache.h>
#include <ServiceI.h>
#include <IceUtil/IceUtil.h>
#include <QueryRunner.h>

namespace xce {
namespace searchcache {

const char* KEY_SUFFIX = "sc_date_";

const int IO_BUFFER_SIZE = 8192;

const Ice::Int SEARCH_CACHE = 0;

const std::string SEARCH_DB_CACHE_MANAGER = "M";

const int TASK_LEVEL_FILL = 1;
const int TASK_LEVEL_UPDATE_DBCACHE = 9;

//***************************************************************************

class SearchDataSourceUpdateLogicI: virtual public SearchCacheManager,
		public MyUtil::Singleton<SearchDataSourceUpdateLogicI> {
public:
	SearchDataSourceUpdateLogicI();
	virtual SearchCacheResult getSearchCache(::Ice::Int, const Ice::Current&);

	virtual Int2SearchCacheResultMap getSearchCacheList(const ::MyUtil::IntSeq&,
			const Ice::Current&);

	virtual void load(const ::MyUtil::IntSeq&, const Ice::Current&);

	virtual bool isValid(const Ice::Current& = Ice::Current());
	virtual void setValid(bool valid, const Ice::Current& = Ice::Current());

	void bootstrap(string dsn);
    string getDSN() { return _dsn;}

    void doLoad(const ::MyUtil::IntSeq& ids);
private:
    //为了记录日志方便
    std::string IntSeq2String(const ::MyUtil::IntSeq& ids);

	SearchCacheDataPtr getSearchCacheData(int userId);
	MyUtil::ObjectResultPtr getSearchCacheData(const MyUtil::IntSeq& userIds);

	bool _valid;
	IceUtil::RWRecMutex _validMutex;
    string _dsn;
};

class LoadUpdateDataToDbCacheTask: virtual public MyUtil::Task {
private:
    MyUtil::IntSeq uids_;
public:
	LoadUpdateDataToDbCacheTask(MyUtil::IntSeq uids);
	virtual void handle();
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
