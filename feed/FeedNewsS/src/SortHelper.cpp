/*
 * FeedQueueI.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#include "SortHelper.h"
#include "FeedItem.h"
#include "LogWrapper.h"
#include "QueryRunner.h"
#include "ServiceI.h"
#include "NewsUtil.h"
#include <boost/foreach.hpp>
#include "Date.h"

using namespace xce::feed;
using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using xce::feed::MergeUtil::MergeConfig;
using xce::feed::MergeUtil::FeedItemHandler;
using xce::feed::MergeUtil::PrintHelper;
using xce::feed::MergeUtil::TimeStat;
using xce::feed::SortParam;

static void FeedItem1DToLong1D(vector<FeedItem> & fw1d,Long1D & long1d){
	BOOST_FOREACH(FeedItem &fu, fw1d){
		long1d.push_back(fu.feed);
	}
}

class TimeRecord{
public:
	string ToString(){
		ostringstream oss;
		oss << " init:" << init
				<< " merge:" << merge
				<< " extract:" << extract
				<< " gather:" << gather
				<< " total:" << init+merge+extract+gather;
		return oss.str();
	}
	float init,merge,gather,push,extract;
};

SortResultPtr SortHelper::MergeAndSort(
		int uid,
		const ::xce::feed::SortParamPtr & param,
		const Ice::Current&) {
//	return new SortResult;
	TimeStat ts;
	TimeRecord tc;

	GetRequest request(uid,param);

	MergeFeed(uid,param,request);

	SortResultPtr res = new SortResult;
	Long3D & fid3d = res->fids;
	request.ExportEdmAndRandom(fid3d);

	if(request.gather_by_time_){
		GatherFeed(uid, request, fid3d);
	}else{
		GatherFeedSimple(uid, request, fid3d);
	}

	res->edmCount = request.GetEdmCount();
	MCE_INFO("SortUtil::Sort. " << request.ToString()
			<< " inputfeeds:" << param->feeds.size()
			<< " stypemerge_fw1d_:" << request.stypemerge_fw1d_.size()
			<< " fid3d:" << fid3d.size()
			<< " cost:" << tc.ToString()
	);
	return res;
}

void SortHelper::MergeFeed(
		int uid,
		GetRequest & request) {
	map<pair<int,long >, float> stypemerge_score;
	MergeByStypeMergeAndCalScore(stypemerge_score);
	if(request.ShouldPushComments()){
		ReplaceByCommentScoreIfExist(request,stypemerge_score);
	}
	MakeScoreIteratorMap(request,stypemerge_score);
}

void SortHelper::MergeByStypeMergeAndCalScore(GetRequest & request){
	map<pair<int,long >, float> stypemerge_score;
	map<pair<int, long> , FeedItem1D > stypemerge_map;
	BOOST_FOREACH(FeedItem & item,request.orig_items_){
		if(request.SeperateCommentAndRandom(item)){
			continue;
		}
		FeedItemPtr itemptr = &item;
		pair<int,long> stype_merge = FeedItemHandler::GetStypeMergeKey(item);
		map<pair<int, long> , FeedItem1D >::iterator mit =
				stypemerge_map.find(key);
		bool insertscore = false;
		if (mit != request.stypemerge_map_.end()) {
			mit->second.push_back(fu);
			if(FeedItemHandler::IsUpdateTime(item)){
				insertscore = true;
			}
		} else {
			request.stypemerge_map_[key].push_back(itemptr);
			insertscore = true;
		}
		if(insertscore){
			stypemerge_score[key] = FeedItemHandler::GetScoreTest(uid,request.sort_);
		}
	}
}

void SortHelper::ReplaceByCommentScoreIfExist(
		GetRequest & request,
		map<pair<int,long >, float> & stypemerge_score){
	BOOST_FOREACH(StypemergeMap::value_type & p, request.comments_){
		map<pair<int,long >, float>::iterator mit = stypemerge_score.find(p.first);
		if(mit != stypemerge_score.end()){
			mit->second = p.second.front().GetScoreTest();//用评论分数代替本体分数
		}
	}
}

void SortHelper::MakeScoreIteratorMap(){
	for(map<pair<int,long >, float>::iterator mit = stypemerge_score.begin();
			mit != stypemerge_score.end();++mit){
		request.score_map_.insert(make_pair(mit->second,request.stypemerge_fw1d_.find(mit->first)));
	}
}

void AppendFeedItem1dToFeedItem3D(FeedItem1D & f1d,FeedItem3D & f3d){
	FeedItem2d f2d;
	FeedItem3D.push_back(f2d);
	f3d.rbegin()->begin().push_back(f1d);
}
void AppendToLastFeedItem2D(Feed1D & item1d,int idx,FeedItem3D & item3d){
	FeedItem1D & fw1d = mit->second->second;
	Long1D fid1d;
	request.PushCommentsIfExist(fu, fid1d);
	FeedItem1DToLong1D(fw1d, fid1d);
	item3d[idx].push_back(item1d);

}
void SortHelper::GatherFeed(int uid,
		GetRequest & request,
		Long3D & fid3d){
	map<pair<int, int> , pair<time_t, int> > stypeactor_timeindex;
	for(multimap<float, StypeMerge_FW1D_Map::iterator>::reverse_iterator mit=
			request.score_map_.rbegin();mit != request.score_map_.rend();++mit){
		FeedItem1D & item1d = mit->second->second;
		FeedItemPtr itemptr = item1d.front();
		item = &itemptr;
		map<pair<int, int> , pair<time_t, int> >::iterator ti;
		if(FindSameStypeActor(item,stypeactor_timeindex,ti)){
			if(TimeDiffLessThanLimit(item.time,ti->second.first)){
				FeedItem1D & item1d = mit->second->second;
				request.item3d[ti->second.second].push_back(item1d);
				continue;
			}
		}else{
			FeedItem2D item2d;
			item2d.push_back(item1d);
			request.item3d.rbegin()->push_back(item2d);
			stypeactor_timeindex[stype_actor] = make_pair(fu.time, fid3d.size()-1);
		}
	}

	for (multimap<float, StypeMerge_FW1D_Map::iterator>::reverse_iterator mit =
			request.score_map_.rbegin(); mit != request.score_map_.rend(); ++mit) {
		FeedItem & fu = mit->second->second.front();
		pair<int, int> stype_actor = FeedItemHandler::GetStypeActorKey(fu);
		time_t gap = FeedItemHandler::GetGatherTimeGap(stype_actor.first);
		map<pair<int, int> , pair<time_t, int> >::iterator ti =
				stypeactor_timeindex.find(stype_actor);
		if (ti != stypeactor_timeindex.end()) {
			time_t diff = abs(fu.time - ti->second.first);
			if (diff < gap) {
				FeedItem1D & fw1d = mit->second->second;
				Long1D fid1d;
				request.PushCommentsIfExist(fu, fid1d);
				FeedItem1DToLong1D(fw1d, fid1d);
				fid3d[ti->second.second].push_back(fid1d);
				continue;
			}
		}
		FeedItem1D & fw1d = mit->second->second;
		AppendFeedItem1dToFeedItem3D(fw1d, fw3d);
		stypeactor_timeindex[stype_actor] = make_pair(fu.time, fid3d.size() - 1);
	}
	MCE_INFO("SortUtil::GatherFeed. stypeactor_timeindex size:" << stypeactor_timeindex.size());
}


//void SortHelper::GatherFeed(int uid,
//		GetRequest & request,
//		Long3D & fid3d){
//	map<pair<int, int> , pair<time_t, int> > stypeactor_timeindex;
//	for(multimap<float, StypeMerge_FW1D_Map::iterator>::reverse_iterator mit=
//			request.score_map_.rbegin();mit != request.score_map_.rend();++mit){
//		FeedItem & fu = mit->second->second.front();
//		pair<int, int> stype_actor = FeedItemHandler::GetStypeActorKey(fu);
//		time_t gap = FeedItemHandler::GetGatherTimeGap(stype_actor.first);
//		map<pair<int, int> , pair<time_t, int> >::iterator ti
//			= stypeactor_timeindex.find(stype_actor);
//		if (ti != stypeactor_timeindex.end()) {
//			time_t diff = abs(fu.time - ti->second.first);
//			if (diff < gap) {
//				FeedItem1D & fw1d = mit->second->second;
//				Long1D fid1d;
//				request.PushCommentsIfExist(fu,fid1d);
//				FeedItem1DToLong1D(fw1d,fid1d);
//				fid3d[ti->second.second].push_back(fid1d);
//				continue;
//			}
//		}
//		FeedItem1D & fw1d = mit->second->second;
//		AppendFeedItem1dToFeedItem3D(fw1d,fw3d);
//		stypeactor_timeindex[stype_actor] = make_pair(fu.time, fid3d.size()-1);
//	}
//	MCE_INFO("SortUtil::GatherFeed. stypeactor_timeindex size:" << stypeactor_timeindex.size());
//}

void SortHelper::GatherFeedSimple(int uid,
		GetRequest & request,
		Long3D & fid3d){
	map<pair<int, int> , pair<time_t, int> > stypeactor_timeindex;
	for(multimap<float, StypeMerge_FW1D_Map::iterator>::reverse_iterator mit=
			request.score_map_.rbegin();mit != request.score_map_.rend();++mit){
		FeedItem & fu = mit->second->second.front();
		FeedItem1D & fw1d = mit->second->second;
		Long1D fid1d;
		request.PushCommentsIfExist(fu,fid1d);
		FeedItem1DToLong1D(fw1d,fid1d);
		Long2D fid2d;
		fid2d.push_back(fid1d);
		fid3d.push_back(fid2d);
	}
	MCE_INFO("SortUtil::GatherFeed. stypeactor_timeindex size:" << stypeactor_timeindex.size());
}

float SortHelper::GetScore(int uid, const FeedItem & fu,bool sort) {
//	return fu.time;
	if(!sort){
		return float(fu.feed);
	}
	float weight = 0;
	int weight_id = FeedItemHandler::GetWeightId(fu);
	int frank = FeedItemHandler::GetFriendRank(fu);

//	float interaction = 5 * fu.reply + fu.click;//现在为0

	float interaction = 0.1f;

	int ft = int(10000 * (1 + weight_id / 100) / pow(time(NULL) - fu.time + 1,
			1.0 / (weight_id)));

	int viewcount = FeedItemHandler::GetView(fu) - 1;
	if (viewcount < 0) {
		viewcount = 0;
	}
	Ice::Long viewed_factor = 0;
	if (uid % 10 == 8) {
		viewed_factor = (Ice::Long) (pow(0.7, viewcount) * 1200);
	} else if (uid % 10 == 0) {
		viewed_factor = (Ice::Long) (pow(0.8, viewcount) * 500);
	}

	weight = float(ft + viewed_factor + pow(0.6, viewcount) * (60
			* interaction * weight_id + 10 * frank));

//	Date date(fu.time);
//	MCE_INFO("SortUtil::GetScore. fid:"<< fu.feed
//		<<" stype:" << FeedItemHandler::GetStype(fu.typeinfo)
//		<< " frank:" << frank
//		<< " wid:" << weight_id << " ft:" << ft << " viewed_factor:" << viewed_factor
//		<< " time:" << date.toDateTime() << " weight:" << weight);

	return weight;
}

float SortHelper::GetScoreTest(int uid, const FeedItem & fu,bool sort) {
//	return fu.time;
		int idx = uid % 100;
	if(!sort){
		return float(fu.feed);
	}
	float weight = 0;
	int weight_id = FeedItemHandler::GetWeightId(fu);
	int frank = FeedItemHandler::GetFriendRank(fu);

//	float interaction = 5 * fu.reply + fu.click;//现在为0

//	float interaction = 0.1f;

	int ft;
	if(idx == 3){
		ft = int(10000 / pow(time(NULL) - fu.time + 1,
				1.0 / (weight_id)));
	}else{
		ft = int(10000 * (1 + weight_id / 100) / pow(time(NULL) - fu.time + 1,
					1.0 / (weight_id)));
	}


	int viewcount = FeedItemHandler::GetView(fu) - 1;
	if (viewcount < 0) {
		viewcount = 0;
	}
	Ice::Long viewed_factor = 0;
	if(idx == 23){
		viewed_factor =  (Ice::Long) (pow(0.2, viewcount) * 1000);
//		MCE_INFO("viewed_factor:" << viewed_factor << " a:" << pow(0.2, viewcount));
	}else if(idx == 43){
		viewed_factor = (Ice::Long) (pow(0.7, viewcount) * 1000);
	}

//	if (uid % 10 == 8) {
//		viewed_factor = (Ice::Long) pow(0.7, viewcount) * 1200;
//	} else if (uid % 10 == 0) {
//		viewed_factor = (Ice::Long) pow(0.8, viewcount) * 500;
//	}

	if(idx == 63){
		frank *= 2;
	}
	weight = float(
			ft +
			viewed_factor +
			pow(0.6, viewcount) * (6 * weight_id + 10 * frank));

	if(uid == 239556923){
		Date date(fu.time);
		MCE_INFO("SortUtil::GetScore. fid:"
			<< fu.feed
			<<" stype:" << FeedItemHandler::GetStype(fu.typeinfo)
			<< " frank:" << frank
			<< " wid:" << weight_id
			<< " ft:" << ft
			<< " viewcnt:" << viewcount
			<< " viewed_factor:" << viewed_factor
			<< " time:" << date.toDateTime() << " weight:" << weight);
	}
	return weight;
}

//-------------------------------------------------

bool GetRequest::SeperateCommentAndRandom(FeedItem & fu){
	pair<int, long> key = fu::GetStypeMergeKey();
	if (MergeConfig::instance().IsComment(key.first)) {
	 	if(!show_comment_){
			return true;
		}
		key.first = MergeConfig::instance().Comment2Main(key.first);
		comments_[key].push_back(fu);
		return true;
	} else	if (show_random_ && FeedItemHandler::IsRandomType(key.first) && FeedItemHandler::GetView(fu) == 0) {
		random_fid_ = fu.GetFeedId();
		return true;
	}
	return false;
}

void GetRequest::ExportEdmAndRandom(Long3D &fid3d){
	if (show_edm_) {
		for (unsigned i = 0; i < edms_.size(); ++i) {
			Long1D fid1d;
			fid1d.push_back(edms_.at(i).feed);
			Long2D fid2d;
			fid2d.push_back(fid1d);
			fid3d.push_back(fid2d);
		}
	}
	if (show_random_ && random_fid_ != 0) {
		Long1D fid1d;
		fid1d.push_back(random_fid_);
		Long2D fid2d;
		fid2d.push_back(fid1d);
		fid3d.push_back(fid2d);
	}
}

string GetRequest::ToString(){
	ostringstream oss;
	oss << " uid_:" << uid_
			<< " last_get_time_:" << last_get_time_
			<< " random_fid_:" <<random_fid_
			<< " show_edm_:" << show_edm_
			<< " show_random_:" << show_random_
			<< " sort_:" << sort_
			<< " gather_by_time_:" << gather_by_time_
			<< " comments_ size:" << comments_.size()
			<< " edms_ size:" << edms_.size()
			<< " edms_:" << PrintHelper::Print(edms_);
	return oss.str();
}

void GetRequest::PushCommentsIfExist(
		const FeedItem & fu,
		vector<long> & fid1d){
	pair<int,long> key= FeedItemHandler::GetStypeMergeKey(fu);
	map<pair<int, long>  , FeedItem1D >::iterator mit = comments_.find(key);
	if(mit != comments_.end()){
		BOOST_FOREACH(FeedItem & fu,mit->second){
			fid1d.push_back(fu.feed);
		}
	}
}
