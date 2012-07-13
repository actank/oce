/*
 * LowQualityAdFilterHandler.cpp
 *
 *  Created on: 2011-8-25
 *      Author: sunzz
 */

#include <connection.h>
#include <query.h>
#include <vector>
#include "UserRankHandler.h"
#include "AdMatchManagerI.h"
#include "SomeMemberHandler.h"
#include "AdConfig.h"
#include "AdBlindness.h"
#include "UserDemographicCache.h"
#include "MemcacheClient.h"
#include "LowQualityAdFilterHandler.h"

using namespace std;
using namespace mysqlpp;
namespace xce {
namespace ad {

bool LowQualityAdFilterHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  int pick_count = para.PickCount();

  set<SelectedAdGroupPtr>& selected_groups = para.zone_selected_groups_;
  set<SelectedAdGroupPtr> competed_groups(selected_groups.begin(),selected_groups.end());
  vector<AdGroupPtr> default_groups= para.adzone_->GetAllDefaultGroups();
  MCE_DEBUG("LowQualityAdFilterHandler::handle,Load all default ad size:" << default_groups.size());
  long zone_id = para.adzone_->id();
  if((zone_id == 100000000070 || zone_id == 100000000072 || zone_id == 100000000063 || zone_id == 100000000001)){
    set<SelectedAdGroupPtr> new_result;
    bool is_replace_taobao = false;
    for(set<SelectedAdGroupPtr>::iterator its = selected_groups.begin(); its != selected_groups.end(); ++its) {
      int ecpm_level = LowQualityAdCache::instance().GetAdEcpmLevel((*its)->group_->group_id());
      if(ecpm_level > 2) {
        MCE_DEBUG("LowQualityAdFilterHandler::handle no need replace ");
        new_result.insert(*its);
        continue;
      }
      AdGroupPtr ptr = NULL;
      if(1 == ecpm_level && !is_replace_taobao) {//如果在最低级别，替换淘宝默认,如果已经发生过替换，不再进行替换
        ptr = GetDefaultTaobao(para);
        is_replace_taobao = true;
        MCE_DEBUG("LowQualityAdFilterHandler::handle replace with taobao,be replaced gid:"<< (*its)->group_->group_id());
      }
      if(2 == ecpm_level || !ptr) {//如果在第二级别，替换默认
        ptr = GetDefaultAd(competed_groups,default_groups); //选择默认广告，保证于已经存在的默认广告不同
        MCE_DEBUG("LowQualityAdFilterHandler::handle replace with default,be replaced gid:" << (*its)->group_->group_id());
      }
      if(ptr) {
        MCE_DEBUG("LowQualityAdFilterHandler::handle replace new gid:" << ptr->group_id() << "zone_id" << zone_id);
        SelectedAdGroupPtr selected = new SelectedAdGroup(ptr);
        if(ptr->member_id() == 999) {//默认广告扣费为0
          selected->cost_ = 0;
        } else {
          selected->cost_ = ptr->calculate_cost(zone_id);
        }
        selected->pos_ = (*its)->pos_;
        selected->demo_state_ = (*its)->demo_state_;
        set<SelectedAdGroupPtr>& total = para.total_selected_groups_;
        for(set<SelectedAdGroupPtr>::iterator itt = total.begin(); itt != total.end(); ++itt) {
          if((*itt)->group_->group_id() == (*its)->group_->group_id()) {//在已经选择的列表中去除已选广告
            total.erase(itt);
            break;
          }
        }
        new_result.insert(selected);
        competed_groups.insert(selected);
      }
    }
    if (new_result.size() < pick_count && !is_replace_taobao && zone_id != 100000000070 && zone_id != 100000000072) {
      AdGroupPtr ptr = NULL;
      ptr = GetDefaultTaobao(para);
      if(ptr) {
        MCE_DEBUG("LowQualityAdFilterHandler::handle replace add taobao gid:" << ptr->group_id());
        SelectedAdGroupPtr selected = new SelectedAdGroup(ptr);
        selected->cost_ = ptr->calculate_cost(zone_id);
        selected->pos_ = new_result.size() + 1;
        selected->demo_state_ = (zone_id % 100) * 1000 + selected->pos_;
        new_result.insert(selected);
        competed_groups.insert(selected);
      }
    }
    selected_groups.swap(new_result);  
    MCE_DEBUG("USE AS INFO LOG: user " << para.userProfile_.id() << " zone " << zone_id << " LowQualityAdFilterHandler " << selected_groups.size()); //缩减引擎INFO LOG
  }
 /* for(set<SelectedAdGroupPtr>::iterator it = selected_groups.begin(); it != selected_groups.end(); ++it) {
    MCE_DEBUG("user " << para.userProfile_.id() << " zone " << zone_id << " LowQualityAdFilterHandler selected group = " << (*it)->group_->group_id()<<",pos:" << (*it)->pos_);
  }*/
  return true;
}


void LowQualityAdCache::LoadGidsEcpmLevelPool() {
  MCE_INFO("[loader],LoadGidsEcpmLevelPool::Load start");
  map<Ice::Long, int> gids_ecpm_level;
  try {
    gids_ecpm_level = AdDemoQualityAdapter::instance().GetLimitedAdGroupMap(EngineConfig::instance().server_index());
    IceUtil::RWRecMutex::WLock lock(mutex_);
    gids_ecpm_level_.swap(gids_ecpm_level);
    MCE_INFO("[cpm loader]: Load stop, LoadGidsEcpmLevelPool size is:"  << gids_ecpm_level_.size());
  }catch (Ice::Exception& e) {
    MCE_WARN("LowQualityAdCache::LoadGidsEcpmLevelPool --> call LoadGidsEcpmLevelPool  ERR  line:"
        << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("LowQualityAdCache::LoadGidsEcpmLevelPool --> call LoadGidsEcpmLevelPool  ERR  line:"
        << __LINE__ << " err:" << e.what());
  }
}

int LowQualityAdCache::GetAdEcpmLevel(Ice::Long group_id){
  IceUtil::RWRecMutex::RLock lock(mutex_);
  map<Ice::Long, int>::iterator itg = gids_ecpm_level_.find(group_id);
  if(itg != gids_ecpm_level_.end()){
    return itg->second;
  }
  return 3;
}

AdGroupPtr LowQualityAdFilterHandler::GetDefaultTaobao(MatchHandlerParamter& para){
  const UserProfile & user_profile = para.userProfile_;
  long zone_id = para.adzone_->id();
  AdGroupPtr result = NULL;
  set<AdGroupPtr> bid_groups = UserAdMatcher::instance().GetAds(const_cast<UserProfile&>(user_profile), zone_id);
  vector<AdGroupPtr> taobao_gids;
  for(set<AdGroupPtr>::iterator it = bid_groups.begin(); it != bid_groups.end(); ++it){
    if((*it)->member_id()== 100000017431){//获得淘宝客户的广告组
      taobao_gids.push_back(*it);
    }
  }
  MCE_INFO("GetDefaultTaobao-->taobao_gids size:" << taobao_gids.size() << ",zone_id:" << zone_id);
  if(taobao_gids.empty()){
    return result;
  }
  int rand_index = rand()% taobao_gids.size();
  MCE_DEBUG("GetDefaultTaobao-->taobao_gids size:" << taobao_gids.size() << ",rand inedx:" << rand_index);
  return taobao_gids[rand_index];//随机选择
}
AdGroupPtr LowQualityAdFilterHandler::GetDefaultAd(set<SelectedAdGroupPtr>& selected_groups,
    vector<AdGroupPtr>& default_groups){
  for(vector<AdGroupPtr>::iterator it = default_groups.begin(); it != default_groups.end(); ++it){
    bool find_flag = false;
    for(set<SelectedAdGroupPtr>::iterator its = selected_groups.begin(); its != selected_groups.end(); ++its){
      if((*its)->group_->group_id() == (*it)->group_id()){
        find_flag = true;
      }
    }
    if(!find_flag) {//如果不在候选广告列表中需要
      return (*it);
    }
  }
}

}
}
