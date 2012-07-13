

#include "GetRequest.h"
#include "Date.h"
#include "EDMFeed.h"
#include <boost/foreach.hpp>
using namespace xce::feed;
using namespace std;
using namespace MyUtil;

set<long> & GetRequest::GetShowedFids(){
	if(getshow_) return showed_fids_;
	BOOST_FOREACH(SFeedPtr1D & sfeed1d,sfeed2d_){
		BOOST_FOREACH(SharedFeedPtr & sfeed,sfeed1d){
			showed_fids_.insert(sfeed->GetFid());
		}
	}
	int normal = showed_fids_.size();
	BOOST_FOREACH(StypemergeSFeed1DMap::value_type & cmt,showed_comment_map_){
		BOOST_FOREACH(SFeedPtr1D::value_type & sfeed,cmt.second){
			showed_fids_.insert(sfeed->GetFid());
		}
	}
	int comment = showed_fids_.size() - normal;
//  for(int i3d = begin_; i3d < begin_+limit_ && i3d < (int)hfeed3d_.size();++i3d ){
//  	HFeedPtr2D & hfeed2d = hfeed3d_.at(i3d);
//  	showed_sfeed3d_.push_back(SFeedPtr2D());
//  	SFeedPtr2D & sfeed2d = *(showed_sfeed3d_.rbegin());
//    BOOST_FOREACH(HFeedPtr1D & hfeed1d,hfeed2d){
//    	sfeed2d.push_back(SFeedPtr1D());
//    	SFeedPtr1D & sfeed = *(sfeed2d.rbegin());
//    	BOOST_FOREACH(HeavyFeedPtr hfeed,hfeed1d){
//    		sfeed.push_back(hfeed->GetSharedFeedPtr());
//
//    		showed_fids_.insert(hfeed->GetFid());
//    		hfeed->IncView();
//    		max_fid_ = max(max_fid_,hfeed->GetFid());
//
//    		pair<int,long> p = hfeed->GetStypeMerge();
//    		if(!MergeConfig::instance().IsMain(p.first)) continue;
//
//    		HFeedPtr1D comments ;
//    		if(!GetComments(p,comments)) continue;
//
//    		BOOST_FOREACH(HeavyFeedPtr & hfeed ,comments){
//      		showed_fids_.insert(hfeed->GetFid());
//      		showed_comment_map_[p].push_back(hfeed->GetSharedFeedPtr());
//      		hfeed->IncView();
//    		}
//
//    	}
//
//    }
//  }
	MCE_INFO("showed_fids:" << showed_fids_.size() << " normal:" << normal << " comment:" << comment );
  getshow_ = true;
  return showed_fids_;
}

HFeedPtr3D & GetRequest::GetShowedItem3d(){
	if(!getshow_)
		GetShowedFids();
	return showed_hfeed3d_;
}

SFeedPtr3D & GetRequest::GetShowedSharedFeed3D(){
	if(!getshow_)
		GetShowedFids();

	return showed_sfeed3d_;
}
SFeedPtr2D & GetRequest::GetShowedSharedFeed2D(){
	return sfeed2d_;
}

long GetRequest::GetMaxFid(){
	if(!getshow_)
		GetShowedFids();

	return max_fid_;
}

bool GetRequest::GetComments(pair<int,long> stypemerge,HFeedPtr1D & comments){
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
		HFeedPtr1D hfeed1d;
		hfeed1d.push_back(random_);
		HFeedPtr2D hfeed2d;
		hfeed2d.push_back(hfeed1d);
		hfeed3d_.push_back(hfeed2d);
	}else{
	}
}

bool GetRequest::CheckArgs(){
	if(uid_ < 0 || begin_ < 0 || limit_ <= 0){
		MCE_WARN("GetRequest::CheckArgs. arguments invalid. uid:" << uid_
				<< " types:" << PrintHelper::Print(types_) << " begin:" << begin_
				<< " limit:" << limit_ << " hot_:" << sort_);
		return false;
	}
	return true;
}
void GetRequest::AddSFeed1D(HFeedPtr1D & hfeed1d){
	SFeedPtr1D sfeed1d;
	BOOST_FOREACH(HFeedPtr1D::value_type & hfeed, hfeed1d){
		sfeed1d.push_back(hfeed->GetSharedFeedPtr());
	}
	sfeed2d_.push_back(sfeed1d);
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
