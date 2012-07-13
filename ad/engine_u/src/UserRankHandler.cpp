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

using namespace std;
using namespace mysqlpp;
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
  test_ratio = TestAdImprRatioAssignCache::instance().GetTestRatio(is_reserved);
  if(100000000001 == adzone) {
    test_ratio = 0.05;
  }
  //test_ratio = EngineConfig::instance().GetStateTestImprRatio(adzone, pos);
  MCE_DEBUG("[Fresh:],GetZonePosFreshGroup,userid:" << userid <<" adzone:" << adzone <<",pos:" << pos << "fresh_map size:" << fresh_groups.size() <<",bid size:" << bid_ids.size() <<",randnum:" << randnum <<",test ratio:" << test_ratio<<"is_reserved:" << is_reserved);
  if(randnum >= test_ratio) {
    return r_ptr;
  }
  string key = GetFreshKey(adzone, pos);
//  if(is_reserved) {
  key = "reserved";
//  }
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
            double weight = it->second;
            if((*itb)->widget_id() == 31 && adzone == 100000000070) {
              weight *= 10;
            }
            total += weight;
            rand_result.push_back(make_pair(weight,(*itb)));

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

vector<AdGroupPtr> UserIndex::RandomSelectAdgroup(const UserProfile & user_profile,set<AdGroupPtr>& bid_gids, set<AdGroupPtr>& all_selected_gids, Ice::Long zone_id ,int & policy_type,int need_count) {

  int policy = 1;
  double reserve_policy_ratio = 0;
  string ratio = EngineParametersCache::instance().GetParameterByName("reserve_policy_ratio");
  try {
    if(ratio != "") {
      reserve_policy_ratio = boost::lexical_cast<double>(ratio);
    }
  } catch (Ice::Exception& e) {
    MCE_WARN("UserIndex::RandomSelectAdgroup para transform error : " << e.what()); 
  } catch (...) {
    MCE_WARN("UserIndex::RandomSelectAdgroup para transform error : unkown error \"...\"");
  }
  int policy_randnum = rand() % 100;
  if(policy_randnum < reserve_policy_ratio) {
    policy = 2;
  }
  policy_type = policy;
  MCE_DEBUG("UserIndex::RandomSelectAdgroup-->bid gid size:" << bid_gids.size() << ",all_selected_gids size:" << all_selected_gids.size() 
      << ",adzone:" <<zone_id <<",need_count:" << need_count << ",reserve_policy_ratio:" << reserve_policy_ratio <<",policy_randnum:" << policy_randnum);

  set<AdGroupPtr> selected_gids(all_selected_gids.begin(), all_selected_gids.end());
  vector<AdGroupPtr> result;
  if(1 == policy) {
    MCE_DEBUG("UserIndex::RandomSelectAdgroup-->bid gid size:" << bid_gids.size() << ",all_selected_gids size:" <<
        all_selected_gids.size() << ",adzone:" << zone_id << ",need_count:" << need_count << ",policy:" << policy <<
        ",rand:" << policy_randnum);
    for(int i = 0; i < need_count; ++i) {
      vector<pair<double, AdGroupPtr> > sort_gids;
      double total = 0;
      for(set<AdGroupPtr>::iterator itb = bid_gids.begin(); itb != bid_gids.end(); ++itb) {
        MCE_DEBUG("RandomSelectAdGroup-->gid:" << (*itb)->group_id() <<",w:" << (*itb)->max_price(zone_id));
        if(!IsCompetedGroup((*itb), selected_gids)){
          double quality_score =  AdsQualityCache::instance().GetAdQuScore((*itb)->group_id(),zone_id);  // 获取score
       		//部分二跳计价用户ecpm打折
       		double discount = DiscountCache::instance().GetDiscountRatio((*itb)->member_id());
       		double w = (*itb)->max_price(zone_id) * discount * quality_score;
       		//double w = (*itb)->max_price(zone_id);
       		if((*itb)->member_id() == 100000017330) {
       			w = 0.00001;
       		}
       		if((*itb)->IsCpm())	{	//cpm
       			w *= 10;
       		}
       		sort_gids.push_back(make_pair(w, (*itb)));
       		total += w;
       	}
      }
      if(total < 0) {
        break;
      }
      double randnum = ((double)(rand() % 1000) / 1000.0) * total;
      double sum = 0;
      for(vector<pair<double, AdGroupPtr> >::iterator its = sort_gids.begin(); its != sort_gids.end(); ++its) {
        sum += (*its).first;
        if(randnum <= sum) {
          result.push_back((*its).second);
          selected_gids.insert((*its).second);
          break;
        }
      }
    }
  }else {//按照ecpm 取topN

    MCE_DEBUG("UserIndex::RandomSelectAdgroup-->bid gid size:" << bid_gids.size() << ",all_selected_gids size:" <<
        all_selected_gids.size() << ",adzone:" << zone_id << ",need_count:" << need_count << ",policy:" << policy <<
        ",rand:" << policy_randnum);

    for(int i = 0; i < need_count; ++i) {
      AdGroupPtr m_ptr = NULL;
      double max_ecpm = -1;
      for(set<AdGroupPtr>::iterator it = bid_gids.begin(); it != bid_gids.end(); ++it) {
        if(!IsCompetedGroup((*it), selected_gids)) { //是否与已选存在竞争，不存在进行下边操作,即：找最大的ecpm
          pair<double,int> ctr_state = UserDemographicCache::instance().GetUserAdGroupsCtr(user_profile, zone_id,
              i+1, (*it)->group_id(),(*it)->member_id(),"");
          double quality_score =  AdsQualityCache::instance().GetAdQuScore((*it)->group_id(),zone_id);  // 获取score
          if((*it)->IsCpc())	{	//cpc广告
            //部分二跳计价的用户ecpm打折
            double discount = DiscountCache::instance().GetDiscountRatio((*it)->member_id());
            double ecpm = ctr_state.first * ((*it)->max_price(zone_id)) * discount * quality_score;
            //  double ecpm = ctr_state.first * ((*it)->max_price(zone_id));
            if((*it)->member_id() == 100000017330) {
              ecpm *= 0.00001;
            }
            if(ecpm > max_ecpm) {
              max_ecpm = ecpm;
              m_ptr = *it;
              ///ctr = ctr_state.first;//TODO打入点击率信息
            }
            MCE_DEBUG("uid=" << user_profile.id() << "'debug info:gid:" <<(*it)->group_id()<< " ctr=" << ctr_state.first<<",pos:" 
                <<i+1 <<" cost=" << (*it)->max_price(zone_id) <<" ecpm=" << ecpm );
          }
          else if((*it)->IsCpm())	{	//cpm广告
            double ecpm = (*it)->max_price(zone_id) / 1000.0 * quality_score;
            if(ecpm > max_ecpm) {
              max_ecpm = ecpm;
              m_ptr = *it;
            }

          }
        }
      }
      if(m_ptr) {
        result.push_back(m_ptr);
        selected_gids.insert(m_ptr);
      }    
    }    
  }
  return result;
}

vector<pair<AdGroupPtr,int> > UserIndex::RandomSelectCpmAdgroup(vector<pair<AdGroupPtr,CpmAdgroup> >& bid_gids, 
  set<AdGroupPtr>& all_selected_gids,int need_count) {
  MCE_DEBUG("UserIndex::RandomSelectCpmAdgroup-->bid cpm gid size:" << bid_gids.size() << ",all_selected_gids size:"
  << all_selected_gids.size() << ",need_count:" << need_count);

  set<AdGroupPtr> selected_gids(all_selected_gids.begin(), all_selected_gids.end());
  vector<pair<AdGroupPtr,int> > result;
  for(int i = 0; i < need_count; ++i) {
    vector<pair<double, pair<AdGroupPtr,CpmAdgroup> > > sort_gids;
    double total = 0;
    for(vector<pair<AdGroupPtr,CpmAdgroup> >::iterator itb = bid_gids.begin(); itb != bid_gids.end(); ++itb) {
      MCE_DEBUG("RandomSelectCpmAdGroup-->gid:" << (*itb).first->group_id() <<",w:" << (*itb).second.cpm);
      if(!IsCompetedGroup((*itb).first, selected_gids)){
        double w = (*itb).second.cpm;

        sort_gids.push_back(make_pair(w,(*itb)));
        total += w;
      }
    }
    if(total < 0) {
      break;
    }
    double randnum = ((double)(rand() % 1000) / 1000.0) * total;
    double sum = 0;
    for(vector<pair<double, pair<AdGroupPtr,CpmAdgroup> > >::iterator its = sort_gids.begin(); its != sort_gids.end(); ++its) {
      sum += (*its).first;
      if(randnum <= sum) {
        result.push_back(make_pair((*its).second.first,(*its).second.second.charge));
        selected_gids.insert((*its).second.first);
        break;
      }
    }
  }
  return result;
}

string GetCpmKey(Ice::Long zone_id, Ice::Long group_id) {
  stringstream s;
  s << group_id << "_" << zone_id;
  return s.str();
}

string GetCpmMemberKey(Ice::Long zone_id, Ice::Long member_id) {
  stringstream s;
  s << member_id << "_member_" << zone_id;
  return s.str();
}

bool RandomDisableCpm(string reduce_ratio_key){
  int cpm_reduce_ratio = 0;
  string cpm_reduce_ratio_str = EngineParametersCache::instance().GetParameterByName(reduce_ratio_key);
  if(cpm_reduce_ratio_str != "") {
    cpm_reduce_ratio = (boost::lexical_cast<int>(cpm_reduce_ratio_str));
  }
  int randnum = rand() % 100; 
  return (randnum < cpm_reduce_ratio);
}

vector<pair<AdGroupPtr,int> > UserIndex::SelectCpmMemberAdgroup(MatchHandlerParamter& para, set<AdGroupPtr>& groups, 
    Ice::Long adzone, int count) { 
  
  MCE_DEBUG("SelectCpmMemberAdgroup-->get Ads--> bid gids size:" << groups.size()<<",zone:" << adzone << ",count:" << count);
  vector<pair<AdGroupPtr,int> > result;
  CpmAdgroupMap cpm_pool = CpmLoaderCache::instance().GetCpmAdGroupMap();
  set<AdGroupPtr> cpm_member_groups; 
  for(set<AdGroupPtr>::iterator itg = groups.begin(); itg != groups.end();) {//查找符合可投的按广告商投放的广告
    CpmAdgroupMap::iterator itc;
    MCE_DEBUG("SelectCpmMemberAdgroup bid gids for user,bid gid: " << (*itg)->group_id()<<" member_id: " << (*itg)->member_id()<< ",zone: " << adzone<<",cpm pool size: " << cpm_pool.size());
    stringstream s;
    s << (*itg)->member_id();
    itc = cpm_pool.find(s.str());
    if(itc != cpm_pool.end()) {
      string key = GetCpmMemberKey(adzone, (*itg)->member_id());  
      itc = cpm_pool.find(key);
      if(itc != cpm_pool.end()) {
        CpmAdgroup cpm_group = itc->second;
        if(cpm_group.pv > 0 && !IsCompetedGroup((*itg), para.all_selected_groups_)) {//如果还需要保量,并且和其他没有竞品关系
          cpm_member_groups.insert(*itg);
          MCE_DEBUG("SelectCpmMemberAdgroup find cpm by member_id,cpm gid:" << (*itg)->group_id()<<",zone:" << adzone<<",cpm_cadidate size: " << cpm_member_groups.size());
        }
      }
      
      groups.erase(itg++);
      MCE_DEBUG("SelectCpmMemberAdgroup find cpm by member_id = " << s.str() << " erase it");
    } else {
      MCE_DEBUG("SelectCpmMemberAdgroup can't find cpm by member_id = " << s.str());
      ++itg;
    }
  }
  int policy = 0;
  
  vector<AdGroupPtr> selected_ids =  RandomSelectAdgroup(para.userProfile_, cpm_member_groups, para.all_selected_groups_, adzone, policy, count);
  MCE_DEBUG("SelectCpmAdgroup cpm bid gid size:" << selected_ids.size()<< " cpm_member_groups size:"<< cpm_member_groups.size()<<",adzone:" << adzone);
  
  for (vector<AdGroupPtr>::iterator itg = selected_ids.begin(); itg != selected_ids.end(); ++itg) {
    result.push_back(make_pair((*itg), (*itg)->max_price(adzone))); 
  }
  MCE_DEBUG("SelectCpmAdgroup cpm member result size:" << result.size());
  return result;  
}

vector<pair<AdGroupPtr,int> > UserIndex::SelectCpmAdgroup(set<AdGroupPtr>& groups, set<AdGroupPtr>& selected_groups,
    Ice::Long adzone,int count) {
  
  MCE_DEBUG("SelectCpmAdgroup-->get Ads--> bid gids size:" << groups.size()<<",zone:" << adzone << ",count:" << count);
  vector<pair<AdGroupPtr,int> > result; 
  vector<pair<AdGroupPtr,CpmAdgroup> > cpm_cadidate_groups;
  CpmAdgroupMap cpm_pool = CpmLoaderCache::instance().GetCpmAdGroupMap();

  //////查找按广告位保量cpm广告
  for(set<AdGroupPtr>::iterator itg = groups.begin(); itg != groups.end();) {//查找符合定投的cpm广告
    stringstream s;
    CpmAdgroupMap::iterator itc;

    MCE_DEBUG("SelectCpmAdgroup bid gids for user,bid gid:" << (*itg)->group_id()<<",zone:" << adzone<<",cpm pool size:" << cpm_pool.size());
    s << (*itg)->group_id();
    itc = cpm_pool.find(s.str());
    if(itc != cpm_pool.end()) {
      MCE_DEBUG("SelectCpmAdgroup find cpm for user,cpm gid:" << (*itg)->group_id()<<",zone:" << adzone);
      string key = GetCpmKey(adzone,(*itg)->group_id());
      itc = cpm_pool.find(key);
      if(itc != cpm_pool.end()) {
        CpmAdgroup cpm_group = itc->second;

        MCE_DEBUG("SelectCpmAdgroup find cpm by zone,cpm gid:" << (*itg)->group_id()<<",zone:" << adzone);
        if(cpm_group.pv > 0 && !IsCompetedGroup((*itg), selected_groups)) {//如果还需要保量,并且和其他没有竞品关系
          cpm_cadidate_groups.push_back(make_pair((*itg),cpm_group));
          MCE_DEBUG("SelectCpmAdgroup find cpm by zone insert,cpm gid:" << (*itg)->group_id()<<",zone:" << adzone<<",cpm_cadidate size: " << cpm_cadidate_groups.size());
        }
      }
      key = GetCpmKey(-1,(*itg)->group_id());
      itc = cpm_pool.find(key);
      if(itc != cpm_pool.end()) {
        CpmAdgroup cpm_group = itc->second;
        MCE_DEBUG("SelectCpmAdgroup find cpm by all,cpm gid:" << (*itg)->group_id()<<",zone;" << adzone);
        if(cpm_group.pv > 0 && !IsCompetedGroup((*itg), selected_groups)) {//如果还需要保量,并且和其他没有竞品关系
          cpm_cadidate_groups.push_back(make_pair((*itg),cpm_group));
          MCE_DEBUG("SelectCpmAdgroup find cpm by all insert,cpm gid:" << (*itg)->group_id()<<",zone:" << adzone<<",cpm_cadidate size: " << cpm_cadidate_groups.size());
        }
      }
      MCE_DEBUG("SelectCpmAdgroup test erase,line:" << __LINE__);
      groups.erase(itg++);
      MCE_DEBUG("SelectCpmAdgroup test erase,line:" << __LINE__);
    }else{
      ++itg;
    }
  }
  MCE_DEBUG("SelectCpmAdgroup cpm bid gid size:" << cpm_cadidate_groups.size()<<",adzone:" << adzone);
  //////按照策略选择需要投放的cpm广告
  if((!cpm_cadidate_groups.empty()) && (count > 0)){
    result = RandomSelectCpmAdgroup(cpm_cadidate_groups, selected_groups,count);
  }
  return result;
}

SelectedAdGroupSeq UserIndex::GetZoneGroups4Reserve(MatchHandlerParamter& para, long adzone, int count) {
  MCE_DEBUG("GetZoneGroups4Reserve-->get ads,userid" << para.userProfile_.id() << ", adzone" << adzone <<",pick count" << count);
  const UserProfile & user_profile = para.userProfile_;
  int uid = para.userProfile_.id();
  long zone_id = para.adzone_->id();
  SelectedAdGroupSeq result;
  int pos = 1;
  bool debug_flag = (EngineConfig::instance().GetUserId() == user_profile.id());
  set<AdGroupPtr> groups = UserAdMatcher::instance().GetAds(const_cast<UserProfile&>(user_profile), adzone);
  if(adzone == 100000000075) {
    FilterTaobao(groups);
  }

  vector<pair<AdGroupPtr,int> > cpm_selected_groups;//选中的广告和需要扣的费
  vector<pair<AdGroupPtr,int> > null_cpm_selected_groups; //空集合，不要删掉
  vector<AdGroupPtr> selected_ids;

  int cpm_count = count;
  if(adzone == 100000000075){
    cpm_count = 1;
  }
  //先去选member保量的
   
  if (RandomDisableCpm("cpm_member_reduce_ratio")){
    null_cpm_selected_groups = SelectCpmMemberAdgroup(para, groups, zone_id, 0);
  } else {   
    cpm_selected_groups = SelectCpmMemberAdgroup(para, groups, zone_id, cpm_count);
  }
  // vector<pair<AdGroupPtr,int> > cpm_groups = GetCpmAdgroups(bid_groups, groups, zone_id);
  if (cpm_selected_groups.empty()){
    if (RandomDisableCpm("cpm_adgroup_reduce_ratio")){
      null_cpm_selected_groups = SelectCpmAdgroup(groups, para.all_selected_groups_, zone_id, 0);
    } else {  
      //两个功能 1、从用户定向广告中过滤掉要保量的广告组；2、选择要保量的广告组,和对应的扣费
      cpm_selected_groups = SelectCpmAdgroup(groups, para.all_selected_groups_, zone_id, cpm_count);
    }
  } else {
    //从用户定向广告中过滤掉要保量的广告
    null_cpm_selected_groups = SelectCpmAdgroup(groups, para.all_selected_groups_, zone_id, 0);
  }
  MCE_DEBUG("GetZoneGroups4Reserve select cpm ads size:" << cpm_selected_groups.size() <<",bid groups" << groups.size() 
  <<",zone" << zone_id <<",cpm_count" << cpm_count);

  count = cpm_count - cpm_selected_groups.size();//扣除已经选中的cpm广告数

  for(vector<pair<AdGroupPtr,int> >::iterator itc = cpm_selected_groups.begin(); itc != cpm_selected_groups.end(); ++itc) {
    AdGroupPtr m_ptr = (*itc).first;
    SelectedAdGroupPtr selected = new SelectedAdGroup(m_ptr);
    double max_cost = m_ptr->calculate_cost(zone_id);
    if((*itc).second > max_cost) {
      selected->cost_ = max_cost;
    } else {
      selected->cost_ = (*itc).second;
    }
    selected->pos_ = pos++;
    selected->demo_state_ = (adzone % 100) * 1000 + selected->pos_ ;
    para.all_selected_groups_.insert(m_ptr);
    MCE_DEBUG("select cpm gid:" << selected->group_->group_id() <<",widget id:" <<m_ptr->widget_id() <<",cost:" 
    << selected->cost_<<",max price:" << m_ptr->max_price(adzone));
    result.push_back(selected);
    if(debug_flag) {
      MCE_INFO("select cpm uid=" << user_profile.id() << "'debug info:" << "[" << selected->pos_ << "] id" << selected->group_->group_id()  <<
          " cost=" << selected->cost_ <<",state:" <<selected->demo_state_<<",pos:" << selected->pos_ <<",zone:"
          <<adzone);
    }
    MCE_DEBUG("select cpm uid=" << user_profile.id() <<",zone:" << adzone << ",pos:" << selected->pos_ <<"selected gid:" << selected->group_->group_id() <<",state:" <<selected->demo_state_<<",widdget:" << m_ptr->widget_id() << ",cost:" << selected->cost_ <<",maxprice:" << m_ptr->max_price(adzone));
  }

  int big_pic_id = EngineConfig::instance().big_pic_id();
  big_pic_id = 69;
  //big_pic_id = 87;//TO DO for QAtest
  int ratio_big_75 = EngineConfig::instance().big_pic_ratio_75();

  if(count > 0) {
    int policy = 0;
    bool is_vip = true;
    if(adzone == 100000000075) {
      set<AdGroupPtr> groups_75_big;
      set<AdGroupPtr> groups_75_little;
      int randnum = rand()% 100;
      //randnum = 80;
      for(set<AdGroupPtr>::iterator itg = groups.begin(); itg != groups.end(); ++itg) {
        if(!IsCompetedGroup((*itg), para.all_selected_groups_)) {//预过滤竞品
          if((*itg)->widget_id() != big_pic_id) {
            groups_75_little.insert(*itg);
          } else {
            groups_75_big.insert(*itg);
          }
        }
      }
      MCE_DEBUG("GetZoneGroups4Reserve-->bid big size:" << groups_75_big.size() << ",little size:" << groups_75_little.size()<<",random:" << randnum <<",big_pic_ratio_75:" << ratio_big_75);
      if(groups_75_big.size() > 0 && groups_75_little.size() > 0) { 
        //set<AdGroupPtr> test_gid;
        if(randnum > ratio_big_75) {//选取小图count个
          is_vip = false;
          GameReduce(groups_75_little, is_vip, 3, zone_id);
          selected_ids = RandomSelectAdgroup(para.userProfile_, groups_75_little, para.all_selected_groups_, zone_id, policy,3);
          //selected_ids = RandomSelectAdgroup(groups_75, test_gid, zone_id, count);

        } else { //选取大图1个
          selected_ids = RandomSelectAdgroup(para.userProfile_, groups_75_big, para.all_selected_groups_, zone_id, policy,1);
        }
      } else if (groups_75_big.size() > 0 && groups_75_little.size() == 0 ) {
        GameReduce(groups_75_big, is_vip, 1, zone_id);
        selected_ids = RandomSelectAdgroup(para.userProfile_, groups_75_big, para.all_selected_groups_, zone_id, policy,1);
      } else if(groups_75_big.size() == 0) {
        is_vip = false;
        GameReduce(groups_75_little, is_vip, 3, zone_id);
        selected_ids = RandomSelectAdgroup(para.userProfile_, groups_75_little, para.all_selected_groups_, zone_id, policy,3);
      }
    } else{
      GameReduce(groups, is_vip, count, zone_id);
      selected_ids = RandomSelectAdgroup(para.userProfile_, groups, para.all_selected_groups_ ,zone_id, policy,count);
    }
    MCE_INFO("UserIndex::GetZoneGroups4Reserve-->user " << user_profile.id() << " zone " << adzone << " matched ad count " << groups.size()<<",debug_flag" << debug_flag);
    //int pos = 1;
    for(vector<AdGroupPtr>::iterator its = selected_ids.begin(); its != selected_ids.end(); ++its) {
      AdGroupPtr m_ptr = *its;
      SelectedAdGroupPtr selected = new SelectedAdGroup(m_ptr);
      selected->cost_ = m_ptr->calculate_cost(zone_id);
      selected->pos_ = pos++;
      selected->demo_state_ = 100000 + policy;
      para.all_selected_groups_.insert(m_ptr);
      MCE_DEBUG("select result widget test,gid:" << selected->group_->group_id() <<",widget id:" <<m_ptr->widget_id() <<",cost:" << selected->cost_
          <<",max price:" << m_ptr->max_price(adzone));
      result.push_back(selected);
      if(debug_flag) {
        MCE_INFO("GetZoneGroups4Reserve uid=" << user_profile.id() << "'debug info:" << "[" << selected->pos_ << "] id" << selected->group_->group_id()  <<
            " cost=" << selected->cost_ <<",state:" <<selected->demo_state_<<",pos:" << selected->pos_ <<",zone:"
            <<adzone);
      }
      MCE_DEBUG("GetZoneGroups4Reserve uid=" << user_profile.id() <<",zone:" << adzone << ",pos:" << selected->pos_ <<"selected gid:" << selected->group_->group_id() <<",state:" <<selected->demo_state_<<",widdget:" << m_ptr->widget_id() << ",cost:" << selected->cost_ <<",maxprice:" << m_ptr->max_price(adzone));
    }
  }
  return result;
}

void UserIndex::FilterTaobao(set<AdGroupPtr>& bid_groups){
  for(set<AdGroupPtr>::iterator it = bid_groups.begin(); it != bid_groups.end(); ) {
    if((*it)->member_id() == 100000017431) {
      bid_groups.erase(it++);
    }else{
      ++it;
    }
  }
}

void UserAdMatcher::FilterCpm(set<AdGroupPtr>& bid_groups){
  for(set<AdGroupPtr>::iterator it = bid_groups.begin(); it != bid_groups.end(); ) {
    if((*it)->IsCpm()) {
      bid_groups.erase(it++);
    }else{
      ++it;
    }
  }
}

SelectedAdGroupSeq UserIndex::GetZoneGroups3(MatchHandlerParamter& para, long adzone, int count) {
  bool is_reserved = false;
  if(para.tag_ != "" && para.tag_.find("REMAIN") != string::npos && (adzone == 100000000070 || adzone == 100000000072 || adzone == 100000000075)){
    is_reserved = true;
  }
  //if(adzone == 100000000075) {//接余量
  //  adzone = 100000000072;
  //  MCE_INFO("GetZoneGroups3-->old zone:" << 100000000075 <<",changed to zone:" << adzone);
  //}
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
  FilterTaobao(bid_groups);
  // filter cpm member
  vector<pair<AdGroupPtr,int> > null_cpm_selected_groups = SelectCpmMemberAdgroup(para, bid_groups, adzone, 0);
  if(adzone == 100000000070 || adzone == 100000000072) {
    //type1:自助小横图、自助大图 type2:VIP客户的新素材
    set<AdGroupPtr> groups_type1;
    set<AdGroupPtr> groups_type2;
    for(set<AdGroupPtr>::iterator itb = bid_groups.begin(); itb != bid_groups.end(); ++itb) {
      Ice::Long widget_id = (*itb)->widget_id();
      if(Widget2TypeCache::instance().GetWidgetType(widget_id) == 1) {
        groups_type1.insert(*itb);
        MCE_DEBUG("WIDGET_TYPE: adgroup_id:" << (*itb)->group_id() <<" widget_id:"<< widget_id << " TYPE1");
      } else if((Widget2TypeCache::instance().GetWidgetType(widget_id) == 2) &&
                (!IsCompetedGroup((*itb), para.all_selected_groups_))) { //vip新尺寸没有默认，先过滤竞品
        MCE_DEBUG("WIDGET_TYPE: adgroup_id:" << (*itb)->group_id()<< " widget_id:"<< widget_id << " TYPE2");
        groups_type2.insert(*itb);
      }
    }

    double ratio_70 = 0;
    string new_material_ratio_70 = EngineParametersCache::instance().GetParameterByName("new_material_ratio_70");
    double ratio_72 = 0;
    string new_material_ratio_72 = EngineParametersCache::instance().GetParameterByName("new_material_ratio_72");
    if(new_material_ratio_70 != "") {
      ratio_70 = boost::lexical_cast<double>(new_material_ratio_70);
    }
    if(new_material_ratio_72 != "") {
      ratio_72 = boost::lexical_cast<double>(new_material_ratio_72);
    }
    int new_material_randnum = rand() % 100;
    MCE_DEBUG("[new_material]randnum = " << new_material_randnum << ",ratio_70:" << ratio_70
        << ",ratio_72:" << ratio_72 );
    if((adzone == 100000000070 && new_material_randnum < ratio_70) ||
        (adzone == 100000000072 && new_material_randnum < ratio_72)) {
      if(groups_type2.size() > 0) {
        bid_groups = groups_type2;
        count = 1;
        para.is_selected_vip_= true;
      } else {
        bid_groups = groups_type1;
      }
    } else {
        bid_groups = groups_type1;
    }
  }
  //game reduce
  if(100000000001 != adzone) {
    bool is_vip = false;
    if(para.is_selected_vip_ == true) {
      is_vip = true;
    }
    GameReduce(bid_groups, is_vip, count, adzone); 
  }
  set<AdGroupPtr> groups;
  int big_pic_id = EngineConfig::instance().big_pic_id();
  int randnum = rand() % 101; 
  int big_widget_reduce_ratio = BigWidgetReduceRatioCache::instance().GetBigWidgetReduceRatio(adzone);
  MCE_DEBUG("randnum = " << randnum << "reduce_ratio = " << big_widget_reduce_ratio);
  bool delete_big_flag = false; 
  for(set<AdGroupPtr>::iterator itb = bid_groups.begin(); itb != bid_groups.end(); ++itb) {
  
   /* if(adzone == 100000000072 && para.photo_ && (*itb)->widget_id() == big_pic_id ) {//大图余量72不投放
      continue;
    }*/
    if(para.is_selected_vip_) {//如果是VIP新尺寸素材不参加大小图过滤
      groups.insert(*itb);
      continue;
    }
    if (big_widget_reduce_ratio > 0 && randnum <= big_widget_reduce_ratio &&  (*itb)->widget_id() == big_pic_id) {
      delete_big_flag = true;
      continue;
    } 
    
    groups.insert(*itb);
  }
    
  MCE_DEBUG("randnum = " << randnum);
  if (groups.size() < count && delete_big_flag == true) { //广告数量不够，且屏蔽过大图
    Ice::Long rollback_key = 123456789;
    int rollback = BigWidgetReduceRatioCache::instance().GetBigWidgetReduceRatio(rollback_key);
    MCE_DEBUG("rollback = " << rollback);
    if (rollback == 1) { //需要召回大图
      for(set<AdGroupPtr>::iterator itb = bid_groups.begin(); itb != bid_groups.end(); ++itb) {
        if(para.photo_ && (*itb)->widget_id() != big_pic_id ) { // 只处理大图
          continue; 
        }
        /*
        if(adzone == 100000000072 && para.photo_ && (*itb)->widget_id() == big_pic_id ) {//保持大图余量72不投放
          continue;
        }
        */
        groups.insert(*itb);
      }
    }
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
          " cost=" << (*it)->max_price(adzone));
    }
  }
  ///////////////////////////////////////////////////////////////////
/*调用calculate_cost()以后用不到这两个字段了
  float cost_discount = EngineConfig::instance().GetCostDiscount();
  int rand_seed = EngineConfig::instance().GetRandSeed();
*/
  ////////////////////////////////////////////////////////////////////
  FreshAdQualityMap fresh_groups = FreshAdGroupsCache::instance().GetFreshGroups();
  set<AdGroupPtr> selected_ids;
  double price_power = EngineConfig::instance().price_power();
  double ctr_power = EngineConfig::instance().ctr_power();
  MCE_DEBUG("GetZoneGroups3,ctr_power:" << ctr_power << ",price_power:" << price_power);
  /////////////////////////////////////
  vector<int> sort_pos;
  if (adzone != 100000000001){
    sort_pos.push_back(1);
    sort_pos.push_back(2);
    sort_pos.push_back(3);
  }else{
    sort_pos = AdSortPosCache::instance().GetAppSortPos(app_name);
  }


//  for(int i = 0; i < count; ++i) {
  for(int index = 0; index < count; ++index) {
    int i = sort_pos[index] - 1;
    double max_ecpm = -1;
    AdGroupPtr m_ptr = NULL;
    int state = 0;
    double ctr = -1.0;

/////////////////////////////select fresh group////////////////////////////////////
    //m_ptr = GetZonePosFreshGroup(user_profile.id(), fresh_groups, groups, selected_ids, adzone, i+1, is_reserved); 
    m_ptr = GetZonePosFreshGroup(user_profile.id(), fresh_groups, groups, para.all_selected_groups_, adzone, i+1, is_reserved); 
    if(m_ptr) {

      SelectedAdGroupPtr selected = new SelectedAdGroup(m_ptr);
      //计价调用calculate_cost()方法
      selected->cost_ =m_ptr->calculate_cost(zone_id);
 /*   selected->cost_ = (int)((cost_discount + rand() % rand_seed / 10000.0) * (m_ptr->max_price(adzone)));
      if (selected->cost_ < EngineConfig::instance().sys_min_price()) {
        selected->cost_ = EngineConfig::instance().sys_min_price();
      } 
      if (selected->cost_ > m_ptr->max_price(adzone)) {
        selected->cost_ = m_ptr->max_price(adzone);
      }
      selected->pos_ = i+1;
      if((m_ptr->widget_id() == 31) && (selected->cost_ < 200)) {
        selected->cost_ = 200;
      }
 */
/*      if(100000000001 == adzone) { //游戏出原价
        selected->cost_ = m_ptr->max_price(adzone);
      }
 */
      selected->pos_ = i+1;
      int factor = 10;
      if(is_reserved) {
        factor = 100;
      }
      selected->demo_state_ = (adzone % 100) * factor + selected->pos_ ;
      MCE_DEBUG("select result widget test,gid:" << selected->group_->group_id() <<",widget id:" <<m_ptr->widget_id() <<",cost:" << selected->cost_
                  <<",max price:" << m_ptr->max_price(adzone));
      result.push_back(selected);
      //selected_ids.insert(m_ptr);
      para.all_selected_groups_.insert(m_ptr);
      if(debug_flag) {
        MCE_INFO("Fresh debug_uid uid=" << user_profile.id() << "'debug info:" << "[" << selected->pos_ << "] id" << selected->group_->group_id()  <<
            " cost=" << selected->cost_ <<",state:" <<selected->demo_state_<<",pos:" << selected->pos_ <<",zone:" 
            <<adzone);
      }
      MCE_INFO("[Fresh:] getfresh,uid=" << user_profile.id() <<",zone:" << adzone << ",pos:" << selected->pos_ <<"selected gid:" << selected->group_->group_id() <<",state:" <<selected->demo_state_<<",widdget:" << m_ptr->widget_id() << ",cost:" << selected->cost_ <<",maxprice:" << m_ptr->max_price(adzone));
      continue;
    }

//////////////////////////////////////////////////////////////////////////    

    for(set<AdGroupPtr>::const_iterator it = groups.begin(); it != groups.end(); ++it) {
      //set<AdGroupPtr>::const_iterator its = selected_ids.find(*it);
      //if(its == selected_ids.end()) { //是否已经选到，不在列表中进行下边操作
      if(!IsCompetedGroup((*it), para.all_selected_groups_)) { //是否与已选存在竞争，不存在进行下边操作,即：找最大的ecpm
        //double ctr = UserDemographicCache::instance().GetUserAdGroupsCtr(user_profile, adzone ,i+1 ,const_cast<AdGroupPtr&>(*it));
        pair<double,int> ctr_state = UserDemographicCache::instance().GetUserAdGroupsCtr(user_profile, adzone ,i+1 ,(*it)->group_id(),(*it)->member_id(),app_name);
        double quality_score =  AdsQualityCache::instance().GetAdQuScore((*it)->group_id(),adzone);  // 获取score
        if((*it)->IsCpc())	{	//cpc
          //部分二跳计价的用户ecpm打折          
          double discount =  DiscountCache::instance().GetDiscountRatio((*it)->member_id());
          MCE_DEBUG("group_id = "<<(*it)->group_id()<<" adzoneid = "<<adzone<<"adsquality_score = " << quality_score);
          double ecpm = pow(ctr_state.first, ctr_power) * pow((*it)->max_price(adzone), price_power) * discount * quality_score;
          //double ecpm = pow(ctr, ctr_power) * pow((*it)->max_price(), price_power);
          //          double ecpm = pow(ctr_state.first, ctr_power) * pow((*it)->max_price(adzone), price_power);
          /*if(!AdBlindness::instance().IsDisplay(user_profile.id(),(*it)->group_id())){//如果呈现屏蔽，则降低权重
            ecpm *= 0.001;
            }*/
            
          if((*it)->member_id() == 100000017330) {
            ecpm *= 0.00001;
          }
          if(ecpm > max_ecpm) {
            max_ecpm = ecpm;
            m_ptr = *it;
            state = ctr_state.second;
            ctr = ctr_state.first;//打入点击率信息
          }

          if(debug_flag) {
            MCE_INFO("uid=" << user_profile.id() << "'debug info:gid:" <<(*it)->group_id()
                << " ctr=" << ctr_state.first<<",pos:" <<i+1
                << " cost=" << (*it)->max_price(adzone) <<" ecpm=" << ecpm );
          }
        }
        else if((*it)->IsCpm())	{	//cpm
          double ecpm = (*it)->max_price(zone_id) / 1000.0 * quality_score; 
          if((*it)->member_id() == 100000017330) {
            ecpm *= 0.00001;
          }
          if(ecpm > max_ecpm) {
            max_ecpm = ecpm;
            m_ptr = *it;
            state = ctr_state.second;
          }
        }
      }
      //}
    }
    //找到该pos的最佳广告后在列表中插入广告
    if(m_ptr) {
      SelectedAdGroupPtr selected = new SelectedAdGroup(m_ptr);
      //计价调用calculate_cost()方法
      selected->cost_ =m_ptr->calculate_cost(zone_id);
/*		selected->cost_ = (int)((cost_discount + rand() % rand_seed / 10000.0) * (m_ptr->max_price(adzone)));
      if (selected->cost_ < EngineConfig::instance().sys_min_price()) {
        selected->cost_ = EngineConfig::instance().sys_min_price();
      } 
      if (selected->cost_ > m_ptr->max_price(adzone)) {
        selected->cost_ = m_ptr->max_price(adzone);
      }
      //selected->cost_ = m_ptr->max_price();
      if((m_ptr->widget_id() == 31) && (selected->cost_ < 200)) {
        selected->cost_ = 200;
      }
*/
/*      if(100000000001 == adzone) { //游戏出原价
        selected->cost_ = m_ptr->max_price(adzone);
      }
*/      
      selected->demo_state_ = state;
      selected->demo_ctr_ = ctr;
      selected->pos_ = i+1;
      
      //MCE_INFO("select result widget test,gid:" << selected->group_->group_id() <<",widget id:" <<m_ptr->widget_id() <<",cost:" << selected->cost_
      //<<",max price:" << m_ptr->max_price(adzone));
      result.push_back(selected);
      //selected_ids.insert(m_ptr);
      para.all_selected_groups_.insert(m_ptr);
      if(debug_flag) {
        MCE_INFO("debug_uid uid=" << user_profile.id() << "'debug info:" << "[" << selected->pos_ << "] id" << selected->group_->group_id()  <<" cost=" << selected->cost_ << " ecpm=" << max_ecpm<<",state:" <<selected->demo_state_<<",ctr:"<<selected->demo_ctr_<<",pos:" << selected->pos_ <<",zone:"<<adzone);
      }
      MCE_DEBUG("debug_uid uid=" << user_profile.id() << "'debug info:" << "[" << selected->pos_ << "] id" << selected->group_->group_id()  <<" cost=" << selected->cost_ << " ecpm=" << max_ecpm<<",state:" <<selected->demo_state_<<",ctr:"<<selected->demo_ctr_<<",pos:" << selected->pos_ <<",zone:"<<adzone);
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
  SelectedAdGroupSeq selected;
  try {
    //SelectedAdGroupSeq selected = user_index->GetZoneGroups2(para.userProfile_, zone_id, pick_count);
    if(zone_id != 100000000072 && zone_id != 100000000070 && zone_id != 100000000063 && zone_id != 100000000001) {//新位置上的数据
      selected = user_index->GetZoneGroups4Reserve(para, zone_id, pick_count);
    } else { //这3个位置上原本就有数据,保证原有非余量走原有策略，这些位置上的余量走原有算法
      selected = user_index->GetZoneGroups3(para, zone_id, pick_count);
    }
  }catch(...) {
    MCE_WARN("UserRankHandler::handle-->unknow err:");
  }
  //selected = user_index->GetZoneGroups3(para, zone_id, pick_count);

  MCE_DEBUG("UserRankHandler::handle:select size:" << selected.size());

  if(!selected.empty()) {
    for(SelectedAdGroupSeq::const_iterator it = selected.begin(); it != selected.end(); ++it) {
      para.zone_selected_groups_.insert(*it);
      //(*it)->pos_ = para.zone_selected_groups_.size();
      //AdBlindness::instance().AddPv(uid,(*it)->group_->group_id());
    }
    para.total_selected_groups_.insert(para.zone_selected_groups_.begin(), para.zone_selected_groups_.end());
  }
  MCE_INFO("user " << uid << " zone " << zone_id << " UserRank bugfix selected size " << selected.size() << "/" << pick_count);
  for(SelectedAdGroupSeq::iterator it = selected.begin(); it != selected.end(); ++it) {
    MCE_DEBUG("user " << uid << " zone " << zone_id << " UserRank selected group = " << (*it)->group_->group_id()<<",pos:" << (*it)->pos_);
  }
  return true;
}

set<AdGroupPtr> UserAdMatcher::GetAds(UserProfile& profile, Ice::Long adzone) {
  try {
    //return DimIndexMatcher::instance().Get(adzone, profile);
    set<AdGroupPtr> result;
    /*if(100000000072 != adzone && 100000000070 != adzone && 100000000063 != adzone && 100000000001 != adzone) {
      adzone = 100000000072;
    }*/
    AdInvertedIndex::instance().Get(profile, adzone, result);
    if (100000000063 == adzone) {
      UserAdMatcher::instance().FilterCpm(result);
    }
    //MemcacheClient::instance().Filter(profile.id(), result);
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
//////////////////////////////////////////////////////////////////////////
void BigWidgetReduceRatioCache::Load() {
  MCE_INFO("Begin: BigWidgetReduceRatioCache::Load");
  try {
    const char* db_host = "10.3.23.69";
    const char* db_name = "ad_strategy";
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql << "select adzone_id, big_reduce_ratio from big_widget_reduce_ratio";
    MCE_INFO("BigWidgetReduceRatioCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("BigWidgetReduceRatioCache::Load BigWidgetReduceRatio Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("BigWidgetReduceRatioCache::Load BigWidgetReduceRatio, adZoneRatios Size=: " << res.num_rows());
    map<Ice::Long, int> pool;
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long adzone_id = (Ice::Long) row["adzone_id"];
      int big_reduce_ratio = (int) row["big_reduce_ratio"];
      pool.insert(pair<Ice::Long, int>(adzone_id, big_reduce_ratio));
    }
    IceUtil::RWRecMutex::WLock lock(mutex_);
    reduce_ratio.swap(pool);
    MCE_INFO("End: BigWidgetReduceRatioCache::Load size = " << reduce_ratio.size());
  }catch (std::exception& e) {
    MCE_WARN("BigWidgetReduceRatioCache::Load BigWidgetReduceRatio error: " << e.what());
  } catch (...) {
    MCE_WARN("BigWidgetReduceRatioCache::Load BigWidgetReduceRatio error!");
  }
}
//////////////////////////////////////////////////////////////////////////
void TestAdImprRatioAssignCache::Load() {
  MCE_INFO("TestAdImprRatioAssignCache::Load");
  try {
    MyUtil::Date yestoday = MyUtil::Date::seconds(time(NULL) - 1 * 24 * 3600);  //昨天
    Ice::Long yestoday_time = ((yestoday.year() * 100 + yestoday.month())*100 + yestoday.day());
    Ice::Long begin_time = yestoday_time * 100;
    const char* db_host = "statdb.db.jebe.d.xiaonei.com";
    const char* db_name = "statdb";
    Connection conn(db_name,db_host,"ad","adjb###");
    stringstream sql;
    sql << "select yl_type , FLOOR(TIME/100) as day ,SUM(pv_count)as sum_impr from Log_HMZD where adzone_id in (100000000070,100000000072) and time >= "
        << begin_time << " group by adzone_id, yl_type, FLOOR(TIME/100)";
    MCE_INFO("TestAdImprRatioAssignCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("FreshAdCache::LoadAuditedOnlineFreshAd,audit gids Size=: " << res.num_rows());
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      int yl_type = (int) row["yl_type"];
      Ice::Long day = (Ice::Long) row["day"];
      Ice::Long impr = (Ice::Long) row["sum_impr"];
      if(impr > 0) { // 如果数据库有问题，按照默认或之前的数据，保证奇异值（impr=0）不出现
        if(yl_type == 0) {
          if(yestoday_time == day) {
            yestoday_unreserve_impr_ = impr;
          } else {
            today_hour_unreserve_impr_ = impr;
          }
        } else {
          if(yestoday_time == day) {
            yestoday_reserve_impr_ = impr;
          } else {
            today_hour_reserve_impr_ = impr;
          }
        }
      }
    }
  }catch (std::exception& e) {
    MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd audit error: " << e.what());
  } catch (...) {
    MCE_WARN("FreshAdCache::LoadAuditedOnlineFreshAd audit error!");
  }

  //////////////////////////预测今天的余量
  double yestoday_ratio = (double)yestoday_reserve_impr_ / (double) yestoday_unreserve_impr_;
  double today_ratio = (double)today_hour_reserve_impr_ / (double) today_hour_unreserve_impr_;
  double ratio = today_ratio / yestoday_ratio;
  today_predict_reserve_impr_ = yestoday_reserve_impr_ * ratio;
  MCE_INFO("TestAdImprRatioAssignCache::Load->over,yestoday_unre:" <<yestoday_unreserve_impr_ << ",yestoday_re:" <<yestoday_reserve_impr_
      <<",today_hour_unreserve_impr:" << today_hour_unreserve_impr_ << ",today_hour_reserve_impr:" << today_hour_reserve_impr_ <<
      ",today_predice_re_impr:" << today_predict_reserve_impr_ << ",ratio:" << ratio);
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
/*  double total_score = TotalScore();
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
      ostringstream oss_debug;
        oss_debug <<"before sorted:";
        for(vector<SelectedAdGroupPtr>::iterator itr = result.begin();itr != result.end();++itr) {
        oss_debug <<"," <<(*itr)->ecpm_;
        }
        oss_debug <<"after sorted";
    std::sort(result.begin(),result.end(),Predicate);
      for(vector<SelectedAdGroupPtr>::iterator itr = result.begin();itr != result.end();++itr) {
        oss_debug <<"," <<(*itr)->ecpm_;
        }
        MCE_DEBUG(oss_debug.str());
  }*/
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

void DiscountCache::Load() {  
  MCE_INFO("[Discount:],DiscountCache::Load start");
  try {
    //从数据库把表load过来
    const char* db_host = "10.3.23.69";
    const char* db_name = "ad_strategy";
    Connection conn(db_name,db_host,"ad","rrad");
    stringstream sql;
    sql << "select * from discount_ratio";
    MCE_INFO("DiscountCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("DiscountCache::Load DiscountCache Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("DiscountCache::Load DiscountCache, Discount_ratio Size=: " << res.num_rows());
    map<Ice::Long, double> pool;
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long member_id = (Ice::Long) row["member_id"];
      double discount_ratio = (double) row["discount_ratio"];
      pool[member_id] = discount_ratio;
    }
    IceUtil::RWRecMutex::WLock lock(mutex_);
    discount_map_.swap(pool);
    MCE_INFO("End: DiscountCache::Load size = " << discount_map_.size());

  }catch (Ice::Exception& e) {
    MCE_WARN("DiscountCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("DiscountCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e.what());
  } catch (...) {
    MCE_WARN("DiscountCache::LoadDiscountCache error!");
  }
}

void Widget2TypeCache::Load() {
  MCE_INFO("[Widget2Type:],Widget2TypeCache::Load start");
  try {
    const char* db_host = "statdb.db.jebe.d.xiaonei.com";
    const char* db_name = "jebe_main";
    Connection conn(db_name,db_host,"ad","adjb###");
    stringstream sql;
    sql << "select * from widget_type";
    MCE_INFO("Widget2TypeCache::Load try to excute " << sql.str());
    string s= sql.str();
    Query q(&conn,true,s.c_str());
    mysqlpp::StoreQueryResult res = q.store();
    if (!res) {
      MCE_WARN("Widget2TypeCache::Load Widget2TypeCache Error " << sql.str() << ".  select  fail!");
      return;
    }
    MCE_INFO("Widget2TypeCache::Load Widget2TypeCache, Widget2TypeCache Size=: " << res.num_rows());
    map<Ice::Long, int> pool;
    for (size_t i = 0; i < res.num_rows(); ++i) {
      mysqlpp::Row row = res.at(i);
      Ice::Long widget_id = (Ice::Long) row["widget_id"];
      int type = (int)row["type"];
      pool[widget_id] = type;
    }
    IceUtil::RWRecMutex::WLock lock(mutex_);
    widget2Type_map_.swap(pool);
    MCE_INFO("End: Widget2TypeCache::Load size = " << widget2Type_map_.size());

  }catch (Ice::Exception& e) {
    MCE_WARN("Widget2TypeCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e);
  } catch (std::exception& e) {
    MCE_WARN("Widget2TypeCache::load --> call Database ERR  line:"
        << __LINE__ << " err:" << e.what());
  } catch (...) {
    MCE_WARN("Widget2TypeCache::LoadDiscountCache error!");
  }
}

void UserIndex::GameReduce(set<AdGroupPtr>& bid_groups, bool is_vip, int count, long adzone_id) {
  string game_reduce_type = "game_reduce_ratio";
  if(is_vip == true) {
    game_reduce_type += "_vip";
  }
  string game_reduce_ratio = EngineParametersCache::instance().GetParameterByName(game_reduce_type);
  int game_reduce_ratio_int = 0;
  if(game_reduce_ratio != "") {
    game_reduce_ratio_int = boost::lexical_cast<int>(game_reduce_ratio);
  }
  //get reduce ratio by adzone
  ostringstream adzone;
  adzone << (adzone_id % 100);
  game_reduce_type += "_" + adzone.str();
  game_reduce_ratio = EngineParametersCache::instance().GetParameterByName(game_reduce_type);
  if(game_reduce_ratio != "") {
    game_reduce_ratio_int = boost::lexical_cast<int>(game_reduce_ratio);
  }
  int game_reduce_randnum = rand() % 100;
  MCE_DEBUG("[GameReduce]game_reduce_type:" << game_reduce_type << ",game_reduce_ratio:" 
      << game_reduce_ratio << "game_reduce_ratio_int:" << game_reduce_ratio_int << ",randnum:" << game_reduce_randnum);
  if(game_reduce_randnum < game_reduce_ratio_int) {
    MCE_DEBUG("[GameReduce]game_reduce:begin");
    set<AdGroupPtr> no_game_groups;
    for(set<AdGroupPtr>::iterator itb = bid_groups.begin(); itb != bid_groups.end(); ++itb) {
      if((*itb)->member_category() != 9) {
        no_game_groups.insert(*itb);
      }
    }
    if(no_game_groups.size() >= count) {
      MCE_DEBUG("[GameReduce]game_reduced");
      bid_groups = no_game_groups;
    }
  }

}

}
}
