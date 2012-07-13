/*
 * AdRank.h
 *
 *  Created on: Sep 9, 2010
 *      Author: yejingwei
 */

#ifndef ADRANK_H_
#define ADRANK_H_

#include "AdStruct.h"
#include <set>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/tag.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <IceUtil/Shared.h>
#include <IceUtil/Handle.h>

namespace xce {
namespace ad {
using boost::multi_index_container;
using namespace boost::multi_index;
using namespace std;



//-----------------------------------------------------------------------------------
class RankStruct:public IceUtil::Shared{
public:
  RankStruct():cost(-1),score(-1),quality(-1),click(-1),pv(-1),click_rate(1),next_gid(-1), next_score(-1), next_max_bid(-1), next_quality(-1){} //－1代表cost还没有计算
  double score;
  double quality;//质量
  Ice::Long gid;
  Ice::Long member;
  Ice::Long campaign;
  int max_bid;

  int click;
  int pv;
  double click_rate;

	double next_score;
	Ice::Long next_gid;  //下家信息
	Ice::Long next_max_bid;
  double next_quality;

  AdGroupPtr group;

  int cost;

  string Print();
};
typedef IceUtil::Handle<RankStruct> RankStructPtr;
typedef vector<RankStructPtr> RankStructSeq;

//-----------------------------------------------------------------------------------

class AdRanker:public MyUtil::Singleton<AdRanker>{
public:
  RankStructSeq Rank(set<AdGroupPtr> & groups,uint64_t key,set<AdGroupPtr> & selected,int count);
  void Filter(set<AdGroupPtr> & groups,const set<AdGroupPtr> & selected);
private:

  //--------------------------------------------------------------------
  struct gid{};
  struct score{};
  struct member{};
  struct campaign{};

  typedef multi_index_container<
    RankStructPtr,
    indexed_by<
      hashed_unique<tag<gid>, BOOST_MULTI_INDEX_MEMBER(RankStruct, Ice::Long,gid)>,
      ordered_non_unique<tag<score>,BOOST_MULTI_INDEX_MEMBER(RankStruct, double,score)>,
      ordered_non_unique<tag<member>,BOOST_MULTI_INDEX_MEMBER(RankStruct, Ice::Long,member)>,
      ordered_non_unique<tag<campaign>,BOOST_MULTI_INDEX_MEMBER(RankStruct, Ice::Long,campaign)>
    >
  > RankPool;
  typedef RankPool::index<gid>::type GIndex;
  typedef RankPool::index<score>::type ScoreIndex;
  typedef RankPool::index<member>::type MemberIndex;
  typedef RankPool::index<campaign>::type CampaignIndex;
  typedef RankPool::iterator iterator;
  typedef ScoreIndex::reverse_iterator score_iterator;
  //--------------------------------------------------------------------

  //过滤原来选择过的member和竟品
  void Filter(RankPool & rank_pool,RankStructPtr & rs, double & sum);
  //构建multi_index;
  void Build(RankPool & rank_pool,set<AdGroupPtr> & groups,AdEffectPoolPtr & effects,
        int &min_bid,double & sum,double & average_clickrate,double & average_quality);
  int Cost(RankStructPtr & rank_struct ,int min_bid);
};

class CompeteRef : public MyUtil::Singleton<CompeteRef>{
public:
  CompeteRef():is_ready_(false){}
  void Init();
  void Load();
  vector<Ice::Long> Get(Ice::Long campaign);
  bool is_ready();

  class ReloadTimer: public MyUtil::Timer {
  public:
    ReloadTimer() : Timer(1000 * 60 *5) {}
    virtual void handle();
  };

private:
  map<Ice::Long,vector<Ice::Long> > ref_;
  IceUtil::RWRecMutex mutex_;
  bool is_ready_;
};

};
};
#endif /* ADRANK_H_ */
