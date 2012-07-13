/*
 * AdRecommendIAdapter.cpp
 *
 *  Created on: 2010-11-24
 *      Author: sunzz
 */
#include "AdRecommendIAdapter.h"

using namespace MyUtil;
using namespace xce::ad;


AdRecommendationHandlerPrx AdRecommendIAdapter::getManager(int id) {
  return locate<AdRecommendationHandlerPrx> (_managers, "M", id, TWO_WAY, 250);
}

AdRecommendationHandlerPrx AdRecommendIAdapter::getManagerOneway(int id) {
  return locate<AdRecommendationHandlerPrx> (_managersOneway, "M", id, ONE_WAY, 250);
}

bool AdRecommendIAdapter::AddUserListL(int userid, Ice::Long zoneid, int sindex) {
  if(userid <= 0) {
    return false;
  }
  //return getManager(0)->AddUserListL(userid,sindex);
  return false;
}
bool AdRecommendIAdapter::AddUserListH(int userid, Ice::Long zoneid, int sindex) {
  if(userid <= 0) {
    return false;
  }
  //return getManager(0)->AddUserListH(userid,sindex);
  return false;
}
void AdRecommendIAdapter::CleanUserClick(int userid, Ice::Long zoneid) {
  if(userid <= 0) {
    return;
  }
  //getManagerOneway(0)->CleanUserClick(userid);
  return;
}

AdResult AdRecommendIAdapter::GetAds(int userid, long zoneid) {
  //if(userid <= 0) {
    return AdResult();
  //}
  //return getManager(0)->GetAds(userid);
}

void AdRecommendIAdapter::Click(int userid, long zoneid, long groupid) {
  //if(userid <= 0){
    return;
  //}
  //getManagerOneway(0)->ClickNew(userid, groupid, ((userid % 10) == 9));
}
