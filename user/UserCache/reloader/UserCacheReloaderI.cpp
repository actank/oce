#include "UserCacheReloaderI.h"
#include "UserCacheAdapter.h"
#include "PresenceAdapter.h"
#include "WapOnlineAdapter.h"
#include "user/UserCache/share/UserCacheDataI.h"
#include "RebuildOnline.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"
#include "util/cpp/TimeCost.h"
#include <ServiceI.h>


using MyUtil::TimeCost;
using MyUtil::Str2StrMap;
using xce::usercache::RebuildOnline;
using xce::usercache::UserCacheDataIPtr;
using xce::usercache::UserCacheData3Ptr;
using xce::adapter::userbase::UserBaseFullViewPtr;
using xce::adapter::userbase::UserBaseAdapter;

#include "OnlineEventMediator.h"
using xce::mediator::onlineevent::OFFLINE;
using xce::mediator::onlineevent::ONLINEWEBPAGGER;
using xce::mediator::onlineevent::ONLINEXNT;
using xce::mediator::onlineevent::ONLINEWAP;
using xce::mediator::onlineevent::OFFLINEWAP;

using namespace com::xiaonei::wap::adapter;
using mop::hi::svc::adapter::PresenceAdapter;
using mop::hi::svc::adapter::WEBPAGER_ONLINE;
using mop::hi::svc::adapter::TALK_ONLINE;

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&xce::usercache::UserCacheReloaderI::instance(), service.createIdentity(xce::usercache::USER_CACHE_RELOADER, ""));
	TaskManager::instance().config(xce::usercache::TASK_LEVEL_RELOAD, MyUtil::ThreadPoolConfig(1, 10));
	TaskManager::instance().config(xce::usercache::TASK_LEVEL_UPDATE, MyUtil::ThreadPoolConfig(1, 10));
	TaskManager::instance().config(xce::usercache::TASK_LEVEL_INSERT, MyUtil::ThreadPoolConfig(1, 5));
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserCacheReloader.Mod", 0);
	int interval = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("UserCacheReloader.Interval", 5);
	xce::serverstate::ServerStateSubscriber::instance().initialize("ControllerUserCacheReloader", &xce::usercache::UserCacheReloaderI::instance(), mod, interval);
	xce::usercache::UserCacheReloaderI::instance().setValid(true);

	//rebuildOnline
	RebuildOnline::instance().start(65535).detach();
}

void xce::usercache::UserCacheReloaderI::reload(const MyUtil::IntSeq& ids, const Ice::Current& current) {
	ostringstream oss;
	oss << "UserCacheReloaderI::reload -> size: "<<ids.size() << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	MyUtil::IntSeq tempids;
	for (MyUtil::IntSeq::const_iterator i=ids.begin(); i!=ids.end();++i){
		if (*i > 0) {
			tempids.push_back(*i);
		}
	}
	MyUtil::TaskManager::instance().execute(new ReloadTask(tempids, current));
}

void xce::usercache::UserCacheReloaderI::rebuildOnline(const MyUtil::IntSeq& ids, const Ice::Current& current) {
	ostringstream oss;
	oss << "UserCacheReloaderI::rebuildOnline -> size: "<< ids.size() << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	MyUtil::IntSeq rebuildIds;
	for (MyUtil::IntSeq::const_iterator i=ids.begin(); i!=ids.end();++i){
		if (*i > 0) {
			rebuildIds.push_back(*i);
		}
	}

	RebuildOnline::instance().rebuild(rebuildIds);
}

void xce::usercache::UserCacheReloaderI::insert(int id, const Ice::Current& current) {
	ostringstream oss;
	oss << "UserCacheReloaderI::insert -> id: "<<id<< " "<< current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	MyUtil::TaskManager::instance().execute(new InsertTask(id));
}

void xce::usercache::UserCacheReloaderI::update(const xce::usercache::UserCacheUpdateOldDataPtr& data, const Ice::Current& current) {
	ostringstream oss;
	oss << "UserCacheReloaderI::update ->: "<<data->userid << " " << current.con->toString().substr(current.con->toString().find("remote",25));
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	
	MyUtil::TaskManager::instance().execute(new UpdateTask(data, current));
}

bool xce::usercache::UserCacheReloaderI::isValid(const Ice::Current& current) {
	IceUtil::RWRecMutex::RLock lock(validmutex_);
	return valid_;
}

void xce::usercache::UserCacheReloaderI::setValid(bool valid, const Ice::Current& current) {
	ostringstream oss;
	oss << "UserCacheReloaderI::setValid -> " << valid;
	TimeCost tc = TimeCost::create(oss.str(), TimeCost::LOG_LEVEL_INFO);
	IceUtil::RWRecMutex::WLock lock(validmutex_);
	valid_ = valid;
	if (valid_) {
		xce::serverstate::ServerStateSubscriber::instance().setStatus(1);
	} else {
		xce::serverstate::ServerStateSubscriber::instance().setStatus(0);
	}
}


void xce::usercache::ReloadTask::handle() {
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

void xce::usercache::InsertTask::handle() {
	std::vector<std::pair<int, Ice::ObjectPtr> > data;
	data.push_back(std::pair<int, Ice::ObjectPtr>(id_, 0));
	InsertCallGuarantee::instance().DoInTime(data);
}

void xce::usercache::ReloadTask::DistributeIds(std::vector<std::vector<int> >& ids) {
	int cluster = UserCacheAdapter::instance().getCluster();
	for (int i = 0; i < cluster; ++i) {
		ids.push_back(vector<int>());
	}
	for (MyUtil::IntSeq::const_iterator i = ids_.begin(); i != ids_.end(); ++i) {
		ids[(*i) % cluster].push_back(*i);
	}
}

void xce::usercache::UserCacheReloaderI::GetUserPassport(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id, status FROM user_passport WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ", ";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("usercache_passport",com::xiaonei::xce::CDbRServer).query(sql, BatchUserPassportResultHandler(data));
}

void xce::usercache::UserCacheReloaderI::GetUserState(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id, state,star,level FROM user_state WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ",";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("usercache_state",com::xiaonei::xce::CDbRServer).query(sql, BatchUserStateResultHandler(data));
}

void xce::usercache::UserCacheReloaderI::GetUserStage(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id, univ FROM user_stage WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ",";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("usercache_stage",com::xiaonei::xce::CDbRServer).query(sql, BatchUserStageResultHandler(data));
}

void xce::usercache::UserCacheReloaderI::GetUserBorn(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id, gender FROM user_born WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ",";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("usercache_born",com::xiaonei::xce::CDbRServer).query(sql, BatchUserBornResultHandler(data));
}

void xce::usercache::UserCacheReloaderI::GetUserNames(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id, name FROM user_names WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ",";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("usercache_names",com::xiaonei::xce::CDbRServer).query(sql, BatchUserNamesResultHandler(data));
}

void xce::usercache::UserCacheReloaderI::GetUserUrl(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        com::xiaonei::xce::Statement sql;
        sql << "SELECT id, tinyurl FROM user_url WHERE id IN (";
        for (MyUtil::IntSeq::const_iterator it = ids.begin(); it != ids.end(); ++it) {
                if (it != ids.begin()) {
                        sql << ",";
                }
                sql << *it;
        }
        sql << ")";
        com::xiaonei::xce::QueryRunner("usercache_url",com::xiaonei::xce::CDbRServer).query(sql, BatchUserUrlResultHandler(data));
}

void xce::usercache::UserCacheReloaderI::GetOnlineStat(const MyUtil::IntSeq& ids, MyUtil::ObjectResultPtr& data) {
        try {
                std::map<int, int> res = mop::hi::svc::adapter::PresenceAdapter::instance().getOnlineStat(ids);
                for (map<int,int>::iterator itr = res.begin(); itr != res.end(); ++itr) {
                        MyUtil::ObjectMap::iterator it = data->data.find(itr->first);
                        if (it == data->data.end()) {
                                MCE_WARN("UserCacheReloaderI::GetOnlineStat -> Invalid Id: "<<itr->first);
                        } else  {
				int online = 0;
				online = (itr->second & WEBPAGER_ONLINE) ? ONLINEWEBPAGGER
							: OFFLINE;
				if (online > 0) {
	                                xce::usercache::UserCacheDataIPtr::dynamicCast(it->second)->online |= (1 << (online - 1));
				}
				online = (itr->second & TALK_ONLINE) ? ONLINEXNT
						: OFFLINE;
				if (online > 0) {
	                                xce::usercache::UserCacheDataIPtr::dynamicCast(it->second)->online |= (1 << (online - 1));
				}
                        }
                }
		
		map<int, int> wap_res = WapOnlineAdapter::instance().getOnlineStatus(ids);
		for (map<int, int>::iterator itr = wap_res.begin(); itr != wap_res.end(); ++itr) {
			MyUtil::ObjectMap::iterator wap_it = data->data.find(itr->first);
			if (wap_it == data->data.end()) {
                                MCE_WARN("UserCacheReloaderI::GetOnlineStat -> Invalid Id: "<<itr->first);
			} else {
				int online = 0;
				online = (itr->second & WAP_ONLINE) ? ONLINEWAP
						: OFFLINEWAP;
				if (online > 0) {
	                                xce::usercache::UserCacheDataIPtr::dynamicCast(wap_it->second)->online |= (1 << (online - 1));
				}
			}
		}
		
        } catch(const Ice::Exception& e) {
                MCE_WARN("UserCacheReloaderI::GetOnlineStat -> exception: "<<e);
        } catch(const std::exception& e) {
                MCE_WARN("UserCacheReloaderI::GetOnlineStat -> exception: "<<e.what());
        } catch(...) {
                MCE_WARN("UserCacheReloaderI::GetOnlineStat -> unknown exception");
        }
}

MyUtil::ObjectResultPtr xce::usercache::UserCacheReloaderI::GetObjectResult(const MyUtil::IntSeq& ids) {
	MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	GetUserPassport(ids, result);
	GetUserState(ids, result);
	GetUserStage(ids, result);
	GetUserBorn(ids, result);
	GetUserNames(ids, result);
	GetUserUrl(ids, result);
	GetOnlineStat(ids, result);

	MyUtil::ObjectResultPtr final_result = new MyUtil::ObjectResult;
	for (map<long, Ice::ObjectPtr>::const_iterator i = result->data.begin(); i != result->data.end(); ++i) {
		if (TABLE_NUMBER == UserCacheDataIPtr::dynamicCast(i->second)->in_tables) {
			final_result->data.insert(make_pair(i->first, i->second));
		} else {
			MCE_WARN("id=" << i->first << " has uncomplete information!");
		}
	}

	return final_result;
}

bool xce::usercache::BatchUserPassportResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"]; 
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it != _result->data.end()) {
                MCE_WARN("BatchUserPassportResultHandler::handle -> Invalid Id: "<<id);
        } else {
                UserCacheDataIPtr data = new UserCacheDataI;
                data->id = id;
                data->setUserPassportProperties(row,"status");
                _result->data.insert(make_pair(id,data));
        }
        return true;
}

bool xce::usercache::BatchUserStateResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it != _result->data.end()) {
                UserCacheDataIPtr::dynamicCast(it->second)->setStateProperties(row,"state", "level");
        } else {
                MCE_WARN("BatchUserStateResultHandler::handle -> user_state has userid = " << id << " but user_passport not.");
        }
        return true;
}

bool xce::usercache::BatchUserNamesResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it != _result->data.end()) {
                UserCacheDataIPtr::dynamicCast(it->second)->setNamesProperties(row, "name");
        } else {
                MCE_WARN("BatchUserNamesResultHandler::handle -> user_names has userid = " << id << " but user_passport not.");
        }
        return true;
}

bool xce::usercache::BatchUserUrlResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it != _result->data.end()) {
                UserCacheDataIPtr::dynamicCast(it->second)->setUrlProperties(row, "tinyurl");
        } else {
                MCE_WARN("BatchUserUrlResultHandler::handle -> user_url has userid = " << id << " but user_passport not.");
        }
        return true;
}

bool xce::usercache::BatchUserStageResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it != _result->data.end()) {
                UserCacheDataIPtr::dynamicCast(it->second)->setStageProperties(row, "univ");
        } else {
                MCE_WARN("BatchUserStageResultHandler::handle -> user_stage has userid = " << id << " but user_passport not.");
        }
        return true;
}

bool xce::usercache::BatchUserBornResultHandler::handle(mysqlpp::Row& row) const {
        int id = (int) row["id"];
        MyUtil::ObjectMap::iterator it = _result->data.find(id);
        if (it != _result->data.end()) {
                UserCacheDataIPtr::dynamicCast(it->second)->setBornProperties(row, "gender");
        } else {
                MCE_WARN("BatchUserBornResultHandler::handle -> user_born has userid = " << id << " but user_passport not.");
        }
        return true;
}

UserCacheData3Ptr xce::usercache::InsertCallGuarantee::Base2Cache(const UserBaseFullViewPtr& data) {
	UserCacheData3Ptr res = new UserCacheData3;
	res->id = data->id();
	res->status = data->status();
	res->univ = data->univ();
	res->state = data->state();
	res->level = data->level();
	res->name = data->name();
	res->tinyUrl = data->tinyurl();
	string gg = data->gender();
	if (gg=="男生") {
		res->gender = 1;
	} else if (gg=="女生") {
		res->gender = 2;
	} else {
		res->gender = 0;
	}
	res->online = 1;
	return res;
}

bool xce::usercache::InsertCallGuarantee::Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params) {
	if (params.size() <= 0) {
                return true;
        }

	try {
		MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
	
		for (std::vector<std::pair<int, Ice::ObjectPtr> >::const_iterator i = params.begin(); i != params.end(); ++i) {
			result->data.insert(std::pair<long, Ice::ObjectPtr>(i->first, Base2Cache(UserBaseAdapter::instance().getUserBaseFullView(i->first))));
		}

		if (result->data.size() <= 0) {
			return true;
		}

		int mod = result->data.begin()->first;
		std::vector<UserCacheManagerPrx> proxies = xce::usercache::UserCacheAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("InsertCallGuarantee::Do -> no proxy");
			return false;
		}
		for (std::vector<UserCacheManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
			(*i)->setData(result);
		}

		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("InsertCallGuarantee::Do -> exception: " << e);
	} catch (const std::exception& e) {
		MCE_WARN("InsertCallGuarantee::Do -> exception: " << e.what());
	} catch (...) {
		MCE_WARN("InsertCallGuarantee::Do -> unknown exception");
	}
	return false;
}

bool xce::usercache::ReloadCallGuarantee::Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params) {
	if (params.size() <= 0) {
                return true;
        }

	try {
		std::vector<int> ids;
	
		for (std::vector<std::pair<int, Ice::ObjectPtr> >::const_iterator i = params.begin(); i != params.end(); ++i) {
			ids.push_back(i->first);
		}

		MyUtil::ObjectResultPtr result = xce::usercache::UserCacheReloaderI::instance().GetObjectResult(ids);

		if (result->data.size() <= 0) {
			return true;
		}

		int mod = *ids.begin();
		std::vector<UserCacheManagerPrx> proxies = xce::usercache::UserCacheAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("ReloadCallGuarantee::Do -> no proxy");
			return false;
		}
		for (std::vector<UserCacheManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
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

bool xce::usercache::ReloadCallGuarantee::Redo(const std::vector<int>& ids) {
	if (ids.size() <= 0) {
		return true;
	}
	try {
		MyUtil::ObjectResultPtr result = xce::usercache::UserCacheReloaderI::instance().GetObjectResult(ids);
		if (result->data.size() <= 0) {
			return true;
		}
		int mod = *ids.begin();
		std::vector<UserCacheManagerPrx> proxies = xce::usercache::UserCacheAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("ReloadCallGuarantee::Redo -> no proxy");
		}
		for (std::vector<UserCacheManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
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

bool xce::usercache::InsertCallGuarantee::Redo(const std::vector<int>& ids) {
	if (ids.size() <= 0) {
		return true;
	}
	try {
		MyUtil::ObjectResultPtr result = new MyUtil::ObjectResult;
		if (result->data.size() <= 0) {
			return true;
		}
		for (std::vector<int>::const_iterator i = ids.begin(); i != ids.end(); ++i) {
                        result->data.insert(std::pair<long, Ice::ObjectPtr>(*i, Base2Cache(UserBaseAdapter::instance().getUserBaseFullView(*i))));
                }
		int mod = *ids.begin();
		std::vector<UserCacheManagerPrx> proxies = xce::usercache::UserCacheAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("InsertCallGuarantee::Redo -> no proxy");
		}
		for (std::vector<UserCacheManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
			(*i)->setData(result);
		}
		return true;
	} catch (const Ice::Exception& e) {
		MCE_WARN("InsertCallGuarantee::Redo -> exception: " << e);
	} catch (const std::exception& e) {
		MCE_WARN("InsertCallGuarantee::Redo -> exception: " << e.what());
	} catch (...) {
		MCE_WARN("InsertCallGuarantee::Redo -> unknown exception");
	}
	return false;
}

bool xce::usercache::UpdateCallGuarantee::Redo(const std::vector<int>& ids) {
	if (ids.size() <= 0) {
		return true;
	}
	try {
		MyUtil::ObjectResultPtr result = xce::usercache::UserCacheReloaderI::instance().GetObjectResult(ids);
		if (result->data.size() <= 0) {
                        return true;
                }
		int mod = *ids.begin();
		std::vector<UserCacheManagerPrx> proxies = xce::usercache::UserCacheAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("UpdateCallGuarantee::Redo -> no proxy");
		}
		for (std::vector<UserCacheManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
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

bool xce::usercache::UpdateCallGuarantee::Do(const std::vector<std::pair<int, Ice::ObjectPtr> >& params) {
	if (params.size() != 1) {
		MCE_WARN("UpdateCallGuarantee::Do -> params.size is not 1 but "<<params.size());
		return true;
	}

	try {
		int mod = params.begin()->first;
		std::vector<UserCacheManagerPrx> proxies = xce::usercache::UserCacheAdapter::instance().getAllProxySeq(mod);
		if (proxies.size() <= 0) {
			MCE_WARN("UpdateCallGuarantee::Do -> no proxy");
			return false;
		}
		for (std::vector<UserCacheManagerPrx>::iterator i = proxies.begin(); i != proxies.end(); ++i) {
			(*i)->update(UserCacheUpdateNewDataPtr::dynamicCast(params.begin()->second));
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

xce::usercache::UserCacheUpdateNewDataPtr xce::usercache::UpdateTask::GetNewData() {
	UserCacheUpdateNewDataPtr data = new UserCacheUpdateNewData;
	data->userid = data_->userid;

	Str2StrMap::const_iterator i;
	switch (data_->table) {
		case TABLEPASSPORT:
			i = data_->data.find("STATUS");
			if (i != data_->data.end()) {
				data->data.insert(make_pair(FIELDSTATUS, i->second));
			}
			break;
		case TABLEBORN:
			i = data_->data.find("GENDER");
			if (i != data_->data.end()) {
				data->data.insert(make_pair(FIELDGENDER, i->second));
			}
			break;
		case TABLESTATE:
			i = data_->data.find("STATE");
			if (i != data_->data.end()) {
				data->data.insert(make_pair(FIELDSTATE, i->second));
			}
			i = data_->data.find("LEVEL");
			if (i != data_->data.end()) {
				data->data.insert(make_pair(FIELDLEVEL, i->second));
			}
			break;
		case TABLENAMES:
			i = data_->data.find("NAME");
			if (i != data_->data.end()) {
				data->data.insert(make_pair(FIELDNAME, i->second));
			}
			break;
		case TABLEURL:
			i = data_->data.find("TINYURL");
			if (i != data_->data.end()) {
				data->data.insert(make_pair(FIELDTINYURL, i->second));
			}
			break;
		case TABLESTAGE:
			i = data_->data.find("UNIV");
			if (i != data_->data.end()) {
				data->data.insert(make_pair(FIELDUNIV, i->second));
			}
			break;
		default:
			MCE_WARN("UpdateTask::GetNewData -> Oh, no!");
	}
	return data;
}

void xce::usercache::UpdateTask::handle() {
	UserCacheUpdateNewDataPtr data = GetNewData();
	std::vector<std::pair<int, Ice::ObjectPtr> > d;
	d.push_back(std::pair<int, Ice::ObjectPtr>(data->userid, data));
	UpdateCallGuarantee::instance().DoInTime(d);
}

