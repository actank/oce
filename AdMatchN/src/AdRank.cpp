/*
 * AdRank.cpp
 *
 *  Created on: Sep 10, 2010
 *      Author: yejingwei
 */

#include "AdMatchManagerI.h"
#include "AdRank.h"

using namespace xce::ad;
using namespace std;

string RankStruct::Print() {
  ostringstream oss;
  oss << "gid = " << gid << ",score = " << score << ",member = " << member
      << ",campaign = " << campaign << ",click = " << click << ",pv = " << pv
      << ",clickrate = " << click_rate << ",score = " << score << ",max_bid = "
      << max_bid << " --> " << ",next_gid = " << next_gid << ",next_score = "
      << next_score << ",next_max_bid = " << next_max_bid << ",next_quality = "
      << next_quality << " --> cost = " << cost;
  return oss.str();
}
void AdRanker::Filter(set<AdGroupPtr> & groups, const set<AdGroupPtr> & selected) {
  set<Ice::Long> compete_campaigns;
  set<Ice::Long> selected_members;

  if(selected.empty()){
    return;
  }
  //将selected_gids的竟品和相同加入compete_gids,selected_campaign;
  for (set<AdGroupPtr>::iterator sit = selected.begin(); sit != selected.end(); ++sit) {
    vector<Ice::Long> tmp = CompeteRef::instance().Get((*sit)->campaign_id());
    for (vector<Ice::Long>::iterator cit = tmp.begin(); cit != tmp.end(); ++cit) {
      compete_campaigns.insert(*cit);
    }
    selected_members.insert((*sit)->member_id());
  }

  for (set<AdGroupPtr>::iterator sit = groups.begin(); sit != groups.end();) {
    if (selected_members.count((*sit)->member_id())) {//如果已经选择过这个member
      groups.erase(sit++);
      continue;
    }
    if (compete_campaigns.count((*sit)->campaign_id())) {//如果已经被选择过
      groups.erase(sit++);
      continue;
    }
    ++sit;
  }
}

void AdRanker::Filter(RankPool & rank_pool, RankStructPtr & rs, double & sum) {
  double testsum = sum;
  MemberIndex & mindex = rank_pool.get<member> ();
  MemberIndex::iterator git = mindex.begin();
  pair<MemberIndex::iterator, MemberIndex::iterator> p = mindex.equal_range(rs->member);
  for (MemberIndex::iterator mit = p.first; mit != p.second; ++mit) {
    sum -= (*mit)->score;
    //MCE_INFO("AdRanker::Filter --> delete mit = " << (*mit)->Print() << "@ now sum = " << sum);
  }
  mindex.erase(p.first, p.second);

  vector<Ice::Long> compete_campaigns = CompeteRef::instance().Get(rs->campaign);
  //MCE_INFO("AdRanker::Filter --> compete_campaigns.size() = " << compete_campaigns.size() << " rs->campaign.id = " << rs->campaign);
  if (!compete_campaigns.empty()) {
    CampaignIndex & cindex = rank_pool.get<campaign> ();
    CampaignIndex::iterator git = cindex.begin();
    for (size_t i = 0; i < compete_campaigns.size(); ++i) {
      pair<CampaignIndex::iterator, CampaignIndex::iterator> p = cindex.equal_range(compete_campaigns.at(i));
      //MCE_INFO("AdRanker::Filter --> deal campaign : " << compete_campaigns.at(i) << " COMPETERS size = " << cindex.count(compete_campaigns.at(i)));
      for (CampaignIndex::iterator cit = p.first; cit != p.second; ++cit) {
      //  MCE_INFO("AdRanker::Filter --> delete cit = " << (*cit)->Print());
        sum -= (*cit)->score;
      }
      cindex.erase(p.first, p.second);
    }
  }
  //MCE_DEBUG("AdRanker::Filter --> rank_pool.size = " << rank_pool.size() <<" before filter sum = " << testsum << ",after sum = " << sum);
  return;

}

void AdRanker::Build(RankPool & rank_pool, set<AdGroupPtr> & groups,
    AdEffectPoolPtr & effects, int &min_maxbid, double & sum,
    double & average_clickrate, double & average_quality) {
  min_maxbid = 99999;
  sum = 0;
  average_quality = 0;
  average_clickrate = 0;
  for (set<AdGroupPtr>::iterator sit = groups.begin(); sit != groups.end(); ++sit) {
    Ice::Long gid = (*sit)->group_id();
    //MCE_DEBUG("AdRanker::Build --> before GetAdEffect");
    AdEffectPtr effect = effects->GetAdEffect(gid);
    double click_rate = effect->GetClickRate();

    int max_bid = (*sit)->max_price();
    double score = click_rate * ((double) max_bid);
    RankStructPtr rs = new RankStruct;
    rs->gid = gid;
    rs->score = score;
    rs->quality = click_rate;
    rs->campaign = (*sit)->campaign_id();
    rs->member = (*sit)->member_id();
    rs->max_bid = max_bid;
    rs->click = effect->click();
    rs->pv = effect->pv();
    rs->click_rate = click_rate;
    //MCE_INFO("AdRanker::Build --> group_id = " << (*sit)->group_id() << "click_rate = " << click_rate << " max_bid = " << (double) max_bid << " pv = " << rs->pv << " click = " << rs->click << " click_rate = " << rs->click_rate );

    rank_pool.insert(rs);

    min_maxbid = max_bid < min_maxbid ? max_bid : min_maxbid;
    sum += score;
    average_quality += rs->quality;
    average_clickrate += rs->click_rate;
  }
  if (rank_pool.size()) {
    average_quality /= rank_pool.size();
    average_clickrate /= rank_pool.size();
  } else {
    average_quality = 0.0;
    average_clickrate = 0.0;
  }
  //填写广告下家信息
  ScoreIndex & sindex = rank_pool.get<score> ();
  ScoreIndex::reverse_iterator rit = sindex.rbegin(); //从score最大的开始
  ScoreIndex::reverse_iterator nit;//用来指向下一个
  for (; rit != sindex.rend(); ++rit) {
    nit = rit;
    ++nit;
    if (nit != sindex.rend()) {
      (*rit)->next_gid = (*nit)->gid;
      (*rit)->next_score = (*nit)->score;
      (*rit)->next_quality = (*nit)->quality;
      (*rit)->next_max_bid = (*nit)->max_bid;
      //MCE_DEBUG("AdRanker::Build --> rank_struct info --> " << (*rit)->Print());
    } else {
      (*rit)->next_gid = 0;//说明rit是score最小的广告,其他项暂时不填写
      break;
      //      MCE_DEBUG("AdRanker::Build --> rank_struct info --> " << (*rit)->Print());
    }
  }
  //MCE_DEBUG("AdRanker::Build --> build done, rank_pool size = " << rank_pool.size());
}
int AdRanker::Cost(RankStructPtr & rs, int min_maxbid) {
  int cost;
  Ice::Long next_gid = rs->next_gid;
  if (next_gid != 0) {
    int term = (rs->next_quality / rs->quality) * sqrt((double) (rs->max_bid)
        * (double) (rs->next_max_bid));
    cost = max(min_maxbid, min(rs->max_bid, term));
  } else {
    cost = rs->max_bid;//TODO 最后一个广告
  }
  if (cost < 50) {
    cost = 50;
  }
  return cost;
}

RankStructSeq AdRanker::Rank(set<AdGroupPtr> & groups, uint64_t key, set<
    AdGroupPtr> & selected, int count) {
  AdEffectPoolPtr effects = AdEffectStat::instance().GetEffectPool(key);
  Filter(groups, selected);
  int min_maxbid;//保存最低出价
  double sum;//保存得分和
  double average_clickrate;
  double average_quality;//平均质量
  RankPool rank_pool;

  Build(rank_pool, groups, effects, min_maxbid, sum, average_clickrate,
      average_quality);
  //MCE_INFO("AdRanker::Rank --> After Build : rank_pool.size = " << rank_pool.size());

  RankStructSeq result;
  for (int i = 0; i < count && !rank_pool.empty(); ++i) {
    double myr = (((double) (rand() % 10000)) / (10000.0));
    double rand = myr * sum;
    double left = 0, right;
    GIndex & gindex = rank_pool.get<gid> ();
    GIndex::iterator git = gindex.begin();
    for (; git != gindex.end(); ++git) {
      right = left + (*git)->score;
      //MCE_INFO("AdRanker::Rank --> myr = " << myr << " rand = " << (double)rand << ",left = " << (double)left << ",right = " << (double)right << ",score = " << (double)((*git)->score) << " sum = " << sum);
      if (left <= rand && rand < right) {
        RankStructPtr rs = *git;
        Ice::Long gid = rs->gid;
        Ice::Long member = rs->member;
        Ice::Long campaign = rs->campaign;
        rs->cost = Cost(rs, min_maxbid);
        result.push_back(rs);
        //MCE_INFO("AdRanker::Rank --> add result, gid = " << rs->gid << " max_bid = " << rs->max_bid);
        int size_a = rank_pool.size();
        Filter(rank_pool, rs, sum);
        //MCE_INFO("AdRanker::Rank --> before filter ,size =  " << size_a << ",after size = " << rank_pool.size());
        break;//跳出第二个for循环
      } else {
        left = right;
      }
    }
  }
  //  for (int i = 0; i < result.size(); ++i) {
  //    MCE_DEBUG("AdRanker::Rank --> index --> " << i << " --> " << result.at(i)->Print());
  //  }

  return result;
}

//--------------------------------------------------------------------
void CompeteRef::Init() {
  Load();
  is_ready_ = true;
}
bool CompeteRef::is_ready() {
  return is_ready_;
}

void CompeteRef::Load() {

  Statement sql;
  sql << "select * from ad_compete_ref ";
  mysqlpp::StoreQueryResult res;
  try {
    res = QueryRunner(DB_INSTANCE, CDbRServer, "ad_compete_ref").store(sql);//TODO
  } catch (Ice::Exception& e) {
    MCE_WARN("CompeteRef::Init :" << e);
  } catch (std::exception& e) {
    MCE_WARN("CompeteRef::Init :" << e.what());
  }

  if (!res) {
    MCE_WARN("CompeteRef::Init --> StoryQueryResult wrong");
    return;
  }
  if (res.empty()) {
    MCE_WARN("CompeteRef::Init --> nothing in db ");
  }
  map<Ice::Long, vector<Ice::Long> > tmpref;
  for (size_t i = 0; i < res.size(); ++i) {
    Ice::Long cp_a = (Ice::Long) (res.at(i)["first_campaign_id"]);
    Ice::Long cp_b = (Ice::Long) (res.at(i)["second_campaign_id"]);
    tmpref[cp_a].push_back(cp_b);
    tmpref[cp_b].push_back(cp_a);
    //MCE_DEBUG("CompeteRef::Load --> add cp_a : " << cp_a << " cmp " << " cp_b : " << cp_b);
  }
  {
    IceUtil::RWRecMutex::WLock lock(mutex_);
    ref_.swap(tmpref);
    MCE_INFO("CompeteRef::Load --> load done,tmpref size = " << ref_.size());
  }

}

vector<Ice::Long> CompeteRef::Get(Ice::Long campaign) {
  if (is_ready_) {
    IceUtil::RWRecMutex::RLock lock(mutex_);
    map<Ice::Long, vector<Ice::Long> >::iterator mit = ref_.find(campaign);
    if (mit != ref_.end()) {
      //MCE_DEBUG("CompeteRef::Get --> success campaign = " << campaign << " ans.size() = " << mit->second.size());
      return mit->second;
    }
    //MCE_DEBUG("CompeteRef::Get --> FAILED no competes with campaign id = " << campaign);
  }
  return vector<Ice::Long> ();
}

void CompeteRef::ReloadTimer::handle() {
  if (!CompeteRef::instance().is_ready()) {
    MCE_INFO("CompeteRef::Get --> is not ready");
    return;
  }
  CompeteRef::instance().Load();
}
