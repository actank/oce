/*
 * =====================================================================================
 *
 *       Filename:  FeedItemWrapperI.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011年11月10日 12时10分56秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  guanghe.ge (ggh), guanghe.ge@opi-corp.com
 *        Company:  XiaoNei
 *
 * =====================================================================================
 */

#include "FeedItemWrapperI.h"
#include "ServiceI.h"
#include "IceLogger.h"
#include "util/cpp/TimeCost.h"
#include "util/cpp/InvokeMonitor.h"
#include "BuddyByOnlineTimeReplicaAdapter.h"
#include "FeedItemCacheByUseridReplicaAdapter.h"
#include "ReplicaCluster/ServerState/src/client/cxx/ServerStateSubscriber.h"

#include "BuddyGroupAdapter.h"
#include "FeedFocusAdapter.h"

using namespace xce::feed;
using namespace xce::serverstate;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace mop::hi::svc::adapter;
using namespace xce::buddy::adapter; 

void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&FeedItemWrapperI::instance(), service.createIdentity("M", ""));

	TaskManager::instance().config(TASK_LEVEL_PRELOAD, ThreadPoolConfig(1,1));

	service.registerObjectCache(USER_TYPE, "UT", new UserTypeFactoryI, false);
	service.registerObjectCache(USER_SHIELD, "US", new UserShieldFactoryI, false);

	//controller
	int mod = service.getCommunicator()->getProperties()->getPropertyAsIntWithDefault("FeedItemWrapper.Mod", 0);
	int interval = 5;
	string controller = "ControllerFeedItemWrapper";
	ServerStateSubscriber::instance().initialize(controller, &FeedItemWrapperI::instance(), mod, interval);

	TaskManager::instance().execute(new BuildUserTypeTask());
}

FeedItemWrapperI::FeedItemWrapperI() {
}

FeedItemWrapperI::~FeedItemWrapperI() {
}

FeedMetaSeq FeedItemWrapperI::get(::Ice::Int userid, ::Ice::Int begin, ::Ice::Int limit, const ::Ice::Current& current) {
	ostringstream oss;
	oss << "userid: " << userid << " begin: " << begin << " limit: " << limit;
	InvokeClient id = InvokeClient::create(current, oss.str(), InvokeClient::INVOKE_LEVEL_INFO);


	FeedMetaSeq result;
	try {
		ostringstream oss;
		oss << "FeedItemWrapperI::get, userid: " << userid;
		TimeCost tm = TimeCost::create(oss.str());

		IntSeq friends = xce::buddy::adapter::BuddyByOnlineTimeCacheAdapter::instance().getFriendList(userid, -1);

    mysqlpp::StoreQueryResult res;
    
    res = getPages(userid);
    for (size_t i = 0; i < res.size(); ++i) {
      int target = res.at(i)["page_id"];
      friends.push_back(target);
    }
    MCE_INFO(" FeedItemWrapperI::get --> friends size: " << friends.size() 
            << "page size:" << res.size());     

    res = getMinisite(userid);
    for (size_t i = 0; i < res.size(); ++i) {
      int target = res.at(i)["site_id"];
      friends.push_back(target);
    }
    MCE_INFO(" FeedItemWrapperI::get --> friends size: " << friends.size() 
            << "minisite size:" << res.size());     

		ostringstream oss1;
		oss1 << "step 1: get friends size: " << friends.size();
		tm.step(oss1.str());

		StrSeq filters;

	  IntSeq types = getUserType(userid);
		for (IntSeq::iterator it = types.begin();
				it != types.end(); ++it) {
			ostringstream oss;
			oss << TypePrefix << ":" << (long)(*it);
			filters.push_back(oss.str());
		}

		ostringstream oss2;
		oss2 << "step 2: get UserType size: " << types.size();
		tm.step(oss2.str());

		IntSeq shields = getUserShield(userid);
		for (IntSeq::iterator it = shields.begin();
				it != shields.end(); ++it) {
			ostringstream oss;
			oss << UseridPrefix << ":" << (long)(*it);
			filters.push_back(oss.str());
		}
	
		ostringstream oss3;
		oss3 << "step 3: get shields size: " << shields.size();
		tm.step(oss3.str());

		IntSeq type_filters = getFixedType();

		result = FeedItemCacheByUseridReplicaAdapter::instance().getByTypesWithFilter(friends, begin, limit, type_filters, filters);

		ostringstream oss4;
		oss4 << "step 4: get FeedItem size: " << result.size();
		tm.step(oss4.str());
	} catch (...) {
		MCE_WARN("FeedItemWrapperI::get, userid: "<< userid << " begin: " << begin << " limit: " << limit << " throw Exception!");
	}
	return result;
}

FeedMetaSeq FeedItemWrapperI::GetFeeds(::Ice::Int userid, ::Ice::Int begin, ::Ice::Int limit,
         const MyUtil::IntSeq& types, const MyUtil::IntSeq& ntypes, const ::Ice::Current& current) {
  FeedMetaSeq result;
  try {
  
    IntSeq friends = BuddyByOnlineTimeCacheAdapter::instance().getFriendList(userid, -1);
    
    mysqlpp::StoreQueryResult res;
    
    res = getPages(userid);
    for (size_t i = 0; i < res.size(); ++i) {
      int target = res.at(i)["page_id"];
      friends.push_back(target);
    }
    MCE_INFO(" FeedItemWrapperI::getfeeds --> friends size: " << friends.size() 
            << "page size:" << res.size());     

    res = getMinisite(userid);
    for (size_t i = 0; i < res.size(); ++i) {
      int target = res.at(i)["site_id"];
      friends.push_back(target);
    }
    MCE_INFO(" FeedItemWrapperI::getfeeds --> friends size: " << friends.size() 
            << "minisite size:" << res.size());     

    StrSeq filters = createFilter(userid, ntypes);
    
    if (types.empty()) { // if types is empty, put in all fixedtypes
      result = FeedItemCacheByUseridReplicaAdapter::instance().getWithFilter(friends, begin, limit, filters);
      MCE_INFO(" FeedItemWrapperI::GetFeedd: getfeeds only by filters");
    } else {
      result = FeedItemCacheByUseridReplicaAdapter::instance().getByTypesWithFilter(friends, begin, limit, types, filters);
    }
		
    MCE_INFO("FeedItemWrapperI::GetFeeds,  friend size:" 
          << friends.size() << " res size: "<< result.size() << " success!");
  
  } catch (...){
		MCE_WARN("FeedItemWrapperI::GetFeeds, userid: "<< userid << " begin: " 
          << begin << " limit: " << limit << " throw Exception!");  
    }
  return result;
}

FeedMetaSeq FeedItemWrapperI::GetFeedsByGroupId(::Ice::Int userid, ::Ice::Int group, 
        ::Ice::Int begin, ::Ice::Int limit, const IntSeq& types, const IntSeq& ntypes, const ::Ice::Current&) {
//TODO
  MCE_WARN(" FeedItemWrapperI::GetFeedsByGroupId: did not support for the momet! also see GetFeedsByGroupName");
  return FeedMetaSeq();
}

FeedMetaSeq FeedItemWrapperI::GetFeedsByGroupName(::Ice::Int userid, const string& name, 
        ::Ice::Int begin, ::Ice::Int limit, const IntSeq& types, const IntSeq& ntypes, const ::Ice::Current&) {
  FeedMetaSeq result;
  try {
    IntSeq friends = BuddyGroupAdapter::instance().getBuddyList(userid, name, begin, -1);
    
    StrSeq filters = createFilter(userid, ntypes);
    
    if (types.empty()) { // if types is empty, put in all fixedtypes
      result = FeedItemCacheByUseridReplicaAdapter::instance().getWithFilter(friends, begin, limit, filters);
      MCE_INFO(" FeedItemWrapperI::GetFeedByGruopName: getfeeds only by filters");
    } else {
      result = FeedItemCacheByUseridReplicaAdapter::instance().getByTypesWithFilter(friends, begin, limit, types, filters);
    }

    MCE_INFO("FeedItemWrapperI::GetFeedByGroupName, friens size: "<< friends.size() 
          <<"  result size: " << result.size() << " success!");
 
  } catch (...){
		MCE_WARN("FeedItemWrapperI::GetFeedByGroupName, userid: "<< userid << " begin: " 
          << begin << " limit: " << limit << " throw Exception!");  
   }
 return result;
}

FeedMetaSeq FeedItemWrapperI::GetFeedsByFocus(int userid, int begin, int limit, const IntSeq& types, 
      const IntSeq& ntypes, const ::Ice::Current& current) {
  FeedMetaSeq result;
  try {
    IntSeq friends = FeedFocusAdapter::instance().GetFocuses(userid);
   
    StrSeq filters = createFilter(userid, ntypes);
    
    if (types.empty()) { // if types is empty, put in all fixedtypes
      result = FeedItemCacheByUseridReplicaAdapter::instance().getWithFilter(friends, begin, limit, filters);
      MCE_INFO(" FeedItemWrapperI::GetFeedByFocus: getfeeds only by filters");
    } else {
      result = FeedItemCacheByUseridReplicaAdapter::instance().getByTypesWithFilter(friends, begin, limit, types, filters);
    }

		result = FeedItemCacheByUseridReplicaAdapter::instance().getByTypesWithFilter(friends, begin, limit, types, filters);
		
    MCE_INFO("FeedItemWrapperI::GetFeedsByFocus, friend size:" 
          << friends.size() << "<< res size: "<< result.size() << " success!");
  
  } catch (...){
		MCE_WARN("FeedItemWrapperI::GetFeedsByFocus, userid: "<< userid << " begin: " 
          << begin << " limit: " << limit << " throw Exception!");  
    }
  return result;
}

mysqlpp::StoreQueryResult FeedItemWrapperI::getPages(int userid) {

  ostringstream pattern;
  mysqlpp::StoreQueryResult  res;
  Statement sql;

  pattern << "fans_pages_" << userid % 100;

  sql
    << "SELECT page_id FROM fans_pages_" << userid % 100
    << " WHERE user_id = " << userid;

  try {
    res = QueryRunner("fans_pages", CDbRServer, pattern.str()).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" FeedItemWrapperI::getPages --> error: " << e);
  } catch (std::exception& e) {
    MCE_WARN(" FeedItemWrapperI::getPages --> error: " << e.what());    
  }

  if (!res) {
    MCE_WARN(" FeedItemWrapperI::getPages --> StoryQueryWrong  @1 !! ");     
  }
  if (res.empty()) {
    MCE_WARN(" FeedItemWrapperI::getPages --> no funs_pages_id!! ");     
  }

  return res;
}

mysqlpp::StoreQueryResult FeedItemWrapperI::getMinisite(int userid) {

  mysqlpp::StoreQueryResult  res;
  Statement sql;
  sql
    << "SELECT site_id FROM minisite_follower"
    << " WHERE follower_id = " << userid;

  try {
    res = QueryRunner("minisite", CDbRServer).store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN(" FeedItemWrapperI::getMinisite --> error: " << e);
  } catch (std::exception& e) {
    MCE_WARN(" FeedItemWrapperI::getMinisite --> error: " << e.what());    
  }

  if (!res) {
    MCE_WARN(" FeedItemWrapperI::getMinisite --> StoryQueryWrong  @2 !! ");     
  }
  if (res.empty()) {
    MCE_WARN(" FeedItemWrapperI::getMinisite --> no minisite_id!! ");     
  }

  return res;
}

MyUtil::StrSeq FeedItemWrapperI::createFilter(int userid, IntSeq ntypes) {

  StrSeq filters;
  IntSeq types = getUserType(userid);
  for (IntSeq::iterator it = types.begin();
      it != types.end(); ++it) {
    ostringstream oss;
    oss << TypePrefix << ":" << (long)(*it);
    filters.push_back(oss.str());
  }

  for (IntSeq::iterator it = ntypes.begin();
      it != ntypes.end(); ++it) {
    ostringstream oss;
    oss << TypePrefix << ":" << (long)(*it);
    filters.push_back(oss.str());
  }

  IntSeq shields = getUserShield(userid);
  for (IntSeq::iterator it = shields.begin();
      it != shields.end(); ++it) {
    ostringstream oss;
    oss << UseridPrefix << ":" << (long)(*it);
    filters.push_back(oss.str());
  }

  return filters; 
}

MyUtil::IntSeq FeedItemWrapperI::getUserType(int userid) {
	UserTypePtr obj = ServiceI::instance().findObject<UserTypePtr>(USER_TYPE, userid);

	if (obj) {
		return obj->get();
	}

	return IntSeq();
}


MyUtil::IntSeq FeedItemWrapperI::getUserShield(int userid) {//blacksheet
	UserShieldPtr obj = ServiceI::instance().locateObject<UserShieldPtr>(USER_SHIELD, userid);
	if (obj) {
		return obj->get();
	}

	return IntSeq();
}

MyUtil::IntSeq FeedItemWrapperI::getFixedType() {//all kinds of types:page group....
	IntSeq result;
	int type_size = sizeof(TYPE)/sizeof(TYPE[0]);
	result.reserve(type_size);
	for (int i = 0; i < type_size; i++) {
		result.push_back(TYPE[i]);
	}
	return result;
}

void BuildUserTypeTask::handle() {
	int interval = 10000;
	for (int begin = 6453051, end = begin + interval;; begin = end, end += interval) {

		ostringstream oss;
		oss << "BuildUserTask::handle, begin: " << begin << " end: " << end;
		TimeCost tm = TimeCost::create(oss.str());

		Statement sql;
		sql << "SELECT * FROM feed_recv_config WHERE id > " << begin << " AND id <= " << end;
		mysqlpp::StoreQueryResult res;
		try {
			res = QueryRunner("feed_db", CDbRServer, "feed_recv_config").store(sql);
		} catch (...) {
			continue;
		}
		if (res.empty()) break;

		ostringstream oss1;
		oss1 << "step 1: res.size: " << res.size();

		for (uint i = 0; i < res.size(); ++i) {
			int userid = (int) res.at(i)["uid"];
			int type = (int) res.at(i)["feed_type"];

			UserTypePtr obj = ServiceI::instance().locateObject<UserTypePtr>(USER_TYPE, userid);
			if (obj) {
				obj->put(type);
			}
		}

	}
}

Ice::ObjectPtr UserShieldFactoryI::create(int userid) {
	Statement sql;
	sql << "SELECT source FROM relation_feed_low WHERE target= " << userid;
	mysqlpp::StoreQueryResult res;
	try {
		res = QueryRunner("feedwb", CDbRServer).store(sql);
	} catch (mysqlpp::Exception& e) {
		MCE_WARN("UserShieldFactoryI::create, userid: " << userid << e.what());
	} catch (...) {
		MCE_WARN("UserShieldFactoryI::create, userid: " << userid << " UNKNOWN EXCEPTION!");
	}
	
	UserShieldPtr shield = new UserShield;
	for (mysqlpp::StoreQueryResult::iterator it = res.begin();
			it != res.end(); ++it) {
		int id = (int)(*it)["source"];
		shield->put(id);
	}

	return shield;
}
