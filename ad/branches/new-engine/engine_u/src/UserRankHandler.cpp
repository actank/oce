#include "UserRankHandler.h"
#include "AdMatchManagerI.h"
#include "SomeMemberHandler.h"
#include "AdConfig.h"
#include "AdBlindness.h"
#include "UserDemographicCache.h"
using namespace std;

namespace xce {
namespace ad {

void AdItemList::Insert(AdItemPtr item) {
  items_.insert(item);
}
double AdItemList::FilterCompete(int uid, AdGroupPtr selected, set<Ice::Long> * evaluated_ids) {
  // typedef ItemContainer::nth_index<0>::type GidIndex;
  double filtered_score = 0.0;
  GidIndex & gid_index = items_.get<0>();
  // 按member过滤
  GidIndex::iterator it = gid_index.begin();

  vector<Ice::Long> compete_campaigns_seq = CompeteRef::instance().Get(selected->campaign_id());
  set<Ice::Long> compete_campaigns;
  compete_campaigns.insert(compete_campaigns_seq.begin(), compete_campaigns_seq.end());

  for(; it != gid_index.end(); ++it) {
    if (evaluated_ids->find((*it)->group_id_) != evaluated_ids->end()) {
      continue;
    }
    if (selected->group_id() == (*it)->group_id_) {
      continue;
    }
    AdGroupPtr g = AdGroupPool::instance().GetAdGroup((*it)->group_id_);
    if (g) {
      if (selected->member_id() == g->member_id()) {
        evaluated_ids->insert((*it)->group_id_);
        filtered_score += (*it)->score_;
        // MCE_DEBUG("uid " << uid << " rm ad " << selected->group_id() << " member compete " << (*it)->group_id_);
        continue;
      }
    } else {
      evaluated_ids->insert((*it)->group_id_);
      filtered_score += (*it)->score_;
      // MCE_DEBUG("uid " << uid << " rm ad " << selected->group_id() << " null compete " << (*it)->group_id_);
      continue;
    }

    if (compete_campaigns.find(g->campaign_id()) != compete_campaigns.end()) {
      evaluated_ids->insert((*it)->group_id_);
      filtered_score += (*it)->score_;
      // MCE_DEBUG("uid " << uid << " rm ad " << selected->group_id() << " compete " << (*it)->group_id_);
    }
  }

  return filtered_score;
}

AdGroupPtr AdItemList::Select(int uid, set<Ice::Long> & evaluated_ids, double& expected_ecpm) {
  AdGroupPtr result = NULL;
  ScoreIndex & score_index = items_.get<1>();
  PriceIndex & price_idx = items_.get<2>();
  double total_score = TotalScore();
  double random = (((double) (rand() % 10000)) / (10000.0)) * total_score;
  double left = 0.0, right = 0.0;
  ScoreIndex::iterator score_it = score_index.begin();
  for( ; score_it != score_index.end(); ++score_it) {
    if (evaluated_ids.end() != evaluated_ids.find((*score_it)->group_id_)) {
      continue;
    }
    right = left + (*score_it)->score_;

    if (left <= random && random <= right) {
      AdGroupPtr group = AdGroupPool::instance().GetAdGroup((*score_it)->group_id_);
      if (group) {
        double filtered_score = FilterCompete(uid, group, &evaluated_ids);
        expected_ecpm = (*score_it)->score_;
        result = group;
      } else {
        MCE_DEBUG("Select gid=" << (*score_it)->group_id_ << " not found");
      }
      evaluated_ids.insert((*score_it)->group_id_);
      break;
    } else {
      left = right;
    }
  }
  return result;
}
//按score从高往低尝试最多10次，试图取出一个广告组
AdGroupPtr AdItemList::Select2(int uid, set<Ice::Long> & evaluated_ids) {
  AdGroupPtr result = NULL;
  ScoreIndex & score_index = items_.get<1>();
  int try_count = 0;
  for(ScoreIndex::reverse_iterator score_it = score_index.rbegin(); score_it != score_index.rend() && try_count < 10; ++score_it, ++try_count) {
    AdGroupPtr group = AdGroupPool::instance().GetAdGroup((*score_it)->group_id_);
    if (group) {
      double filtered_score = FilterCompete(uid, group, &evaluated_ids);
      result = group;
      evaluated_ids.insert((*score_it)->group_id_);
      break;
    } else {
      MCE_DEBUG("Select gid=" << (*score_it)->group_id_ << " not found");
    }
  }
  return result;
}


/*double CalculateScore(const AdQualityU quality, const int max_price) {
  int sort_type = EngineConfig::instance().sort_type();
  int price_power = EngineConfig::instance().price_power();
  int ctr_power = EngineConfig::instance().ctr_power();
  MCE_DEBUG("CalculateScore:sort_type=" << sort_type << ",price_power=" << price_power << ",ctr_power=" << ctr_power);

  return pow(quality.quality, (double)ctr_power) * pow(max_price, (double)price_power);
}*/

SelectedAdGroupSeq UserIndex::GetZoneGroups(const UserProfile & user_profile, long adzone, int count) {
  MemberItemListPtr mem_list_ptr;
  {
    IceUtil::Mutex::Lock lock(user_mutex_);
    map<long, MemberItemListPtr>::const_iterator it = user_zone_items_.find(adzone);
    if (it != user_zone_items_.end()) {
      mem_list_ptr = it->second;
      MCE_DEBUG("uid " << user_profile.id() << " cache hit" );
    }
  }
  if (!mem_list_ptr || mem_list_ptr->IsStale()) {
    set<AdGroupPtr> groups;
    {
      ostringstream oss;
      oss << "Get index uid=" << user_profile.id();
      StatFunc stat(oss.str());
      groups = UserAdMatcher::instance().GetAds(const_cast<UserProfile&>(user_profile), adzone);
    }
    MCE_INFO("user " << user_profile.id() << " zone " << adzone << " matched ad count " << groups.size());
    if (!groups.empty()) {
      // ad_list_ptr = new AdItemList(user_profile, adzone, groups);
      mem_list_ptr = new MemberItemList(user_profile, adzone, groups);
      user_zone_items_[adzone] = mem_list_ptr;
    }
  }

  SelectedAdGroupSeq result;
  if (mem_list_ptr) {
    ostringstream oss;
    oss << "Select ad uid=" << user_profile.id();
    StatFunc stat(oss.str());
    result = mem_list_ptr->Select(user_profile.id(), count);
  } else {
    MCE_DEBUG("user " << uid_ << " adzone " << adzone << " no matched ads");
  }
  return result;
}

//考虑是否与已选广告组构成竞争关系，只为GetZoneGroups2服务
bool UserIndex::IsCompetedGroup(AdGroupPtr candidate_group, set<AdGroupPtr> & selected_ids) {

  vector<Ice::Long> compete_campaigns_seq = CompeteRef::instance().Get(candidate_group->campaign_id());
  set<Ice::Long> compete_campaigns;
  compete_campaigns.insert(compete_campaigns_seq.begin(), compete_campaigns_seq.end());
  MCE_DEBUG("UserIndex::IsCompetedGroup-->gid:" << candidate_group->group_id()<<",compaign size:"<<compete_campaigns.size());

  for(set<AdGroupPtr>::iterator its = selected_ids.begin(); its != selected_ids.end(); ++its) {
    if(candidate_group->member_id() == (*its)->member_id()) { //候选广告和已选广告列表是否存在同一广告主，存在视为竞品
      MCE_DEBUG("UserIndex::IsCompetedGroup candidate_group:" <<candidate_group->group_id() <<",selected_group:"<<(*its)->group_id()<<" has same memeber");
      return true;
    }
    set<Ice::Long>::iterator itc= compete_campaigns.find((*its)->campaign_id()); //候选广告的广告计划的竞品是否是已选广告的广告计划
    if(itc != compete_campaigns.end()) {
      MCE_DEBUG("UserIndex::IsCompetedGroup candidate_group:" <<candidate_group->group_id() <<",selected_group:"<<(*its)->group_id()<<" has same campaign");
      return true;
    }
  }
  return false;
}
string UserIndex::GetFreshKey(Ice::Long zone_id, int pos) {
  Ice::Long key = zone_id * 10 + pos;
  ostringstream sin;
  sin << key;
  return sin.str();

}
/*AdGroupPtr UserIndex::GetZonePosFreshGroup(int userid, FreshAdQualityMap & fresh_groups, set<AdGroupPtr>& bid_ids, set<AdGroupPtr>& selected_ids, long adzone, int pos) {
  AdGroupPtr r_ptr = NULL;
  double randnum = (rand() % 100) / 100.0;
  MCE_DEBUG("[Fresh:],GetZonePosFreshGroup,userid:" << userid <<" adzone:" << adzone <<",pos:" << pos << "fresh_map size:" << fresh_groups.size() <<",bid size:" << bid_ids.size() <<",randnum:" << randnum);
  string key = GetFreshKey(adzone, pos);
  FreshAdQualityMap::iterator itf = fresh_groups.find(key); 
  if(itf != fresh_groups.end()) {
    FreshAdQualitySet& f_groups = itf->second;

    MCE_DEBUG("[Fresh:],GetZonePosFreshGroup,userid:" << userid <<" adzone:" << adzone <<",pos:" << pos << "fresh groups size:" << fresh_groups.size() <<",bid size:" << bid_ids.size());
    for(map<Ice::Long,double>::iterator it_debug = f_groups.begin(); it_debug != f_groups.end(); ++it_debug) {
      MCE_DEBUG("[Fresh:],Fresh groups ratio,zone:" << adzone <<".pos:" <<pos <<",gid:"<<it_debug->first <<",ratio:" << it_debug->second);
    }
    double total = 0.0;
    for(set<AdGroupPtr>::iterator itb = bid_ids.begin(); itb != bid_ids.end(); ++ itb) {
       MCE_DEBUG("[Fresh:],bidgroup ,gid:" << (*itb)->group_id()<<",userid:" << userid);
      if(AdBlindness::instance().IsDisplay(userid ,(*itb)->member_id())){//如果没达到呈现控制,参与随机
        FreshAdQualitySet::iterator it = f_groups.find((*itb)->group_id());
        if(it != f_groups.end()) {//是否在新广告列表中，如果在，参与随机
          MCE_DEBUG("[Fresh:],exist in fresh groups ,gid:" << it->first<<",userid:" << userid<<",ratio:" << it->second);
          if(!IsCompetedGroup((*itb), selected_ids)) { //是否与已选存在竞争，不存在进行下边操作,即:参与随机
            total += it->second;
            if(randnum < total) { //随机到当前group,返回
              return (*itb);
            }
          }
        }
      }
    }
    MCE_DEBUG("[Fresh Not Found]:total score" << total << ",random:" << randnum <<",adzone:" << adzone<< ",pos:" << pos);
  }

  return r_ptr;
}*/
AdGroupPtr UserIndex::GetZonePosFreshGroup(int userid, FreshAdQualityMap & fresh_groups, set<AdGroupPtr>& bid_ids, set<AdGroupPtr>& selected_ids, long adzone, int pos, bool is_reserved) {
  AdGroupPtr r_ptr = NULL;
  double randnum = (rand() % 100) / 100.0;
  double test_ratio = 0.0;
  if(is_reserved){
    test_ratio = EngineConfig::instance().GetReservedStateTestImprRatio(adzone, pos);
  }else {
    test_ratio = EngineConfig::instance().GetStateTestImprRatio(adzone, pos);
  }
  //test_ratio = EngineConfig::instance().GetStateTestImprRatio(adzone, pos);
  MCE_DEBUG("[Fresh:],GetZonePosFreshGroup,userid:" << userid <<" adzone:" << adzone <<",pos:" << pos << "fresh_map size:" << fresh_groups.size() <<",bid size:" << bid_ids.size() <<",randnum:" << randnum <<",test ratio:" << test_ratio<<"is_reserved:" << is_reserved);
  if(randnum > test_ratio) {
    return r_ptr;
  }
  string key = GetFreshKey(adzone, pos);
  FreshAdQualityMap::iterator itf = fresh_groups.find(key); 
  if(itf != fresh_groups.end()) {
    FreshAdQualitySet& f_groups = itf->second;

    MCE_DEBUG("[Fresh:],GetZonePosFreshGroup,userid:" << userid <<" adzone:" << adzone <<",pos:" << pos << "fresh groups size:" << fresh_groups.size() <<",bid size:" << bid_ids.size());
    //for(map<Ice::Long,double>::iterator it_debug = f_groups.begin(); it_debug != f_groups.end(); ++it_debug) {
      //MCE_DEBUG("[Fresh:],Fresh groups ratio,zone:" << adzone <<".pos:" <<pos <<",gid:"<<it_debug->first <<",ratio:" << it_debug->second);
    //}
    double total = 0.0;
    vector<pair<double, AdGroupPtr> > rand_result;
    for(set<AdGroupPtr>::iterator itb = bid_ids.begin(); itb != bid_ids.end(); ++ itb) {
      // MCE_DEBUG("[Fresh:],bidgroup ,gid:" << (*itb)->group_id()<<",userid:" << userid);
     // if(AdBlindness::instance().IsDisplay(userid ,(*itb)->group_id())){//如果没达到呈现控制,参与随机
        FreshAdQualitySet::iterator it = f_groups.find((*itb)->group_id());
        if(it != f_groups.end()) {//是否在新广告列表中，如果在，参与随机
          MCE_DEBUG("[Fresh:],exist in fresh groups ,gid:" << it->first<<",userid:" << userid<<",ratio:" << it->second);
          if(!IsCompetedGroup((*itb), selected_ids)) { //是否与已选存在竞争，不存在进行下边操作,即:参与随机
            total += it->second;
            rand_result.push_back(make_pair(it->second,(*itb)));

            //if(randnum < total) { //随机到当前group,返回
            //  return (*itb);
            //}
          }
        }
      //}
    }
    //double test_ratio = EngineConfig::instance().GetTestImprRatio();
    //double test_ratio = EngineConfig::instance().GetStateTestImprRatio(adzone, pos);
    MCE_DEBUG("[Fresh:]:userid:" <<userid <<",total score" << total << ",random:" << randnum <<",adzone:" << adzone<< ",pos:" << pos<<"test ratio:" <<test_ratio);
    if(total > 0.0) {
      double ratio = 1.0;
      ratio = test_ratio / total;
      MCE_INFO("[Fresh:]:userid" << userid << ",total score" << total << ",random:" << randnum <<",adzone:" << adzone<< ",pos:" << pos<<",test ratio:" << test_ratio <<",ratio:" << ratio <<",is_reserved:" << is_reserved);
      double sum = 0.0;
      for(vector<pair<double,AdGroupPtr> >::iterator itr = rand_result.begin(); itr != rand_result.end(); ++itr) {
        sum += (*itr).first * ratio;
        if(randnum < sum) {
          return (*itr).second;
        }
      }
    }
  }

  return r_ptr;
}


SelectedAdGroupSeq UserIndex::GetZoneGroups3(const MatchHandlerParamter& para, long adzone, int count) {
  bool is_reserved = false;
  if(para.tag_ != "" && para.tag_.find("REMAIN") != string::npos && (adzone == 100000000070 || adzone == 100000000072)){
    is_reserved = true;
  }
  if(adzone == 100000000075) {//接余量
    adzone = 100000000072;
    MCE_INFO("GetZoneGroups3-->old zone:" << 100000000075 <<",changed to zone:" << adzone);
  }

  const UserProfile & user_profile = para.userProfile_;
  int uid = para.userProfile_.id();
  long zone_id = para.adzone_->id();
  const string referer = para.referer_;
  string app_zone; 
  string app_name; 
  vector <string> para_strs;
  vector <string> app_strs;
  app_name = string("");
  
  boost::algorithm::split_regex(para_strs, referer, regex("/"));
  if (adzone == 100000000001){
  if (para_strs.size() >= 4) {
    app_zone = para_strs[3];
  } else {
    app_zone = string("");
  }  
  if (app_zone.size() > 0){
    boost::algorithm::split_regex(app_strs, app_zone, regex("\\?"));
    if (app_strs.size() > 0){
      app_name = app_strs[0];
    }
  }
  }
 
  SelectedAdGroupSeq result;
  bool debug_flag = (EngineConfig::instance().GetUserId() == user_profile.id());
  set<AdGroupPtr> bid_groups = UserAdMatcher::instance().GetAds(const_cast<UserProfile&>(user_profile), adzone);
  set<AdGroupPtr> groups;
  int big_pic_id = EngineConfig::instance().big_pic_id();
  
  for(set<AdGroupPtr>::iterator itb = bid_groups.begin(); itb != bid_groups.end(); ++itb) {
    if(adzone == 100000000072 && para.photo_ && (*itb)->widget_id() == big_pic_id ) {//大图余量72不投放
      continue;
    }
    groups.insert(*itb);
  }
  
  if(groups.empty()) {
    return result;
  }
  MCE_INFO("UserIndex::GetZoneGroups3new2-->user " << user_profile.id() << " zone " << adzone << " matched ad count " << groups.size()<<",debug_flag" << debug_flag);
  if(debug_flag) {
    MCE_INFO("uid=" << user_profile.id() << "'debug info:" << " gender=" << user_profile.gender() << " age=" << user_profile.age() <<
        " stage="<< user_profile.stage() << " school=" << user_profile.school() << " grade=" << user_profile.grade() << " current_area="
        << user_profile.current_area() << " ip_area=" << user_profile.ip_area());
    MCE_INFO("uid=" << user_profile.id() << "'debug info:" << "zone=" << adzone << " matched ad count " << groups.size());
    for(set<AdGroupPtr>::const_iterator it = groups.begin(); it != groups.end(); ++it) {
      MCE_INFO("uid=" << user_profile.id() << "'debug info:" << "gid= " << (*it)->group_id() << " bidunit = " << (*it)->bid_unit_id()<<
          " cost=" << (*it)->max_price());
    }
  }
  ///////////////////////////////////////////////////////////////////
  float cost_discount = EngineConfig::instance().GetCostDiscount();
  int rand_seed = EngineConfig::instance().GetRandSeed();
  ////////////////////////////////////////////////////////////////////
  FreshAdQualityMap fresh_groups = FreshAdGroupsCache::instance().GetFreshGroups();
  set<AdGroupPtr> selected_ids;
  double price_power = EngineConfig::instance().price_power();
  double ctr_power = EngineConfig::instance().ctr_power();
  MCE_DEBUG("GetZoneGroups3,ctr_power:" << ctr_power << ",price_power:" << price_power);
  for(int i = 0; i < count; ++i) {
    double max_ecpm = -1;
    AdGroupPtr m_ptr = NULL;
    int state = 0;

/////////////////////////////select fresh group////////////////////////////////////
    m_ptr = GetZonePosFreshGroup(user_profile.id(), fresh_groups, groups, selected_ids, adzone, i+1, is_reserved); 
    if(m_ptr) {

      SelectedAdGroupPtr selected = new SelectedAdGroup(m_ptr);
      selected->cost_ = (int)((cost_discount + rand() % rand_seed / 10000.0) * (m_ptr->max_price()));
      if (selected->cost_ < EngineConfig::instance().sys_min_price()) {
        selected->cost_ = EngineConfig::instance().sys_min_price();
      } 
      if (selected->cost_ > m_ptr->max_price()) {
        selected->cost_ = m_ptr->max_price();
      }
      selected->pos_ = i+1;
      if((m_ptr->widget_id() == 31) && (selected->cost_ < 200)) {
        selected->cost_ = 200;
      }
      int factor = 10;
      if(is_reserved) {
        factor = 100;
      }
      selected->demo_state_ = (adzone % 100) * factor + selected->pos_ ;
      MCE_DEBUG("select result widget test,gid:" << selected->group_->group_id() <<",widget id:" <<m_ptr->widget_id() <<",cost:" << selected->cost_
                  <<",max price:" << m_ptr->max_price());
      result.push_back(selected);
      selected_ids.insert(m_ptr);
      if(debug_flag) {
        MCE_INFO("Fresh debug_uid uid=" << user_profile.id() << "'debug info:" << "[" << selected->pos_ << "] id" << selected->group_->group_id()  <<
            " cost=" << selected->cost_ <<",state:" <<selected->demo_state_<<",pos:" << selected->pos_ <<",zone:" 
            <<adzone);
      }
      MCE_INFO("[Fresh:] getfresh,uid=" << user_profile.id() <<",zone:" << adzone << ",pos:" << selected->pos_ <<"selected gid:" << selected->group_->group_id() <<",state:" <<selected->demo_state_<<",widdget:" << m_ptr->widget_id() << ",cost:" << selected->cost_ <<",maxprice:" << m_ptr->max_price());
      continue;
    }

//////////////////////////////////////////////////////////////////////////    

    for(set<AdGroupPtr>::const_iterator it = groups.begin(); it != groups.end(); ++it) {
      set<AdGroupPtr>::const_iterator its = selected_ids.find(*it);
      if(its == selected_ids.end()) { //是否已经选到，不在列表中进行下边操作
        if((*it) && !IsCompetedGroup((*it), selected_ids)) { //是否与已选存在竞争，不存在进行下边操作,即：找最大的ecpm
          //double ctr = UserDemographicCache::instance().GetUserAdGroupsCtr(user_profile, adzone ,i+1 ,const_cast<AdGroupPtr&>(*it));
          pair<double,int> ctr_state = UserDemographicCache::instance().GetUserAdGroupsCtr(user_profile, adzone ,i+1 ,(*it)->group_id(),(*it)->member_id(),app_name);

          //double ecpm = pow(ctr, ctr_power) * pow((*it)->max_price(), price_power);
          double ecpm = pow(ctr_state.first, ctr_power) * pow((*it)->max_price(), price_power);
          /*if(!AdBlindness::instance().IsDisplay(user_profile.id(),(*it)->group_id())){//如果呈现屏蔽，则降低权重
            ecpm *= 0.001;
          }*/
          if(ecpm > max_ecpm) {
            max_ecpm = ecpm;
            m_ptr = *it;
            state = ctr_state.second;
          }
          if(debug_flag) {
            MCE_INFO("uid=" << user_profile.id() << "'debug info:gid:" <<(*it)->group_id()<< " ctr=" << ctr_state.first<<",pos:" <<i+1 <<
                " cost=" << (*it)->max_price() <<" ecpm=" << ecpm ); 
          }
        }
      }
    }
    //找到该pos的最佳广告后在列表中插入广告
    if(m_ptr) {
      SelectedAdGroupPtr selected = new SelectedAdGroup(m_ptr);
      selected->cost_ = (int)((cost_discount + rand() % rand_seed / 10000.0) * (m_ptr->max_price()));
      if (selected->cost_ < EngineConfig::instance().sys_min_price()) {
        selected->cost_ = EngineConfig::instance().sys_min_price();
      } 
      if (selected->cost_ > m_ptr->max_price()) {
        selected->cost_ = m_ptr->max_price();
      }
      //selected->cost_ = m_ptr->max_price();
      if((m_ptr->widget_id() == 31) && (selected->cost_ < 200)) {
        selected->cost_ = 200;
      }
      selected->demo_state_ = state;
      selected->pos_ = i+1;
      
      MCE_INFO("select result widget test,gid:" << selected->group_->group_id() <<",widget id:" <<m_ptr->widget_id() <<",cost:" << selected->cost_
      <<",max price:" << m_ptr->max_price());
      result.push_back(selected);
      selected_ids.insert(m_ptr);
      if(debug_flag) {
        MCE_INFO("debug_uid uid=" << user_profile.id() << "'debug info:" << "[" << selected->pos_ << "] id" << selected->group_->group_id()  <<
            " cost=" << selected->cost_ << " ecpm=" << max_ecpm<<",state:" <<selected->demo_state_<<",pos:" << selected->pos_ <<",zone:" 
            <<adzone);
      }
      //MCE_INFO("[Fresh:] notgetfresh,uid=" << user_profile.id() <<",zone:" << adzone << ",pos:" << selected->pos_ <<"selected gid:" << selected->group_->group_id() <<",state:" <<selected->demo_state_);
    }
  }
  MCE_DEBUG("after ad selection, " << result.size() << " ads selected for " << user_profile.id() << ".");
  return result;
}

//挑选ECPM最高的count个
/*SelectedAdGroupSeq UserIndex::GetZoneGroups2(const UserProfile & user_profile, long adzone, int count) {

  bool debug_flag = (EngineConfig::instance().GetUserId() == user_profile.id());
  set<AdGroupPtr> groups = UserAdMatcher::instance().GetAds(const_cast<UserProfile&>(user_profile), adzone);
  MCE_INFO("UserIndex::GetZoneGroups2-->user " << user_profile.id() << " zone " << adzone << " matched ad count " << groups.size());
  if(debug_flag) {
    MCE_INFO("uid=" << user_profile.id() << "'debug info:" << " gender=" << user_profile.gender() << " age=" << user_profile.age() <<
    " stage="<< user_profile.stage() << " school=" << user_profile.school() << " grade=" << user_profile.grade() << " current_area="
    << user_profile.current_area() << " ip_area=" << user_profile.ip_area());
    MCE_INFO("uid=" << user_profile.id() << "'debug info:" << "zone=" << adzone << " matched ad count " << groups.size());
    for(set<AdGroupPtr>::const_iterator it = groups.begin(); it != groups.end(); ++it) {
      MCE_INFO("uid=" << user_profile.id() << "'debug info:" << "gid= " << (*it)->group_id() << " bidunit = " << (*it)->bid_unit_id()<<
      " cost=" << (*it)->max_price());
    }
  }
  SelectedAdGroupSeq result;
  TimeStat ts;
  map<AdGroupPtr, AdQualityU> qualitys;// = UserDemographicCache::instance().GetUserAdGroupsCtr(user_profile, groups, adzone);
  MCE_INFO("[USEDTIME] GetZoneGroups --> call UserDemographic GetUserAdGroupsCtr used time = " << ts.getTime());
  MCE_DEBUG("GetZoneGroup2 Demographic get ctr size:" << qualitys.size()<<",userid:" << user_profile.id());
  if(!qualitys.empty()) {

    set<AdGroupPtr> selected_ids;
    //循环count次，找出count个最高的ecpm
    for(int i = 0; i < count;) {
      double max_ecpm = -1;
      map<AdGroupPtr, AdQualityU>::iterator max_qit = qualitys.end();
      for(map<AdGroupPtr, AdQualityU>::iterator qit = qualitys.begin(); qit != qualitys.end(); ++qit) {
        double ecpm = (qit->second.quality) * (qit->first->max_price());
        if(ecpm > max_ecpm) {
          max_ecpm = ecpm;
          max_qit = qit;
        }
        if(debug_flag) {
          MCE_INFO("uid=" << user_profile.id() << "'debug info:gid:" <<qit->first->group_id()<< " ctr=" << qit->second.quality <<
          " cost=" << qit->first->max_price() <<" ecpm=" << ecpm);
        }
      }

      if(max_qit != qualitys.end()) {
        AdGroupPtr m_ptr = max_qit->first;

        qualitys.erase(max_qit);
        if(m_ptr && !IsCompetedGroup(m_ptr, selected_ids)) {
          SelectedAdGroupPtr selected = new SelectedAdGroup(m_ptr);
          selected->cost_ = m_ptr->max_price();
          selected->pos_ = i+1;
          selected->demo_state_ = m_ptr->GetDemoState();
          result.push_back(selected);
          selected_ids.insert(m_ptr);
          i++;
          if(debug_flag) {
            MCE_INFO("uid=" << user_profile.id() << "'debug info:" << "[" << selected->pos_ << "] id" << selected->group_->group_id()  <<
            " cost=" << selected->cost_ << " ecpm=" << max_ecpm);
          }
        }
     //   MCE_DEBUG("UserIndex::GetZoneGroups2-->ecpm:" << max_ecpm <<",max price:"<<",ctr"<<max_qit->second.quality<< selected->cost_ << ",user " << user_profile.id() <<",gender:"<<user_profile.gender()<<"," << user_profile.age() <<"," << user_profile.stage()<<","<<user_profile.current_area()<< " zone " << adzone << " matched ad count " << groups.size());
      } else { //当所有的都遍历完跳出循环
        return result;
      }
    }
  }
  return result;
}*/


bool UserRankHandler::handle(MatchHandlerParamter& para, AdResultMap& result) {
  int pick_count = para.PickCount();
  if (pick_count <= 0) {
    return true;
  }

  int uid = para.userProfile_.id();
  UserIndexPtr user_index = UserIndexCache::instance().GetUserIndex(uid);
  long zone_id = para.adzone_->id();
  //SelectedAdGroupSeq selected = user_index->GetZoneGroups2(para.userProfile_, zone_id, pick_count);
  SelectedAdGroupSeq selected = user_index->GetZoneGroups3(para, zone_id, pick_count);

  MCE_DEBUG("UserRankHandler::handle:select size:" << selected.size());

  if(!selected.empty()) {
    for(SelectedAdGroupSeq::const_iterator it = selected.begin(); it != selected.end(); ++it) {
      para.zone_selected_groups_.insert(*it);
      (*it)->pos_ = para.zone_selected_groups_.size();
      //AdBlindness::instance().AddPv(uid,(*it)->group_->group_id());
    }
    para.total_selected_groups_.insert(para.zone_selected_groups_.begin(), para.zone_selected_groups_.end());
  }
  MCE_INFO("user " << uid << " zone " << zone_id << " UserRank selected size " << selected.size() << "/" << pick_count);
  for(SelectedAdGroupSeq::iterator it = selected.begin(); it != selected.end(); ++it) {
    MCE_DEBUG("user " << uid << " zone " << zone_id << " UserRank selected group = " << (*it)->group_->group_id());
  }
  return true;
}

set<AdGroupPtr> UserAdMatcher::GetAds(UserProfile& profile, Ice::Long adzone) {
  try {
    //return DimIndexMatcher::instance().Get(adzone, profile);
    set<AdGroupPtr> result;
    if(100000000072 != adzone && 100000000070 != adzone && 100000000063 != adzone && 100000000001 != adzone) {
      adzone = 100000000072;
    }
    AdInvertedIndex::instance().Get(profile, adzone, result);

    ostringstream data_check;
    data_check <<"data_check : adzone=" << adzone << " & userid=" << profile.id() << " [ ";
    for( set<AdGroupPtr>::const_iterator it = result.begin() ; it != result.end() ; ++ it){
      data_check << "\t" << (*it)->group_id();
    }
    data_check <<" ]";
    MCE_DEBUG(data_check.str());
    return result;
  } catch(Ice::Exception& e) {
    MCE_WARN("UserAdMatcher::GetAds error : " << e);
  } catch(std::exception& e) {
    MCE_WARN("UserAdMatcher::GetAds std::exception error : " << e.what());
  } catch(...) {
    MCE_WARN("UserAdMatcher::GetAds unknown error.");
  }
  return set<AdGroupPtr>();
}

///////////////////////////////////////////////////////////////////////
void MemberItem::Add(AdItemPtr ad_item) {
  if (ad_item->score_ > score_) {
    score_ = ad_item->score_;
  }
  ad_count_++;
  ad_items_->Insert(ad_item);
}
bool Predicate(const SelectedAdGroupPtr& g1, const SelectedAdGroupPtr& g2) {
  return g1->ecpm_ - g2->ecpm_ > 0;
}
SelectedAdGroupSeq MemberItemList::Select(int uid, int count) {
  SelectedAdGroupSeq result;
  double total_score = TotalScore();
  if (total_score > 0) {
    set<Ice::Long> evaluated_ids;
    set<AdGroupPtr> selected_groups;
    int loop_count = 0;
    int inner_loop = 0;
    float cost_discount = 0.85;
    int rand_seed = 1500;
    ///////////////////////////////////////以下只为debu
    //  ostringstream oss_debug;
    //  oss_debug << "uid = " << uid ;
    //  for (map<Ice::Long, MemberItemPtr>::iterator it = items_.begin(); it != items_.end(); it++) {
    //    oss_debug <<",member_id:" << it->first <<",score:"<<it->second->score_;
    //  }
    //  MCE_DEBUG("MemberItemList::Select:"<<oss_debug.str());

    ///////////////////////////////////////////////////////////////////
    cost_discount = EngineConfig::instance().GetCostDiscount();
    rand_seed = EngineConfig::instance().GetRandSeed();
    //MCE_INFO("MemberItemList::Select -->cost_discount:" << cost_discount << ",rand_seed:" << rand_seed);
    ostringstream oss;
    oss << "MemberItemList::Select() --> uid = " << uid << ", count = " << count
      << ", ad_count_ = " << ad_count_ << ", total_score = " << total_score
      << ", items_.size() = " << items_.size();
    while(selected_groups.size() < count && evaluated_ids.size() < ad_count_) {
      double random = (((double) (rand() % 10000)) / (10000.0)) * total_score;
      double left = 0.0, right = 0.0;
      inner_loop = 0;
      map<Ice::Long, MemberItemPtr>::iterator score_it = items_.begin();
      for( ; score_it != items_.end(); ++score_it) {
        ++loop_count;
        ++inner_loop;
        double ecpm = -1.0;
        right = left + score_it->second->score_;

        if (left <= random && random <= right) {
          //Ice::Long select_gid = score_it->second->ad_items_->Select(evaluated_ids);
          AdGroupPtr group = score_it->second->ad_items_->Select(uid, evaluated_ids, ecpm);
          if (group) {
            SelectedAdGroupPtr selected = new SelectedAdGroup(group); 
            selected->ecpm_ = ecpm;

            // cost 计算
            //selected->cost_ = (int)((0.85 + rand() % 1500 / 10000.0) * group->max_price());
            selected->cost_ = (int)((cost_discount + rand() % rand_seed / 10000.0) * group->max_price());
            if (selected->cost_ < EngineConfig::instance().sys_min_price()) {
              selected->cost_ = EngineConfig::instance().sys_min_price();
            } 
            if (selected->cost_ > group->max_price()) {
              selected->cost_ = group->max_price();
            }

            MCE_DEBUG("UserRank Select mid=" << group->member_id() << "gid=" << group->group_id());
            result.push_back(selected);
            selected_groups.insert(group);
            //double filtered_score = FilterCompete(uid, group, &evaluated_ids);
          } else {
            MCE_DEBUG("uid " << uid << ",Select from mid=" << score_it->first << " not found");
          }
          // evaluated_ids.insert((*score_it)->group_id_); 
          total_score -= score_it->second->score_; 
          break;
        } else {
          left = right;
        }
      }
      if (total_score <= 0.0) {
        break;
      }
    }
    for(map<Ice::Long, MemberItemPtr>::iterator it = items_.begin(); result.size() < count && it != items_.end(); ++it) {
      AdGroupPtr group = it->second->ad_items_->Select2(uid, evaluated_ids);
      if (group) {
        SelectedAdGroupPtr selected = new SelectedAdGroup(group); 
        selected->cost_ = (int)((cost_discount + rand() % rand_seed / 10000.0) * group->max_price());
        if (selected->cost_ < EngineConfig::instance().sys_min_price()) {
          selected->cost_ = EngineConfig::instance().sys_min_price();
        } 
        if (selected->cost_ > group->max_price()) {
          selected->cost_ = group->max_price();
        }
        MCE_DEBUG("MemberItemList::Select() --> uid = " << uid << ", fix user rank, add one group");
        result.push_back(selected);
        selected_groups.insert(group);
      }
    }
    oss << ", loop_count = " << loop_count << ", result.size() = " << result.size();
    MCE_DEBUG(oss.str());
    /*  ostringstream oss_debug;
        oss_debug <<"before sorted:";
        for(vector<SelectedAdGroupPtr>::iterator itr = result.begin();itr != result.end();++itr) {
        oss_debug <<"," <<(*itr)->ecpm_;
        }
        oss_debug <<"after sorted";*/
    std::sort(result.begin(),result.end(),Predicate);
    /*  for(vector<SelectedAdGroupPtr>::iterator itr = result.begin();itr != result.end();++itr) {
        oss_debug <<"," <<(*itr)->ecpm_;
        }
        MCE_DEBUG(oss_debug.str());*/
  }
  return result;
}
/*void MemberItemList::InitScore(const UserProfile & user_profile, const set<AdGroupPtr> & groups) {
  int uid = user_profile.id();
  //double ecpm_threshold = 10;//10分，0.1元
  double ecpm_threshold = 0; //线上7, 8, 9 以分为单位
  map<AdGroupPtr,AdQualityU> ctr_map;// = UserDemographicCache::instance().GetUserAdGroupsCtr(user_profile, groups, zone_id_);
  MCE_DEBUG("Demographic: getctr size:" <<ctr_map.size());
  for(map<AdGroupPtr,AdQualityU>::iterator itc = ctr_map.begin(); itc != ctr_map.end(); ++itc) {
    AdGroupPtr group = itc->first;;
    AdQualityU quality = itc->second;
    MCE_DEBUG("MemberItemList::InitScore() --> uid = " << uid << ", quality.isFresh = " << quality.isFresh << ", quality.ecpm = " << quality.ecpm << ",quality.quality:"<<quality.quality<< ", zone_id = " << zone_id_);
    if (quality.isFresh) {
      quality.quality = 0.00001;
      quality.ecpm = 0.00001;
    }
    double score = CalculateScore(quality, group->max_price());
    AdItemPtr ad_item = new AdItem(group->group_id(), group->max_price(), quality.quality, score);
    Insert(group->member_id(), ad_item);
  }

  MCE_DEBUG("MemberItemList::InitScore() --> uid = " << uid << ", groups.size() = " << groups.size() << ", number of members: items_.size() = " << items_.size());
}*/


void MemberItemList::Insert(Ice::Long mid, AdItemPtr ad_item) {
  map<Ice::Long, MemberItemPtr>::iterator it = items_.find(mid);
  if (it != items_.end()) {
    if(EngineConfig::instance().sort_type() == 7) {
      //只保存最高ecpm的那一个
      if(it->second->score_ < ad_item->score_) {
        MemberItemPtr item = new MemberItem(mid);
        item->Add(ad_item);
        items_[mid] = item;
      }
    } else {
      it->second->Add(ad_item);
    }
  } else {
    MemberItemPtr item = new MemberItem(mid);
    item->Add(ad_item);
    items_[mid] = item;
  }
  ad_count_++;
}

}
}
