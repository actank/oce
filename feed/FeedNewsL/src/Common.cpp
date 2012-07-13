/*
 * Common.cpp
 *
 *  Created on: Sep 27, 2011
 *      Author: yejingwei
 */

#include "Common.h"
#include "Date.h"
#include "EDMFeed.h"
#include <algorithm>
#include <limits>
#include "FeedBuilder.h"

using namespace xce::feed;
using namespace std;
using namespace MyUtil;

//int FtBonus10Minutes(time_t t){
//	float diff_miniutes = (float)((time(NULL) - t)) / 60;
//	if(diff_miniutes > 10.0f ){ return 0; }
//	return int((10.0f - diff_miniutes) * 100);//最多1000
//}
//int FtBonus1Hour(time_t t) {
//	float diff_miniutes = (float) ((time(NULL) - t)) / 60;
//	if (diff_miniutes > 60.0f) { return 0; }
//	return int((60.0f - diff_miniutes) * (1000.0f/60));//最多1000
//}
//int StypeBonus(int stype,time_t t){
//	if(stype != 502){ return 0; }
//	float diff_hours = (float) ((time(NULL) - t)) / 3600;
//	if (diff_hours > 4.0f) { return 0; }
//	return int((4.0f - diff_hours) * (80));//最多1000
//}



//set<long> & GetRequest::GetShowedFids(){
//	if(getshow_) return showed_fids_;
//
//  for(int i3d = begin_; i3d < begin_+limit_ && i3d < (int)hfeed3d_.size();++i3d ){
//  	HFeed2D & hfeed2d = hfeed3d_.at(i3d);
//  	showed_hfeed3d_.push_back(hfeed2d);
//
//    BOOST_FOREACH(HFeed1D & hfeed1d,hfeed2d){
//    	BOOST_FOREACH(HeavyFeedPtr hfeed,hfeed1d){
//    		showed_fids_.insert(hfeed->GetFid());
//    		hfeed->IncView();
//    		max_fid_ = max(max_fid_,hfeed->GetFid());
//
//    		pair<int,long> p = hfeed->GetStypeMerge();
//    		if(!MergeConfig::instance().IsMain(p.first)) continue;
//
//    		HFeed1D comments ;
//    		if(!GetComments(p,comments)) continue;
//
//    		BOOST_FOREACH(HeavyFeedPtr & hfeed ,comments){
//      		showed_fids_.insert(hfeed->GetFid());
//      		hfeed->IncView();
//    		}
//    	}
//    }
//  }
//
//  getshow_ = true;
//  return showed_fids_;
////	showids.assign(showed_fids_.begin(),showed_fids_.end());
//}

//void GetRequest::ThrowUselessFeeds(SFeed1D & sfeed1d){
//	if(sfeed1d.empty() || sfeed1d.size() == 1)
//		return;
//	//默认是从新到老排序
//	SharedFeedPtr & sfeed = *sfeed1d.begin();
//	int mergetype = sfeed->GetFeedNewsMergeType();
//	if(mergetype != FeedMergeReplace && mergetype != FeedMergeAppend){
//		MCE_WARN("GetRequest::ThrowUselessFeeds. unknown mergetype: " << mergetype
//				<< " uid:" << uid_ << " first fid:" << sfeed->GetFid() << " s:" << sfeed->GetStype() << " a:" << sfeed->GetActor() << " sfeed1d:" << sfeed1d.size()
//				);
//		mergetype = FeedMergeReplace;
//	}
//	if(mergetype == FeedMergeReplace){
//		MCE_INFO("GetRequest::ThrowUselessFeeds. replace type. "
//				<< " uid:" << uid_ << " first fid:" << sfeed->GetFid() << " s:" << sfeed->GetStype() << " a:" << sfeed->GetActor() << " sfeed1d:" << sfeed1d.size()
//		);
//		sfeed1d.erase(sfeed1d.begin(),sfeed1d.end());
//		return;
//	}
//	if(mergetype == FeedMergeAppend && sfeed1d.size() > 5){
//		MCE_INFO("GetRequest::ThrowUselessFeeds. FeedMergeAppend type. "
//				<< " uid:" << uid_ << " first fid:" << sfeed->GetFid() << " s:" << sfeed->GetStype() << " a:" << sfeed->GetActor() << " sfeed1d:" << sfeed1d.size()
//		);
//		sfeed1d.erase(sfeed1d.begin(),sfeed1d.begin() + 5);
//	}
//}

int GetRequest::GetShowedFidsFromHFeed1D(HFeed1D & hfeed1d,SFeed2D & sfeed2d){
	sfeed2d.push_back(SFeed1D());
	SFeed1D & sfeed1d = *(sfeed2d.rbegin());

	HeavyFeedPtr first_hf = *(hfeed1d.begin());
	int mergetype = first_hf->GetFeedNewsMergeType();
	if(mergetype != FeedMergeReplace && mergetype != FeedMergeAppend){
		MCE_WARN("GetRequest::GetShowedFidsFromHFeed1D. unknown mergetype: " << mergetype
				<< " uid:" << uid_ << first_hf->ToString() );
		mergetype = FeedMergeReplace;
	}

//	HFeed1D useless_hfeeds;
	vector<long> useless_fids;
	BOOST_FOREACH(HeavyFeedPtr hfeed,hfeed1d){
		hfeed->IncView();//TODO 无论这个feed有没有用到，都需要增加viewcount．还需考虑
		if((mergetype == FeedMergeReplace && sfeed1d.size() >= 1 * 2)//适当放宽条件，避免唯一的本体取不到
				|| (mergetype == FeedMergeAppend && (int)sfeed1d.size() >= FeedBuilder::MERGECOUNT )){
//			MCE_INFO("GetRequest::GetShowedFidsFromHFeed1D. uid:" << uid_ << " useless feed." << " hfeed:" << hfeed->ToString());
//			useless_hfeeds.push_back(hfeed);
			useless_fids.push_back(hfeed->GetFid());
			continue;
		}

		sfeed1d.push_back(hfeed->GetSharedFeedPtr());

		showed_fids_.insert(hfeed->GetFid());
		max_fid_ = max(max_fid_,hfeed->GetFid());

		pair<int,long> p = hfeed->GetStypeMerge();
		if(!MergeConfig::instance().IsMain(p.first)) continue;

		HFeed1D comments ;
		if(!GetComments(p,comments)) continue;

		BOOST_FOREACH(HeavyFeedPtr & hfeed ,comments){//fix
			showed_fids_.insert(hfeed->GetFid());
			showed_comment_map_[p].push_back(hfeed->GetSharedFeedPtr());
			hfeed->IncView();
		}
	}
//	if(!useless_fids.empty()){
//		MCE_INFO("GetRequest::GetShowedFidsFromHFeed1D. uid:" << uid_
//				<< " mergetype:" << mergetype
//				<< " first feed " << first_hf->ToString()
//				<< " hfeed1d size:" << hfeed1d.size()
//				<< " useless_fids count:" << useless_fids.size()
//				<< " useless fids:" << PrintHelper::Print(useless_fids));
//	}
	return useless_fids.size();
}
set<long> & GetRequest::GetShowedFids(){
	if(getshow_) return showed_fids_;

  for(int i3d = begin_; i3d < begin_+limit_ && i3d < (int)hfeed3d_.size();++i3d ){
  	HFeed2D & hfeed2d = hfeed3d_.at(i3d);
  	showed_sfeed3d_.push_back(SFeed2D());
  	SFeed2D & sfeed2d = *(showed_sfeed3d_.rbegin());
    BOOST_FOREACH(HFeed1D & hfeed1d,hfeed2d){
//    	sfeed2d.push_back(SFeed1D());
//    	SFeed1D & sfeed1d = *(sfeed2d.rbegin());

    	useless_count_ += GetShowedFidsFromHFeed1D(hfeed1d,sfeed2d);

//
//    	BOOST_FOREACH(HeavyFeedPtr hfeed,hfeed1d){
//    		sfeed1d.push_back(hfeed->GetSharedFeedPtr());
//
//    		showed_fids_.insert(hfeed->GetFid());
//    		hfeed->IncView();
//    		max_fid_ = max(max_fid_,hfeed->GetFid());
//
//    		pair<int,long> p = hfeed->GetStypeMerge();
//    		if(!MergeConfig::instance().IsMain(p.first)) continue;
//
//    		HFeed1D comments ;
//    		if(!GetComments(p,comments)) continue;
//
//    		BOOST_FOREACH(HeavyFeedPtr & hfeed ,comments){
//      		showed_fids_.insert(hfeed->GetFid());
//      		showed_comment_map_[p].push_back(hfeed->GetSharedFeedPtr());
//      		hfeed->IncView();
//    		}
//
//    	}

    }
  }

  older_count_ =std::max( int(hfeed3d_.size() - (begin_ + limit_)), 0);

  getshow_ = true;
  return showed_fids_;
//	showids.assign(showed_fids_.begin(),showed_fids_.end());
}

//HFeed3D & GetRequest::GetShowedItem3d(){
//	if(!getshow_)
//		GetShowedFids();
//	return showed_hfeed3d_;
//}
int GetRequest::GetOlderCount(){
	if(!getshow_)
		GetShowedFids();
	return older_count_;
}

SFeed3D & GetRequest::GetShowedSharedFeed3D(){
	if(!getshow_)
		GetShowedFids();

	return showed_sfeed3d_;
}

long GetRequest::GetMaxFid(){
	if(!getshow_)
		GetShowedFids();

	return max_fid_;
}

bool GetRequest::GetComments(pair<int,long> stypemerge,HFeed1D & comments){
	StypemergeHFeed1DMap::iterator cit= comment_map_.find(stypemerge);

	if(cit == comment_map_.end()) return false;

	comments = cit->second;
	return true;
}

string GetRequest::ToString(){
	ostringstream oss;
	oss << " uid_:" << uid_
			<< " sort_:" << sort_
//			<< " orig_items_:" << orig_items_.size()
			<< " comments_:" << comment_map_.size()
			<< " stypemerge_item1d_map_:"  << stypemerge_item1d_map_.size()
			<< " score_iterator_multimap_:" << score_iterator_multimap_.size()
			<< " item3d:" << hfeed3d_.size()
	;
	return oss.str();
}

void GetRequest::PushBackRandom(){
	if(random_){
		HFeed1D hfeed1d;
		hfeed1d.push_back(random_);
		HFeed2D hfeed2d;
		hfeed2d.push_back(hfeed1d);
		hfeed3d_.push_back(hfeed2d);
	}else{
	}
}

bool GetRequest::CheckArgs(){

	if(filter_style_ == kError or uid_ < 0 or begin_ < 0 or limit_ <= 0){
		MCE_WARN("GetRequest::CheckArgs. arguments invalid. uid:" << uid_
//				<< " types:" << PrintHelper::Print(types_)
				<< " begin:" << begin_
				<< " limit:" << limit_ << " hot_:" << sort_
				<< " include_types:" << PrintHelper::Print(include_types_)
				<< " exclude_types:" << PrintHelper::Print(exclude_types_)
		);
		return false;
	}
	return true;
}
void GetRequest::ClearEdms(){
	bool clear = false;
	set<long> new_edms;
	BOOST_FOREACH(FeedItem & item,edm_fitems_){
		MCE_INFO("GetRequest::ClearEdms. uid:" << uid_ << " fid:" << item.feed << " time:" << item.time);
		if(item.time < 0){
			clear = true;
			new_edms.insert(item.feed);
		}
	}
	if(clear){
		MCE_INFO("GetRequest::ClearEdms. done.  uid:" << uid_
				<< " new edms:" << PrintHelper::Print(new_edms));
		EDMFeedManager::instance().removeUser(uid_);
	}
}

void GetRequest::DumpComments(){
	BOOST_FOREACH(StypemergeHFeed1DMap::value_type & e, comment_map_){
		MCE_INFO("GetRequest::DumpComments. stype:" << e.first.first << " merge:" << e.first.second );
		BOOST_FOREACH(HeavyFeedPtr & hfeed,e.second){
			MCE_INFO("GetRequest::DumpComments. \tfid:" << hfeed->GetFid());
		}
	}
}

bool IsUgc(int stype){
	int btype = (stype & 0xFFFF) / 100 ;
	return btype == 5 or btype == 6 or btype == 7;
}

void GetRequest::RaiseUgc(){
	static const int kRaiseCount = 6;
	if(!MustRaiseUgc())
		return;
	MCE_INFO("GetRequest::RaiseUgc. before raise uid:" << uid_);
	double maxscore = score_iterator_multimap_.rbegin()->first;
	int orig = score_iterator_multimap_.size();

	vector<pair<double,StypemergeHFeed1DMap::iterator> > v;
	typedef pair<double,StypemergeHFeed1DMap::iterator>  vvt;
	BOOST_FOREACH(ScoreIteratorMultiMap::value_type & mvt, score_iterator_multimap_){
		v.push_back(make_pair(mvt.first,mvt.second));
	}
	score_iterator_multimap_.clear();
	int raised = 0;
	set<long> raised_set;
	BOOST_REVERSE_FOREACH(vvt & vt, v){
		if(raised < kRaiseCount and IsUgc(vt.second->first.first)){
			vt.first = vt.first + maxscore;
//			HeavyFeedPtr & hfeed = *(vt.second->second.begin());
			raised_set.insert((*(vt.second->second.begin()))->GetFid());
			++raised;
		}
		score_iterator_multimap_.insert(vt);
	}

	MCE_INFO("GetRequest::RaiseUgc. after raise uid:" << uid_
			<< " maxscore:" << maxscore
			<< " orig:" << orig
			<< " final:" << score_iterator_multimap_.size()
			<< " raised:" << raised
			<< " fids:" << PrintHelper::Print(raised_set)
			 )
}
