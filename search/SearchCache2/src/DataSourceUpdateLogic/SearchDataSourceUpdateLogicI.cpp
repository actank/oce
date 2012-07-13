#include <execinfo.h>
//#include <SearchPersistentCacheAdapter.h>
#include "QueryRunner.h"
#include "../Util/DatabaseCacheFactory.h"
#include "SearchDataSourceUpdateLogicI.h"

using namespace MyUtil;
using namespace IceUtil;
using namespace com::xiaonei::xce;
using namespace xce::searchcache;

//***************************************************************************

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	string dsn  = service.getCommunicator()->getProperties()->getPropertyWithDefault("SearchCache.Dsn", TOKYOTYRANT_DSN_PREFIX"127.0.0.1:1981");
    MCE_DEBUG("data source name: " << dsn);


	service.getAdapter()->add(&SearchDataSourceUpdateLogicI::instance(),service.createIdentity(SEARCH_DB_CACHE_MANAGER, ""));

	SearchDataSourceUpdateLogicI::instance().bootstrap(dsn);

	service.registerXceObserver(new ServiceSchemaI);
}

//***************************************************************************

SearchDataSourceUpdateLogicI::SearchDataSourceUpdateLogicI() : _valid(false) {
}

void SearchDataSourceUpdateLogicI::bootstrap(string dsn) {
    _dsn = dsn;
}

void SearchDataSourceUpdateLogicI::doLoad(const ::MyUtil::IntSeq& ids) {
    char valuebuf[IO_BUFFER_SIZE];
    char *pBuf = valuebuf;
    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache(SearchDataSourceUpdateLogicI::instance().getDSN());

    //
    if (ids.size()*sizeof(int) > IO_BUFFER_SIZE) {
        pBuf = new char [ids.size()*sizeof(int)];
    } 

    char* value_buf = pBuf;
    size_t value_size = 0;

    //将所有的userid按顺序存储到buf中
    for (::MyUtil::IntSeq::const_iterator it = ids.begin();
         it != ids.end(); ++it) {
        int id = (*it);
        memcpy(pBuf, &id, sizeof(int));
        pBuf += sizeof(int);
        value_size += sizeof(int);
    }

    //获得当前时间
   std::ostringstream strDate;
   time_t now = time(NULL);
   struct tm result;
   if (!localtime_r(&now, &result)) {
       MCE_FATAL("不能获得正确的时间 " << IntSeq2String(ids));
       goto _exit;
   }

   char date[200];
   if (strftime(date, sizeof(date), "%Y%m%d%H%M", &result) == 0) {
       MCE_FATAL("格式化时间错误 " << IntSeq2String(ids));
       goto _exit;
   }

   strDate << KEY_SUFFIX << date;

   if (db_cache->Initialize() == false) {
       MCE_FATAL("Db initialize failed" << IntSeq2String(ids));
       goto _exit;
   }
   if (!db_cache->Concatenate(strDate.str().c_str(), strDate.str().length(), value_buf, value_size)) 
   {
       MCE_FATAL("insert failed" << IntSeq2String(ids));
   }
   db_cache->Close();

_exit:
   if (value_size> IO_BUFFER_SIZE) {
       delete pBuf;
   }
   return;
}

std::string SearchDataSourceUpdateLogicI::IntSeq2String(const ::MyUtil::IntSeq& uids) {
    std::ostringstream str;
    str << " ";
	for (MyUtil::IntSeq::const_iterator it = uids.begin(); it != uids.end(); ++it) {
        str << boost::lexical_cast<string>(*it) << " ";
	}
    return str.str();
}

void SearchDataSourceUpdateLogicI::load(const ::MyUtil::IntSeq& ids, const Ice::Current&) {
    ::MyUtil::IntSeq uids = ids;
    std::ostringstream str;
    str << " ";
	for (MyUtil::IntSeq::const_iterator it = uids.begin(); it != uids.end(); ++it) {
        str << boost::lexical_cast<string>(*it) << " ";
	}

    //SearchPersistentCacheAdapter::instance().load(uids);
    TaskManager::instance().execute(new LoadUpdateDataToDbCacheTask(uids));

	MCE_FATAL("SearchCacheUpdateLog: " << str.str());
}

//***************************************************************************
LoadUpdateDataToDbCacheTask::LoadUpdateDataToDbCacheTask(MyUtil::IntSeq uids) : 
	MyUtil::Task(TASK_LEVEL_UPDATE_DBCACHE), uids_(uids) {
}

void LoadUpdateDataToDbCacheTask::handle() {
    SearchDataSourceUpdateLogicI::instance().doLoad(uids_);
}

bool SearchDataSourceUpdateLogicI::isValid(const Ice::Current&) {
	RWRecMutex::RLock lock(_validMutex);
	return _valid;
}

void SearchDataSourceUpdateLogicI::setValid(bool valid, const Ice::Current&) {
	//RWRecMutex::WLock lock(_validMutex);
	//_valid = valid;
}

SearchCacheDataPtr SearchDataSourceUpdateLogicI::getSearchCacheData(int userId) {
	return ServiceI::instance().findObject<SearchCacheDataPtr> (SEARCH_CACHE,userId);
}

ObjectResultPtr SearchDataSourceUpdateLogicI::getSearchCacheData(const MyUtil::IntSeq& userIds) {
	MyUtil::LongSeq uids;
	for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it!= userIds.end(); ++it) {
		uids.push_back(*it);
	}
	return ServiceI::instance().getObjectCache()->findObjects(SEARCH_CACHE,uids);
}

SearchCacheResult SearchDataSourceUpdateLogicI::getSearchCache(::Ice::Int userId,const Ice::Current&) {
	return getSearchCacheData(userId)->getProperties();
}

Int2SearchCacheResultMap SearchDataSourceUpdateLogicI::getSearchCacheList(const ::MyUtil::IntSeq& userIds, const Ice::Current&) {
	Int2SearchCacheResultMap result;
	return result;
}

//***************************************************************************

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	TaskManager::instance().config(props, "Fill", TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_UPDATE_DBCACHE, ThreadPoolConfig(8, 256));
}
