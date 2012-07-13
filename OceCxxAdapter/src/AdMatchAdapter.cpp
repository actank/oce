/*
 * AdNotifyAdapter.cpp
 *
 *  Created on: Jul 2, 2010
 *      Author: yejingwei
 */

#include "AdMatchAdapter.h"
#include <boost/lexical_cast.hpp>
#include "FeedMemcProxy/client/user_profile_client.h"
using namespace MyUtil;
using namespace xce::ad;
using namespace boost;

AdMatchPrx AdMatchAdapter::getManager(int id) {
  return locate<AdMatchPrx> (_managers, "M", id, TWO_WAY);
}

AdMatchPrx AdMatchAdapter::getManagerOneway(int id) {
  return locate<AdMatchPrx> (_managersOneway, "M", id, ONE_WAY);
}

AdInfoSeq AdMatchAdapter::GetAds(string & user_profile, int count,const string & referer) {
  UserProfile profile;
  UserProfileClient::instance().Deserialize(user_profile, &profile);
  MCE_DEBUG("AdMatchAdapter::GetAds --> age = " << profile.age() <<
      ",stage = " << profile.stage() << ",grade = " << profile.grade() <<
      ",current_area = " << profile.current_area());
  return getManager(profile.age() % 10)->GetAds(user_profile, count,referer);
}

void AdMatchAdapter::Click(int uid, const string & ip, string & text,const string & referer,const string & click_url) {
  int i = lexical_cast<int> (text.substr(0, 1));
  getManagerOneway(i)->Click(uid, ip, text,referer,click_url);
}

//void AdMatchAdapter::Suspend(Ice::Long group_id) {
//  for (size_t i = 0; i < cluster(); ++i) {
//    getManagerOneway(i)->Suspend(group_id);
//  }
//}
//
//void AdMatchAdapter::Resume(Ice::Long group_id) {
//  for (size_t i = 0; i < cluster(); ++i) {
//    getManagerOneway(i)->Resume(group_id);
//  }
//}

void AdMatchAdapter::LoadOneGroup(Ice::Long group_id) {
  for (size_t i = 0; i < cluster(); ++i) {
    getManagerOneway(i)->LoadOneGroup(group_id);
  }
}
void AdMatchAdapter::ReloadOneGroup(Ice::Long group_id) {
  for (size_t i = 0; i < cluster(); ++i) {
    getManagerOneway(i)->ReloadOneGroup(group_id);
  }
}
void AdMatchAdapter::DelOneGroup(Ice::Long group_id) {
  for (size_t i = 0; i < cluster(); ++i) {
    getManagerOneway(i)->DelGroup(group_id);
  }
}


void AdMatchAdapter::GetAvailableAds(vector<Ice::Long> & ids) {
  for (size_t i = 0; i < cluster(); ++i) {
    getManager(i)->GetAvailableAds(ids);
  }
}

void AdMatchAdapter::ReportDefault(int uid,const string & ip,int count,const DefaultSeq & ads){
  getManagerOneway(5)->ReportDefault(uid,ip,count,ads);
}
