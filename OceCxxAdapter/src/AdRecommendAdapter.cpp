/*
 * AdRecommendAdapter.cpp
 *
 *  Created on: 2010-11-24
 *      Author: sunzz
 */
#include "AdRecommendAdapter.h"

using namespace MyUtil;
using namespace xce::ad;


AdRecommendationHandlerPrx AdRecommendAdapter::getManager(int id) {
  return locate<AdRecommendationHandlerPrx> (_managers, "M", id, TWO_WAY, 250);
}

AdRecommendationHandlerPrx AdRecommendAdapter::getManagerOneway(int id) {
  return locate<AdRecommendationHandlerPrx> (_managersOneway, "M", id, ONE_WAY, 250);
}

//AdResult AdRecommendAdapter::GetAds(int userid) {
//  if(userid <= 0){
//    return AdResult();
//  }
//  return getManager((userid / 10) % cluster())->GetAds(userid);
//}
//void AdRecommendAdapter::Click(int userid, long groupid) {
//  if(userid <= 0){
//    return;
//  }
//  for (int i = 0; i < cluster(); i++) {
//    if (i == ((userid / 10) % cluster())) {
//      getManagerOneway(i)->ClickNew(userid, groupid, true);
//    } else {
//      getManagerOneway(i)->ClickNew(userid, groupid, false);
//    }
//  }
//}

bool AdRecommendAdapter::AddUserListL(int userid, Ice::Long zoneid, int sindex) {
  if(userid <= 0) {
    return false;
  }
  //return getManager(GetIndex(userid, zoneid))->AddUserListL(userid,sindex);
  return false;
}
bool AdRecommendAdapter::AddUserListH(int userid, Ice::Long zoneid, int sindex) {
  if(userid <= 0) {
    return false;
  }
  //return getManager(GetIndex(userid, zoneid))->AddUserListH(userid,sindex);
  return false;
}
void AdRecommendAdapter::CleanUserClick(int userid, Ice::Long zoneid) {
  if(userid <= 0) {
    return;
  }
  //getManagerOneway(GetIndex(userid, zoneid))->CleanUserClick(userid);
}

AdResult AdRecommendAdapter::GetAds(int userid, long zoneid) {
  if(userid <= 0) {
    return AdResult();
  }
  //return getManager(GetIndex(userid, zoneid))->GetAds(userid);
  return AdResult();
}

void AdRecommendAdapter::Click(int userid, long zoneid, long groupid) {
  if(userid <= 0){
    return;
  }
  /*int index = GetIndex(userid, zoneid);
  if (index < 5) {
    for (int i = 0; i < 5; i++) {
      if (i == index) {
        getManagerOneway(i)->ClickNew(userid, groupid, true);
      } else {
        getManagerOneway(i)->ClickNew(userid, groupid, false);
      }
    }
  } else {
    for (int i = 5; i < 10; i++) {
      if (i == index) {
        getManagerOneway(i)->ClickNew(userid, groupid, true);
      } else {
        getManagerOneway(i)->ClickNew(userid, groupid, false);
      }
    }
  }*/
}

int AdRecommendAdapter::GetIndex(int userid, long zoneid) {
  /*if (zoneid == 100000000001) {
    return (userid / 10) % (cluster() / 2);
  } else if (zoneid == 100000000063) {
    return ((userid / 10) % (cluster() / 2)) + 5;
  } else {
    return (userid / 10) % cluster();
  }*/
  return 0;
}





