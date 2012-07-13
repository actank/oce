#include "UserApiCacheLoaderI.h"

#include "ReplicaCluster/CacheLoader/src/CacheLoader.h"

using namespace xce::userapicache;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::cacheloader;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserApiCacheLoaderI::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserApiCache.Controller", "ControllerUserApiCache");
        int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
        int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserApiCache.Proxy.Timeout", 300);
        int threadSize = 4;
	        CacheLoader<UserApiCacheManagerPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserApiCacheFactory,
                ctlEndpoint,
                subInterval,
                timeout,
                PRODUCE_BATCH_SIZE,
                CONSUME_BATCH_SIZE,
                WRITE_BATCH_SIZE,
                threadSize );
}


void UserApiCacheLoaderI::load(const MyUtil::IntSeq& ids, const Ice::Current& current) {
	MCE_DEBUG("UserApiCacheLoaderI::load -> size: "<<ids.size());
	CacheLoader<UserApiCacheManagerPrx>::instance().reload(ids);
}

ObjectResultPtr UserApiCacheFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_DEBUG("UserApiCacheLoaderI::create group -> id from: "<<minId <<" to: "<<maxId);
        ObjectResultPtr result = new ObjectResult;
        {
                Statement sql;
		sql<<	"SELECT	id,status  FROM "<<	TABLE_USER_PASSPORT <<"	WHERE	id >= "<<minId<<" AND id <= "<<maxId;
                QueryRunner(DB_USER_PASSPORT_PRELOADER,CDbRServer).query(sql, BatchUserPassportResultHandler(result));
        }
        {
                Statement sql;
		sql<< "SELECT id,stage,univ FROM " << TABLE_USER_STAGE << " WHERE id >= "<<minId<<" AND id<= "<<maxId;
        	QueryRunner(DB_USER_STAGE_PRELOADER,CDbRServer).query(sql, BatchUserStageResultHandler(result));
        }
        {
                Statement sql;
		sql<< "SELECT id,gender,birth_year,birth_month,birth_day,home_province, home_city FROM " << TABLE_USER_BORN << " WHERE id >= "<<minId<<" AND id<= "<<maxId;
        	QueryRunner(DB_USER_BORN_PRELOADER,CDbRServer).query(sql, BatchUserBornResultHandler(result));
        }
        {
                Statement sql;
		sql<< "SELECT id,basic_config,status_config,profile_privacy FROM "<< TABLE_USER_CONFIG <<" WHERE id >= " <<minId << " AND id <= "<<maxId;
                QueryRunner(DB_USER_CONFIG_PRELOADER,CDbRServer).query(sql, BatchUserConfigResultHandler(result));
        }
        {
                Statement sql;
		sql<<	"SELECT	id,mainurl,headurl,tinyurl FROM "<<	TABLE_USER_URL	<<"	WHERE	id >= "<<minId<<" AND id <= "<<maxId;
                QueryRunner(DB_USER_URL_PRELOADER,CDbRServer).query(sql, BatchUserUrlResultHandler(result));
        }
        {
                Statement sql;
		sql<<	"SELECT	id,name FROM "<<	TABLE_USER_NAME	<<"	WHERE id >= " << minId<<" AND id <= "<<maxId;
                QueryRunner(DB_USER_NAME_PRELOADER,CDbRServer).query(sql, BatchUserNameResultHandler(result));
        }
        {
                Statement sql;
		sql<<	"SELECT	id,auth FROM "<<	TABLE_USER_RIGHT	<<"	WHERE	id >= "<< minId<<" AND id <= "<<maxId;
                QueryRunner(DB_USER_RIGHT_PRELOADER,CDbRServer).query(sql, BatchUserRightResultHandler(result));
        }
        {
                Statement sql;
		sql<<	"SELECT	id,state,level FROM "<<	TABLE_USER_STATE	<<"	WHERE	id >= "<< minId<<" AND id <= "<<maxId;
                QueryRunner(DB_USER_STATE_PRELOADER,CDbRServer).query(sql, BatchUserStateResultHandler(result));
        }
	ObjectResultPtr newresult = new ObjectResult;
	for (map<long, Ice::ObjectPtr>::iterator i = result->data.begin(); i != result->data.end(); ++i) {
		if (UserApiCacheDataIPtr::dynamicCast(i->second)->getTableNum() == TABLE_NUM) {
			newresult->data.insert(*i);
		} else {
			MCE_WARN("UserApiCacheFactory::create -> id: "<<i->first<<" info not full");
		}
	}
        return newresult;
}
ObjectResultPtr UserApiCacheFactory::create(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("UserApiCacheLoaderI::create group -> id size: "<<ids.size());
        ObjectResultPtr result = new ObjectResult;
        {
                Statement sql;
				sql<<	"SELECT	id,status  FROM "<<	TABLE_USER_PASSPORT <<"	WHERE	id	IN (	";
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ",";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner(DB_USER_PASSPORT,CDbRServer).query(sql, BatchUserPassportResultHandler(result));
        }
        {
                Statement sql;
				sql<< "SELECT id,stage,univ FROM " << TABLE_USER_STAGE << " WHERE id IN ( " ;
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ", ";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner(DB_USER_STAGE,CDbRServer).query(sql, BatchUserStageResultHandler(result));
        }
        {
                Statement sql;
				sql<< "SELECT id,gender,birth_year,birth_month,birth_day,home_province, home_city FROM " << TABLE_USER_BORN << " WHERE id IN ( " ;
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ", ";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner(DB_USER_BORN,CDbRServer).query(sql, BatchUserBornResultHandler(result));
        }
        {
                Statement sql;
				sql<< "SELECT id,basic_config,status_config,profile_privacy FROM "<< TABLE_USER_CONFIG <<" WHERE id IN ( " ;
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ",";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner(DB_USER_CONFIG,CDbRServer).query(sql, BatchUserConfigResultHandler(result));
        }
        {
                Statement sql;
				sql<<	"SELECT	id,mainurl,headurl,tinyurl FROM "<<	TABLE_USER_URL	<<"	WHERE	id	IN ( ";
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ",";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner(DB_USER_URL,CDbRServer).query(sql, BatchUserUrlResultHandler(result));
        }
        {
                Statement sql;
				sql<<	"SELECT	id,name FROM "<<	TABLE_USER_NAME	<<"	WHERE	id	IN ( ";
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ",";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner(DB_USER_NAME,CDbRServer).query(sql, BatchUserNameResultHandler(result));
        }
        {
                Statement sql;
				sql<<	"SELECT	id,auth FROM "<<	TABLE_USER_RIGHT	<<"	WHERE	id	IN (	";
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ",";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner(DB_USER_RIGHT,CDbRServer).query(sql, BatchUserRightResultHandler(result));
        }
        {
                Statement sql;
				sql<<	"SELECT	id,state,level FROM "<<	TABLE_USER_STATE	<<"	WHERE	id	IN (	";
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ",";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner(DB_USER_STATE,CDbRServer).query(sql, BatchUserStateResultHandler(result));
        }
	ObjectResultPtr newresult = new ObjectResult;
	for (map<long, Ice::ObjectPtr>::iterator i = result->data.begin(); i != result->data.end(); ++i) {
		if (UserApiCacheDataIPtr::dynamicCast(i->second)->getTableNum() == TABLE_NUM) {
			newresult->data.insert(*i);
		}else {
			MCE_WARN("UserApiCacheFactory::create -> id: "<<i->first<<" info not full");
		}
	}
        return newresult;
}

BatchUserBornResultHandler::BatchUserBornResultHandler(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserBornResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
        } else {
			try
			{
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserBornProperties(row, "gender","birth_year", "birth_month", "birth_day", "home_province","home_city");
			}catch( ... )
			{
				MCE_WARN( "BatchUserBornResultHandler::handle id = " << id << " Exception!" );
			}
        }
        return true;
    
}

BatchUserStageResultHandler::BatchUserStageResultHandler(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserStageResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
        } else {
			try
			{
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserStageProperties(row, "univ", "stage");
			}catch( ... )
			{
				MCE_WARN( "BatchUserStageResultHandler::handle id = " << id << " Exception!" );
			}
        }
        return true;
    
}

BatchUserConfigResultHandler::BatchUserConfigResultHandler(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserConfigResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
        } else {
			try
			{
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserConfigProperties(row, "basic_config", "status_config", "profile_privacy");
			}catch( ... )
			{
				MCE_WARN( "BatchUserConfigResultHandler::handle id = " << id << " Exception!" );
			}
        }
        return true;
}

BatchUserUrlResultHandler::BatchUserUrlResultHandler(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserUrlResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
        } else {
			try
			{
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserUrlProperties(row, "mainurl","headurl","tinyurl");
			}catch( ... )
			{
				MCE_WARN( "BatchUserUrlResultHandler::handle id = " << id << " Exception!" );
			}
        }
        return true;
}



BatchUserPassportResultHandler::BatchUserPassportResultHandler(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserPassportResultHandler::handle(mysqlpp::Row& row) const {
		
	int id = (int) row["id"];
	/*
	int status = (int) row["status"];
	if (status >=3 ){
		MCE_WARN("status is error, id=" << id << " status=" << status);
		return true;
	}*/
    MyUtil::ObjectMap::iterator it = _result->data.find(id);
    if (it == _result->data.end()) {
    UserApiCacheDataIPtr obj = new UserApiCacheDataI;
    obj->id = id;
	try
	{
		obj->setUserPassportProperties(row,"id","status");
	}catch( ... )
	{
		MCE_WARN( "BatchUserPassportResultHandler::handle id = " << id << " Exception!" );
	}
    _result->data.insert(make_pair(id, obj));
    } else {
    MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
    }
    return true;
}

BatchUserNameResultHandler::BatchUserNameResultHandler(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserNameResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
        } else {
			try
			{
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserNameProperties(row, "name");
			}catch( ... )
			{
				MCE_WARN( "BatchUserNameResultHandler::handle id = " << id << " Exception!" );
			}
        }
        return true;
}

BatchUserRightResultHandler::BatchUserRightResultHandler(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserRightResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
        } else {
			try
			{
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserRightProperties(row, "auth");
			}catch( ... )
			{
				MCE_WARN( "BatchUserRightResultHandler::handle id = " << id << " Exception!" );
			}
        }
        return true;
}

BatchUserStateResultHandler::BatchUserStateResultHandler(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserStateResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
        } else {
			try
			{
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserStateProperties(row, "state", "level");
			}catch( ... )
			{
				MCE_WARN( "BatchUserStateResultHandler::handle id = " << id << " Exception!" );
			}
        }
        return true;
}

