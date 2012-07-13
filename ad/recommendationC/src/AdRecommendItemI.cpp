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

AdKNN AdRecommendationHandlerI::GetKNN(Ice::Long groupid,
    const Ice::Current& current) {
  AdKNN result;
  map<Ice::Long,double> tmp;
  MCE_INFO("RecommendationHandlerI::GetKNN-->gid:" << groupid);
//  result = ItemUsersPool::instance().GetKNNItem(groupid);
  return result;
}





void AdRecommendationHandlerI::Click(int userid, Ice::Long groupid,
    const Ice::Current& current) {

  MCE_INFO("AdRecommendationHandlerI::Click --> userid:" << userid
      << ", groupid:" << groupid);
  if (init_ok()) {
//    ItemUsersPool::instance().UpdateClick(userid, groupid,-1);
  }

  AdRecommendCacheI::instance().DisposeClick(userid, groupid);


}

void AdRecommendationHandlerI::ClickNew(int userid, Ice::Long groupid, bool flag,
    const Ice::Current& current) {

}


void AdRecommendationHandlerI::Init() {
  MCE_INFO("InitTask::handle --> begin init :" << Date(time(NULL)).toDateTime());
  AdMemberPool::instance().Init();
  MCE_INFO("InitTask::handle --> AdMemberPool::instance().Init(); DONE");
  AdCampaignPool::instance().Init();
  MCE_INFO("InitTask::handle -->  AdCampaignPool::instance().Init(); DONE");
  AdGroupPool::instance().Init();
  MCE_INFO("InitTask::handle -->  AdGroupPool::instance().Init(); DONE");
  UserUgcPool::instance().Init();
  MCE_INFO("InitTask::handle -->  UserUgcPool::instance().Init(); DONE");
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
  MCE_INFO("[INSERT] AdRecommendLoader::AddUserId--> set size = " << set_.size() << ", listH size = " << listH_.size()  << ", userid:" << userid);
  return true;
}
bool AdRecommendLoader::AddUserListL(int userid) {
  if(userid > 0) {
    bool flag = true;
    IceUtil::Mutex::Lock lock(mutex_);
    if(listL_.size() > WaitListNum) {
      flag = false;
    } else {
      pair<set<int>::iterator, bool> p = set_.insert(userid);
      if(p.second) {
        listL_.push_back(userid);
      }
      flag = true;
    }
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

  try {
  //  AdResultE result = AdRecommend::instance().GetRecommendation(userid);
    AdResultE result = UserUgcPool::instance().GetRecommendation(userid);
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

}




