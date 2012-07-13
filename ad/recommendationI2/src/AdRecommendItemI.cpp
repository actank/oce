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
#include "AdStruct.h"
//#include "AdRecommendCache.h"
#include "client/linux/handler/exception_handler.h"
#include "AdRecommendItemI.h"
#include "AdRecommendation.h"
#include "AdRecommendCacheI.h"
using namespace com::xiaonei::xce;
using namespace xce::ad;


bool MyCallback(const char *dump_path, const char *minidump_id, void *context, bool succeeded) {
  printf("got crash %s %s\n", dump_path, minidump_id);
  return true;
}
void MyUtil::initialize() {
  static google_breakpad::ExceptionHandler eh(".", 0, MyCallback, 0, true);
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AdRecommendationHandlerI::instance(),
      service.createIdentity("M", ""));
  MCE_DEBUG("the server is start");
  TaskManager::instance().config(TASK_LEVEL_CALCULATE, MyUtil::ThreadPoolConfig(1, 10));
  TaskManager::instance().execute(new InitTask());
  TaskManager::instance().scheduleRepeated(new ReloadTimer());
//  TaskManager::instance().scheduleRepeated(new UpdateKNNTimer());

  //TaskManager::instance().scheduleRepeated(new CheckValidTimer());
}

AdResultE AdRecommendationHandlerI::GetRecommend(int userid,
    const Ice::Current& current) {
  TimeStat ts;
//  AdResult result;
  AdResultE result;
  if (init_ok()) {
    result = AdRecommendCacheI::instance().GetAdResult(userid);

    MCE_INFO("AdRecommendationHandlerI::GetRecommend-->userid:" << userid << ",result size:" << result.size());

    return result;
  }
}

AdResult AdRecommendationHandlerI::GetAds(int userid,const Ice::Current& current) {
  TimeStat ts;
//  AdResult result;
  AdResultE resultE;
  AdResult result;
  if (init_ok()) {
    resultE = GetRecommend(userid);

    MCE_INFO("AdRecommendationHandlerI::GetAds-->userid:" << userid << ",result size:" << resultE.size());
  }
  ////////////////transform to AdResultE to AdResult
  for (map<Ice::Long, double>::iterator it = resultE.begin(); it != resultE.end(); ++it) {
        //MCE_INFO("AdRecommendationHandlerI::GetAds-->userid:"<<userid <<" result Adid:" << it->first << " value:" << it->second);
    result.insert(pair<Ice::Long, float>(it->first, (float)(it->second)));
  }

  return result;
}


bool AdRecommendationHandlerI::IsValidAd(Ice::Long groupid,
    const Ice::Current& current) {
  bool flag;
//      flag = AdGroupPool::instance().HasAdGroup(groupid);
  return flag;
}
UserSeq AdRecommendationHandlerI::GetInvalidList(
    const Ice::Current& current) {
  UserSeq result;
     // result = AdRecommendCache::instance().GetInvalidList();
  return result;

}
AdSeq AdRecommendationHandlerI::GetUserItem(int userid,
    const Ice::Current& current) {
  AdSeq result;
//      result = UserItemPool::instance().GetUserItemValue(userid);
  return result;
}

UserSeq AdRecommendationHandlerI::GetGidUser(Ice::Long groupid,
    const Ice::Current& current) {
  UserSeq result;
//      result = UserItemPool::instance().GetGidUser(groupid);
  return result;
}
double AdRecommendationHandlerI::GetUserDistance(int user1, int user2,
    const Ice::Current& current) {
  double result;
//      result = AdRecommend::instance().GetUserDistance(user1, user2);
  return result;
}
bool AdRecommendationHandlerI::AddUserListH(int userid, int sindex,
    const Ice::Current& current) {
  MCE_INFO("AdRecommendationHandlerI::AddUserListH-->userid:" << userid << ",server index:" <<sindex);
  bool success = true;
//        success = AdRecommendLoader::instance().AddUserListH(userid);
  return success;
}
bool AdRecommendationHandlerI::AddUserListL(int userid, int sindex,
    const Ice::Current& current) {
  MCE_INFO("AdRecommendationHandlerI::AddUserListL-->userid:" << userid << ",server index:" <<sindex);
  bool success = true;
//        success = AdRecommendLoader::instance().AddUserListL(userid);
  return success;
}
void AdRecommendationHandlerI::CleanUserClick(int userid, const Ice::Current& current) {
 //   MCE_INFO("AdRecommendationHandlerI::CleanUserClick-->userid:" << userid );
//      UserItemPool::instance().CleanClick(userid);

}
/*AdResult AdRecommendationHandlerI::GetAds(int userid,
    const Ice::Current& current) {
  TimeStat ts;
  AdResult ad_result;
  map<long,double> result;
  if (init_ok()) {
    result = ItemUsersPool::instance().GetRecommendation(userid);
    MCE_DEBUG("[USEDTIME] AdRecommendationHandlerI::GetAds --> ~~~~~~~~~~~~~~~~~~~~~TIME_TEST ALL END  time = "
            << ts.getTime());
  }

  for(map<long,double>::iterator it = result.begin(); it != result.end(); ++it) {
    ad_result.insert(pair<long,float>(it->first, (float)(it->second)));
  }
  

  return ad_result;
}*/

AdKNN AdRecommendationHandlerI::GetKNN(Ice::Long groupid,
    const Ice::Current& current) {
  int cluster = groupid;
  AdKNN result;
//  map<Ice::Long,double> tmp;
  MCE_INFO("RecommendationHandlerI::GetKNN-->gid:" << groupid);
  map<int,double>r = ItemUsersPool::instance().GetKNNItem(cluster);
  for(map<int,double>::iterator it = r.begin(); it != r.end(); ++it) {
    result.insert(pair<Ice::Long,double>((Ice::Long)(it->first), it->second));
  }
  return result;
}





void AdRecommendationHandlerI::Click(int userid, Ice::Long groupid,
    const Ice::Current& current) {

  MCE_INFO("AdRecommendationHandlerI::Click --> userid:" << userid
      << ", groupid:" << groupid);
  if (init_ok()) {
    ItemUsersPool::instance().UpdateClick(userid, groupid,-1);
  }

  RecentClickCache::instance().Click(userid,groupid);
  AdRecommendCacheI::instance().DisposeClick(userid, groupid);

//  TaskManager::instance().execute(new UpdateGidUidTask(userid, groupid));
//  TaskManager::instance().execute(new UpdateUidGidTask(userid, groupid));
//  UpdateGidUid(userid, groupid);
//  UpdateUidGid(userid, groupid);

}

void AdRecommendationHandlerI::ClickNew(int userid, Ice::Long groupid, bool flag,
    const Ice::Current& current) {
  MCE_INFO("AdRecommendationHandlerI::ClickNew --> userid:" << userid
      << ", groupid:" << groupid);

  AdGroupPtr group = AdGroupPool::instance().GetAdGroup(groupid);
  if (!group) {
    MCE_INFO("AdRecommendationHandlerI::ClickNew-->err, groupid hasn't insert AdCache,userid:"
        <<userid << ",gid:" <<groupid);
    return;
  }
  bool isbrand = group->IsBrand();
  if (init_ok()) {
    MCE_INFO("AdRecommendationHandlerI::ClickNew --> userid:" << userid
          << ", groupid:" << groupid <<",gidtype:" << isbrand);
    RecentClickCache::instance().Click(userid, groupid);
    ItemUsersPool::instance().UpdateClick(userid, groupid, isbrand);
    if(!isbrand) {
      AdRecommendCacheI::instance().DisposeClick(userid, groupid);
    }

    if(flag == true) {
      MCE_INFO("AdRecommendationHandlerI::ClickNew call AdRecommendCacheI.ReCalculate, userid:"
          <<userid <<",groupid:" << groupid);
      AdRecommendCacheI::instance().ReCalculate(userid);
  //    TaskManager::instance().execute(new UpdateUidGidTask(userid, groupid));
    }
  }



  // 更新数据库
//  TaskManager::instance().execute(new UpdateGidUidTask(userid, groupid));



}

/*bool AdRecommendationHandlerI::IsValidAd(Ice::Long groupid,
    const Ice::Current& current) {
  bool flag;
  flag = AdGroupPool::instance().HasAdGroup(groupid);
  return flag;
}
UserSeq AdRecommendationHandlerI::GetInvalidList(
    const Ice::Current& current) {
  UserSeq result;
  //result = AdRecommendCache::instance().GetInvalidList();
  return result;

}
AdSeq AdRecommendationHandlerI::GetUserItem(int userid,
    const Ice::Current& current) {
  AdSeq result;
  //result = ItemUsersPool::instance().GetUserItemValue(userid);
  return result;
}

UserSeq AdRecommendationHandlerI::GetGidUser(Ice::Long groupid,
    const Ice::Current& current) {
  UserSeq result;
  result = ItemUsersPool::instance().GetGidUser(groupid);
  return result;
}
double AdRecommendationHandlerI::GetUserDistance(int user1, int user2,
    const Ice::Current& current) {
  double result;
 // result = ItemUsersPool::instance().GetUserSimValue(user1, user2);
  return result;
}*/

void UpdateGidUidTask::UpdateGidUid(int userid, Ice::Long groupid) {
  Statement sql;
  sql << "SELECT * FROM ad_gid_uid_inpage where gids = " << groupid;
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner("ads_db", CDbRServer, "ad_gid_uid_inpage").store(sql);
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
  sqlresult << "insert into ad_gid_uid_inpage (gids, userid) values(" << groupid
      << "," << mysqlpp::quote << result << ")"
      << "on duplicate key update gids=values(gids),userid=values(userid)";
  try {
    QueryRunner("ads_db", CDbWServer, "ad_gid_uid_inpage").store(sqlresult);
  } catch (Ice::Exception& e) {
    MCE_WARN("ClickTask::UpdateGidUid Set--> error:" << e);
  } catch (std::exception& e) {
    MCE_WARN("ClickTask::UpdateGidUid Set--> error:" << e.what());
  }
}
void UpdateUidGidTask::UpdateUidGid(int userid, Ice::Long groupid) {
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
  AdZonePool::instance().Init();
  MCE_INFO("InitTask::handle -->  AdZonePool::instance().Init(); DONE");
  ItemUsersPool::instance().Init();
  MCE_INFO("InitTask::handle -->  ItemUsersPool::instance().Init(); DONE");
//  ItemUsersPool::instance().UpdateKNNItem();
//  MCE_INFO("InitTask::handle -->  ItemUsersPool::instance().UpdateKNNTimer(); DONE");
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
  AdZonePool::instance().Init();
  MCE_INFO("ReloadTimer::handle -->  AdZonePool::instance().Init(); DONE");
  MCE_DEBUG("ReloadTimer::handle --> END RELOAD");
}



/////////////////////////////////////////////////////////////////////////////////////////////
int AdRecommendLoader::GetUserId() {
  int uid = 0;

  IceUtil::Mutex::Lock lock(mutex_);
  if (listH_.empty() && listL_.empty()) {
    return 0;
  }
  if(!listH_.empty()) {
    uid = listH_.front();
    listH_.pop_front();
    MCE_INFO("AdRecommendLoader::GetUserId Load listH --> uid = " << uid << ", set size = " << set_.size());
    return uid;
  }
  if(!listL_.empty()) {
    uid = listL_.front();
    listL_.pop_front();
    MCE_INFO("AdRecommendLoader::GetUserId Load listL--> uid = " << uid << ", set size = " << set_.size());
    return uid;
  }

  return uid;
}
bool AdRecommendLoader::AddUserListH(int userid) {
  if (userid <= 0) {
    return false;
  }
//  UpdateServerIndex(userid, sindex);
 // IceUtil::RWRecMutex::WLock lock(mutex_);
  IceUtil::Mutex::Lock lock(mutex_);

  pair<set<int>::iterator, bool> p = set_.insert(userid);
  if (p.second) {
    listH_.push_back(userid);
  } else {
    list<int>::iterator it = find(listL_.begin(), listL_.end(), userid);
    if(it != listL_.end()) {
      MCE_INFO("AdRecommendLoader::AddUserListH-->change priority from H to L,userid:" <<userid);
      listH_.push_back(userid);
      listL_.erase(it);
    }
  }

  //debug
  //PrintList("AdRecommendLoader::AddUserListH");


  MCE_INFO("[INSERT] AdRecommendLoader::AddUserId--> set size = " << set_.size() << ", listH size = " << listH_.size()  << ", userid:" << userid);
  return true;
}
bool AdRecommendLoader::AddUserListL(int userid) {
  if(userid > 0) {
//    UpdateServerIndex(userid, sindex);

 //   IceUtil::RWRecMutex::WLock lock(mutex_);
    bool flag = true;
    IceUtil::Mutex::Lock lock(mutex_);
    if(listL_.size() > WaitListNum) {
      flag = false;
    //  return false;
    } else {
      pair<set<int>::iterator, bool> p = set_.insert(userid);
      if(p.second) {
        listL_.push_back(userid);
        //PrintList("AdRecommendLoader::AddUserListL");
        //MCE_INFO("[INSERT] AdRecommendLoader::AddUserListL set size = " << set_.size() << ", listL size = " << listL_.size()  << ", userid:" << userid);
        //PrintList("AdRecommendLoader::AddUserListL");
       // return true;
      }
      flag = true;
    }
   // PrintList("AdRecommendLoader::AddUserListL");
    MCE_INFO("[INSERT] AdRecommendLoader::AddUserListL set size = " << set_.size() << ", listL size = " << listL_.size()  << ", userid    :" << userid);
    return flag;
  }
}
void AdRecommendLoader::PrintList(string callname) {
  {
    string result;
    for(list<int>::iterator it = listH_.begin(); it != listH_.end(); ++it) {
      result += boost::lexical_cast<string>(*it);
      result += ",";
    }
    MCE_DEBUG("AdRecommendLoader::PrintList-->:"<< callname<<" after insert,listH:" << result <<" set size:" << set_.size() << ",listH size:" <<listH_.size());
  }
  {
    string result;
    for(list<int>::iterator it = listL_.begin(); it != listL_.end(); ++it) {
      result += boost::lexical_cast<string>(*it);
      result += ",";
    }
    MCE_DEBUG("AdRecommendLoader::PrintList-->:"<< callname<<" after insert,listL:" << result<<" set size:" << set_.size() << ",listL size:" <<listL_.size());
  }
}

void AdRecommendLoader::execute(int userid) {
 // MCE_INFO("[LOADER] AdRecommendLoader::execute-->userid:" << userid <<",listH size:" <<listH_.size() << ",listL size:" << listL_.size() <<",set size:" << set_.size());
//  AdResult result = AdRecommend::instance().GetRecommendation(userid);

  //  if (!result.empty()) {
//  int sindex = GetServerIndex(userid);
  /////todo 上线要删除
//  for (map<Ice::Long, double>::iterator it = result.begin(); it != result.end(); ++it) {
//      MCE_INFO("AdRecommendLoader::execute to insert-->userid:"<<userid <<" result Adid:" << it->first << " value:" << it->second);
//    }
  try {
  //  AdResultE result = AdRecommend::instance().GetRecommendation(userid);
    AdResultE result = ItemUsersPool::instance().GetRecommendation(userid);
    MCE_INFO("[LOADER] AdRecommendCacheI.Insert :  userid =" << userid << " result size:" << result.size());
    AdRecommendCacheI::instance().Insert(userid, result);
  } catch (Ice::Exception& e) {
    MCE_WARN("[LOADER] AdRecommendCacheI.Insert error : " << e
        << " userid =" << userid );
  } catch (std::exception& e) {
    MCE_WARN("[LOADER] AdRecommendCacheI.Insert  error : " << e.what()
        << " userid =" << userid );
  }
  {
    IceUtil::Mutex::Lock lock(mutex_);
    set_.erase(userid);
  //  PrintList("after AdRecommendLoader::execute:");

    MCE_INFO("[LOADER] AdRecommendLoader::execute-->userid:" << userid <<",listH size:" <<listH_.size() << ",listL size:" << listL_.size() <<",set size:" << set_.size());

  }

  //  }
}




