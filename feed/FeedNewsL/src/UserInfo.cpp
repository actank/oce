/*
 * UserInfo.cpp
 *
 *  Created on: Sep 27, 2011
 *      Author: yejingwei
 */

#include "UserInfo.h"
#include "Common.h"
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
//#include "FeedCacheAdapter.h"
#include "cmath"
#include <algorithm>
#include "FeedMemcProxy/client/feed_user_property.h"
#include "FeedCacheNAdapter.h"
#include "FeedCacheLogicAdapter.h"
#include "Date.h"
#include "EDMFeed.h"
#include <boost/tuple/tuple.hpp>
using namespace boost;
using namespace xce::feed;
using namespace std;

//UserInfo::~UserInfo(){
//	MyUtil::Date date(stamp_);
//	MCE_INFO("UserInfo::~UserInfo. uid:" << uid_ << " live time:" << time(0)-stamp_ << " create at " << date.toDateTime());
//}
long UserInfo::DownloadMaxRead(){
	long old = pre_max_read_fid_;
	FeedUserPropertyData data = FeedUserPropertyClient::instance().Get(uid_);
	pre_max_read_fid_ = data.GetMaxFeedId();
	MCE_INFO("UserInfo::DownloadMaxRead. uid:" << uid_ << " old pre_max_read_fid:" << old << " pre_max_read_fid:" << pre_max_read_fid_);
	return pre_max_read_fid_;
}

bool UserInfo::SetMaxRead(long fid){
	if(pre_max_read_fid_ >= fid)
		return false;

	pre_max_read_fid_ = fid;
	return true;
}
//void UserInfo::RaiseUgc(GetRequest & req){
//}
void UserInfo::MergeAndSort(GetRequest & req){
	IceUtil::Mutex::Lock lock(mutex_);
	MergeFeed(req);

	//req.RaiseUgc();

	if(req.IsSort()){
		GatherFeed(req);
	}else{
		GatherFeedSimple(req);
	}
	req.GetShowedFids();
}

static void PrintScore(StypemergeScoreMap & vstypemerge_score){//临时测试用
	BOOST_FOREACH(StypemergeScoreMap::value_type & p, vstypemerge_score){
		ostringstream oss;
		oss.setf(ios::fixed);
		oss << p.second;
		MCE_INFO(p.first.first << " " << p.first.second << " " << oss.str());
	}
}
void UserInfo::MergeFeed(GetRequest & req){
	StypemergeScoreMap vstypemerge_score;

	MergeByStypeMergeAndGetScore(req,vstypemerge_score);
	if(req.ShowComment()){
		ReplaceScoreByCommentScore(req,req.comment_map_,vstypemerge_score);
	}
	MakeScoreIteratorMultiMap(req,vstypemerge_score);

	MCE_INFO("UserInfo::MergeFeed. merge done." << req.ToString());
}

void UserInfo::ReplaceScoreByCommentScore(GetRequest & req,StypemergeHFeed1DMap & comment_map_,
		StypemergeScoreMap & vstypemerge_score ){
	BOOST_FOREACH(StypemergeHFeed1DMap::value_type & p, comment_map_){
		StypemergeScoreMap::iterator mit = vstypemerge_score.find(p.first);
		if(mit != vstypemerge_score.end()){
			mit->second =(p.second.front())->GetScore(req.uid_,req.IsSort(),req.GetNowTime(),GetMaxRead());//TODO 判断评论是否updatetime
		}
	}
}
void UserInfo::MergeByStypeMergeAndGetScore(GetRequest & req,StypemergeScoreMap & vstypemerge_score){

//	BOOST_FOREACH(HeavyFeed & h,hbuf_){
	for(HBuf::iterator it = hbuf_.begin(); it != hbuf_.end();){
		HeavyFeedPtr hfeed = &(*it);
		if(!req.CheckType(hfeed->GetStype()) || req.SeperateComment(hfeed)) {
			++it;
			continue;
		}

		bool is_edm,must_erase;
		boost::tie(is_edm,must_erase) = req.SeperateEdm(hfeed);
		if(is_edm){
			if(must_erase) it = hbuf_.erase(it);
			else ++it;
			continue;
		}

		pair<int,long> key = hfeed->GetVersionStypeMerge();
		StypemergeHFeed1DMap::iterator mit =
				req.stypemerge_item1d_map_.find(key);
		if (mit != req.stypemerge_item1d_map_.end()) {
			mit->second.push_back(hfeed);
			if(hfeed->IsUpdateTime()){
				vstypemerge_score[key] = hfeed->GetScore(req.uid_,req.IsSort(),req.GetNowTime(),GetMaxRead());
			}
		} else {
			req.stypemerge_item1d_map_[key].push_back(hfeed);
			vstypemerge_score[key] = hfeed->GetScore(req.uid_,req.IsSort(),req.GetNowTime(),GetMaxRead());
		}
		++it;
	}
}
void UserInfo::MakeScoreIteratorMultiMap(
		GetRequest & req,StypemergeScoreMap & vstypemerge_score){
	for(StypemergeScoreMap::iterator mit = vstypemerge_score.begin();
			mit != vstypemerge_score.end();++mit){
		req.score_iterator_multimap_.insert(make_pair(mit->second,req.stypemerge_item1d_map_.find(mit->first)));
	}
}


void UserInfo::GatherFeed(GetRequest & req){
	StypeactorTimeindexMap stypeactor_timeindex;

	BOOST_REVERSE_FOREACH(ScoreIteratorMultiMap::value_type & p,req.score_iterator_multimap_){
		StypemergeHFeed1DMap::iterator & tmpitr = p.second;
		HFeed1D & item1d = tmpitr->second;
		HeavyFeedPtr hfeed = item1d.front();

		StypeactorTimeindexMap::iterator ti;
		if(FindSameStypeActor(hfeed,stypeactor_timeindex,ti)
				&& (TimeDiffLessThanLimit(hfeed->GetTime(),ti->second.first))){
			req.hfeed3d_[ti->second.second].push_back(item1d);

//			int idx = ti->second.second;
//			if(req.InRange(idx)){
//				idx -= req.begin_;
//				showed_item3d_.at(idx).push_back(item1d);
//			}
		}else{
		//	if(req.item3d_.size() > req.begin_ + req.limit_) break;

			HFeed2D item2d;
			item2d.push_back(item1d);
			req.hfeed3d_.push_back(item2d);
			stypeactor_timeindex[hfeed->GetGatherKey()]
			                     = make_pair(hfeed->GetTime(), req.hfeed3d_.size()-1);

//			int idx = req.item3d_.size()-2;
//			if(req.InRange(idx)){
//				idx -= req.begin_;
//				showed_item3d_.push_back(item2d);
//			}
		}
	}
	MCE_INFO("UserInfo::GatherFeed. uid:" << req.uid_ << " item3d:" << req.hfeed3d_.size());
//	PrintHItem3D(req.item3d_);
}

void UserInfo::GatherFeedSimple(GetRequest & req){
	MCE_INFO("UserInfo::GatherFeedSimple.begin. scoremap size:" << req.score_iterator_multimap_.size());

	BOOST_REVERSE_FOREACH(ScoreIteratorMultiMap::value_type & p,req.score_iterator_multimap_){
		StypemergeHFeed1DMap::iterator & tmpitr = p.second;
		HFeed1D & item1d = tmpitr->second;

		HFeed2D item2d;
		item2d.push_back(item1d);

		req.hfeed3d_.push_back(item2d);
//		MCE_INFO("UserInfo::GatherFeedSimple. push one");
	}
//	PrintHelper::Print(req.item3d_);
}

//废弃
long UserInfo::MakeNewbuf(FeedItemSeq & fitems,HBuf & newbuf){
	long newmax = 0;
	BOOST_FOREACH(FeedItem & fitem, fitems){
		if(fitem.feed < max_fid_) continue;

		SharedFeedPtr hdl = SharedFeedCache::instance().Add(fitem);
//		HeavyFeedPtr hfeed = new HeavyFeed(fitem,hdl);
		HeavyFeed hfeed(fitem,hdl);
		newbuf.push_back(hfeed);
		newmax = max(newmax,hfeed.GetFid());
	}
	return newmax;
}

//bool UserInfo::GetFromFeedCache(FeedItemSeq & fitems){
//	TimeStat ts;
//	try{
//		fitems = FeedCacheNAdapter::instance().get(uid_);
////		fitems = FeedCacheNTestAdapter::instance().getMore(uid_,max_fid_);//结果的顺序是从大到小
////		MCE_INFO("UserInfo::GetMoreFromFeedCache. uid:" << uid_ << " time:" << ts.Get() << " line:" << __LINE__
////				<< " fids:" << PrintHelper::Print(fitems)
////		);
//		return true;
//	}catch(Ice::Exception& e){
//		MCE_WARN("UserInfo::GetMoreFromFeedCache. uid:" << uid_ << " time:" << ts.Get()<< " error:" << e );
//		return false;
//	}
//}


void UserInfo::GetSharedFeedsAndAddBuffer(FeedItemSeq & fitems,bool normalfeed){
	//默认fitems按时间从大到小排序，需要从小到大放到hbuf前端
	BOOST_REVERSE_FOREACH(FeedItem & fitem, fitems){
//		if(!TypeFilter::instance().Check(uid_,fitem.type & 0xFFFF)){
//			continue;
//		}
		if(normalfeed && fitem.time < 0){
			MCE_WARN("UserInfo::GetSharedFeedsAndAddBuffer. uid:" << uid_
					<< " fid:" << fitem.feed
					<< " stype:" << (fitem.type & 0xFFFF)
					<< " actor:" << fitem.actor
					<< " time:" << fitem.time
					);
			continue;
		}

		SharedFeedPtr hdl = SharedFeedCache::instance().Add(fitem);
		HeavyFeed hfeed(fitem,hdl);
		int idx = uid_ % 10;
		if(idx != 2 and fitem.feed <= pre_max_read_fid_)
			hfeed.IncView();

		hbuf_.push_front(hfeed);
		max_fid_ = max(max_fid_,hfeed.GetFid());
	}
}
bool UserInfo::LoadFeedCache(){
	FeedItemSeq fitems;
	TimeStat ts;
	try{
		fitems = FeedCacheNAdapter::instance().get(uid_);
//		fitems = FeedCacheAdapter::instance().get(uid_);
//		fitems = FeedCacheNF55Adapter::instance().get(uid_);
	}catch(Ice::Exception& e){
		MCE_WARN("UserInfo::LoadFeedCache. uid:" << uid_ << " time:" << ts.Get()<< " error:" << e );
		return false;
	}

	GetSharedFeedsAndAddBuffer(fitems,true);

	MCE_INFO("UserInfo::LoadFeedCache. after get." << ToString()
				<<  " fitems from FeedCache:" << fitems.size() << " cost:" << ts.Get() << " line:" << __LINE__);
	return true;
}

bool UserInfo::LoadEdms(){
	FeedItemSeq fitems = EDMFeedManager::instance().getEDMFeed(uid_);
	ostringstream os;
	MCE_INFO("UserInfo::LoadEdms. uid:" << uid_ << " edms:" << PrintHelper::Print(fitems));

//	GetSharedFeedsAndAddBuffer(fitems);
	if(!fitems.empty()){
		FeedItemSeq onefitem;
//		onefitem.push_back(fitems.at(0));
		onefitem.push_back(*fitems.rbegin());//最新的一个edm在最后
		GetSharedFeedsAndAddBuffer(onefitem,false);
	}
	return true;
}

void UserInfo::AddHeavyItem(int uid,HeavyFeed & hfeed/*,bool updatemaxfid*/){
//	if(!TypeFilter::instance().Check(uid,hfeed.GetStype())){
//		return;
//	}
	IceUtil::Mutex::Lock lock(mutex_);
	hbuf_.push_front(hfeed);
	max_fid_ = max(max_fid_,hfeed.GetFid());
}

void UserInfo::SetReadAll(){
	IceUtil::Mutex::Lock lock(mutex_);
	hbuf_.clear();
}

vector<long> UserInfo::SetRead(int stype,long merge ){
	vector<long> res;
	{
		IceUtil::Mutex::Lock lock(mutex_);
		for(HBuf::iterator it = hbuf_.begin(); it != hbuf_.end();){
			HeavyFeed & hfeed = *it;
			if(hfeed.SameStypeMerge(stype,merge)){
				res.push_back(hfeed.GetFid());
				it = hbuf_.erase(it);
			}else{
				++it;
			}

//			HeavyFeed & hfeed = *it;
//			if(hfeed->SameStypeMerge(stype,merge)){
//				res.push_back((*it).GetFid());
//				it = hbuf_.erase(it);
//			}else{
//				++it;
//			}
		}
	}
	return res;
}

vector<long> UserInfo::SetReadById(long fid){
	pair<int,long> stypemerge = make_pair<int,long>(0,0);
	{
		IceUtil::Mutex::Lock lock(mutex_);
		BOOST_FOREACH(HBuf::value_type & hfeed , hbuf_){
			if(hfeed.GetFid() == fid)
				stypemerge = hfeed.GetStypeMerge() ;
		}
	}
	if(stypemerge.first) return SetRead(stypemerge.first,stypemerge.second);
	else return vector<long>();
}

void UserInfo::SetReadByIdsExactly(const set<long> & fids,bool remove_feedcache){
	if(fids.empty())
		return;
	vector<HeavyFeed> hfeeds;
	int removed = 0;
	{
		IceUtil::Mutex::Lock lock(mutex_);
		for(HBuf::iterator it = hbuf_.begin(); it != hbuf_.end();){
			HeavyFeed & hfeed = *it;
			if(fids.count(hfeed.GetFid())){
				hfeeds.push_back(hfeed);
				it = hbuf_.erase(it);

				if(++removed == (int)fids.size()) break;
			}else{
				++it;
			}
		}
	}
	if(remove_feedcache){
		try{
			BOOST_FOREACH(long fid,fids){
				FeedCacheLogicAdapter::instance().remove(uid_,fid,true);//现在暂时不调用
			}
		}catch (Ice::Exception& e) {
			MCE_WARN("UserInfo::SetReadByIdsExactly. call cache logic fail. uid:" << uid_ << " fids:" << PrintHelper::Print(fids)<<" error:"<< e);
		}
	}

	MCE_INFO("UserInfo::SetReadByIdsExactly . uid:" << uid_ << " fids size:"
			<< fids.size() << " fids:"<< PrintHelper::Print(fids) << " remove hfeeds:" << PrintHelper::Print(hfeeds));
}

FeedItemSeq UserInfo::GetOrigFeedItems(){
	IceUtil::Mutex::Lock lock(mutex_);
	FeedItemSeq res;
	BOOST_FOREACH(const HBuf::value_type & hfeed,hbuf_){
		res.push_back(hfeed.ToFeedItem());
	}
	return res;
}

int UserInfo::GetUnviewCount(const set<int> & types){
	IceUtil::Mutex::Lock lock(mutex_);
	int res = 0;
	BOOST_FOREACH(HBuf::value_type & hfeed,hbuf_){
		if(hfeed.GetView() == 0) ++res;
	}
	return res;
}

map<int,int> UserInfo::GetCountByStype(const set<int> & stypes,bool unviewed){
	map<int,int> res;
	IceUtil::Mutex::Lock lock(mutex_);
	BOOST_FOREACH(HBuf::value_type & hfeed,hbuf_){
		if(!unviewed || (unviewed && hfeed.GetFid() > pre_max_read_fid_)){
			int stype = hfeed.GetStype();
			if(stypes.count(stype)){
				++res[stype];
			}
		}
	}
	return res;
}

map<int,int> UserInfo::GetCountByStypeMerged(const set<int> & stypes,bool unviewed){
	vector<int> stypes_v(stypes.begin(),stypes.end());
	GetRequest req(uid_,stypes_v,true,0,250,false);

	map<int,int> res;
	StypemergeScoreMap vstypemerge_score;

	{
		IceUtil::Mutex::Lock lock(mutex_);
		MergeByStypeMergeAndGetScore(req, vstypemerge_score );
		BOOST_FOREACH(StypemergeHFeed1DMap::value_type & vt,req.stypemerge_item1d_map_){
			int stype = vt.first.first & 0xFFFF;
			if(vt.second.empty())
				continue;
			long fid = vt.second.front()->GetFid();
			if(stypes.count(stype) && ((!unviewed)|| fid < pre_max_read_fid_) ){
				++res[stype];
			}
		}
	}
	MCE_INFO("UserInfo::GetCountByStypeMerged. @@@@ uid:" << uid_ << " stypemerge_item1d_map_ size:" << req.stypemerge_item1d_map_.size()
			<< " stypes:" << PrintHelper::Print(stypes_v));
	return res;
}


template<typename T>
void UserInfo::SetReadInFeedCache(const T & fids){
	BOOST_FOREACH(long fid,fids){
		FeedCacheLogicAdapter::instance().remove(uid_, fid,true);
	}
	MCE_INFO("UserInfo::SetReadInFeedCache. uid:" << uid_ << " fids:" << PrintHelper::Print(fids));
}

int UserInfo::Size() const {
	IceUtil::Mutex::Lock lock(mutex_);
	return hbuf_.size();
}

string UserInfo::ToString() const {
	ostringstream os;
	os << " uid:" << uid_ << " bufsize:" << hbuf_.size()<<" max_fid:" << max_fid_ << " pre_max_read_fid:" << pre_max_read_fid_;
	return os.str();
//	int i = 0;
//	BOOST_FOREACH(HeavyItemPtr & hfeed,hbuf_){
//		MCE_INFO("UserInfo::Dump.hfeed:" << i++ << " " <<hfeed->ToString());
//	}
}

void UserInfo::IncViewBelowFid(long fid){
	IceUtil::Mutex::Lock lock(mutex_);
	BOOST_FOREACH(HBuf::value_type & hfeed, hbuf_){
		if(hfeed.GetFid() < fid and hfeed.GetView() == 0)
			hfeed.IncView();
	}
}

bool UserInfo::CheckUniq(){
	set<long> uniqids;
	set<long> dups;
	IceUtil::Mutex::Lock lock(mutex_);
	BOOST_FOREACH(HBuf::value_type & hfeed, hbuf_){
		pair<set<long>::iterator ,bool> p = uniqids.insert(hfeed.GetFid());
		if(!(p.second)){
			dups.insert(hfeed.GetFid());
		}
	}
	if(dups.empty()){
		MCE_INFO("UserInfo::CheckUniq. uid:" << uid_ << " success");
		return true;
	}else{
		MCE_WARN("UserInfo::CheckUniq. uid:" << uid_ << " fail. dup fids:" << PrintHelper::Print(dups));
		return false;
	}
}

//确认从大到小
bool UserInfo::CheckOrder(FeedItemSeq & fitems){
	long fid = 0;
	bool ordered = true;
	BOOST_REVERSE_FOREACH(FeedItem & fitem,fitems){
		if(fid > fitem.feed){
			MCE_WARN("UserInfo::CheckOrder. bad order.uid:" << uid_<<" last:" << fid << " this:" << fitem.feed);
			ordered = false;
		}
	}
	MCE_INFO("UserInfo::CheckOrder. uid:" << uid_ << " ordered:" << ordered);
	return ordered;
}



void MaxFidUploader::handle(){
	TimeStat ts;
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
  MCE_INFO("MaxFidUploader::handle. uids:" << map_swap.size() << " cost:" << ts.Get());
  TaskManager::instance().schedule(this);
}
