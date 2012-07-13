/*
 * AdRecommendationHandlerI.cpp
 *
 *  Created on: 2010-11-22
 *      Author: sunzz
 */
#include <Ice/Ice.h>
#include <QueryRunner.h>
#include "ServiceI.h"
#include "Date.h"
#include "AdCache.h"
#include "AdRecommendCache.h"
#include "AdRecommendationHandlerI.h"
#include "AdRecommendation.h"
using namespace com::xiaonei::xce;
using namespace xce::ad;

void MyUtil::initialize() {

  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AdRecommendationHandlerI::instance(),
      service.createIdentity("M", ""));
  MCE_DEBUG("the server is start");
  TaskManager::instance().execute(new InitTask());
  TaskManager::instance().scheduleRepeated(new ReloadTimer());
//  TaskManager::instance().scheduleRepeated(new CheckValidTimer());
}

AdResult AdRecommendationHandlerI::GetAds(int userid,
    const Ice::Current& current) {
  TimeStat ts;
  pair<AdResult, bool> result;
//  AdResult result;
  if (init_ok()) {
    result = AdRecommendCache::instance().GetAdResult(userid);

    MCE_DEBUG("[USEDTIME] AdRecommendationHandlerI::GetAds --> ~~~~~~~~~~~~~~~~~~~~~TIME_TEST ALL END  time = "
            << ts.getTime());
    if (result.second) {
      MCE_INFO("AdRecommendationHandlerI::GetAds get from cache -->user_id"
          << userid);
    } else {
      MCE_INFO("AdRecommendationHandlerI::GetAds cache no, to load -->user_id"
          << userid);
      LoaderManager::instance().AddUserId(userid);
    }
  }
  return result.first;
}

void AdRecommendationHandlerI::Click(int userid, long groupid,
    const Ice::Current& current) {
  MCE_INFO("AdRecommendationHandlerI::Click --> userid:" << userid
      << ", groupid:" << groupid);
  UserItemEvictor::instance().SetUserItemValue(userid, groupid);

  UpdateGidUid(userid, groupid);
  UpdateUidGid(userid, groupid);

}

bool AdRecommendationHandlerI::IsValidAd(long groupid,
    const Ice::Current& current) {
  bool flag;
  flag = AdGroupPool::instance().HasAdGroup(groupid);
  return flag;
}
UserSeq AdRecommendationHandlerI::GetInvalidList(
    const Ice::Current& current) {
  UserSeq result;
  result = AdRecommendCache::instance().GetInvalidList();
  return result;

}
AdSeq AdRecommendationHandlerI::GetUserItem(int userid,
    const Ice::Current& current) {
  AdSeq result;
  result = UserItemEvictor::instance().GetUserItemValue(userid);
  return result;
}

UserSeq AdRecommendationHandlerI::GetGidUser(long groupid,
    const Ice::Current& current) {
  UserSeq result;
  result = UserItemEvictor::instance().GetGidUser(groupid);
  return result;
}
float AdRecommendationHandlerI::GetUserDistance(int user1, int user2,
    const Ice::Current& current) {
  float result;
  result = UserItemEvictor::instance().GetUserSimValue(user1, user2);
  return result;
}

void AdRecommendationHandlerI::UpdateGidUid(int userid, long groupid) {
  Statement sql;
  sql << "SELECT * FROM ad_gid_uid where gids = " << groupid;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner("ads_db", CDbRServer, "ad_gid_uid").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("ClickTask::UpdateGidUid --> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ClickTask::UpdateGidUid --> error:" << e.what());
  }

  string result;
  if (res && res.num_rows() > 0) {
    for (int i = 0; i < res.num_rows(); i++) {
      result = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)(res[i]["userid"]);
      string str = boost::lexical_cast<string>(userid);
      size_t pos = result.find(str);
      if (pos != string::npos) {
        MCE_DEBUG("ClickTask::UpdateGidUid-->userid already exist gids:"
            << groupid << " userid:" << str);
        return;
      }
      result += ",";
      result += str;
    }
  } else {
    result = boost::lexical_cast<string>(userid);
    MCE_DEBUG("ClickTask::UpdateGidUid --> Gid not exist in DB gid:" << groupid
        << "userid:" << result);
  }
  Statement sqlresult;
  sqlresult << "insert into ad_gid_uid (gids, userid) values(" << groupid
      << "," << mysqlpp::quote << result << ")"
      << "on duplicate key update gids=values(gids),userid=values(userid)";
  try {
    QueryRunner("ads_db", CDbWServer, "ad_gid_uid").store(sqlresult);
  } catch (Ice::Exception& e) {
    MCE_WARN("ClickTask::UpdateGidUid Set--> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ClickTask::UpdateGidUid Set--> error:" << e.what());
  }
}
void AdRecommendationHandlerI::UpdateUidGid(int userid, long groupid) {
  Statement sql;
  sql << "SELECT * FROM ad_uid_gid where userid = " << userid;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner("ads_db", CDbRServer, "ad_uid_gid").store(sql);
  } catch (Ice::Exception& e) {
    MCE_WARN("ClickTask::UpdateUidGid Get--> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ClickTask::UpdateUidGid Get--> error:" << e.what());
  }

  string result;
  if (res && res.num_rows() > 0) {
    for (int i = 0; i < res.num_rows(); i++) {
      result = (mysqlpp::Null<string, mysqlpp::NullIsBlank>)(res[i]["gids"]);
      string str = boost::lexical_cast<string>(groupid);
      size_t pos = result.find(str);
      if (pos != string::npos) {
        MCE_DEBUG("ClickTask::UpdateUidGid-->gids already exist userid:"
            << userid << " gids:" << str);
        return;
      }
      result += ",";
      result += str;
    }
  } else {
    result = boost::lexical_cast<string>(groupid);
    MCE_DEBUG("ClickTask::UpdateUidGid Get--> userid not exist in DB userid:"
        << userid << "groupid:" << result);
  }
  Statement sqlresult;
  sqlresult << "insert into ad_uid_gid (userid, gids) values(" << userid << ","
      << mysqlpp::quote << result << ")"
      << "on duplicate key update userid=values(userid),gids=values(gids)";
  try {
    QueryRunner("ads_db", CDbWServer, "ad_uid_gid").store(sqlresult);
  } catch (Ice::Exception& e) {
    MCE_WARN("ClickTask::UpdateUidGid Set--> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ClickTask::UpdateUidGid Set--> error:" << e.what());
  }

}

void AdRecommendationHandlerI::Init() {
  MCE_INFO("InitTask::handle --> begin init :" << Date(time(NULL)).toDateTime());
  AdMemberPool::instance().Init();
  MCE_INFO("InitTask::handle --> AdMemberPool::instance().Init(); DONE");
  AdCampaignPool::instance().Init();
  MCE_INFO("InitTask::handle -->  AdCampaignPool::instance().Init(); DONE");
  AdGroupPool::instance().Init();
  MCE_INFO("InitTask::handle -->  AdGroupPool::instance().Init(); DONE");
  UserItemEvictor::instance().Init();
  MCE_INFO("InitTask::handle -->  UserItemEvictor::instance().Init(); DONE");
  set_init_ok(true);
}

/***********************************************************************************************************************/
void InitTask::handle() {
  AdRecommendationHandlerI::instance().Init();
}

void ReloadTimer::handle() {
  MCE_DEBUG("ReloadTimer::handle --> BEGIN RELOAD");
  AdMemberPool::instance().Init();
  MCE_INFO("ReloadTimer::handle --> AdMemberPool::instance().Init(); DONE");
  AdCampaignPool::instance().Init();
  MCE_INFO("ReloadTimer::handle -->  AdCampaignPool::instance().Init(); DONE");
  AdGroupPool::instance().Init();
  MCE_INFO("ReloadTimer::handle -->  AdGroupPool::instance().Init(); DONE");
  MCE_DEBUG("ReloadTimer::handle --> END RELOAD");
}
void CheckValidTimer::handle() {
  MCE_DEBUG("CheckValidTimer::handle --> BEGIN");
  vector<int> invalidlist;
  invalidlist = AdRecommendCache::instance().GetInvalidList();
  if(!invalidlist.empty()) {
    for(vector<int>::iterator it = invalidlist.begin(); it != invalidlist.end(); ++it) {
      LoaderManager::instance().AddUserId(*it);
    }
  }

  MCE_DEBUG("CheckValidTimer::handle --> DONE");
}

/***********************************************************************************************************************/
void RecommendLoader::AddUserId(int user_id) {
  if (!user_id) {
    return;
  }

  IceUtil::RWRecMutex::WLock lock(mutex_);
  pair<set<int>::iterator, bool> p = set_.insert(user_id);
  if (p.second) {
    list_.push_back(user_id);
  }
  MCE_DEBUG("[LOADER] RecommendLoader::AddUserId--> index = " << index_
      << ", set size = " << set_.size() << ", list size = " << list_.size()  << ", user_id" << user_id);
}


bool RecommendLoader::Load() {
  int u = 0;
  {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    if (list_.empty()) {
      return false;
    }
    u = list_.front();
    MCE_DEBUG("[LOADER] RecommendLoader::Load--> index = " << index_
        << ", set size = " << set_.size() << ", list size = " << list_.size() << ", user_id" << u);
  }

  execute(u);

  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    list_.pop_front();
    set_.erase(u);
  }
  return true;
}

void RecommendLoader::execute(int user_id) {
  MCE_DEBUG("[LOADER] RecommendLoader::execute--> index = " << index_
      << ", user_id" << user_id);
  AdResult result = UserItemEvictor::instance().GetRecommendation(user_id);
  //  if (!result.empty()) {
  RecommendPtr recommend = new Recommend();
  recommend->set_ad_result(result);
  AdRecommendCache::instance().insert(user_id, recommend);
  //  }
}

