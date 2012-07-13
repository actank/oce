/*
 * Common.h
 *
 *  Created on: Sep 27, 2011
 *      Author: yejingwei
 */

#ifndef GETREQUEST_H_
#define GETREQUEST_H_
#include <list>
#include <set>
#include <Ice/Object.h>
#include <Singleton.h>
#include <LogWrapper.h>
#include <boost/foreach.hpp>
#include <boost/bimap.hpp>
#include <boost/typeof/typeof.hpp>
#include "Date.h"

#include "RFeed.h"
#include "SharedFeedCache.h"
#include "UserInfoCache.h"
using namespace std;
using namespace MyUtil;


namespace xce {
namespace feed {

typedef map<pair<int, long> , HFeedPtr1D> StypemergeHFeed1DMap;
typedef map<pair<int,long >, double> StypemergeScoreMap;
typedef multimap<double, StypemergeHFeed1DMap::iterator> ScoreIteratorMultiMap;
typedef multimap<int,pair<int,long> > TimeStypeMergeMap;
typedef map<pair<int, long> , SFeedPtr1D> StypemergeSFeed1DMap;

class GetRequest {
public:
	//参数
	int uid_;
	bool sort_;
	bool by_stype_not_btype_;
	int begin_;
	int limit_;
	set<int> types_;
	set<long> marked_fids;
	//排序用临时数据
	StypemergeHFeed1DMap comment_map_; //全部的评论
	StypemergeSFeed1DMap showed_comment_map_; //需要显示的评论
	StypemergeHFeed1DMap stypemerge_item1d_map_;
	ScoreIteratorMultiMap score_iterator_multimap_;

	SFeedPtr2D sfeed2d_;
	//计算的结果
	HFeedPtr3D hfeed3d_;//有按照时间折叠的
	HFeedPtr3D showed_hfeed3d_;
	SFeedPtr3D showed_sfeed3d_;
	HeavyFeedPtr random_;
	set<long> showed_fids_;
	vector<FeedItem> edm_fitems_;
	long max_fid_;
	bool getshow_;
	time_t nowtime_;

public:
	GetRequest(int uid, const vector<int>& types, bool stype, int begin, int limit, bool hot) {
		uid_ = uid;
		sort_ = hot;
		begin_ = begin;
		limit_ = limit;
		by_stype_not_btype_ = stype;
		types_.insert(types.begin(),types.end());
		max_fid_ = 0;
		getshow_ = false;
		nowtime_ = time(NULL);
	}
	time_t GetNowTime() const { return nowtime_;}
	bool CheckArgs();
	void AddSFeed1D(HFeedPtr1D & hfeed1d);
	bool CheckType(int stype){
		if(types_.empty()) return true;//TODO 不严谨
		if (by_stype_not_btype_) {
			return types_.count(stype);
		}	else{
			return types_.count(stype / 100 * 100);
		}
	}

	void PushBackRandom();
	void ReplaceScoreByCommentScore(StypemergeScoreMap & stypemerge_score );

	set<long> & GetShowedFids();
	HFeedPtr3D & GetShowedItem3d();
	SFeedPtr3D & GetShowedSharedFeed3D();
	SFeedPtr2D & GetShowedSharedFeed2D();
	long GetMaxFid();

	bool ShowComment()const {return sort_;}
	bool ShowEdm()const{return begin_ == 0;}
	bool IsSort()const{return sort_;};
	bool ByBlock()const {return sort_ && types_.empty();}

	bool ShowSplitter() const { return ByBlock();}

	bool HandleEdm(GetRequest & req);
	void ClearEdms();
	void ClearOldComments();
	bool GetComments(pair<int,long> stypemerge,HFeedPtr1D & comments);


	long GetOldestFid();
	void MakeItem2dAndPush(const HeavyFeedPtr & item){
		HFeedPtr1D item1d;
		item1d.push_back(item);
		HFeedPtr2D item2d;
		item2d.push_back(item1d);
		hfeed3d_.push_back(item2d); //放到顶端
	}

	bool InRange(int idx){
		return idx >= begin_ && idx< begin_+limit_;
	}

	string ToString();
	void DumpComments();
};



}
}
#endif /* COMMON_H_ */
