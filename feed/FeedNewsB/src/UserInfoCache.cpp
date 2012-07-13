/*
 * UserInfo.cpp
 *
 *  Created on: Sep 27, 2011
 *      Author: yejingwei
 */

#include "UserInfoCache.h"
#include "GetRequest.h"
#include <boost/foreach.hpp>
//#include "FeedCacheAdapter.h"
#include "cmath"
#include <algorithm>
#include "FeedMemcProxy/client/feed_user_property.h"
#include "FeedCacheNAdapter.h"
//#include "FeedCacheAdapter.h"
#include "FeedCacheLogicAdapter.h"
#include "Date.h"
#include "EDMFeed.h"
#include <climits>
#include <map>
using namespace boost;
using namespace xce::feed;
using namespace std;

bool UserInfo::Init(){
	if(inited_){
		MCE_WARN("UserInfo::Init. should be init once. uid:" << uid_);
		return true;
	}
	DownloadMaxRead();
	LoadEdms();
	//读取FeedCache失败，就废弃这个UserInfo
	if(!LoadFeedCache()) return false;
	inited_ = true;
	return true;
}

long UserInfo::DownloadMaxRead(){
	FeedUserPropertyData data = FeedUserPropertyClient::instance().Get(uid_);
	pre_max_read_fid_ = data.GetMaxFeedId();
	MCE_INFO("*********** UserInfo::DownloadMaxRead. uid:" << uid_
			<< " downloaded pre_max_read_fid:"	<< pre_max_read_fid_);
	return pre_max_read_fid_;
}
void UserInfo::UploadMaxRead()const{
	FeedUserPropertyData data = FeedUserPropertyClient::instance().Get(uid_);
	long maxfix_memc = data.GetMaxFeedId();
	if(max_fid_ > data.GetMaxFeedId()){
		data.SetMaxFeedId(max_fid_);
		FeedUserPropertyClient::instance().Set(uid_,data);
	}
	MCE_INFO("UserInfo::UploadMaxRead. uid:" << uid_
			<< " pre_max_read_fid_:" << pre_max_read_fid_
			<< " max_fid_:" << max_fid_
			<< " maxfix_memc:" << maxfix_memc
	);
//	MaxFidUploader::instance().Add(uid_,max_fid_);
}

long UserInfo::GetMaxRead( )const { return max_fid_; }

long UserInfo::GetPreMaxRead( )const{ return pre_max_read_fid_; }

void UserInfo::SetMaxRead(long fid){
	if(pre_max_read_fid_ >= fid) return;
	pre_max_read_fid_ = fid;
}

void UserInfo::RefreshPreMaxRead(){
	MCE_INFO("UserInfo::RefreshPreMaxRead. uid:" << uid_ << " pre_max_read_fid_:" << pre_max_read_fid_ << " max_fid_:" << max_fid_);
	pre_max_read_fid_ = max_fid_;
}

void AddSFeed1D(SFeedPtr2D & sfeed2d,HFeed1D & hfeed1d){
	SFeedPtr1D sfeed1d;
	BOOST_FOREACH(HeavyFeed & hfeed, hfeed1d){
		sfeed1d.push_back(hfeed.GetSharedFeedPtr());
	}
	sfeed2d.push_back(sfeed1d);
}
void UserInfo::AddRefresh(time_t time,long fid){
	IceUtil::Mutex::Lock lock(mutex_);
	refreshes_[time] = fid;
}
//map<long,long> UserInfo::GetRefresh() const {
//	IceUtil::Mutex::Lock lock(mutex_);
//	return refreshes_;
//}

bool UserInfo::HandleEdm(GetRequest & req){
	if(edms_.empty())
		return false;

	bool has_new_edm = false;
	HeavyFeed & hfeed = edms_.at(0);

	if(hfeed.GetTime() > 0 && hfeed.GetTime() < time(NULL) ){
		MCE_INFO("UserInfo::HandleEdm. clear edm fid:" << hfeed.GetFid() << " stype:" << hfeed.GetStype() );
		edms_.clear();//过了指定时间
		return has_new_edm;
	}

	if(hfeed.GetTime() < 0){
		hfeed.ResetEdmTime();
		has_new_edm = true;
	}

	if(req.CheckType(hfeed.GetStype())){
		SFeedPtr1D sfeed1d;
		sfeed1d.push_back(hfeed.GetSharedFeedPtr());
		req.sfeed2d_.push_back(sfeed1d);
	}
	return has_new_edm;
}
void UserInfo::ClearOldComments(){
	long fid = GetOldestFid();
	vector<long> removed;
	for(HFeed1D::iterator it = comments_.begin(); it != comments_.end();/**/){
		if(it->GetFid() < fid){
			removed.push_back(it->GetFid());
			it = comments_.erase(it);
		}else{
			++it;
		}
	}
	MCE_INFO("UserInfo::ClearOldComments. uid:" << uid_
			<< " oldest fid:" << fid
			<< " removed:" << PrintHelper::Print(removed));
}

void UserInfo::GetShowedComments(StypemergeSFeed1DMap& commentmap){

	BOOST_FOREACH(HFeed1D::value_type & cmt,comments_){
		pair<int,long> key = cmt.GetStypeMerge();
		key.first = MergeConfig::instance().Comment2Main(key.first);
		StypemergeSFeed1DMap::iterator it = commentmap.find(key);
		if(it != commentmap.end()){
			it->second.push_back(cmt.GetSharedFeedPtr());
			MCE_INFO("@@@@@@@@@@@@@@ UserInfo::GetShowedComments. stype:" << cmt.GetStype() << " merge:" << cmt.GetMerge());
		}
	}
	for(StypemergeSFeed1DMap::iterator it = commentmap.begin(); it != commentmap.end();){
		if(it->second.empty()){
			commentmap.erase(it++);
		}else{
			++it;
		}
	}
	MCE_INFO("@@ UserInfo::GetShowedComments. showed commentmap size:" << commentmap.size() << " comments:" << comments_.size());
}

HeavyFeed UserInfo::MakeSplitterHeavyFeed(){
	int old_idx;
	{
		IceUtil::Mutex::Lock lock(mutex_);
		old_idx = splitter_idx_++;
	}
	long fid = uid_;
	long idx = splitter_idx_;
	fid = (idx << 32) | fid;

	FeedItem fitem;
	fitem.time = time(NULL);
	fitem.type = kSplitterStype;
	fitem.actor = uid_;
	fitem.feed = fid;
	SharedFeedPtr hdl = SharedFeedCache::instance().Add(fitem);

	HeavyFeed hfeed(fitem,hdl);
	MCE_INFO("UserInfo::MakeSplitterHeavyFeed. fid:" << hfeed.GetFid() << " stype:" << hfeed.GetStype() );
	return hfeed;
}

void UserInfo::PlaceSplitter(){
	HeavyFeed hfeed = MakeSplitterHeavyFeed();
	{
		IceUtil::Mutex::Lock lock(mutex_);
		if(!sorted_.empty()){
			HeavyFeed & old = *(sorted_.begin()->begin());
			if(old.IsSplitter()){//上一个就是splitter
				old = hfeed;
				MCE_INFO("UserInfo::PlaceSplitter. replace splitter uid:" << uid_ << " splitter_idx_:" << splitter_idx_
						<< " splitter hfeed:" << hfeed.ToString());
				return;
			}
		}
		HFeed1D h1d;
		h1d.push_back(hfeed);
		sorted_.push_front(h1d);
		MCE_INFO("UserInfo::PlaceSplitter. add splitter uid:" << uid_ << " splitter_idx_:" << splitter_idx_
			<< " splitter hfeed:" << hfeed.ToString());
	}
}
void UserInfo::GetFeeds(GetRequest & req){
	bool has_new_edm = false;
	PlaceSplitter();
	{
		IceUtil::Mutex::Lock lock(mutex_);

//		MoveFeedsFromNewToSorted();
		MoveFeedsFromNewToSortedV2(req.GetNowTime());
		if(req.ShowEdm())
			has_new_edm = HandleEdm(req);


		if(req.ByBlock())
			GetFeedsByBlock(req);
		else
			GetFeedsByTime(req);

		ClearOldComments();

		if(req.ShowComment())
			GetShowedComments(req.showed_comment_map_);
	}
	if(has_new_edm){
		MCE_INFO("UserInfo::GetFeeds. uid:" << uid_ << " call removeUser");
		EDMFeedManager::instance().removeUser(uid_);
	}else{
		MCE_INFO("UserInfo::GetFeeds. uid:" << uid_ << " not call removeUser");
	}
}

void UserInfo::GetFeedsByBlock(GetRequest & req){
	SFeedPtr2D res;
	int size = 0;
	for(unsigned i = 0; i < sorted_.size() ;++i){
		HFeed1D h1d = sorted_.at(i);
		HeavyFeed hfeed = h1d.at(0);

		if(hfeed.IsSplitter()){
			if(req.ShowSplitter())
				AddSFeed1D(req.sfeed2d_,sorted_.at(i));		
			continue;
		}

		int stype = hfeed.GetStype();
		
		if(!req.CheckType(stype)){
			continue;
		}
		if(size >= req.begin_ && size < req.begin_+req.limit_){
			AddSFeed1D(req.sfeed2d_,sorted_.at(i));
			if(sorted_.at(i).at(0).IsMarked()){
				BOOST_FOREACH(HFeed1D::value_type & hfeed, sorted_.at(i)){
					req.marked_fids.insert(hfeed.GetFid());
				}
			}
			if(req.ShowComment()){
				pair<int,long> key = sorted_.at(i).at(0).GetStypeMerge();
				if(MergeConfig::instance().IsMain(key.first)){
					req.showed_comment_map_[key];
					MCE_INFO("UserInfo::GetFeedsByBlock. key:" << key.first << ":" << key.second << " fid:" << sorted_.at(i).at(0).GetFid());
				}
			}
		}
		++size;
		if(size > req.begin_+req.limit_){
			break;
		}
	}
//	for(HFeed1D::iterator it = comments_.begin(); it != comments_.end();++it){
//		pair<int,long> key = it->GetStypeMerge();
//		req.showed_comment_map_[key].push_back(it->GetSharedFeedPtr());
//	}

	MCE_INFO("UserInfo::GetFeedsByBlock. req.sfeed2d size:" << req.sfeed2d_.size() << " comments_:" << comments_.size()
			<< " show_comment_map:" << req.showed_comment_map_.size() << " marked fids:" << PrintHelper::Print(req.marked_fids));
}

//按照时间的
void UserInfo::GetFeedsByTime(GetRequest & req){

	typedef multimap<int,HFeed1D *> TimeMap;
	TimeMap time_map;
	BOOST_FOREACH(HFeed1D& hfeed1d,sorted_){
		HeavyFeed & hf = hfeed1d.at(0);
		if(!req.CheckType(hf.GetStype()))
			continue;

		int score = hf.GetTime();
		time_map.insert(make_pair(score,&hfeed1d));
	}

	int idx = 0;
	BOOST_REVERSE_FOREACH(TimeMap::value_type & vt, time_map){
		if(idx < req.begin_){
			++idx;
			continue;
		}else	if(idx >= req.begin_+ req.limit_){
			break;
		}else{
			HFeed1D & hfeed1d = *(vt.second);
			AddSFeed1D(req.sfeed2d_,hfeed1d);
			if(hfeed1d.at(0).IsMarked()){
				BOOST_FOREACH(HFeed1D::value_type & hfeed, hfeed1d){
					req.marked_fids.insert(hfeed.GetFid());
				}
			}
			pair<int,long> key = hfeed1d.at(0).GetStypeMerge();
			req.showed_comment_map_[key];
			++idx;
		}
	}

	MCE_INFO("UserInfo::GetFeedsByTime. req.sfeed2d size:" << req.sfeed2d_.size() << " comments_:" << comments_.size()
			<< " show_comment_map:" << req.showed_comment_map_.size());
}


//GetFeeds触发一次
//LoadFeedCache触发两次
void UserInfo::MoveFeedsFromNewToSorted(){

	StypeMergeMap stypemerge_map;
	ScoreMap score_map;

	MakeStypeMergeMap(stypemerge_map, score_map);

	int topcount = GetTopCount(score_map.size());
	int timelinecount = score_map.size() - topcount;


	TimelineMap timeline_map;
	int added = 0;

	MCE_INFO("UserInfo::MoveFeedsFromNewToSorted. before. topcount:" << topcount
			<< " timelinecount:" << timelinecount
			<< " total:" << score_map.size()
			<< " sorted:" << sorted_.size()
			<< " new:" << newh1d_.size()
			<< " line:" << __LINE__
			);
	//取出分数小的，放入到timeline_map
	MakeTimelineMap(score_map,timelinecount,timeline_map);

	BOOST_FOREACH(TimelineMap::value_type & p, timeline_map){
		PushHFeedPtr1DToSorted(*(p.second),false);
	}
	MCE_INFO("UserInfo::MoveFeedsFromNewToSorted. after get timeline. sorted:" << sorted_.size() << " line:" << __LINE__);

	//挑出top的

	int idx = 0;
	BOOST_FOREACH(ScoreMap::value_type & vt , score_map){
		if(idx++ < timelinecount)
			continue;
		PushHFeedPtr1DToSorted(*(vt.second.second),true);
	}

	newh1d_.clear();
	MCE_INFO("UserInfo::MoveFeedsFromNewToSorted. after get top. sorted:" << sorted_.size() << " line:" << __LINE__);
}

void UserInfo::MoveFeedsFromNewToSortedV2(time_t nowtime){

	StypeMergeMap stypemerge_map;
	ScoreMap score_map;

	MakeStypeMergeMapV2(stypemerge_map, score_map);

	int marked = 0, not_marked = 0;
	vector<time_t> scores;
	BOOST_FOREACH(ScoreMap::value_type & vt,score_map){
		time_t score = vt.first;
		if(score >= nowtime){
			PushHFeedPtr1DToSorted(*(vt.second.second),true);
			++marked;
		}else{
			PushHFeedPtr1DToSorted(*(vt.second.second),false);
			++not_marked;
		}
		scores.push_back(score);
	}

	newh1d_.clear();
	MCE_INFO("UserInfo::MoveFeedsFromNewToSorted. after get top. sorted:" << sorted_.size() << " marked:" << marked
			<< " not_marked:" << not_marked << " line:" << __LINE__ << " scores:" << PrintHelper::Print(scores) << " now:" << nowtime);
}



void UserInfo::MakeStypeMergeMap(StypeMergeMap & stypemerge_map,ScoreMap & score_map){
	BOOST_FOREACH(HeavyFeed & h,newh1d_){
		HeavyFeedPtr hfeed = &h;

		pair<int,long> key = hfeed->GetStypeMerge();
		StypeMergeMap::iterator mit =
				stypemerge_map.find(key);
		if(mit != stypemerge_map.end()){
			mit->second.push_back(hfeed);
			stypemerge_map[key].push_back(hfeed);
		}else{
			HFeedPtr1D hfeed1d;
			hfeed1d.push_back(hfeed);
			stypemerge_map[key] = hfeed1d;
			score_map.insert(make_pair(hfeed->GetScore(uid_,true),make_pair(hfeed->GetTime(),&(stypemerge_map[key]))));
		}
	}
}

void UserInfo::MakeStypeMergeMapV2(StypeMergeMap & stypemerge_map,ScoreMap & score_map){
	long nowtime = time(NULL);
	BOOST_FOREACH(HeavyFeed & h,newh1d_){
		HeavyFeedPtr hfeed = &h;

		pair<int,long> key = hfeed->GetStypeMerge();
		StypeMergeMap::iterator mit =
				stypemerge_map.find(key);
		if(mit != stypemerge_map.end()){
			mit->second.push_back(hfeed);
			stypemerge_map[key].push_back(hfeed);
		}else{
			HFeedPtr1D hfeed1d;
			hfeed1d.push_back(hfeed);
			stypemerge_map[key] = hfeed1d;
			double score = hfeed->GetScore(uid_,true);
			if(hfeed->IsFocusSource() or score >= 90)
				score += nowtime;
			else
				score = hfeed->GetScore(uid_,false);

			score_map.insert(make_pair(score,make_pair(hfeed->GetTime(),&(stypemerge_map[key]))));
		}
	}
}

//void UserInfo::SeperateTopAndTimeline(StypeMergeMap & stypemerge_map,ScoreMap & score_map){
//	BOOST_FOREACH(HeavyFeed & h,newh1d_){
//		HeavyFeedPtr hfeed = &h;
//
//		pair<int,long> key = hfeed->GetStypeMerge();
//		StypeMergeMap::iterator mit =
//				stypemerge_map.find(key);
//		if(mit != stypemerge_map.end()){
//			mit->second.push_back(hfeed);
//			stypemerge_map[key].push_back(hfeed);
//		}else{
//			HFeedPtr1D hfeed1d;
//			hfeed1d.push_back(hfeed);
//			stypemerge_map[key] = hfeed1d;
//			score_map.insert(make_pair(hfeed->GetScore(uid_,true),make_pair(hfeed->GetTime(),&(stypemerge_map[key]))));
//		}
//	}
//}

void UserInfo::MakeTimelineMap(ScoreMap & score_map,int timelinecount,TimelineMap & timeline_map){
	//取出分数小的，放入到timeline_map
	BOOST_FOREACH(ScoreMap::value_type & p, score_map){
		if(timeline_map.size() >= (unsigned int)timelinecount)
			break;
		timeline_map.insert(p.second);
	}
}

void UserInfo::PushHFeedPtr1DToSorted(HFeedPtr1D & hfeedptr1d,bool mark){
	HFeed1D hfeed1d;
	BOOST_FOREACH(HeavyFeedPtr hp, hfeedptr1d){
		hp->Mark(mark);
		hfeed1d.push_back(*hp);
	}
	sorted_.push_front(hfeed1d);
}


bool UserInfo::LoadFeedCache(){
	FeedItemSeq fitems;
	TimeStat ts;
	try{
//		fitems = FeedCacheNF55Adapter::instance().get(uid_);
		fitems = FeedCacheNAdapter::instance().get(uid_);
	}catch(Ice::Exception& e){
		MCE_WARN("UserInfo::LoadFeedCache. uid:" << uid_ << " time:" << ts.Get()<< " error:" << e );
		return false;
	}

	AddFeedItemsFromFeedCache(fitems);
	DumpSorted();
	MCE_INFO("UserInfo::LoadFeedCache. after get." << ToString()
				<<  " fitems from FeedCache:" << fitems.size() << " cost:" << ts.Get() << " line:" << __LINE__);
	return true;
}
int UserInfo::GetTopCount(int blocksize){
	int f1 = 10 + log(blocksize);
	int f2 = min(3,int(blocksize * 0.1));
	int res = min(blocksize,max(f1,f2));
	return min(res,blocksize/2 + 1);
}

void UserInfo::SeperateByPreMaxFid(FeedItemSeq & fitems,HDeq & next){
	BOOST_REVERSE_FOREACH(FeedItem & fitem, fitems){
		SharedFeedPtr hdl = SharedFeedCache::instance().Add(fitem);
		HeavyFeed hfeed(fitem,hdl);
		if(MergeConfig::instance().IsComment(hfeed.GetStype())){
			comments_.push_back(hfeed);
			continue;
		}

		if(hfeed.GetFid() <= pre_max_read_fid_)
			newh1d_.push_back(hfeed);
		else
			next.push_back(hfeed);//最最新的

		max_fid_ = max(max_fid_,hfeed.GetFid());
	}
}
void UserInfo::AddFeedItemsFromFeedCache(FeedItemSeq & fitems){
//	HFeed1D next;
	HDeq next;

	SeperateByPreMaxFid(fitems,next);

	MCE_INFO("UserInfo::AddFeedItemsFromFeedCache. newh1d_:" << newh1d_.size() << " next:" << next.size() << " comments:" << comments_.size());

	time_t nowtime = time(NULL);
	if(!newh1d_.empty())
		MoveFeedsFromNewToSortedV2(nowtime);

	MCE_INFO("UserInfo::AddFeedItemsFromFeedCache. after new. sorted:" << sorted_.size());
	if(!next.empty()){
		newh1d_.swap(next);
		MoveFeedsFromNewToSortedV2(nowtime);
	}
	MCE_INFO("UserInfo::AddFeedItemsFromFeedCache. after next. sorted:" << sorted_.size());
	newh1d_.clear();
}

bool UserInfo::LoadEdms(){
	FeedItemSeq fitems = EDMFeedManager::instance().getEDMFeed(uid_);
	BOOST_FOREACH(FeedItem & fitem,fitems){
		SharedFeedPtr hdl = SharedFeedCache::instance().Add(fitem);
		HeavyFeed hfeed(fitem,hdl);
		edms_.push_back(hfeed);
	}
	MCE_INFO("UserInfo::LoadEdms. uid:" << uid_ << " edms:" << edms_.size() );
	return true;
}

void UserInfo::AddHeavyItem(int uid,HeavyFeed & hfeed/*,bool updatemaxfid*/){

	IceUtil::Mutex::Lock lock(mutex_);
//	Trace trace("UserInfo::AddHeavyItem");
	if(hfeed.GetTime() < 0){
		edms_.clear();
		edms_.push_back(hfeed);
		MCE_INFO("UserInfo::AddHeavyItem.is edm uid:" << uid << " fid:" << hfeed.GetFid() << " new:" << newh1d_.size() << " sorted:" << sorted_.size());
		return;
	}
	newh1d_.push_front(hfeed);
	while(Size() > kHbufSize){
		if(!sorted_.empty()){
			sorted_.pop_back();
		}else{
			newh1d_.pop_back();
		}
	}
	max_fid_ = max(max_fid_,hfeed.GetFid());
	MCE_INFO("UserInfo::AddHeavyItem. uid:" << uid << " fid:" << hfeed.GetFid() << " new:" << newh1d_.size() << " sorted:" << sorted_.size());
}
//
void UserInfo::SetReadAll(){
	IceUtil::Mutex::Lock lock(mutex_);
	sorted_.clear();
	edms_.clear();
	comments_.clear();
	newh1d_.clear();
}
//
vector<long> UserInfo::SetRead(int stype,long merge ){
	IceUtil::Mutex::Lock lock(mutex_);
	bool del = false;
	vector<long> fids;
	for(H1DBuf::iterator it = sorted_.begin();
			it != sorted_.end();++it){
		BOOST_FOREACH(HeavyFeed &hf, *it){
			if(hf.GetStype() == stype && hf.GetMerge() == merge){
				del = true;
				break;
			}
		}
		if(del){
			BOOST_FOREACH(HeavyFeed &hf, *it){
				fids.push_back(hf.GetFid());
			}
			sorted_.erase(it);
			break;
		}
	}
	MCE_INFO("UserInfo::SetRead. uid:" << uid_ << " stype:" << stype << " merge:" << merge << " deleted:" << del);
	return fids;
}
//
vector<long> UserInfo::SetReadById(long fid){
	IceUtil::Mutex::Lock lock(mutex_);
	bool del = false;
	vector<long> fids;
	for(H1DBuf::iterator it = sorted_.begin();
			it != sorted_.end();++it){
		BOOST_FOREACH(HeavyFeed &hf, *it){
			if(hf.GetFid() == fid){
				del = true;
				break;
			}
		}
		if(del){
			BOOST_FOREACH(HeavyFeed &hf, *it){
				fids.push_back(hf.GetFid());
			}
			sorted_.erase(it);
			break;
		}
	}
	MCE_INFO("UserInfo::SetReadById. uid:" << uid_ << " fid:" << fid << " deleted:" << del);
	return fids;
}

void UserInfo::SetReadByIdsExactly(const set<long> & fids,bool remove_feedcache){
	set<long> in_fids = fids;
	set<long> deleted_fids;
	{
		IceUtil::Mutex::Lock lock(mutex_);
		for(H1DBuf::iterator it = sorted_.begin(); it != sorted_.end();/**/){
			HFeed1D & hfeed1d = *it;
			for(HFeed1D::iterator vit = hfeed1d.begin(); vit != hfeed1d.end(); /**/){
				long fid = vit->GetFid();
				if(fids.count(fid)){
					vit = hfeed1d.erase(vit);
					in_fids.erase(fid);
					deleted_fids.insert(fid);
				}else{
					++vit;
				}
			}

			if(hfeed1d.empty())	it = sorted_.erase(it);
			else ++it;

			if(in_fids.empty())	break;
		}
	}
	MCE_INFO("UserInfo::SetReadByIdsExactly. uid:" << uid_ << " fids:" << PrintHelper::Print(fids)
		<< " deleted fids:" << PrintHelper::Print(deleted_fids));
}

FeedItemSeq UserInfo::GetOrigFeedItems(){
	IceUtil::Mutex::Lock lock(mutex_);
	FeedItemSeq res;
	BOOST_FOREACH(HFeed1D& hfeed1d,sorted_){
		BOOST_FOREACH(HeavyFeed & hfeed,hfeed1d){
			if(!hfeed.IsSplitter())
				res.push_back(hfeed.ToFeedItem());
		}
	}
	return res;
}

long UserInfo::GetOldestFid(){
	long fid = LONG_MAX;
	FeedItemSeq res;
	BOOST_FOREACH(HFeed1D& hfeed1d,sorted_){
		BOOST_FOREACH(HeavyFeed & hfeed,hfeed1d){
			fid = min(fid,hfeed.GetFid());
		}
	}
	return fid;
}

//int UserInfo::GetUnviewCount(const set<int> & types){
//	IceUtil::Mutex::Lock lock(mutex_);
//	int res = 0;
//	BOOST_FOREACH(HBuf::value_type & hfeed,hbuf_){
//		if(hfeed.GetView() == 0) ++res;
//	}
//	return res;
//}



int UserInfo::Size() const {
//	IceUtil::Mutex::Lock lock(mutex_);
	return sorted_.size();
}

string UserInfo::ToString() const {
	ostringstream os;
//	os << " uid:" << uid_ << " bufsize:" << hbuf_.size()<<" max_fid:" << max_fid_ << " pre_max_read_fid:" << pre_max_read_fid_;
	return os.str();
}


void MaxFidUploader::handle(){
  map<int,long> map_swap;
  {
  	IceUtil::Mutex::Lock lock(mutex_);
  	map_swap.swap(map_);
  }
  typedef map<int,long>::value_type vt;
  BOOST_FOREACH(vt & p, map_swap){
  	FeedUserPropertyData data = FeedUserPropertyClient::instance().Get(p.first);
  	if(p.second <= data.GetMaxFeedId()){
  		continue;
  	}
  	data.SetMaxFeedId(p.second);
  	FeedUserPropertyClient::instance().Set(p.first,data);
  }
  MCE_INFO("MaxFidUploader::handle. uids:" << map_swap.size());
  TaskManager::instance().schedule(this);
}
