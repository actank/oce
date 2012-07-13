#include "UserCachePreloader.h"
#include "GeneralPreloader/src/GeneralPreloader.h"
#include "user/UserCache/share/UserCacheDataI.h"
#include "PresenceAdapter.h"
#include "UserCacheAdapter.h"

using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace std;
using namespace mysqlpp;
using namespace xce::generalpreloader;
using namespace xce::usercachepreloader;
using namespace xce::usercache;

using namespace IceUtil;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&UserCachePreloader::instance(), service.createIdentity("M", ""));
	string ctlEndpoint = ServiceI::instance().getCommunicator()->getProperties()->getPropertyWithDefault("UserCache.Controller", "ControllerUserCacheN");
        int subInterval = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("Subscriber.Controller.Interval", 5);
        int timeout = ServiceI::instance().getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserCache.Proxy.Timeout", -1);
		
        int cluster= UserCacheAdapter::instance().getCluster();
		
		GeneralPreloader<xce::usercache::UserCacheManagerPrx>::instance().initialize(
                new BatchUsersIdProducer,
                new UserCachePreloaderFactory,
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


ObjectResultPtr UserCachePreloaderFactory::create(Ice::Int minId,Ice::Int maxId) {
	ObjectResultPtr result = new ObjectResult;
	{
		Statement sql;
		sql << "SELECT id, status FROM user_passport WHERE id >= " << minId << " AND id <= " << maxId;
                QueryRunner("usercache_passport_preloader",CDbRServer).query(sql, BatchUserPassportResultHandler(result));

	}
        {
                Statement sql;
                sql << "SELECT id, state,star,level FROM user_state WHERE id >= "<<minId << " AND id <= "<< maxId;
                QueryRunner("usercache_state_preloader",CDbRServer).query(sql, BatchUserStateResultHandler(result));
        }
        {
                Statement sql;
                sql << "SELECT id, univ FROM user_stage WHERE id >= "<<minId<<" AND id <= "<< maxId;
                QueryRunner("usercache_stage_preloader",CDbRServer).query(sql, BatchUserStageResultHandler(result));
        }
        {
                Statement sql;
                sql << "SELECT id, gender FROM user_born WHERE id >= "<<minId<<" AND id <= "<< maxId;
                QueryRunner("usercache_born_preloader",CDbRServer).query(sql, BatchUserBornResultHandler(result));
        }
        {
                Statement sql;
                sql << "SELECT id, name FROM user_names WHERE id >= "<<minId<<" AND id <= "<<maxId;
                QueryRunner("usercache_names_preloader",CDbRServer).query(sql, BatchUserNamesResultHandler(result));
        }
        {
                Statement sql;
                sql << "SELECT id, tinyurl FROM user_url WHERE id >= "<< minId<<" AND id <=" << maxId;
                QueryRunner("usercache_url_preloader",CDbRServer).query(sql, BatchUserUrlResultHandler(result));
        }


	MyUtil::ObjectResultPtr final_result = new MyUtil::ObjectResult;
	for (map<long, Ice::ObjectPtr>::const_iterator i = result->data.begin(); i != result->data.end(); ++i) {
		if (TABLE_NUMBER == UserCacheDataIPtr::dynamicCast(i->second)->in_tables) {
			final_result->data.insert(make_pair(i->first, i->second));
		} else {
			MCE_WARN("id=" << i->first << " has uncomplete information");
		}
	}

	return final_result;
}

ObjectResultPtr UserCachePreloaderFactory::create(const MyUtil::IntSeq& ids) {
        ObjectResultPtr result = new ObjectResult;
        {
                Statement sql;
                sql << "SELECT id, status FROM user_passport WHERE id IN (";
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ", ";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner("usercache_passport_preloader",CDbRServer).query(sql, BatchUserPassportResultHandler(result));
        }
        {
                Statement sql;
                sql << "SELECT id, state,star,level FROM user_state WHERE id IN (";
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ",";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner("usercache_state_preloader",CDbRServer).query(sql, BatchUserStateResultHandler(result));
        }
        {
                Statement sql;
                sql << "SELECT id, univ FROM user_stage WHERE id IN (";
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ",";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner("usercache_stage_preloader",CDbRServer).query(sql, BatchUserStageResultHandler(result));
        }
        {
                Statement sql;
                sql << "SELECT id, gender FROM user_born WHERE id IN (";
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ",";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner("usercache_born_preloader",CDbRServer).query(sql, BatchUserBornResultHandler(result));
        }
        {
                Statement sql;
                sql << "SELECT id, name FROM user_names WHERE id IN (";
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ",";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner("usercache_names_preloader",CDbRServer).query(sql, BatchUserNamesResultHandler(result));
        }
        {
                Statement sql;
                sql << "SELECT id, tinyurl FROM user_url WHERE id IN (";
                for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                        if (it != ids.begin()) {
                                sql << ",";
                        }
                        sql << *it;
                }
                sql << ")";
                QueryRunner("usercache_url_preloader",CDbRServer).query(sql, BatchUserUrlResultHandler(result));
        }
	{
		try {
			map<int, int> res = mop::hi::svc::adapter::PresenceAdapter::instance().getOnlineStat(ids);
			for (map<int,int>::iterator itr = res.begin(); itr != res.end(); ++itr) {
				MyUtil::ObjectMap::iterator it = result->data.find(itr->first);
				if (it == result->data.end()) {
					MCE_WARN("unreachable code segment " << __FILE__ << " " << __LINE__);
				} else  {
					xce::usercache::UserCacheDataIPtr::dynamicCast(it->second)->online = itr->second;
				}
			}
		} catch(const Ice::Exception& e) {
			MCE_WARN("UserCachePreloaderFactory::create -> exception: "<<e);
		} catch(const std::exception& e) {
			MCE_WARN("UserCachePreloaderFactory::create -> exception: "<<e.what());
		} catch(...) {
			MCE_WARN("UserCachePreloaderFactory::create -> unknown exception");
		}
	}

	MyUtil::ObjectResultPtr final_result = new MyUtil::ObjectResult;
	for (map<long, Ice::ObjectPtr>::const_iterator i = result->data.begin(); i != result->data.end(); ++i) {
		if (TABLE_NUMBER == UserCacheDataIPtr::dynamicCast(i->second)->in_tables) {
			final_result->data.insert(make_pair(i->first, i->second));
		} else {
			MCE_WARN("id=" << i->first << " has uncomplete information");
		}
	}

        return final_result;
}

bool BatchUserPassportResultHandler::handle(Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it != _result->data.end()) {
                MCE_WARN("BatchUserPassportResultHandler::handle -> Invalid Id: " <<  id);
        } else {
		xce::usercache::UserCacheDataIPtr data = new xce::usercache::UserCacheDataI;
		data->id = id;
		data->setUserPassportProperties(row,"status");
		_result->data.insert(make_pair(id,data));
        }
        return true;
}

bool BatchUserStateResultHandler::handle(Row& row) const {
	int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if (it != _result->data.end()) {
		xce::usercache::UserCacheDataIPtr::dynamicCast(it->second)->setStateProperties(row,"state", "level");
	} else {
		MCE_WARN("BatchUserStateResultHandler::handle -> user_state has userid = " << id << " but user_passport not.");
	}
	return true;
}

bool BatchUserNamesResultHandler::handle(Row& row) const {
	int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if (it != _result->data.end()) {
		xce::usercache::UserCacheDataIPtr::dynamicCast(it->second)->setNamesProperties(row, "name");
	} else {
		MCE_WARN("BatchUserNamesResultHandler::handle -> user_names has userid = " << id << " but user_passport not.");
	}
	return true;
}

bool BatchUserUrlResultHandler::handle(Row& row) const {
	int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if (it != _result->data.end()) {
		xce::usercache::UserCacheDataIPtr::dynamicCast(it->second)->setUrlProperties(row, "tinyurl");
	} else {
		MCE_WARN("BatchUserUrlResultHandler::handle -> user_url has userid = " << id << " but user_passport not.");
	}
	return true;
}

bool BatchUserBornResultHandler::handle(Row& row) const {
	int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if (it != _result->data.end()) {
		xce::usercache::UserCacheDataIPtr::dynamicCast(it->second)->setBornProperties(row, "gender");
	} else {
		MCE_WARN("BatchUserBornResultHandler::handle -> user_born has userid = " << id << " but user_passport not.");
	}
	return true;
}

bool BatchUserStageResultHandler::handle(Row& row) const {
	int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if (it != _result->data.end()) {
		xce::usercache::UserCacheDataIPtr::dynamicCast(it->second)->setStageProperties(row, "univ");
	} else {
		MCE_WARN("BatchUserStageResultHandler::handle -> user_stage has userid = " << id << " but user_passport not.");
	}
	return true;
}

