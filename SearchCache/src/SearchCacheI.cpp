#include <execinfo.h>
#include "QueryRunner.h"
#include "DatabaseCacheFactory.h"
#include "SearchCacheI.h"

using namespace MyUtil;
using namespace IceUtil;
using namespace com::xiaonei::xce;
using namespace xce::searchcache;

//***************************************************************************

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();

	int cluster = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCache.Cluster");
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("SearchCache.Mod");
    //获得cached的存储位置
    //cached=0:内存cache,从mysql数据库加载数据到DBCache
    //cached=1:从DbCached加载数据，对外提供服务
	int cached = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("SearchCache.FromCache", 0);

    if (cached == 0) {
        service.registerObjectCache(SEARCH_CACHE, "SC", new SearchCacheFactoryI());
    } else {
        service.registerObjectCache(SEARCH_CACHE, "SC", new SearchCacheFromDBFactoryI(cached));
    }

	service.getAdapter()->add(&SearchCacheManagerI::instance(),service.createIdentity(SEARCH_CACHE_MANAGER, ""));

	TaskManager::instance().config(TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
	TaskManager::instance().config(TASK_LEVEL_FILL_BASIC, ThreadPoolConfig(1, 64));
	TaskManager::instance().config(TASK_LEVEL_FILL_CONFIG, ThreadPoolConfig(1, 8));
	TaskManager::instance().config(TASK_LEVEL_FILL_USERS, ThreadPoolConfig(1, 8));
	TaskManager::instance().config(TASK_LEVEL_CREATE_BASIC, ThreadPoolConfig(1, 16));
	TaskManager::instance().config(TASK_LEVEL_CREATE_CONFIG, ThreadPoolConfig(1, 2));
	TaskManager::instance().config(TASK_LEVEL_CREATE_USERS, ThreadPoolConfig(1, 2));
	TaskManager::instance().config(TASK_LEVEL_FILL_DBCACHE, ThreadPoolConfig(1, 1));

	SearchCacheManagerI::instance().bootstrap(cluster, mod, cached);

	service.registerXceObserver(new ServiceSchemaI);
}

//***************************************************************************

SearchCacheManagerI::SearchCacheManagerI() : _valid(false) {
}

void SearchCacheManagerI::bootstrap(int cluster, int mod, int cached) {
	_cluster = cluster;
	_mod = mod;
	_cached = cached;

    if (_cached == 0)
        TaskManager::instance().execute(new FillTask());
    else 
        TaskManager::instance().execute(new FillFromDbCacheTask());

	MCE_INFO("bootstrap cluster="<<_cluster << " mod="<<_mod);
}

SearchCacheResult SearchCacheManagerI::getSearchCache(::Ice::Int userId,const Ice::Current&) {
	MCE_DEBUG("Get SearchCache");
	return getSearchCacheData(userId)->getProperties();
}

Int2SearchCacheResultMap SearchCacheManagerI::getSearchCacheList(const ::MyUtil::IntSeq& userIds, const Ice::Current&) {
	MCE_DEBUG("Get SearchCache List");
	ObjectResultPtr objres = getSearchCacheData(userIds);
	Int2SearchCacheResultMap result;
	for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it!= userIds.end(); ++it) {
		ObjectMap::iterator hasRes = objres->data.find(*it);
		if (hasRes != objres->data.end()) {
			result[*it] = SearchCacheDataPtr::dynamicCast(hasRes->second)->getProperties();
		} else {
			MCE_WARN("Not Exists: " << *it);
		}
	}
	return result;
}

void SearchCacheManagerI::load(const ::MyUtil::IntSeq& ids, const Ice::Current&) {
	MyUtil::LongSeq uids;
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (belongsMe(*it)) {
			uids.push_back(*it);
			MCE_DEBUG("SearchCacheManagerI::load IS " << *it);
		} else {
			MCE_DEBUG("SearchCacheManagerI::load NOT " << *it);
		}
	}

    if (!uids.empty()) {
        std::sort(uids.begin(),uids.end()); 
        std::unique(uids.begin(),uids.end()); 
    }

    ServiceI::instance().getObjectCache()->removeObjects(SEARCH_CACHE, uids);

    //从mysql数据库加载的factory，需要实时加载。
    //从DBCache加载数据的facory，等到用户请求时，从DBCache加载。
    if (_cached == 0) 
        ServiceI::instance().getObjectCache()->locateObjects(SEARCH_CACHE, uids);

	MCE_INFO("SearchCacheManagerI::load " << ids.size() << " objects. " << uids.size() << " belongs to me");
}

bool SearchCacheManagerI::isValid(const Ice::Current&) {
	RWRecMutex::RLock lock(_validMutex);
	return _valid;
}

void SearchCacheManagerI::setValid(bool valid, const Ice::Current&) {
	RWRecMutex::WLock lock(_validMutex);
	_valid = valid;
}

SearchCacheDataPtr SearchCacheManagerI::getSearchCacheData(int userId) {
	return ServiceI::instance().locateObject<SearchCacheDataPtr> (SEARCH_CACHE,userId);
}

ObjectResultPtr SearchCacheManagerI::getSearchCacheData(const MyUtil::IntSeq& userIds) {
	MyUtil::LongSeq uids;
	for (MyUtil::IntSeq::const_iterator it = userIds.begin(); it!= userIds.end(); ++it) {
		uids.push_back(*it);
	}
	return ServiceI::instance().getObjectCache()->locateObjects(SEARCH_CACHE,uids);
}

//***************************************************************************

FillDataToDbCacheTask::FillDataToDbCacheTask(int start, int end ) : 
	MyUtil::Task(TASK_LEVEL_FILL_DBCACHE), _nStart(start), _nEnd(end) {
}

void FillDataToDbCacheTask::handle() {
	try{
        /*
         */
        boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache();
        if (db_cache->Initialize() == false) {
            MCE_WARN("Db initialize failed");
            return;
        }

        for(int userId = _nStart; userId<=_nEnd; ++userId) {
            if (ServiceI::instance().getObjectCache()->hasObject(SEARCH_CACHE,userId) && 
                    db_cache->Insert(userId, 
                        *ServiceI::instance().locateObject<SearchCacheDataPtr>(SEARCH_CACHE,userId).get())) {
                ServiceI::instance().getObjectCache()->removeObject(SEARCH_CACHE,userId);
            } else {
                //MCE_WARN("cached Id = " << userId << " is failed");
            }
        }
        db_cache->Close();
	}catch(...){
		MCE_WARN("FillDataToDbCacheTask done with exception");
	}
}

FillFromDbCacheTask::FillFromDbCacheTask() :
	MyUtil::Task(TASK_LEVEL_FILL) {
}

void FillFromDbCacheTask::handle() {
	try{
        boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache();
        
        if (db_cache->Initialize() == false) {
            MCE_WARN("Db initialize failed");
            return;
        }

        IDbResult* pDbResult = db_cache->Query();
        while (SearchCacheDataPtr obj = pDbResult->FetchObject()) {
            if(!SearchCacheManagerI::instance().belongsMe(obj->_id) || obj->_id == 0) continue;
            
            MCE_WARN("loading userid=" << obj->_id);
            ServiceI::instance().getObjectCache()->addObject(SEARCH_CACHE, obj->_id, obj);
        }
        db_cache->Close();
	}catch(...){
		MCE_WARN("FillTask done with exception");
	}
	SearchCacheManagerI::instance().setValid(true);
	MCE_INFO("FillTask done.");
}

FillTask::FillTask() :
	MyUtil::Task(TASK_LEVEL_FILL) {
}

void FillTask::handle() {
	int beginId = 0;
	int maxId = 0;
    int count = 0;
    int startId = 0;

	try{

	do {
		beginId = maxId;
		ObjectResultPtr results = new ObjectResult;
		{
			Statement sql;
			sql << "SELECT id, status, star, selected, name, tinyurl, headurl,home_city,home_province FROM user_basic WHERE id > "
					<< beginId << " LIMIT " << FILL_BATCH_SIZE;
			BatchBasicResultHandlerI handler(results, maxId);
			QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
			ServiceI::instance().getObjectCache()->addObjects(SEARCH_CACHE, results->data);
		}

		{
			Statement sql1;
	                sql1 << "SELECT id, browse_config, status_config FROM user_config WHERE id > "
        	             << beginId << " AND id <= " << maxId << " LIMIT "
                	     << FILL_BATCH_SIZE;
			TaskManager::instance().execute(new BatchConfigTask(sql1));
		}

		{
			Statement sql1;
	                sql1 << "SELECT id, nickname FROM user_names WHERE id > "
        	             << beginId << " AND id <= " << maxId << " LIMIT "
                	     << FILL_BATCH_SIZE;
			TaskManager::instance().execute(new BatchUsersTask(sql1));
		}

		{
			Statement sql1;
	                sql1 << "SELECT id, headurl, tinyurl FROM user_url WHERE id > "
        	             << beginId << " AND id <= " << maxId << " LIMIT "
                	     << FILL_BATCH_SIZE;
			TaskManager::instance().execute(new BatchUserUrlTask(sql1));
		}

		{
			Statement sql2;
	                sql2 << "SELECT userid, network_id, network_name, stage FROM network_history WHERE status<=0 AND userid  > "
        	             << beginId << " AND userid <= " << maxId << " LIMIT "
                	     << FILL_BATCH_SIZE;
			TaskManager::instance().execute(new BatchNetworkTask(sql2));
		}

		{
			Statement sql3;
	                sql3 << "SELECT userid, elementary_school_id, elementary_school_name, elementary_school_year FROM elementary_school_info WHERE userid  > "
        	             << beginId << " AND userid <= " << maxId << " LIMIT "
                	     << FILL_BATCH_SIZE;
			TaskManager::instance().execute(new BatchElementaryTask(sql3));
		}

		{
			Statement sql4;
	                sql4 << "SELECT userid, junior_high_school_id, junior_high_school_name, junior_high_school_year FROM junior_high_school_info WHERE userid  > "
        	             << beginId << " AND userid <= " << maxId << " LIMIT "
                	     << FILL_BATCH_SIZE;
			TaskManager::instance().execute(new BatchJuniorTask(sql4));
		}

		{
			Statement sql5;
	                sql5 << "SELECT userid, high_school_id, high_school_name, enroll_year FROM high_school_info WHERE userid  > "
        	             << beginId << " AND userid <= " << maxId << " LIMIT "
                	     << FILL_BATCH_SIZE;
			TaskManager::instance().execute(new BatchHighSchoolTask(sql5));
		}

		{
			Statement sql6;
	                sql6 << "SELECT userid, college_id, college_name, department,enroll_year FROM college_info WHERE userid  > "
        	             << beginId << " AND userid <= " << maxId << " LIMIT "
                	     << FILL_BATCH_SIZE;
			TaskManager::instance().execute(new BatchCollegeTask(sql6));
		}

		{
			Statement sql7;
	                sql7 << "SELECT userid, university_id, university_name, department, enroll_year FROM university_info WHERE userid  > "
        	             << beginId << " AND userid <= " << maxId << " LIMIT "
                	     << FILL_BATCH_SIZE;
			TaskManager::instance().execute(new BatchUniversityTask(sql7));
		}

		{
			Statement sql8;
	                sql8 << "SELECT userid, workplace_id, workplace_name FROM workplace_info WHERE userid  > "
        	             << beginId << " AND userid <= " << maxId << " LIMIT "
                	     << FILL_BATCH_SIZE;
			TaskManager::instance().execute(new BatchWorkplaceTask(sql8));
		}

		{
			Statement sql9;
	                sql9 << "SELECT userid, region_id, province_name, city_name FROM region_info WHERE userid  > "
        	             << beginId << " AND userid <= " << maxId << " LIMIT "
                	     << FILL_BATCH_SIZE;
			TaskManager::instance().execute(new BatchRegionTask(sql9));
		}

        count += (maxId - beginId);

        //每5万存储一次
        if (count > 50000)
        {
            count = 0;
            while(TaskManager::instance().size(TASK_LEVEL_FILL_BASIC) > 0 || 
                    TaskManager::instance().size(TASK_LEVEL_FILL_CONFIG) > 0) {
                sleep(1);
            }
			//TaskManager::instance().execute(new FillDataToDbCacheTask(startId, maxId));
            boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache();
            if (db_cache->Initialize() == false) {
                MCE_WARN("Db initialize failed");
                return;
            }

            for(int userId = startId; userId<=maxId; ++userId) {
                if (ServiceI::instance().getObjectCache()->hasObject(SEARCH_CACHE,userId) && 
                        db_cache->Insert(userId, 
                            *ServiceI::instance().locateObject<SearchCacheDataPtr>(SEARCH_CACHE,userId).get())) {
                    ServiceI::instance().getObjectCache()->removeObject(SEARCH_CACHE,userId);
                } else {
                    //MCE_WARN("cached Id = " << userId << " is failed");
                }
            }
            db_cache->Close();
            startId = maxId;
        }
	} while (beginId < maxId);


    while(TaskManager::instance().size(TASK_LEVEL_FILL_BASIC) > 0 || 
            TaskManager::instance().size(TASK_LEVEL_FILL_CONFIG) > 0) {
        sleep(5);
    }

    TaskManager::instance().execute(new FillDataToDbCacheTask(startId, maxId));

	}catch(...){
		MCE_WARN("FillTask done with exception");
	}
	//SearchCacheManagerI::instance().setValid(true);
	MCE_INFO("FillTask done.");
}
//***************************************************************************

SearchCacheFromDBFactoryI::SearchCacheFromDBFactoryI(int cached) : _cached(cached) {
}

Ice::ObjectPtr SearchCacheFromDBFactoryI::create(Ice::Long id) {
	MCE_DEBUG("Loading " << id);
	SearchCacheDataPtr result = new SearchCacheData;

	if(!SearchCacheManagerI::instance().belongsMe(id)) return result;

	result->_id = id;

	//userid=0是没用的数据,直接过滤掉
	if (id == 0) {
		return result;
	}


    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache();
    if (db_cache->Initialize() == false)
    {
        MCE_WARN("Db initialize failed");
        return result;
    }
     if (!db_cache->Query(id, result.get())) {
        MCE_WARN("cached Id = " << id << " is failed");
    }
    db_cache->Close();

	return result;
}

MyUtil::ObjectResultPtr SearchCacheFromDBFactoryI::create(const MyUtil::LongSeq& ids) {
	MCE_DEBUG("Loading " << ids.size() << " objects");
	ObjectResultPtr results = new ObjectResult;
	if (ids.empty()) {
		return results;
	}

    /*
     */
    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache();

    if (db_cache->Initialize() == false)
    {
        MCE_WARN("Db initialize failed");
        return results;
    }


    for (MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {

        if(!SearchCacheManagerI::instance().belongsMe(*it)) continue;

        MyUtil::ObjectMap::iterator itResult = results->data.find(*it);
        if (itResult == results->data.end()) {
            SearchCacheDataPtr obj = new SearchCacheData;
            if (db_cache->Query(*it, obj.get())) {
                results->data.insert(make_pair(*it, obj));
            } else {
                MCE_WARN("cached Id = " << *it << " is failed");
            }
        } else {
            MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
        }
    }
    db_cache->Close();

	return results;
}

//***************************************************************************

SearchCacheFactoryI::SearchCacheFactoryI() {
}

Ice::ObjectPtr SearchCacheFactoryI::create(Ice::Long id) {
	MCE_DEBUG("Loading " << id);

	//userid=0是没用的数据,直接过滤掉
	if (id == 0) {
		return NULL;
	}

	SearchCacheDataPtr result = new SearchCacheData;

	result->_id = id;

    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache();
    if (db_cache->Initialize() == false) {
        MCE_WARN("Db initialize failed");
    } else {
        db_cache->Query(id, result.get());
    }

	{
		Statement sql;
		sql << "SELECT id, status, name, star, selected, tinyurl,headurl,home_city,home_province FROM user_basic WHERE id='" << id
				<< "' LIMIT 1";
		SingleBasicResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
	}
	{
		Statement sql;
		sql << "SELECT id, browse_config, status_config FROM user_config WHERE id='"
				<< id << "' LIMIT 1";
		SingleConfigResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USERCONFIG, CDbRServer).query(sql, handler);
	}
	{
		Statement sql;
		sql << "SELECT id, nickname FROM user_names WHERE id='"
				<< id << "' LIMIT 1";
		SingleUsersResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USER, CDbRServer).query(sql, handler);
	}
	{
		Statement sql;
		sql << "SELECT id, headurl, tinyurl FROM user_url WHERE id='"
				<< id << "' LIMIT 1";
		SingleUserUrlResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USERURL, CDbRServer).query(sql, handler);
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, network_id, network_name, stage FROM network_history WHERE status <=0 AND userid='"
				<< id << "' LIMIT 1";
		SingleNetworkResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql2, handler);
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, elementary_school_id, elementary_school_name, elementary_school_year FROM elementary_school_info WHERE userid='"<<id<<"' LIMIT 1";
		SingleElementarySchoolResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql2, handler);
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, junior_high_school_id, junior_high_school_name, junior_high_school_year FROM junior_high_school_info WHERE userid='"<<id<<"' LIMIT 1";
		SingleJuniorSchoolResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql2, handler);
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, high_school_id, high_school_name, enroll_year FROM high_school_info WHERE userid='"<<id<<"' LIMIT 1";
		SingleHighSchoolResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql2, handler);
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, college_id, college_name, department, enroll_year FROM college_info WHERE userid='"<<id<<"' LIMIT 1";
		SingleCollegeResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql2, handler);
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, university_id, university_name, department, enroll_year FROM university_info WHERE userid='"<<id<<"' LIMIT 1";
		SingleUniversityResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql2, handler);
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, workplace_id, workplace_name FROM workplace_info WHERE userid='"
				<<id<<"' LIMIT 1";
		SingleWorkplaceResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql2, handler);
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, region_id, province_name, city_name FROM region_info WHERE userid='"
				<< id << "' LIMIT 1";
		SingleRegionResultHandlerI handler(result);
		QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql2, handler);
	}
	MCE_DEBUG("Loading " << id << " done.");

    if (!db_cache->Insert(id, *result.get())) {
        MCE_WARN("cached Id = " << id << " is failed");
    }
    db_cache->Close();

	return NULL;
}

MyUtil::ObjectResultPtr SearchCacheFactoryI::create(const MyUtil::LongSeq& ids) {

	MCE_DEBUG("Loading " << ids.size() << " objects");
	ObjectResultPtr results = new ObjectResult;
	if (ids.empty()) {
		return results;
	}

	ostringstream oss;
	string strSuffix;
	MyUtil::LongSeq::const_iterator it = ids.begin();

	//如果userid=0，直接过滤掉
    if (ids.size() == 1 && *it == 0) {
		return results;
    }

	oss << "(" ;
	//去掉userid=0的情况
    if (*it != 0) {
        oss << *it;
    }
    else {
        oss <<*(++it);
    }
	++it;
	for (; it != ids.end(); ++it) {
        if (*it != 0) {
            oss << ", " << *it;
        }
	}
	oss << ")";

	strSuffix = oss.str();
	{
		Statement sql;
		sql << "SELECT id, status, star, selected, name, tinyurl, headurl,home_city,home_province FROM user_basic WHERE id IN "
				<< strSuffix;

		int maxId;
		BatchBasicResultHandlerI handler(results, maxId);
		QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(sql, handler);
		ServiceI::instance().getObjectCache()->addObjects(SEARCH_CACHE, results->data);
	}
	{
		Statement sql;
		sql << "SELECT id, browse_config, status_config FROM user_config WHERE id IN "
				<< strSuffix;
		TaskManager::instance().execute(new BatchConfigTask(sql,TASK_LEVEL_CREATE_CONFIG));
	}
	{
		Statement sql;
		sql << "SELECT id, nickname FROM user_names WHERE id IN "
				<< strSuffix;
		TaskManager::instance().execute(new BatchUsersTask(sql,TASK_LEVEL_CREATE_USERS));
	}
	{
	{
		Statement sql;
		sql << "SELECT id, headurl, tinyurl FROM user_url WHERE id IN "
				<< strSuffix;
		TaskManager::instance().execute(new BatchUserUrlTask(sql,TASK_LEVEL_CREATE_USERS));
	}
		Statement sql2;
		sql2
				<< "SELECT userid, network_id, network_name, stage FROM network_history WHERE status <=0 AND userid IN "
				<< strSuffix;
		TaskManager::instance().execute(new BatchNetworkTask(sql2,TASK_LEVEL_CREATE_BASIC));
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, elementary_school_id, elementary_school_name, elementary_school_year FROM elementary_school_info WHERE userid IN "<< strSuffix;
		TaskManager::instance().execute(new BatchElementaryTask(sql2,TASK_LEVEL_CREATE_BASIC));
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, junior_high_school_id, junior_high_school_name, junior_high_school_year FROM junior_high_school_info WHERE userid IN "<< strSuffix;
		TaskManager::instance().execute(new BatchJuniorTask(sql2, TASK_LEVEL_CREATE_BASIC));
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, high_school_id, high_school_name, enroll_year FROM high_school_info WHERE userid IN "<< strSuffix;
		TaskManager::instance().execute(new BatchHighSchoolTask(sql2, TASK_LEVEL_CREATE_BASIC));
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, college_id, college_name, department,enroll_year FROM college_info WHERE userid IN "<< strSuffix;
		TaskManager::instance().execute(new BatchCollegeTask(sql2, TASK_LEVEL_CREATE_BASIC));
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, university_id, university_name, department, enroll_year FROM university_info WHERE userid IN "<< strSuffix;
		TaskManager::instance().execute(new BatchUniversityTask(sql2, TASK_LEVEL_CREATE_BASIC));
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, workplace_id, workplace_name FROM workplace_info WHERE userid IN "
				<< strSuffix;
		TaskManager::instance().execute(new BatchWorkplaceTask(sql2, TASK_LEVEL_CREATE_BASIC));
	}
	{
		Statement sql2;
		sql2
				<< "SELECT userid, region_id, province_name, city_name FROM region_info WHERE userid IN "
				<< strSuffix;
		TaskManager::instance().execute(new BatchRegionTask(sql2, TASK_LEVEL_CREATE_BASIC));
	}

    while(TaskManager::instance().size(TASK_LEVEL_CREATE_BASIC) > 0 || 
            TaskManager::instance().size(TASK_LEVEL_CREATE_CONFIG) > 0 ||
            TaskManager::instance().size(TASK_LEVEL_CREATE_USERS) > 0) {
        sleep(1);
    }
    /*
     */
    boost::shared_ptr<IDbCache> db_cache = DatabaseCacheFactory::instance().getDbCache();

    if (db_cache->Initialize() == false)
    {
        MCE_WARN("Db initialize failed");
        return results;
    }

    for (MyUtil::LongSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
        if (ServiceI::instance().getObjectCache()->hasObject(SEARCH_CACHE,*it) && 
            db_cache->Insert(*it, *ServiceI::instance().locateObject<SearchCacheDataPtr>(SEARCH_CACHE,*it).get())) {
            ServiceI::instance().getObjectCache()->removeObject(SEARCH_CACHE,*it);
        } else {
            MCE_WARN("cached Id = " << *it << " is failed");
        }
    }

    db_cache->Close();

	return results;
}

//***************************************************************************

SingleConfigResultHandlerI::SingleConfigResultHandlerI(SearchCacheDataPtr& result) :
	_result(result) {
}

bool SingleConfigResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setConfigProperties(row, "browse_config", "status_config");
	return false;
}

SingleUserUrlResultHandlerI::SingleUserUrlResultHandlerI(SearchCacheDataPtr& result) :
	_result(result) {
}

bool SingleUserUrlResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setUserUrlProperties(row, "tinyurl", "headurl");
	return false;
}

SingleUsersResultHandlerI::SingleUsersResultHandlerI(SearchCacheDataPtr& result) :
	_result(result) {
}

bool SingleUsersResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setUsersProperties(row, "nickname");
	return false;
}

SingleNetworkResultHandlerI::SingleNetworkResultHandlerI(SearchCacheDataPtr& result) :
	_result(result) {
}

bool SingleNetworkResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setNetworkProperties(row, "network_id", "network_name", "stage");
	return true;
}

SingleElementarySchoolResultHandlerI::SingleElementarySchoolResultHandlerI(SearchCacheDataPtr& result) :
	_result(result){
}

bool SingleElementarySchoolResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setElementarySchoolProperties(row, "elementary_school_id", "elementary_school_year", "elementary_school_name");
	return true;
}

SingleJuniorSchoolResultHandlerI::SingleJuniorSchoolResultHandlerI(SearchCacheDataPtr& result) :
	_result(result){
}

bool SingleJuniorSchoolResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setJuniorSchoolProperties(row, "junior_high_school_id", "junior_high_school_year", "junior_high_school_name");
	return true;
}

SingleHighSchoolResultHandlerI::SingleHighSchoolResultHandlerI(SearchCacheDataPtr& result) :
	_result(result){
}

bool SingleHighSchoolResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setHighSchoolProperties(row, "high_school_id", "enroll_year", "high_school_name");
	return true;
}

SingleCollegeResultHandlerI::SingleCollegeResultHandlerI(SearchCacheDataPtr& result) :
	_result(result){
}

bool SingleCollegeResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setCollegeProperties(row, "college_id", "college_name", "department", "enroll_year");
	return true;
}

SingleUniversityResultHandlerI::SingleUniversityResultHandlerI(SearchCacheDataPtr& result) :
	_result(result){
}

bool SingleUniversityResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setUniversityProperties(row, "university_id", "enroll_year", "university_name", "department");
	return true;
}

SingleWorkplaceResultHandlerI::SingleWorkplaceResultHandlerI(SearchCacheDataPtr& result) :
	_result(result){
}

bool SingleWorkplaceResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setWorkplaceProperties(row, "workplace_id", "workplace_name");
	return true;
}

SingleRegionResultHandlerI::SingleRegionResultHandlerI(SearchCacheDataPtr& result) :
	_result(result){
}

bool SingleRegionResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setRegionProperties(row, "region_id", "city_name", "province_name");
	return true;
}

SingleBasicResultHandlerI::SingleBasicResultHandlerI(SearchCacheDataPtr& result) :
	_result(result) {
}
bool SingleBasicResultHandlerI::handle(mysqlpp::Row& row) const {
	_result->setBasicProperties(row, "name", "star", "selected", "status", "home_city", "home_province");
	return false;
}

//***************************************************************************

BatchConfigResultHandlerI::BatchConfigResultHandlerI() {
}

bool BatchConfigResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];

	if(!SearchCacheManagerI::instance().belongsMe(id)) return true;
	SearchCacheDataPtr data=ServiceI::instance().findObject<SearchCacheDataPtr> (SEARCH_CACHE,id);
	if (data == 0) {
		// MCE_WARN("IMPOSSIABLE, user_config has id=" << id << " but user_basic not. "
		// 		<< __FILE__ << " " << __LINE__);
	} else {
		data->setConfigProperties(row, "browse_config", "status_config");
	}
	return true;
}

BatchUserUrlResultHandlerI::BatchUserUrlResultHandlerI() {
}

bool BatchUserUrlResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];
	if(!SearchCacheManagerI::instance().belongsMe(id)) return true;
	SearchCacheDataPtr data=ServiceI::instance().findObject<SearchCacheDataPtr> (SEARCH_CACHE,id);
	if (data == 0) {
		// MCE_WARN("IMPOSSIABLE, users has id=" << id << " but user_basic not. "
		// 		<< __FILE__ << " " << __LINE__);
	} else {
		data->setUserUrlProperties(row, "tinyurl", "headurl");
	}
	return true;
}

BatchUsersResultHandlerI::BatchUsersResultHandlerI() {
}

bool BatchUsersResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];
	if(!SearchCacheManagerI::instance().belongsMe(id)) return true;
	SearchCacheDataPtr data=ServiceI::instance().findObject<SearchCacheDataPtr> (SEARCH_CACHE,id);
	if (data == 0) {
		// MCE_WARN("IMPOSSIABLE, users has id=" << id << " but user_basic not. "
		// 		<< __FILE__ << " " << __LINE__);
	} else {
		data->setUsersProperties(row, "nickname");
	}
	return true;
}

BatchNetworkResultHandlerI::BatchNetworkResultHandlerI() {
}

bool BatchNetworkResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int) row["userid"];
	if(!SearchCacheManagerI::instance().belongsMe(id)) return true;
	SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE,id);
	if (data == 0) {
		// MCE_WARN("IMPOSSIABLE, network_history has userid=" << id << " but user_basic not. "
		// 		<< __FILE__ << " " << __LINE__);
	} else {
		data->setNetworkProperties(row,	"network_id", "network_name", "stage");
	}
	return true;
}

BatchElementarySchoolResultHandlerI::BatchElementarySchoolResultHandlerI() {
}

bool BatchElementarySchoolResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int) row["userid"];
	if (!SearchCacheManagerI::instance().belongsMe(id)) return true;
	SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE,id);
	if (data == 0){
		// MCE_WARN("IMPOSSIABLE, elementary_school_info has userid="<<id<<" but user_basic not. "
		// 		<<__FILE__<<" "<<__LINE__);
	} else {
		data->setElementarySchoolProperties(row, "elementary_school_id", "elementary_school_year", "elementary_school_name");
	}
	return true;
}

BatchJuniorSchoolResultHandlerI::BatchJuniorSchoolResultHandlerI() {
}

bool BatchJuniorSchoolResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int)row["userid"];
	if (!SearchCacheManagerI::instance().belongsMe(id)) return true;
	SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE,id);
	if (data == 0){
		// MCE_WARN("IMPOSSIABLE, unior_high_school_info has userid="<<id<<" but user_basic not. "
		// 		<<__FILE__<<" "<<__LINE__);
	} else {
		data->setJuniorSchoolProperties(row, "junior_high_school_id", "junior_high_school_year", "junior_high_school_name");
	}
	return true;
}

BatchHighSchoolResultHandlerI::BatchHighSchoolResultHandlerI() {
}

bool BatchHighSchoolResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int)row["userid"];
	if (!SearchCacheManagerI::instance().belongsMe(id)) return true;
	SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE,id);
	if (data == 0){
		// MCE_WARN("IMPOSSIABLE, high_school_info has userid="<<id<<" but user_basic not. "
		// 		<<__FILE__<<" "<<__LINE__);
	} else {
		data->setHighSchoolProperties(row, "high_school_id", "enroll_year", "high_school_name");
	}
	return true;
}

BatchCollegeResultHandlerI::BatchCollegeResultHandlerI() {
}

bool BatchCollegeResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int)row["userid"];
	if (!SearchCacheManagerI::instance().belongsMe(id)) return true;
	SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE,id);
	if (data == 0){
		// MCE_WARN("IMPOSSIABLE, college_info has userid="<<id<<" but user_basic not. "
		// 		<<__FILE__<<" "<<__LINE__);
	} else {
		data->setCollegeProperties(row,	"college_id", "college_name", "department", "enroll_year");
	}
	return true;
}

BatchUniversityResultHandlerI::BatchUniversityResultHandlerI() {
}

bool BatchUniversityResultHandlerI::handle(mysqlpp::Row &row) const {
	int id = (int)row["userid"];
	if (!SearchCacheManagerI::instance().belongsMe(id)) return true;
	SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, id);
	if (data == 0){
		// MCE_WARN("IMPOSSIABLE, university_info has userid="<<id<<" but user_basic not. "
		// 		<<__FILE__<<" "<<__LINE__);
	}else {
		data->setUniversityProperties(row,"university_id","enroll_year", "university_name", "department");
	}
	return true;
}

BatchWorkplaceResultHandlerI::BatchWorkplaceResultHandlerI() {
}

bool BatchWorkplaceResultHandlerI::handle(mysqlpp::Row &row) const {
	int id = (int)row["userid"];
	if (!SearchCacheManagerI::instance().belongsMe(id)) return true;
	SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, id);
	if (data == 0){
		// MCE_WARN("IMPOSSIABLE, workplace_info has userid="<<id<<" but user_basic not. "
		// 		<<__FILE__<<" "<<__LINE__);
	}else {
		data->setWorkplaceProperties(row,"workplace_id", "workplace_name");
	}
	return true;
}

BatchRegionResultHandlerI::BatchRegionResultHandlerI() {
}

bool BatchRegionResultHandlerI::handle(mysqlpp::Row& row) const{
	int id = (int)row["userid"];
	if (!SearchCacheManagerI::instance().belongsMe(id)) return true;
	SearchCacheDataPtr data = ServiceI::instance().findObject<SearchCacheDataPtr>(SEARCH_CACHE, id);
	if (data == 0){
		// MCE_WARN("IMPOSSIABLE, region_info has userid=" << id<<" but user_basic not. " 
		// 		<<__FILE__<<" "<<__LINE__);
	} else {
		data->setRegionProperties(row,"region_id", "city_name", "province_name");
	}
	return true;
}

BatchBasicResultHandlerI::BatchBasicResultHandlerI(MyUtil::ObjectResultPtr& result, Ice::Int& maxId) :
	_result(result), _maxId(maxId) {
        /*
        _initDbSusscess = true;
        _db_cache = DatabaseCacheFactory::instance().getDbCache();
        if (_db_cache->Initialize() == false) {
            MCE_WARN("BatchBasicResultHandlerI: Db initialize failed");
            _initDbSusscess = false;
        }
        */
}

bool BatchBasicResultHandlerI::handle(mysqlpp::Row& row) const {
	int id = (int) row["id"];
	_maxId = id > _maxId ? id : _maxId;
	if(!SearchCacheManagerI::instance().belongsMe(id))return true;
	int status = (int)row["status"];
	if (status>=3){
		MCE_WARN("the user "<<id<<" status is "<<status);
		return true;
	}
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if (it == _result->data.end()) {
		SearchCacheDataPtr obj = new SearchCacheData;
        /*
        if (_initDbSusscess) {
            _db_cache->Query(id, obj.get());
        }
        */
		obj->_id = id;
		obj->setBasicProperties(row, "name", "star", "selected", "status", "home_city", "home_province");
		_result->data.insert(make_pair(id, obj));
	} else {
		MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
	}
	return true;
}

//***************************************************************************

BatchConfigTask::BatchConfigTask(const Statement& sql,int level):Task(level),_sql(sql){
}

void BatchConfigTask::handle(){
        BatchConfigResultHandlerI handler;
        QueryRunner(DB_SEARCH2_USERCONFIG, CDbRServer).query(_sql, handler);
}

BatchUserUrlTask::BatchUserUrlTask(const Statement& sql,int level):Task(level),_sql(sql){
}

void BatchUserUrlTask::handle(){
	BatchUserUrlResultHandlerI handler;
	QueryRunner(DB_SEARCH2_USERURL, CDbRServer).query(_sql, handler);
}

BatchUsersTask::BatchUsersTask(const Statement& sql,int level):Task(level),_sql(sql){
}

void BatchUsersTask::handle(){
	BatchUsersResultHandlerI handler;
	QueryRunner(DB_SEARCH2_USER, CDbRServer).query(_sql, handler);
}

BatchNetworkTask::BatchNetworkTask(const Statement& sql,int level):Task(level),_sql(sql){
}

void BatchNetworkTask::handle(){
        BatchNetworkResultHandlerI handler;
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(_sql, handler);
}

BatchElementaryTask::BatchElementaryTask(const Statement& sql,int level):Task(level),_sql(sql){
}

void BatchElementaryTask::handle(){
        BatchElementarySchoolResultHandlerI handler;
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(_sql, handler);
}

BatchJuniorTask::BatchJuniorTask(const Statement& sql,int level):Task(level), _sql(sql){
}

void BatchJuniorTask::handle(){
        BatchJuniorSchoolResultHandlerI handler;
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(_sql, handler);
}

BatchHighSchoolTask::BatchHighSchoolTask(const Statement& sql,int level):Task(level), _sql(sql){
}

void BatchHighSchoolTask::handle(){
        BatchHighSchoolResultHandlerI handler;
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(_sql, handler);
}

BatchCollegeTask::BatchCollegeTask(const Statement& sql,int level):Task(level), _sql(sql){
}

void BatchCollegeTask::handle(){
        BatchCollegeResultHandlerI handler;
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(_sql, handler);
}

BatchUniversityTask::BatchUniversityTask(const Statement& sql,int level):Task(level), _sql(sql){
}

void BatchUniversityTask::handle(){
        BatchUniversityResultHandlerI handler;
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(_sql, handler);
}

BatchWorkplaceTask::BatchWorkplaceTask(const Statement& sql,int level):Task(level), _sql(sql){
}

void BatchWorkplaceTask::handle(){
        BatchWorkplaceResultHandlerI handler;
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(_sql, handler);
}

BatchRegionTask::BatchRegionTask(const Statement& sql,int level):Task(level), _sql(sql){
}

void BatchRegionTask::handle(){
        BatchRegionResultHandlerI handler;
        QueryRunner(DB_SEARCH2_USERBASIC, CDbRServer).query(_sql, handler);
}

//***************************************************************************

void ServiceSchemaI::configure(const Ice::PropertiesPtr& props) {
	TaskManager::instance().config(props, "Fill", TASK_LEVEL_FILL, ThreadPoolConfig(1, 1));
}
