#include "UserApiCachePreloader.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "user/UserApiCache/share/UserApiCacheDataI.cpp"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::userapicachepreloader;

using namespace IceUtil;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserApiCachePreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserApiCache.Controller", "ControllerUserApiCache");
        int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
        int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserApiCache.Proxy.Timeout", 300);
		
        int cluster= 4;
		
		GeneralPreloader<xce::userapicache::UserApiCacheManagerPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserApiCachePreloaderFactory,
                ctlEndpoint,
				cluster,
				subInterval,
                timeout,
				PRX_DATAQUEUE_MAX_SIZE ,
                PRODUCE_BATCH_SIZE ,
                CONSUME_BATCH_SIZE ,
				WRITE_BATCH_SIZE ,
                CONSUME_BATCH_THREAD_SIZE);
}

ObjectResultPtr UserApiCachePreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	MCE_DEBUG("UserApiCachePreloaderLoaderI::create group -> id from: "<<minId <<" to: "<<maxId);
        ObjectResultPtr result = new ObjectResult;
        {
                Statement sql;
		sql<<	"SELECT	id,status  FROM "<<	TABLE_USER_PASSPORT <<"	WHERE	id >= "<<minId<<" AND id <= "<<maxId;
                QueryRunner(DB_USER_PASSPORT,CDbRServer).query(sql, BatchUserPassportResultHandler(result));
        }
        {
                Statement sql;
		sql<< "SELECT id,univ, stage FROM " << TABLE_USER_STAGE << " WHERE id >= "<<minId<<" AND id<= "<<maxId;
        	QueryRunner(DB_USER_STAGE,CDbRServer).query(sql, BatchUserStageResultHandler(result));
        }
        {
                Statement sql;
		sql<< "SELECT id,gender,birth_year,birth_month,birth_day,home_province, home_city FROM " << TABLE_USER_BORN << " WHERE id >= "<<minId<<" AND id<= "<<maxId;
        	QueryRunner(DB_USER_BORN,CDbRServer).query(sql, BatchUserBornResultHandler(result));
        }
        {
                Statement sql;
		sql<< "SELECT id,basic_config,status_config,profile_privacy FROM "<< TABLE_USER_CONFIG <<" WHERE id >= " <<minId << " AND id <= "<<maxId;
                QueryRunner(DB_USER_CONFIG,CDbRServer).query(sql, BatchUserConfigResultHandler(result));
        }
        {
                Statement sql;
		sql<<	"SELECT	id,mainurl,headurl,tinyurl FROM "<<	TABLE_USER_URL	<<"	WHERE	id >= "<<minId<<" AND id <= "<<maxId;
                QueryRunner(DB_USER_URL,CDbRServer).query(sql, BatchUserUrlResultHandler(result));
        }
        {
                Statement sql;
		sql<<	"SELECT	id,name FROM "<<	TABLE_USER_NAME	<<"	WHERE id >= " << minId<<" AND id <= "<<maxId;
                QueryRunner(DB_USER_NAME,CDbRServer).query(sql, BatchUserNameResultHandler(result));
        }
        {
                Statement sql;
		sql<<	"SELECT	id,auth FROM "<<	TABLE_USER_RIGHT	<<"	WHERE	id >= "<< minId<<" AND id <= "<<maxId;
                QueryRunner(DB_USER_RIGHT,CDbRServer).query(sql, BatchUserRightResultHandler(result));
        }
        {
                Statement sql;
		sql<<	"SELECT	id,state,level FROM "<<	TABLE_USER_STATE	<<"	WHERE	id >= "<< minId<<" AND id <= "<<maxId;
                QueryRunner(DB_USER_STATE,CDbRServer).query(sql, BatchUserStateResultHandler(result));
        }
        return result;
}
ObjectResultPtr UserApiCachePreloaderFactory::create(const MyUtil::IntSeq& ids) {
	MCE_DEBUG("UserApiCachePreloaderLoaderI::create group -> id size: "<<ids.size());
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
        return result;
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
                xce::userapicache::UserApiCacheDataIPtr::dynamicCast(it->second)->setUserBornProperties(row, "gender","birth_year", "birth_month", "birth_day", "home_province","home_city");
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
                xce::userapicache::UserApiCacheDataIPtr::dynamicCast(it->second)->setUserStageProperties(row, "univ", "stage");
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
                xce::userapicache::UserApiCacheDataIPtr::dynamicCast(it->second)->setUserConfigProperties(row, "basic_config", "status_config", "profile_privacy");
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
                xce::userapicache::UserApiCacheDataIPtr::dynamicCast(it->second)->setUserUrlProperties(row, "mainurl","headurl","tinyurl");
        }
        return true;
}



BatchUserPassportResultHandler::BatchUserPassportResultHandler(
                MyUtil::ObjectResultPtr& result) :
        _result(result) {
}

bool BatchUserPassportResultHandler::handle(mysqlpp::Row& row) const {
		
	int id = (int) row["id"];
	int status = (int) row["status"];
	if (status >=3 ){
		return true;
	}
    MyUtil::ObjectMap::iterator it = _result->data.find(id);
    if (it == _result->data.end()) {
    xce::userapicache::UserApiCacheDataIPtr obj = new xce::userapicache::UserApiCacheDataI;
    obj->id = id;
    obj->setUserPassportProperties(row,"id","status");
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
                xce::userapicache::UserApiCacheDataIPtr::dynamicCast(it->second)->setUserNameProperties(row, "name");
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
                xce::userapicache::UserApiCacheDataIPtr::dynamicCast(it->second)->setUserRightProperties(row, "auth");
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
                xce::userapicache::UserApiCacheDataIPtr::dynamicCast(it->second)->setUserStateProperties(row, "state", "level");
        }
        return true;
}

