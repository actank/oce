/*
 * FeedQueueI.cpp
 *
 *  Created on: Feb 17, 2011
 *      Author: yejingwei
 */

#include "FeedSortI.h"
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
using xce::feed::MergeUtil::FeedWrapperHandler;
using xce::feed::MergeUtil::PrintHelper;
using xce::feed::MergeUtil::TimeStat;
using xce::feed::SortParam;
void MyUtil::initialize() {
	ServiceI& service = ServiceI::instance();
	service.getAdapter()->add(&FeedSortI::instance(), service.createIdentity("M",
			""));
}

static void FeedWrapper1DToLong1D(vector<FeedWrapper> & fw1d,Long1D & long1d){
	BOOST_FOREACH(FeedWrapper &fw, fw1d){
		long1d.push_back(fw.feed);
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

SortResultPtr FeedSortI::Sort(
		int uid,
		const ::xce::feed::SortParamPtr & param,
		const Ice::Current&) {
//	return new SortResult;
	TimeStat ts;
	TimeRecord tc;

	SortRequest request(uid,param);
	tc.init = ts.getTime();

	MergeFeed(uid,param,request);
	tc.merge = ts.getTime();

	SortResultPtr res = new SortResult;
	Long3D & fid3d = res->fids;
	request.ExportEdmAndRandom(fid3d);
	tc.extract = ts.getTime();

	if(request.gather_by_time_){
		GatherFeed(uid, request, fid3d);
	}else{
		GatherFeedSimple(uid, request, fid3d);
	}
	tc.gather = ts.getTime();

	res->edmCount = request.GetEdmCount();
	MCE_INFO("FeedSortI::Sort. " << request.ToString()
			<< " inputfeeds:" << param->feeds.size()
			<< " stypemerge_fw1d_:" << request.stypemerge_fw1d_.size()
			<< " fid3d:" << fid3d.size()
			<< " cost:" << tc.ToString()
	);
	return res;
}

void FeedSortI::MergeFeed(
		int uid,
		const SortParamPtr & param,
		SortRequest & request) {

	map<pair<int,long >, float> stypemerge_score;

	BOOST_FOREACH(FeedWrapper & fw, param->feeds)
	{
		if(request.SeperateCommentEdmRandom(fw)){
			continue;
		}

		pair<int, long> key = FeedWrapperHandler::GetStypeMergeKey(fw);
		map<pair<int, long> , FeedWrapper1D >::iterator mit =
		request.stypemerge_fw1d_.find(key);
		if (mit != request.stypemerge_fw1d_.end()) {
			mit->second.push_back(fw);
			if(FeedWrapperHandler::IsUpdateTime(fw)){
				stypemerge_score[key] = GetScore(uid,fw,request.sort_);
			}
		} else {
			request.stypemerge_fw1d_[key].push_back(fw);
			stypemerge_score[key] = GetScore(uid,fw,request.sort_);
		}
	}

	//ReplaceByCommentScoreAndPushComments(uid,request,stypemerge_score);

	for(map<pair<int,long >, float>::iterator mit = stypemerge_score.begin();
			mit != stypemerge_score.end();++mit){
		request.score_map_.insert(make_pair(mit->second,request.stypemerge_fw1d_.find(mit->first)));
	}
}

void FeedSortI::ReplaceByCommentScoreAndPushComments(
		int uid,
		SortRequest & request,
		map<pair<int,long >, float> & stypemerge_score){
	for(map<pair<int, long> , FeedWrapper1D>::iterator cit = request.comments_.begin();
			cit != request.comments_.end();++cit){
		map<pair<int,long >, float>::iterator mit = stypemerge_score.find(cit->first);
		if(mit != stypemerge_score.end()){
			mit->second = GetScore(uid,cit->second.front(),request.sort_);//用评论分数代替本体分数
			FeedWrapper1D & fw1d = request.stypemerge_fw1d_.find(cit->first)->second;
			fw1d.insert(fw1d.begin(),cit->second.begin(),cit->second.end());
		}
	}
}
void FeedSortI::GatherFeed(int uid,
		SortRequest & request,
		Long3D & fid3d){
	map<pair<int, int> , pair<time_t, int> > stypeactor_timeindex;
	for(multimap<float, StypeMerge_FW1D_Map::iterator>::reverse_iterator mit=
			request.score_map_.rbegin();mit != request.score_map_.rend();++mit){
		FeedWrapper & fw = mit->second->second.front();
		pair<int, int> stype_actor = FeedWrapperHandler::GetStypeActorKey(fw);
		time_t gap = FeedWrapperHandler::GetGatherTimeGap(stype_actor.first);
		map<pair<int, int> , pair<time_t, int> >::iterator ti
			= stypeactor_timeindex.find(stype_actor);
		if (ti != stypeactor_timeindex.end()) {
			time_t diff = abs(fw.time - ti->second.first);
			if (diff < gap) {
				FeedWrapper1D & fw1d = mit->second->second;
				Long1D fid1d;
				request.PushCommentsIfExist(fw,fid1d);
				FeedWrapper1DToLong1D(fw1d,fid1d);
				fid3d[ti->second.second].push_back(fid1d);
				continue;
			}
		}
		FeedWrapper1D & fw1d = mit->second->second;
		Long1D fid1d;
		request.PushCommentsIfExist(fw,fid1d);
		FeedWrapper1DToLong1D(fw1d,fid1d);
		Long2D fid2d;
		fid2d.push_back(fid1d);
		fid3d.push_back(fid2d);
		stypeactor_timeindex[stype_actor] = make_pair(fw.time, fid3d.size()-1);
	}
	MCE_INFO("FeedSortI::GatherFeed. stypeactor_timeindex size:" << stypeactor_timeindex.size());
}

void FeedSortI::GatherFeedSimple(int uid,
		SortRequest & request,
		Long3D & fid3d){
	map<pair<int, int> , pair<time_t, int> > stypeactor_timeindex;
	for(multimap<float, StypeMerge_FW1D_Map::iterator>::reverse_iterator mit=
			request.score_map_.rbegin();mit != request.score_map_.rend();++mit){
		FeedWrapper & fw = mit->second->second.front();
		FeedWrapper1D & fw1d = mit->second->second;
		Long1D fid1d;
		request.PushCommentsIfExist(fw,fid1d);
		FeedWrapper1DToLong1D(fw1d,fid1d);
		Long2D fid2d;
		fid2d.push_back(fid1d);
		fid3d.push_back(fid2d);
	}
	MCE_INFO("FeedSortI::GatherFeed. stypeactor_timeindex size:" << stypeactor_timeindex.size());
}



//float FeedSortI::GetScore(int uid, const FeedWrapper & fw,bool sort) {
////	return fw.time;
//	if(!sort){
//		return float(fw.feed);
//	}
//	float weight = 0;
//	int weight_id = FeedWrapperHandler::GetWeightId(fw);
//	int frank = FeedWrapperHandler::GetFriendRank(fw);
//
////	float interaction = 5 * fw.reply + fw.click;//现在为0
//
//	float interaction = 0.1f;
//
//	int ft = int(10000 * (1 + weight_id / 100) / pow(time(NULL) - fw.time + 1,
//			1.0 / (weight_id)));
//
//	int viewcount = FeedWrapperHandler::GetView(fw) - 1;
//	if (viewcount < 0) {
//		viewcount = 0;
//	}
//	Ice::Long viewed_factor = 0;
//	if (uid % 10 == 8) {
//		viewed_factor = (Ice::Long) (pow(0.7, viewcount) * 1200);
//	} else if (uid % 10 == 0) {
//		viewed_factor = (Ice::Long) (pow(0.8, viewcount) * 500);
//	}
//
//	weight = float(ft + viewed_factor + pow(0.6, viewcount) * (60
//			* interaction * weight_id + 10 * frank));
//
////	Date date(fw.time);
////	MCE_INFO("FeedSortI::GetScore. fid:"<< fw.feed
////		<<" stype:" << FeedWrapperHandler::GetStype(fw.typeinfo)
////		<< " frank:" << frank
////		<< " wid:" << weight_id << " ft:" << ft << " viewed_factor:" << viewed_factor
////		<< " time:" << date.toDateTime() << " weight:" << weight);
//
//	return weight;
//}
float FtBonus10Minutes(time_t t){
	float diff_miniutes = (float)((time(NULL) - t)) / 60;
	if(diff_miniutes > 10.0f ){ return 0; }
	return (10.0f - diff_miniutes) * 100;//最多1000
}

float FtBonus1Hour(time_t t) {
	float diff_miniutes = (float) ((time(NULL) - t)) / 60;
	if (diff_miniutes > 60.0f) { return 0; }
	return (60.0f - diff_miniutes) * (1000.0f/60);//最多1000
}
float StypeBonus(int stype,time_t t){
	if(stype != 502){ return 0; }
	float diff_hours = (float) ((time(NULL) - t)) / 3600;
//	MCE_INFO("StypeBonus." << diff_hours);
	if (diff_hours > 4.0f) { return 0; }
	return (4.0f - diff_hours) * (80);//最多1000
}
float FeedSortI::GetScore(int uid, const FeedWrapper & fw, bool sort) {
	if (!sort) { return float(fw.feed); }
	int idx = uid % 100;
	float weight = 0;
	float weight_id = FeedWrapperHandler::GetWeightId(fw);
	float frank = FeedWrapperHandler::GetFriendRank(fw);
	float ft = (10000.0f * (1 + weight_id / 100) / pow(time(NULL) - fw.time + 1, 1.0 / (weight_id)));

	float ftbonus = FtBonus10Minutes(fw.time);

	int viewcount = FeedWrapperHandler::GetView(fw) - 1;
	if (viewcount < 0) { viewcount = 0; }
	float viewed_factor = 0;
	if (idx == 3) { viewed_factor =  (pow(0.2, viewcount) * 1200); }
	else if (idx == 23) { viewed_factor = (pow(0.2, viewcount) * 800); }

//	float stypebonus = 0;
//	if(idx == 43|| uid == 239556923){ stypebonus = StypeBonus(FeedWrapperHandler::GetStype(fw.typeinfo),fw.time); }
  if (idx == 43) {
//		weight = ft + viewed_factor + pow(0.7, viewcount) * (6 * weight_id + 10 * frank + ftbonus);
		weight = ft + viewed_factor + pow(0.6, viewcount) * ( 6 * weight_id + 10 * frank );
	} else if (idx == 63) {
		weight = viewed_factor + pow(0.7, viewcount) * (ft + 6 * weight_id + 10 * frank + ftbonus);
	} else if (idx == 83) {
		weight = ft + ftbonus + viewed_factor + pow(0.6, viewcount) * (4.5 * weight_id + 10 * frank);
	} else {
		weight = ft + ftbonus + viewed_factor + pow(0.6, viewcount) * (6 * weight_id + 10 * frank);
	}

	if (uid == 239556923) {
		Date date(fw.time);
		MCE_INFO("GetScore. fid:"
				<< fw.feed
				<< " type:" << FeedWrapperHandler::GetStype(fw.typeinfo)
				<< " actor:"<< fw.actor
				<< " prefer:" << frank
				<< " wid:" << weight_id
				<< " ft:" << ft
				<< " ftbns:" << ftbonus
				<< " viewcnt:" << viewcount
				<< " viewf:" << viewed_factor
				<< " time:" << date.toDateTime() << " weight: " << weight
		);
	}
	return weight;
}

//quhao
//float FeedSortI::GetScore(int uid, const FeedWrapper & fw, bool sort) {
//	if (!sort) { return float(fw.feed); }
////	int idx = uid % 100;
//	float weight = 0;
//	float weight_id = FeedWrapperHandler::GetWeightId(fw);
//	float frank = FeedWrapperHandler::GetFriendRank(fw);
//	float ft = (10000.0f * (1 + weight_id / 100) / pow(time(NULL) - fw.time + 1, 1.0 / (weight_id)));
//
////	float ftbonus = FtBonus10Minutes(fw.time);
//
//	float viewcount = FeedWrapperHandler::GetView(fw) - 1;
//	if (viewcount < 0) { viewcount = 0; }
//	float viewed_factor = 0;
//
//	viewed_factor = (pow(0.2, int(viewcount)) * 800);
//
//	weight = ft + viewed_factor + pow(0.6, int(viewcount)) * (6 * weight_id + 10 * frank);
//
//	if (uid == 239556923) {
//		Date date(fw.time);
//		MCE_INFO("GetScore. fid:"
//				<< fw.feed
//				<< " type:" << FeedWrapperHandler::GetStype(fw.typeinfo)
//				<< " actor:"<< fw.actor
//				<< " prefer:" << frank
//				<< " wid:" << weight_id
//				<< " ft:" << ft
//				<< " viewcnt:" << viewcount
//				<< " viewf:" << viewed_factor
//				<< " time_delta:" << (time(NULL) - fw.time)
//				<< " weight: " << weight
//		);
//	}
//	return weight;
//}

//-------------------------------------------------

bool SortRequest::SeperateCommentEdmRandom(FeedWrapper & fw){
	pair<int, long> key = FeedWrapperHandler::GetStypeMergeKey(fw);
	if (MergeConfig::instance().IsComment(key.first)) {
	 	if(!show_comment_){
			return true;
		}
		pair<int,long> key = FeedWrapperHandler::GetStypeMergeKey(fw);
		key.first = MergeConfig::instance().Comment2Main(key.first);
		comments_[key].push_back(fw);
		return true;
	} else 	if(FeedWrapperHandler::IsEdm(fw)) {
	 	if(!show_edm_){
			return true;
		}
		edms_.push_back(fw);
		return true;
	}else	if (show_random_ && FeedWrapperHandler::IsRandomType(key.first) && FeedWrapperHandler::GetView(fw) == 0) {
		random_fid_ = fw.feed;
		return true;
	}
	return false;
}

void SortRequest::ExportEdmAndRandom(Long3D &fid3d){
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

string SortRequest::ToString(){
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

void SortRequest::PushCommentsIfExist(
		const FeedWrapper & fw,
		vector<long> & fid1d){
	pair<int,long> key= FeedWrapperHandler::GetStypeMergeKey(fw);
	map<pair<int, long>  , FeedWrapper1D >::iterator mit = comments_.find(key);
	if(mit != comments_.end()){
		BOOST_FOREACH(FeedWrapper & fw,mit->second){
			fid1d.push_back(fw.feed);
		}
	}
}

//float FeedSortI::GetScoreTest(int uid, const FeedWrapper & fw,bool sort) {
////	if(true || uid == 239556923){
//	return GetScoreTestV2(uid,fw,sort);
//
//	int idx = uid % 100;
//	if(!sort){
//		return float(fw.feed);
//	}
//	float weight = 0;
//	int weight_id = FeedWrapperHandler::GetWeightId(fw);
//	int frank = FeedWrapperHandler::GetFriendRank(fw);
////	float interaction = 5 * fw.reply + fw.click;//现在为0
////	float interaction = 0.1f;
//	int ft = int(10000 * (1 + weight_id / 100) / pow(time(NULL) - fw.time + 1,
//			1.0 / (weight_id)));
//
//	int ftbonus = 0;
//	if(idx == 3 || uid == 239556923){
//		ftbonus += FtBonus10Minutes(fw.time);
////		ft = int(10000 / pow(time(NULL) - fw.time + 1,
////				1.0 / (weight_id)));
////	}else{
////		ft = int(10000 * (1 + weight_id / 100) / pow(time(NULL) - fw.time + 1,
////					1.0 / (weight_id)));
//	}
//	ft += ftbonus;
//	int viewcount = FeedWrapperHandler::GetView(fw) - 1;
//	if (viewcount < 0) {
//		viewcount = 0;
//	}
//	Ice::Long viewed_factor = 0;
//	if(idx == 23){
//		viewed_factor =  (Ice::Long) (pow(0.2, viewcount) * 1000);
////		MCE_INFO("viewed_factor:" << viewed_factor << " a:" << pow(0.2, viewcount));
//	}else if(idx == 43){
//		viewed_factor = (Ice::Long) (pow(0.7, viewcount) * 1000);
//	}else if(idx == 83){
//		viewed_factor = (Ice::Long)pow(0.7, viewcount) * 1200;//FeedNews搬过来的
//	}
////	if (uid % 10 == 8) {
////		viewed_factor = (Ice::Long) pow(0.7, viewcount) * 1200;
////	} else if (uid % 10 == 0) {
////		viewed_factor = (Ice::Long) pow(0.8, viewcount) * 500;
////	}
//	if(idx == 63){
//		frank *= 2;
//	}
//	weight = float(
//			ft +
//			viewed_factor +
//			pow(0.6, viewcount) * (6 * weight_id + 10 * frank));
//	if(uid == 239556923){
//		Date date(fw.time);
//		MCE_INFO("GetScoreTest. fid:"
//			<< fw.feed
//			<<" stype:" << FeedWrapperHandler::GetStype(fw.typeinfo)
//			<< " frank:" << frank
//			<< " wid:" << weight_id
//			<< " ft:" << ft
//			<< " ftbonus:" << ftbonus
//			<< " viewcnt:" << viewcount
//			<< " viewed_factor:" << viewed_factor
//			<< " time:" << date.toDateTime() << " weight:" << weight);
//	}
//	return weight;
//}
