#include "UserApiCacheReloaderI.h"
#include "UserApiCacheAdapter.h"
#include "user/UserApiCache/share/UserApiCacheDataI.cpp"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"

using xce::userapicache::UserApiCacheUpdateNewDataPtr;
using xce::userapicache::UserApiCacheUpdateNewData;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&xce::userapicache::UserApiCacheReloaderI::instance(), service.createIdentity(xce::userapicache::USER_API_CACHE_RELOADER, ""));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsInt("UserApiCacheReloader.Mod");
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserApiCacheReloader.Interval", 5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerUserApiCacheReloader", &xce::userapicache::UserApiCacheReloaderI::instance(), mod, interval);
}

bool xce::userapicache::UserApiCacheReloaderI::isValid(const Ice::Current&) {
        IceUtil::RWRecMutex::RLock lock(validmutex_);
        return valid_;
}

void xce::userapicache::UserApiCacheReloaderI::setValid(bool valid, const Ice::Current& current) {
	ostringstream oss;
	oss << "UserApiCacheReloaderI::setValid -> " << valid << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
        IceUtil::RWRecMutex::WLock lock(validmutex_);
        valid_ = valid;
        if (valid_) {
                xce::serverstate::ServerStateSubscriber::instance().setStatus(1);
        } else {
                xce::serverstate::ServerStateSubscriber::instance().setStatus(0);
        }
}

void xce::userapicache::UserApiCacheReloaderI::reload(const MyUtil::IntSeq& ids, const Ice::Current& current) {
	ostringstream oss;
	oss << "UserApiCacheReloaderI::reload -> size: "<<ids.size()<< " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	MyUtil::IntSeq tempids;
	for (MyUtil::IntSeq::const_iterator i=ids.begin(); i!=ids.end();++i){
		if (*i > 0) {
			tempids.push_back(*i);
		}
	}
	MyUtil::TaskManager::instance().execute(new ReloadTask(tempids));
}

void xce::userapicache::UserApiCacheReloaderI::update(const xce::userapicache::UserApiCacheUpdateOldDataPtr& data, const Ice::Current& current) {
	ostringstream oss;
	oss << "UserApiCacheReloaderI::update -> id: "<<data->userid <<" "<< current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	
	MyUtil::TaskManager::instance().execute(new UpdateTask(data));
}

UserApiCacheUpdateNewDataPtr xce::userapicache::UpdateTask::GetNewData() {
	UserApiCacheUpdateNewDataPtr data = new UserApiCacheUpdateNewData;
	data->userid = data_->userid;
	Str2StrMap::const_iterator i;
	switch (data_->table) {
        	case TABLEBORN:
			i = data_->data.find("BIRTH_DAY");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDBIRTHDAY, i->second));
                        }
			i = data_->data.find("GENDER");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDGENDER, i->second));
                        }
			i = data_->data.find("BIRTH_YEAR");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDBIRTHYEAR, i->second));
                        }
			i = data_->data.find("BIRTH_MONTH");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDBIRTHMONTH, i->second));
                        }
			i = data_->data.find("HOME_CITY");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDHOMECITY, i->second));
                        }
			i = data_->data.find("HOME_PROVINCE");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDHOMEPROVINCE, i->second));
                        }
			break;
        	case TABLESTAGE:
			i = data_->data.find("STAGE");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDSTAGE, i->second));
                        }
			i = data_->data.find("UNIV");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDUNIV, i->second));
                        }
			break;
        	case TABLESTATE:
			i = data_->data.find("LEVEL");
                        if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDLEVEL, i->second));
                        }
			i = data_->data.find("STATE");
                        if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDSTATE, i->second));
                        }
			break;
        	case TABLENAMES:
			i = data_->data.find("NAME");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDNAME, i->second));
                        }
			break;
        	case TABLERIGHT:
			i = data_->data.find("AUTH");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDAUTH, i->second));
                        }
			break;
        	case TABLEPASSPORT:
			i = data_->data.find("STATUS");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDSTATUS, i->second));
                        }
			break;
        	case TABLEURL:
			i = data_->data.find("TINYURL");
                        if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDTINYURL, i->second));
                        }
			i = data_->data.find("MAINURL");
                        if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDMAINURL, i->second));
                        }
			i = data_->data.find("HEADURL");
                        if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDHEADURL, i->second));
                        }
			break;
        	case TABLECONFIG:
			i = data_->data.find("BASIC_CONFIG");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDBASICCONFIG, i->second));
                        }
			i = data_->data.find("STATUS_CONFIG");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDSTATUSCONFIG, i->second));
                        }
			i = data_->data.find("PROFILE_PRIVACY");
			if (i != data_->data.end()) {
                                data->data.insert(make_pair(FIELDPROFILEPRIVACY, i->second));
                        }
			break;
		default:
                        MCE_WARN("UpdateTask::GetNewData -> Oh, no!");
	}
	return data;
}

void xce::userapicache::UpdateTask::handle() {
	UserApiCacheUpdateNewDataPtr data = GetNewData();
	std::vector<std::pair<int, Ice::ObjectPtr> > d;
	d.push_back(std::pair<int, Ice::ObjectPtr>(data->userid, data));
	UpdateCallGuarantee::instance().DoInTime(d);
}

void xce::userapicache::ReloadTask::handle() {
	std::vector<std::vector<int> > ids;
	DistributeIds(ids);
	for (unsigned int i = 0; i < ids.size(); ++i) {
		if (ids[i].size() <= 0) {
			continue;
		}
		std::vector<std::pair<int, Ice::ObjectPtr> > data;
		for (std::vector<int>::const_iterator j = ids[i].begin(); j != ids[i].end(); ++j) {
			data.push_back(std::pair<int, Ice::ObjectPtr>(*j, 0));
		}
		ReloadCallGuarantee::instance().DoInTime(data);
	}
}

void xce::userapicache::ReloadTask::DistributeIds(std::vector<std::vector<int> >& ids) {
	int cluster = 4;
	for (int i = 0; i < cluster; ++i) {
		ids.push_back(vector<int>());
	}
	for (MyUtil::IntSeq::const_iterator i = ids_.begin(); i != ids_.end(); ++i) {
		ids[(*i) % cluster].push_back(*i);
	}
}

MyUtil::ObjectResultPtr xce::userapicache::UserApiCacheReloaderI::GetObjectResult(const MyUtil::IntSeq& ids) {
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	GetUserPassport(ids, result);
	GetUserState(ids, result);
	GetUserRight(ids, result);
	GetUserConfig(ids, result);
	GetUserNames(ids, result);
	GetUserUrl(ids, result);
	GetUserBorn(ids, result);
	GetUserStage(ids, result);
	return result;
}

void xce::userapicache::UserApiCacheReloaderI::GetUserPassport(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id, status FROM user_passport WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ", ";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("userapicache_user_passport",com::xiaonei::xce::CDbRServer).query(sql, BatchUserPassportResultHandler(data));
}

void xce::userapicache::UserApiCacheReloaderI::GetUserState(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id, state,level FROM user_state WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ",";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("userapicache_user_state",com::xiaonei::xce::CDbRServer).query(sql, BatchUserStateResultHandler(data));
}

void xce::userapicache::UserApiCacheReloaderI::GetUserBorn(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id,gender,birth_year,birth_month,birth_day,home_province, home_city FROM user_born WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ",";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("userapicache_user_born",com::xiaonei::xce::CDbRServer).query(sql, BatchUserBornResultHandler(data));
}

void xce::userapicache::UserApiCacheReloaderI::GetUserStage(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id,stage,univ FROM user_stage WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ",";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("userapicache_user_stage",com::xiaonei::xce::CDbRServer).query(sql, BatchUserStageResultHandler(data));
}

void xce::userapicache::UserApiCacheReloaderI::GetUserNames(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id, name FROM user_names WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ",";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("userapicache_user_names",com::xiaonei::xce::CDbRServer).query(sql, BatchUserNamesResultHandler(data));
}

void xce::userapicache::UserApiCacheReloaderI::GetUserRight(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
	com::xiaonei::xce::Statement sql;
	sql << "SELECT id,auth FROM user_right WHERE id IN (";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";
	com::xiaonei::xce::QueryRunner("userapicache_user_right",com::xiaonei::xce::CDbRServer).query(sql, BatchUserRightResultHandler(data));
}

void xce::userapicache::UserApiCacheReloaderI::GetUserConfig(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
	com::xiaonei::xce::Statement sql;
	sql << "SELECT id,basic_config,status_config,profile_privacy FROM user_config WHERE id IN (";
	for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
		if (it != ids.begin()) {
			sql << ",";
		}
		sql << *it;
	}
	sql << ")";
	com::xiaonei::xce::QueryRunner("userapicache_user_config",com::xiaonei::xce::CDbRServer).query(sql, BatchUserConfigResultHandler(data));
}

void xce::userapicache::UserApiCacheReloaderI::GetUserUrl(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id,mainurl,headurl,tinyurl FROM user_url WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ",";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("userapicache_user_url",com::xiaonei::xce::CDbRServer).query(sql, BatchUserUrlResultHandler(data));
}

bool BatchUserPassportResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
	MyUtil::ObjectMap::iterator it = _result->data.find(id);
	if (it == _result->data.end()) {
		UserApiCacheDataIPtr obj = new UserApiCacheDataI;
		obj->id = id;
		obj->setUserPassportProperties(row,"id","status");
		_result->data.insert(make_pair(id, obj));
	} else {
		MCE_WARN("BatchUserPassportResultHandler::handle -> Invalid Id: " << id);
	}
	return true;
}

bool BatchUserNamesResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
		MCE_WARN("BatchUserNamesResultHandler::handle -> user_names has userid = "<<id <<" but user_passport not");
        } else {
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserNameProperties(row, "name");
        }
        return true;
}

bool BatchUserRightResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
		MCE_WARN("BatchUserRightResultHandler::handle -> user_right has userid = "<<id <<" but user_passport not");
        } else {
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserRightProperties(row, "auth");
        }
        return true;
}

bool BatchUserStateResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
		MCE_WARN("BatchUserStateResultHandler::handle -> user_state has userid = "<<id <<" but user_passport not");
        } else {
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserStateProperties(row, "state", "level");
        }
        return true;
}

bool BatchUserUrlResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
		MCE_WARN("BatchUserUrlResultHandler::handle -> user_url has userid = "<<id <<" but user_passport not");
        } else {
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserUrlProperties(row, "mainurl","headurl","tinyurl");
        }
        return true;
}

bool BatchUserConfigResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
		MCE_WARN("BatchUserConfigResultHandler::handle -> user_config has userid = "<<id <<" but user_passport not");
        } else {
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserConfigProperties(row, "basic_config", "status_config", "profile_privacy");
        }
        return true;
}

bool BatchUserBornResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
		MCE_WARN("BatchUserBornResultHandler::handle -> user_born has userid = "<<id <<" but user_passport not");
        } else {
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserBornProperties(row, "gender","birth_year", "birth_month", "birth_day", "home_province","home_city");
        }
        return true;

}

bool BatchUserStageResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it == _result->data.end()) {
		MCE_WARN("BatchUserStageResultHandler::handle -> user_stage has userid = "<<id <<" but user_passport not");
        } else {
                UserApiCacheDataIPtr::dynamicCast(it->second)->setUserStageProperties(row, "univ", "stage");
        }
        return true;

}

bool xce::userapicache::UpdateCallGuarantee::Redo(const std::vector<int>& ids) {
        if (ids.size() <= 0) {
                return true;
        }
        try {
        	MyUtil::ObjectResultPtr result = xce::userapicache::UserApiCacheReloaderI::instance().GetObjectResult(ids);
		if (result->data.size() <= 0) {
			return true;
		}
                int mod = *ids.begin();
                std::vector<UserApiCacheManagerPrx> proxies = xce::userapicache::UserApiCacheAdapter::instance().getAllProxySeq(mod);
                if (proxies.size() <= 0) {
                        MCE_WARN("UpdateCallGuarantee::Redo -> no proxy");
                }
                for (std::vector<UserApiCacheManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
                        (*i)->setData(result);
                }
                return true;
        } catch (const Ice::Exception& e) {
                MCE_WARN("UpdateCallGuarantee::Redo -> exception: " << e);
	} catch (const std::exception& e) {
		MCE_WARN("UpdateCallGuarantee::Redo -> exception: " << e.what());
        } catch (...) {
                MCE_WARN("UpdateCallGuarantee::Redo -> unknown exception");
        }
        return false;
}

bool xce::userapicache::UpdateCallGuarantee::Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params) {
        if (params.size() <= 0) {
		MCE_WARN("UpdateCallGuarantee::Do -> params.size is not 1 but "<<params.size());
                return true;
        }

        try {
		int mod = params.begin()->first;
		std::vector<UserApiCacheManagerPrx> proxies = xce::userapicache::UserApiCacheAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("UpdateCallGuarantee::Do -> no proxy");
			return false;
		}
		for (std::vector<UserApiCacheManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
                        (*i)->update(UserApiCacheUpdateNewDataPtr::dynamicCast(params.begin()->second));
                }
                return true;
        } catch (const Ice::Exception& e) {
                MCE_WARN("UpdateCallGuarantee::Do -> exception: " << e);
	} catch (const std::exception& e) {
		MCE_WARN("UpdateCallGuarantee::Do -> exception: " << e.what());
        } catch (...) {
                MCE_WARN("UpdateCallGuarantee::Do -> unknown exception");
        }
        return false;
}


bool xce::userapicache::ReloadCallGuarantee::Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params) {
	if (params.size() <= 0) {
		return false;
	}

	try {
		std::vector<int> ids;

		for (std::vector<std::pair<int, Ice::ObjectPtr> >::const_iterator i = params.begin(); i != params.end(); ++i) {
			ids.push_back(i->first);
		}

		MyUtil::ObjectResultPtr result = xce::userapicache::UserApiCacheReloaderI::instance().GetObjectResult(ids);

		if (result->data.size()<=0) {
			return true;
		}
		int mod = *ids.begin();
		std::vector<UserApiCacheManagerPrx> proxies = xce::userapicache::UserApiCacheAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("ReloadCallGuarantee::Do -> no proxy");
			return false;
		}
		for (std::vector<UserApiCacheManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
			(*i)->setData(result);
		}
		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("ReloadCallGuarantee::Do -> exception: " << e);
	} catch (const std::exception& e) {
		MCE_WARN("ReloadCallGuarantee::Do -> exception: " << e.what());
	} catch (...) {
		MCE_WARN("ReloadCallGuarantee::Do -> unknown exception");
	}
	return false;
}

bool xce::userapicache::ReloadCallGuarantee::Redo(const std::vector<int>& ids) {
	if (ids.size() <= 0) {
                return true;
        }
        try {
		MyUtil::ObjectResultPtr result = xce::userapicache::UserApiCacheReloaderI::instance().GetObjectResult(ids);
		if (result->data.size() <= 0) {
			return true;
		}
                int mod = *ids.begin();
                std::vector<UserApiCacheManagerPrx> proxies = xce::userapicache::UserApiCacheAdapter::instance().getAllProxySeq(mod);
                if (proxies.size() <= 0) {
                        MCE_WARN("ReloadCallGuarantee::Redo -> no proxy");
                }
                for (std::vector<UserApiCacheManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
                        (*i)->setData(result);
                }
                return true;
        } catch (const Ice::Exception& e) {
                MCE_WARN("ReloadCallGuarantee::Redo -> exception: " << e);
        } catch (const std::exception& e) {
                MCE_WARN("ReloadCallGuarantee::Redo -> exception: " << e.what());
	} catch (...) {
                MCE_WARN("ReloadCallGuarantee::Redo -> unknown exception");
        }
        return false;
}


