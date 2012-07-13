/*
 * AdBlindness.cpp
 *
 *  Created on: 2011-4-18
 *      Author: sunzz
 */
#include "IceLogger.h"
#include "AdBlindness.h"
#include "AdConfig.h"
namespace xce{
namespace ad{
void AdBlindness::AddUserData(int userid, Ice::Long group_id, bool pv_flag) {

  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex & index = evictor_.get<1> ();
  UidIndex::iterator uit = index.find(userid);
  if (uit != index.end()) {
    if(pv_flag) {
      (*uit)->AddPv(group_id);
    } else {
      (*uit)->AddClick(group_id);
    }
    SeqIndex::iterator sit = evictor_.project<0> (uit);//更新之后放到前面
    evictor_.relocate(evictor_.get<0> ().begin(), sit);
    return;
  }
  //如果不在cache中，产生新的
  UserPtr upt = new User();
  upt->user_id_ = userid;
  if(pv_flag) {
    upt->AddPv(group_id);
  } else {
    upt->AddClick(group_id);
  }
  SeqIndex & sindex = evictor_.get<0> ();
  pair<SeqIndex::iterator, bool> p = evictor_.push_front(upt); //用户提前
  if (!p.second) { //
    sindex.relocate(sindex.begin(), p.first);
  } else { //用户逐出
    while (evictor_.size() > kUserSize) {//判定是否要逐出用户
      MCE_INFO("AdBlindness::AddClick:userid:" <<userid <<",evictor size:" << evictor_.size()<<"evictor userid:"<<(*(evictor_.rbegin()))->user_id_);
      evictor_.pop_back();
    }
  }
  MCE_DEBUG("AdBlindness::AddClick:userid:" <<userid <<",evictor size:" << evictor_.size());
}
void AdBlindness::AddClick(int userid, Ice::Long group_id) {
  AddUserData(userid, group_id, false);
}
void AdBlindness::AddPv(int userid, Ice::Long group_id) {
  AddUserData(userid, group_id, true);
}

//该函数用来判定用户是否对广告产生疲倦
bool AdBlindness::IsDisplay(int userid, Ice::Long group_id) {
//  MCE_DEBUG("AdBlindness::IsDisplay-->userid:" << userid <<",groupid:"<<group_id);
  int pv_threshold = EngineConfig::instance().AdBlindThreshold();
//  MCE_DEBUG("AdBlindness::IsDisplay-->userid:" << userid <<",groupid:"<<group_id<<",pv_threshold:" <<pv_threshold);
  IceUtil::Mutex::Lock lock(mutex_);
  UidIndex & index = evictor_.get<1> ();
  UidIndex::iterator uit = index.find(userid);
  if (uit != index.end()) {
    SeqIndex::iterator sit = evictor_.project<0> (uit);//新查的放到前面
    evictor_.relocate(evictor_.get<0> ().begin(), sit);
    return (*uit)->GetPv(group_id) < pv_threshold; //如果展示次数过多则认为用户对该广告产生疲劳
  }
  return true;
}
}
}
