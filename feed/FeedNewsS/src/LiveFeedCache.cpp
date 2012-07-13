/*
 * LiveFeedCache.cpp
 *
 *  Created on: May 5, 2010
 *      Author: antonio
 */

#include "LiveFeedCache.h"
#include "ItemCache.h"
#include "IceLogger.h"
#include "NewsUtil.h"
#include <cmath> 

using namespace xce::feed;
using namespace MyUtil;
void UniqueFeedList::Add(int uid, const FeedItem& item, bool updatetime,
		bool checkenter) {
	SharedFeedPtr feed = SharedFeedPool::instance().Add(item);
	UniqueFeed uniquefeed(feed, item.weight, item.time);
	return Add2(uid, uniquefeed, updatetime, checkenter);
}

void UniqueFeedList::Add(int uid, const UniqueFeed& uniquefeed, bool updatetime,
		bool checkenter) {
	//if(size_ >= COUNT && feeds_[end_].feed_ && feeds_[end_].IsEntered()){
	//    hot_size_--;
	//}

	unique_feeds_[end_] = uniquefeed;
	unique_feeds_[end_].SetUpdateTime(updatetime);
	if (checkenter) {
	} else {
		for (int i = 0; i < INIT_VIEWED_COUNT; i++) {
			unique_feeds_[end_].IncViewedCount();
		}
	}
	//CheckEnter(uid, feeds_[end_]);
	//MCE_INFO("@@@@@@@@ " << uid << " size:" << size_ << " feed:" << uniquefeed.feed_->id_ << " enter:" << entered);
	//LiveFeedItem* p = feeds_ + end_;
	size_ = (++size_ > COUNT) ? COUNT : size_;
	end_ = (end_ + 1) % COUNT;
	//return p;
}

multimap<Ice::Long, SharedFeedPtr> UniqueFeedList::GetFeedSeq(int begin, int limit) {
	multimap<Ice::Long, SharedFeedPtr> res;
	/*  for(list<FeedPtr>::iterator it = feeds_.begin(); it != feeds_.end(); ){
	 if((*it)->IsRemove()){
	 it = feeds_.erase(it);
	 continue;
	 }
	 Ice::Long key = ((*it)->SmallType() << 32) | (*it)->merge();
	 if(res.count(key) && (*it)->NewsMergeType == FeedMergeReplace){
	 ++it;
	 continue;
	 }
	 res.insert(pair<Ice::Long,FeedPtr>(key,*it));
	 ++it;
	 }*/
	return res;
}

vector<SharedFeedPtr> UniqueFeedList::GetFeed() {
	vector<SharedFeedPtr> res;
	int pos = end_ + COUNT - 1;
	int c = 0;
	for (; c < size_; c++, pos--) {
		SharedFeedPtr feed = unique_feeds_[pos % COUNT].sharedfeed_;
		res.push_back(feed);
	}
	return res;
}

//struct FeedItem{
//	long feed;
//	long merge;
//	int type;	// 包含 NewsMergeType(2位), MiniMergeType(2位), ReplyType(2位), 空两位，其余24位为type 和 Stype通过mod 100确定大类和小类
//	int time;
//	int weight;	//高8位为 friendrank,次高8位为baseWeightId, 低16位为weight
//	int actor;	//from
//};

//vector<FeedItem> LiveFeed::GetFeedItems() {
//	vector<FeedItem> items;
//  int pos = end_ + COUNT - 1;
//  int c = 0;
//  for (; c < size_; c++, pos--) {
//  	FeedItem item;
//    FeedPtr feed = feeds_[pos % COUNT].feed_;
//    item.feed = feed->id_;
//    item.merge = feed->merge_;
//    item.type = feed->type_;
//    item.actor = feed->actor_;
//    item.time = feed->time_;
//    item.weight = feeds_[pos % COUNT].weight_;
//
//    items.push_back(item);
//  }
//  return items;
//}

vector<UniqueFeed*> UniqueFeedList::GetUniqueFeeds() {
	vector<UniqueFeed*> res;
	int pos = end_ + COUNT - 1;
	int c = 0;
	for (; c < size_; c++, pos--) {
		//MCE_INFO("@@@@@@@ --> " << feeds_[pos%COUNT].feed_->feed_id() << " time:" << feeds_[pos%COUNT].feed_->time_)
		res.push_back(unique_feeds_ + pos % COUNT);
	}
	return res;
}

vector<Ice::Long> UniqueFeedList::Remove(int stype, Ice::Long merge) {
	//  MCE_INFO("LiveFeed::Remove before." << DumpToString());
	vector<Ice::Long> feedids;
	int other = stype;
	if (MergeConfig::instance().IsComment(stype)) {
		other = MergeConfig::instance().Comment2Main(stype);
	} else if (MergeConfig::instance().IsMain(stype)) {
		other = MergeConfig::instance().Main2Comment(stype);
	}
	int begin = -1;
	int c = 0;
	int size = size_;
	for (int i = end_ + COUNT - 1; i >= end_ && c < size; c++, i--) {
		int cur_stype = unique_feeds_[i % COUNT].sharedfeed_->SmallType();
		if (begin > 0) {
			if ((cur_stype == stype || cur_stype == other)
					&& unique_feeds_[i % COUNT].sharedfeed_->merge() == merge) {
				feedids.push_back(unique_feeds_[i % COUNT].sharedfeed_->feed_id());
				unique_feeds_[i % COUNT].sharedfeed_ = 0;
				size_--;
				IncPartSize(cur_stype, -1);
			} else {
				unique_feeds_[begin % COUNT] = unique_feeds_[i % COUNT];
				unique_feeds_[i % COUNT].sharedfeed_ = 0;
				begin--;
			}
		} else if ((cur_stype == stype || cur_stype == other)
				&& unique_feeds_[i % COUNT].sharedfeed_->merge() == merge) {
			feedids.push_back(unique_feeds_[i % COUNT].sharedfeed_->feed_id());
			size_--;
			begin = i;
			IncPartSize(cur_stype, -1);
		}
	}
	//  MCE_INFO("LiveFeed::Remove after." << DumpToString());
	return feedids;
}

vector<Ice::Long> UniqueFeedList::Remove(Ice::Long feed_id) {
	SharedFeedPtr feed = SharedFeedPool::instance().Get(feed_id);
	if (!feed) {
		return vector<Ice::Long> ();
	}
	return Remove(feed->SmallType(), feed->merge());
}

void UniqueFeedList::RemoveAll() {
	size_ = 0;
	end_ = 0;
	for (unsigned i = 0; i != part_size_.size(); ++i) {
		part_size_.at(i) = 0;
	}
}

void UniqueFeedList::RemoveExactly(const vector<Ice::Long> & fids) {
	set<Ice::Long> ids(fids.begin(), fids.end());
	int begin = -1;
	int c = 0;
	int size = size_;
	for (int i = end_ + COUNT - 1; i >= end_ && c < size; c++, i--) {
		int stype = unique_feeds_[i % COUNT].sharedfeed_->SmallType();
		if (begin > 0) {
			if (ids.count(unique_feeds_[i % COUNT].sharedfeed_->feed_id())) {
				unique_feeds_[i % COUNT].sharedfeed_ = 0;
				size_--;
				IncPartSize(stype, -1);
			} else {
				unique_feeds_[begin % COUNT] = unique_feeds_[i % COUNT];
				unique_feeds_[i % COUNT].sharedfeed_ = 0;
				begin--;
			}
		} else if (ids.count(unique_feeds_[i % COUNT].sharedfeed_->feed_id())) {
			size_--;
			begin = i;
			IncPartSize(stype, -1);
		}
	}
	return;
}

//void LiveFeed::MergeFeed(vector<vector<LiveFeedItem*> >& merge_feeds, vector<bool>& enter_hot_flag, 
void UniqueFeedList::MergeFeed(int uid, vector<vector<UniqueFeed*> >& merge_feeds,
		multimap<Ice::Long, int>& order_by_time, vector<int>& viewed_count,
		int begin, bool sort, int& edm_count, bool getcomment) const {
	vector<int> update_times;
	vector<bool> update_bool;
	vector<bool> comment_bool;
	map<pair<int, Ice::Long> , int> merge_index;
	vector<Ice::Long> weights;
	int pos = end_ + COUNT - 1;
	int c = 0;
	bool get_edm = false;
	for (; c < size_; c++, pos--) {
		UniqueFeed* uniquefeed = const_cast<UniqueFeed*> (unique_feeds_ + (pos % COUNT));
		SharedFeedPtr feed = unique_feeds_[pos % COUNT].sharedfeed_;
		//MCE_INFO("@@@@ --> " << feeds_[pos%COUNT].feed_->id_ << " actor:" << feeds_[pos%COUNT].feed_->actor_
		//          << " stype:" << feeds_[pos%COUNT].feed_->SmallType() << " viewed:" << feeds_[pos%COUNT].GetViewedCount()
		//          << " enter:" << feeds_[pos%COUNT].IsEntered() << " randomed:" << feeds_[pos%COUNT].IsRandomed());
		if (begin == 0) {
			uniquefeed->ClearRandomed();
		}
		if (uniquefeed->IsRandomed()) {
			continue;
		}
		//map<pair<int, Ice::Long>, int>::iterator it = merge_index.find(make_pair<int, Ice::Long>(feed->SmallType(), feed->merge()));
		map<pair<Ice::Int, Ice::Long> , int>::iterator it;

		//    Ice::Long feed_id = feed->feed_id();
		int stype = feed->SmallType();
		//    int merge = feed->merge();
		Ice::Int new_stype = MergeConfig::instance().Main2Comment(stype);
		new_stype = (feed->Ver() << 16) | new_stype;
		bool is_main = MergeConfig::instance().IsMain(stype);
		bool is_comment = MergeConfig::instance().IsComment(stype);
		if (!getcomment && is_comment) {
			continue;
		}
		it = merge_index.find(make_pair<Ice::Int, Ice::Long> (new_stype,
				feed->merge()));

		if (it != merge_index.end()) {
			//MCE_INFO("@@@@ --> " << feeds_[pos%COUNT].feed_->id_ << " actor:" << feeds_[pos%COUNT].feed_->actor_
			//        << " stype:" << feeds_[pos%COUNT].feed_->SmallType() << " viewed:" << feeds_[pos%COUNT].GetViewedCount()
			//        << " enter:" << feeds_[pos%COUNT].IsEntered());
			//if(feed->NewsMergeType() == FeedMergeAppend
			//  && merge_feeds[it->second].size() < 5){
			merge_feeds[it->second].push_back(uniquefeed);
			//  if(feeds_[pos%COUNT].IsEntered()){
			//    enter_hot_flag[it->second] = true;
			//  }
			//}
			//if(!feeds_[pos%COUNT].IsUpdateTime()){


			//第一个指明不更新时间，并且是普通feed，或者评论feed，或者前面没有评论feed的本体feed
			if (!update_bool[it->second] && (!is_main || (is_main
					&& !comment_bool[it->second]))) {
				update_times[it->second] = unique_feeds_[pos % COUNT].time_;
				Ice::Long w = 0;
				if (unique_feeds_[pos % COUNT].time_ < 0 || unique_feeds_[pos % COUNT].time_ > time(
						NULL)) {
					MCE_INFO("LiveFeed::MergeFeed --> merge edm " << uid << " feed:" << unique_feeds_[pos%COUNT].sharedfeed_->id_ << " time:" << unique_feeds_[pos%COUNT].time_);
					w = 0x0FFFFFFF;
					if (unique_feeds_[pos % COUNT].time_ < 0) {
						MCE_INFO("LiveFeed::MergeFeed --> merge edm first" << uid << " feed:" << unique_feeds_[pos%COUNT].sharedfeed_->id_ << " time:" << unique_feeds_[pos%COUNT].time_);
						unique_feeds_[pos % COUNT].time_ = time(NULL) + 8 * 60;
						unique_feeds_[pos % COUNT].weight_ = (9 << 16)
								| (unique_feeds_[pos % COUNT].weight_ & 0xFF00FFFF);
					}
				} else {
					w = CaculateWeightForSort(uid, unique_feeds_[pos % COUNT]);
				}
				weights[it->second] = w;
				//}
			}/*else{
			 Ice::Long w = CaculateWeightForSort(feeds_[pos%COUNT]);
			 if(w>weights[it->second]){
			 weights[it->second] = w;
			 }
			 }*/
			if (unique_feeds_[pos % COUNT].GetViewedCount() > viewed_count.at(it->second)) {
				viewed_count[it->second] = unique_feeds_[pos % COUNT].GetViewedCount();
			}
			if (is_comment) { //如果不是comment,不改变
				comment_bool[it->second] = true;
			}
			//MCE_INFO("@@@@@ --> " << feeds_[pos%COUNT].feed_->id_ << " actor:" << feeds_[pos%COUNT].feed_->actor_
			//        << " stype:" << feeds_[pos%COUNT].feed_->SmallType() << " viewed:" << feeds_[pos%COUNT].GetViewedCount()
			//        << " enter:" << feeds_[pos%COUNT].IsEntered());
		} else {
			//enter_hot_flag.push_back(feeds_[pos%COUNT].IsEntered());
			if (unique_feeds_[pos % COUNT].time_ < 0 || unique_feeds_[pos % COUNT].time_ > time(
					NULL)) {
				MCE_INFO("LiveFeed::MergeFeed --> edm " << uid << " feed:" << unique_feeds_[pos%COUNT].sharedfeed_->id_
						<< " time:" << unique_feeds_[pos%COUNT].time_ << " get_edm:" << get_edm);
				int tmp_edm_count = 0;
				if (unique_feeds_[pos % COUNT].time_ < 0) {
					if (!get_edm) {
						MCE_INFO("LiveFeed::MergeFeed --> edm top " << uid << " feed:" << unique_feeds_[pos%COUNT].sharedfeed_->id_ << " time:" << unique_feeds_[pos%COUNT].time_);
						unique_feeds_[pos % COUNT].time_ = time(NULL) + 8 * 60;
					} else {
						MCE_INFO("LiveFeed::MergeFeed --> edm not top " << uid << " feed:" << unique_feeds_[pos%COUNT].sharedfeed_->id_ << " time:" << unique_feeds_[pos%COUNT].time_);
						unique_feeds_[pos % COUNT].time_ = time(NULL);
					}
					unique_feeds_[pos % COUNT].weight_ = (9 << 16)
							| (unique_feeds_[pos % COUNT].weight_ & 0xFF00FFFF);
					tmp_edm_count = -1;
				} else {
					tmp_edm_count = 1;
				}
				if (!get_edm) {
					merge_feeds.push_back(vector<UniqueFeed*> ());
					merge_feeds.back().push_back(uniquefeed);
					weights.push_back(0x0FFFFFFF);
					viewed_count.push_back(unique_feeds_[pos % COUNT].GetViewedCount());
					update_times.push_back(unique_feeds_[pos % COUNT].time_);
					update_bool.push_back(unique_feeds_[pos % COUNT].IsUpdateTime());
					comment_bool.push_back(is_comment);
					merge_index.insert(make_pair<pair<int, Ice::Long> , int> (make_pair<
							int, Ice::Long> (new_stype, feed->merge()), merge_feeds.size()
							- 1));
					get_edm = true;
					edm_count = tmp_edm_count;
				}
			} else {
				merge_feeds.push_back(vector<UniqueFeed*> ());
				merge_feeds.back().push_back(uniquefeed);
				weights.push_back(CaculateWeightForSort(uid, unique_feeds_[pos % COUNT]));
				viewed_count.push_back(unique_feeds_[pos % COUNT].GetViewedCount());
				update_times.push_back(unique_feeds_[pos % COUNT].time_);
				update_bool.push_back(unique_feeds_[pos % COUNT].IsUpdateTime());
				comment_bool.push_back(is_comment);
				merge_index.insert(make_pair<pair<int, Ice::Long> , int> (make_pair<
						int, Ice::Long> (new_stype, feed->merge()), merge_feeds.size() - 1));

			}
		}
	}
	for (size_t i = 0; i < weights.size(); i++) {
		if (sort) {
			order_by_time.insert(make_pair<Ice::Long, int> (weights.at(i), i));
		} else {
			order_by_time.insert(make_pair<Ice::Long, int> (update_times.at(i), i));
		}
	}
}

vector<vector<vector<SharedFeedPtr> > > UniqueFeedList::GetGatheredFeed(int uid, int begin,
		int limit, bool hot, vector<vector<vector<SharedFeedPtr> > >& random,
		int& unviewed_count, int& edm_count) {
	unviewed_count = 0;
	vector<vector<UniqueFeed*> > merge_feeds;
	//vector<bool> enter_hot_flag;
	multimap<Ice::Long, int> order_by_time;
	vector<int> viewed_count;
	int least_viewed_count = RANDOM_VIEWED_COUNT;
	//multimap<int, int> least_viewed;
	vector<int> least_viewed;
	int least_viewed_index = -1;

	//MergeFeed(merge_feeds, enter_hot_flag, order_by_time, viewed_count, begin, hot);
	MergeFeed(uid, merge_feeds, order_by_time, viewed_count, begin, hot,
			edm_count, true);

	map<pair<int, int> , pair<int, int> > flags;
	vector<vector<vector<SharedFeedPtr> > > res;
	for (multimap<Ice::Long, int>::reverse_iterator it = order_by_time.rbegin(); it
			!= order_by_time.rend(); ++it) {
		//FeedPtr feed = merge_feeds.at(it->second).front()->feed_;
		SharedFeedPtr feed;
		vector<UniqueFeed*> & merge_group = merge_feeds.at(it->second);
		if (merge_group.empty()) {
			/*      MCE_WARN("GetGatered. empty merge_group");*/
			continue;
		}
		//从最新开始，取第一个非评论
		for (unsigned i = 0; i != merge_group.size(); ++i) {
			if (!MergeConfig::instance().IsComment(
					merge_group.at(i)->sharedfeed_->SmallType())) {
				feed = merge_group.at(i)->sharedfeed_;
				/*        MCE_DEBUG("GetGathered. i:" << i << " fid:" << feed->feed_id() << " stype:" << feed->SmallType() << " actor:" << feed->actor());*/
				break;
			}
		}
		if (!feed) { //所有feed都是评论,说明这个merge中没有本体
			/*      MCE_DEBUG("GetGathered. all feeds are comment. recent comment id:" << merge_group.at(0)->feed_->feed_id());*/
			continue;
		}
		int stype = feed->SmallType();
		//int weight_id = (merge_feeds.at(it->second).front()->weight_ >> 16) & 0xFF;

		//MCE_INFO("@@@@ --> " << feed->id_ << " actor:" << feed->actor_
		//          << " stype:" << feed->SmallType() << " viewed:" << merge_feeds.at(it->second).front()->GetViewedCount()
		//           << " least:" << least_viewed_count << " w:" << it->first
		//          );
		//<< " enter:" << merge_feeds.at(it->second).front()->IsEntered() << " randomed:"
		//<< merge_feeds.at(it->second).front()->IsRandomed() << " weight:" << it->first);

		if (begin == 0 && hot &&
		//(stype==708 ||stype==701 ||stype==709 ||stype==601) &&
				(stype == 601) && viewed_count[it->second] < least_viewed_count
				&& res.size() > 20) {
			if (viewed_count[it->second] < least_viewed_count) {
				least_viewed.clear();
			}
			least_viewed_index = it->second;
			least_viewed_count = viewed_count[it->second];
			//least_viewed.push_back(it->second);
			//least_viewed.insert(make_pair<int, int>(weight_id, it->second));
			least_viewed.push_back(it->second);
		}
		int actor = feed->actor_;
		//    static const int GATHER_TIME = 5 * 60; //TODO 修改

		map<pair<int, int> , pair<int, int> >::iterator flags_it = flags.find(
				feed->GatherKey());
		int GATHER_TIME =
				(feed->SmallType() == 2002 || feed->SmallType() == 3707) ? 60 * 60 * 24
						: 60 * 5;//对于stype为2002的，时间比较长

		//int deltat = abs(flags_it->second.first
		//    - merge_feeds.at(it->second).front()->time_);
		if (stype != 2901 && flags_it != flags.end() && (abs(flags_it->second.first
				- feed->time_) < GATHER_TIME/* || (stype==110 && deltat<6*60*60)*/)) {
			if (viewed_count[it->second] == 0) {
				unviewed_count++;
			}
			/*if(begin==0 && flags_it->second.second == least_viewed_index){
			 least_viewed.push_back(it->second);
			 }*/
			//if(hot && !enter_hot_flag[it->second]){
			//  continue;
			//}
			res.at(flags_it->second.second).push_back(vector<SharedFeedPtr> ());
			for (size_t i = 0; i < merge_feeds.at(it->second).size(); i++) {
				res.at(flags_it->second.second).back().push_back(merge_feeds.at(
						it->second).at(i)->sharedfeed_);
				//MCE_INFO("### gather --> " << merge_feeds.at(it->second).at(i)->sharedfeed_->id_ << " stype:" << merge_feeds.at(it->second).at(i)->sharedfeed_->SmallType());

				if (limit > 0 && (merge_feeds.at(it->second).at(i)->GetViewedCount()
						== 0)) {
					merge_feeds.at(it->second).at(i)->IncViewedCount();
					//MCE_INFO("@@@@@@@@@@ --> 1 feed:" << feed->feed_id() << " view:" << merge_feeds.at(it->second).front()->GetViewedCount());
				}
				if (limit > 0 && (((int) res.size() >= begin && (int) res.size()
						< begin + limit))) {
					merge_feeds.at(it->second).at(i)->IncViewedCount();
					//MCE_INFO("@@@@@@@@@@ --> 2 feed:" << feed->feed_id() << " view:" << merge_feeds.at(it->second).front()->GetViewedCount());

					//MCE_INFO("@@@@@@@@@@ --> feed:" << feed->feed_id() << " view:" << merge_feeds.at(it->second).front()->GetViewedCount());
				}
			}
		} else {
			//MCE_INFO("@@@@@@@@@@ --> feed:" << feed->feed_id() << " view:" << merge_feeds.at(it->second).front()->IsViewed());
			if (viewed_count[it->second] == 0) {
				unviewed_count++;
			}
			/*if(begin==0 && viewed_count[it->second] < least_viewed_count){
			 least_viewed.clear();
			 least_viewed_index = it->second;
			 least_viewed_count = viewed_count[it->second];
			 least_viewed.push_back(it->second);
			 }*/
			//if(merge_feeds.at(it->second).front()->GetViewedCount()==0){
			//  unviewed_count++;
			//}
			//if(hot && !enter_hot_flag[it->second]){
			//  continue;
			//}
			res.push_back(vector<vector<SharedFeedPtr> > ());
			res.back().push_back(vector<SharedFeedPtr> ());
			for (size_t i = 0; i < merge_feeds.at(it->second).size(); i++) {
				if (limit > 0 && (merge_feeds.at(it->second).at(i)->GetViewedCount()
						== 0)) {
					merge_feeds.at(it->second).at(i)->IncViewedCount();
					//MCE_INFO("########## --> 3 feed:" << feed->feed_id() << " view:" << merge_feeds.at(it->second).front()->GetViewedCount());
				}
				if (limit > 0 && ((int) res.size() >= begin && (int) res.size() < begin
						+ limit)) {
					//MCE_INFO("########## --> 4 feed:" << feed->feed_id() << " view:" << merge_feeds.at(it->second).front()->GetViewedCount());
					merge_feeds.at(it->second).at(i)->IncViewedCount();
				}
				res.back().back().push_back(merge_feeds.at(it->second).at(i)->sharedfeed_);
				//MCE_INFO("@@@@ push --> " << feed->id_ << " actor:" << feed->actor_
				//      << " stype:" << feed->SmallType() << " viewed:" << merge_feeds.at(it->second).front()->GetViewedCount()
				//      << " enter:" << merge_feeds.at(it->second).front()->IsEntered() << " randomed:"
				//      << merge_feeds.at(it->second).front()->IsRandomed() << " weight:" << it->first);


				//MCE_INFO("### gather --> first " << merge_feeds.at(it->second).at(i)->sharedfeed_->id_ << " stype:" << merge_feeds.at(it->second).at(i)->sharedfeed_->SmallType());
			}

			/*if(begin==0 && viewed_count[it->second] < least_viewed_count){
			 least_viewed_index = res.size()-1;
			 least_viewed_count = viewed_count[it->second];
			 }*/
			//      flags[make_pair<int, int> (actor, stype)] = make_pair<int, int> (feed->time_, res.size() - 1);
			flags[feed->GatherKey()] = make_pair<int, int> (feed->time_, res.size()
					- 1);
		}
	}
	//unviewed_count_ = unviewed_count;


	if (least_viewed.empty()) {
		//MCE_INFO("@@@@@@@@@@ --> no random feed");
		return res;
	}
	MCE_INFO("@@@@@@@@@@ --> least view size:" << least_viewed.size());

	/*
	 for(int i=0; i<least_viewed.size(); i++){
	 for(size_t j=0; j<merge_feeds.at(least_viewed[i]).size(); j++){
	 MCE_INFO("@@@@@@@@@ " << merge_feeds.at(least_viewed[i]).at(j)->feed_->id_);
	 }
	 }*/
	//int random_count = 3;
	//set<int> tmp;
	//srand(time(NULL));
	//for(int i=0; i<random_count; i++){
	for (int i = 0;;) {
		//int r = rand()%least_viewed.size();
		//pair<set<int>::iterator,bool> p = tmp.insert(r);
		//if(p.second){
		random.push_back(vector<vector<SharedFeedPtr> > ());
		random.back().push_back(vector<SharedFeedPtr> ());
		for (size_t j = 0; j < merge_feeds.at(least_viewed[i]).size(); j++) {
			int tmp_count = merge_feeds.at(least_viewed[i]).at(j)->GetViewedCount();
			for (int n = 0; n < INIT_VIEWED_COUNT - tmp_count; n++) {
				merge_feeds.at(least_viewed[i]).at(j)->IncViewedCount();
			}
			merge_feeds.at(least_viewed[i]).at(j)->IncViewedCount();
			merge_feeds.at(least_viewed[i]).at(j)->SetRandomed();
			random.back().back().push_back(
					merge_feeds.at(least_viewed[i]).at(j)->sharedfeed_);
		}
		i++;
		//}
		if (i >= RANDOM_COUNT || i >= (int) least_viewed.size()) {
			break;
		}
	}

	/*for(map<int, int>::reverse_iterator it = least_viewed.rbegin(); it!=least_viewed.rend()&&random_count>0; ++it){
	 //for(size_t i=0; i<least_viewed.size(); i++){
	 random.push_back(vector<vector<FeedPtr> >());
	 random.back().push_back(vector<FeedPtr>());
	 for(size_t j=0; j<merge_feeds.at(it->second).size(); j++){
	 merge_feeds.at(it->second).at(j)->IncViewedCount();
	 random.back().back().push_back(merge_feeds.at(it->second).at(j)->feed_);
	 }
	 random_count--;
	 }*/
	return res;
}

vector<vector<vector<SharedFeedPtr> > > UniqueFeedList::GetFeedByType(int uid,
		const IntSeq& types, int begin, int limit, bool hot) const {
	set<int> type_set;
	for (size_t i = 0; i < types.size(); ++i) {
		type_set.insert(types.at(i));
	}
	vector<vector<UniqueFeed*> > merge_uniquefeeds;
	//vector<bool> enter_hot_flag;
	multimap<Ice::Long, int> order_by_time;

	vector<int> tmp;
	//MergeFeed(merge_feeds, enter_hot_flag, order_by_time, tmp, begin, hot);
	int edm_count = 0;
	MergeFeed(uid, merge_uniquefeeds, order_by_time, tmp, begin, hot, edm_count, false);
	vector<vector<vector<SharedFeedPtr> > > res;
	for (multimap<Ice::Long, int>::reverse_iterator it = order_by_time.rbegin(); it
			!= order_by_time.rend(); ++it) {
		//if(hot && !enter_hot_flag[it->second]){
		//  continue;
		//}
		SharedFeedPtr feed = merge_uniquefeeds.at(it->second).front()->sharedfeed_;
		//int actor = feed->actor_;
		int type = feed->BigType();
		if (type_set.count(type) > 0) {
			res.push_back(vector<vector<SharedFeedPtr> > ());
			//res.back().push_back(merge_feeds.at(it->second));
			res.back().push_back(vector<SharedFeedPtr> ());
			for (size_t i = 0; i < merge_uniquefeeds.at(it->second).size(); i++) {
				res.back().back().push_back(merge_uniquefeeds.at(it->second).at(i)->sharedfeed_);
			}
		}
	}
	return res;
}

vector<vector<vector<SharedFeedPtr> > > UniqueFeedList::GetFeedByStype(int uid,
		const IntSeq& types, int begin, int limit, bool hot) const {
	set<int> type_set;
	for (size_t i = 0; i < types.size(); ++i) {
		type_set.insert(types.at(i));
	}
	vector<vector<UniqueFeed*> > merge_feeds;
	//vector<bool> enter_hot_flag;
	multimap<Ice::Long, int> order_by_time;

	vector<int> tmp;
	//MergeFeed(merge_feeds, enter_hot_flag, order_by_time, tmp, begin, hot);
	int edm_count = 0;
	MergeFeed(uid, merge_feeds, order_by_time, tmp, begin, hot, edm_count, false);
	vector<vector<vector<SharedFeedPtr> > > res;
	for (multimap<Ice::Long, int>::reverse_iterator it = order_by_time.rbegin(); it
			!= order_by_time.rend(); ++it) {
		//if(hot && !enter_hot_flag[it->second]){
		//  continue;
		//}
		SharedFeedPtr feed = merge_feeds.at(it->second).front()->sharedfeed_;
		//int actor = feed->actor_;
		int stype = feed->SmallType();
		if (type_set.count(stype) > 0) {
			res.push_back(vector<vector<SharedFeedPtr> > ());
			//res.back().push_back(merge_feeds.at(it->second));
			res.back().push_back(vector<SharedFeedPtr> ());
			for (size_t i = 0; i < merge_feeds.at(it->second).size(); i++) {
				res.back().back().push_back(merge_feeds.at(it->second).at(i)->sharedfeed_);
			}
		}
	}
	return res;
}

Ice::Long UniqueFeedList::CaculateWeightForSort(int uid,
		const UniqueFeed& uniquefeed) const {
	Ice::Long weight = 0;
	float weight_id = (uniquefeed.weight_ >> 16) & 0xFF;
	//if(weight_id<13){
	//  return 0;
	//}

	int frank = (uniquefeed.weight_ >> 24) >= 80 ? 100 : (uniquefeed.weight_ >> 24);
	float interaction = 5 * uniquefeed.sharedfeed_->reply_count_
			+ uniquefeed.sharedfeed_->click_count_;
	int range = uniquefeed.sharedfeed_->range_;

	if ((range <= 1) && interaction > 30) {
		interaction = 0.1;
	} else {
		interaction = interaction / pow(range + 30, 0.85);
	}

	int ft = int(10000 * (1 + weight_id / 100) / pow(time(NULL) - uniquefeed.time_
			+ 1, 1.0 / (weight_id)));

	int viewcount = uniquefeed.GetViewedCount();
	viewcount--;
	if (viewcount < 0) {
		viewcount = 0;
	}
	Ice::Long viewed_factor = 0;
	if (uid % 10 == 8) {
		viewed_factor = (Ice::Long) pow(0.7, viewcount) * 1200;
	} else if (uid % 10 == 0) {
		viewed_factor = (Ice::Long) pow(0.8, viewcount) * 500;
	}
	//weight = pow(0.95, uniquefeed.GetViewedCount())*ft + 40*interaction*weight_id + 10*frank;
	weight = Ice::Long(ft + viewed_factor + pow(0.6, viewcount) * (60
			* interaction * weight_id + 10 * frank));
	//weight = 0.95*ft + 40*interaction*weight_id + 10*frank;
	/*MCE_INFO("@@@@ LiveFeed::CaculateWeightForSort --> feed:" << uniquefeed.feed_->feed_id() << " " << uniquefeed.feed_->merge() << " actor:" << uniquefeed.feed_->actor_
	 << " range:" << range << " frank:" << frank << " wid:" << weight_id
	 << " click:" <<uniquefeed.feed_->click_count_ << " reply:" << uniquefeed.feed_->reply_count_
	 << " ft:" << ft << " viewed:" << viewcount << " weight:" << weight
	 << " inter:" << interaction);
	 */
	return weight;
}

Ice::Long UniqueFeedList::CaculateWeight(int uid, const UniqueFeed& uniquefeed,
		bool log) {
	Ice::Long weight = 0;//uniquefeed.feed_->time_;
	int weight_id = (uniquefeed.weight_ >> 16) & 0xFF;
	if (weight_id < 10) {
		return 0;
	}
	int frank = (uniquefeed.weight_ >> 24) >= 1 ? 3800 : 0;
	int interaction = 2 * uniquefeed.sharedfeed_->reply_count_
			+ uniquefeed.sharedfeed_->click_count_;
	int range = uniquefeed.sharedfeed_->range_;
	weight = (Ice::Long) ((80000 * interaction / pow(range + 20, 0.9) + 1)
			* weight_id + frank);
	if (uid == 238489851) {
		MCE_INFO("HotFeed::CaculateWeight --> uid:" << uid << " feed:" << uniquefeed.sharedfeed_->feed_id() << " merge:" << uniquefeed.sharedfeed_->merge()
				<< " stype:" << uniquefeed.sharedfeed_->SmallType()
				<< " wid:" << weight_id << " actor:" << uniquefeed.sharedfeed_->actor_ << " frank:"
				<< (uniquefeed.weight_>>24) << " reply:" << uniquefeed.sharedfeed_->reply_count_ << " click:" << uniquefeed.sharedfeed_->click_count_ << " range:" << range
				<< " time:" << (time(NULL)-uniquefeed.time_)<< " weight:" << weight);
	}
	return weight;
}

int UniqueFeedList::GetIndex(int stype) {
	int type = (stype / 100) * 100;
	switch (type) {
	case 100:
		return 1;
	case 500:
		return 2;
	case 600:
		return 3;
	case 700:
		return 4;
	case 2000:
		return 5;
	default:
		return 0;
	}
}
int UniqueFeedList::GetSize(int index) {
	if (index < 0 || index >= (int) part_size_.size()) {
		MCE_WARN("LiveFeedItemContainer::GetSize. index error:" << index);
		return -1;
	}
	return part_size_[index];
}
void UniqueFeedList::Add2(int uid, const UniqueFeed& item, bool updatetime,
		bool checkenter) {

	//  long fid = item.feed_->feed_id();
	int stype = item.sharedfeed_->SmallType();
	int index = GetIndex(stype);

	if (size_ >= COUNT) {
		RemoveOldest();
	}
	unique_feeds_[end_] = item;
	unique_feeds_[end_].SetUpdateTime(updatetime);
	if (checkenter) {
	} else {
		for (int i = 0; i < INIT_VIEWED_COUNT; i++) {
			unique_feeds_[end_].IncViewedCount();
		}
	}
	++size_;
	end_ = Mod(end_ + 1);
	++part_size_[index];
}

void UniqueFeedList::RemoveOldest() {
	int begin = Mod(end_ - size_);
	int cur = begin;
	bool shift = false;
	for (int consumed = 0; consumed < size_; ++consumed, cur = Mod(cur + 1)) {
		int cur_stype = unique_feeds_[cur].sharedfeed_->SmallType();
		int cur_index = GetIndex(cur_stype);
		if (cur_index == 0 || GetSize(cur_index) > KEY_COUNT) { //普通新鲜事 或者 重要新鲜事但是条数已经足够
			--size_;
			--part_size_.at(cur_index);
			if (consumed > 0) {
				ShiftItems(cur, begin);
			}
			ShiftStatTimer::instance().Update(cur, begin, COUNT);
			shift = true;
			return;
		}
	}
	//特殊情况下，所有的feed都是重要新鲜事，而且还没有填满，把第一条删除
	//一般不会出现这种情况
	if (!shift) {
		--size_;
		long fid = unique_feeds_[begin].sharedfeed_->feed_id();
		int cur_stype = unique_feeds_[begin].sharedfeed_->SmallType();
		int cur_index = GetIndex(cur_stype);
		--part_size_.at(cur_index);
		MCE_WARN("LiveFeed::RemoveOldest. all feed are important. size:" << size_+1
				<< "  del oldest feed:" << fid << " stype:" << cur_stype << " typeindex:" << cur_index << " begin:" << begin);
	}
}

//void LiveFeed::RemoveOldestKey(int stype, int index) {
//  int begin = Mod(end_ - size_);
//  int cur = begin;
//  int consumed = 0;
//  for (int consumed = 0; consumed < size_; ++consumed, cur = Mod(cur + 1)) {
//    int cur_stype = feeds_[cur].feed_->SmallType();
//    int cur_index = GetIndex(cur_stype);
//    if (cur_index == index) {
//      --size_;
//      --part_size_.at(index);
//      if (consumed > 0) {
//        ShiftItems(cur, begin);
//      }
//      break;
//    }
//  }
//}

//void LiveFeed::RemoveOldestNormal() {
//  int begin = Mod(end_ - size_);
//  int cur = begin;
//
//  for (int consumed = 0; consumed < size_; ++consumed, cur = Mod(cur + 1)) {
//    int cur_stype = feeds_[cur].feed_->SmallType();
//    int cur_index = GetIndex(cur_stype);
//    if (cur_index == 0) {
//      if (consumed > 0) {
//        ShiftItems(cur, begin);
//      }
//      --size_;
//      --(part_size_.at(cur_index));
//      break;
//    }
//  }
//}

string UniqueFeedList::DumpToString() {
	ostringstream oss;
	oss << "  index/bigtype/size: ";
	int i = 0;
	oss << i << "/normal/" << part_size_.at(i) << " ";
	++i;
	oss << i << "/100/" << part_size_.at(i) << " ";
	++i;
	oss << i << "/500/" << part_size_.at(i) << " ";
	++i;
	oss << i << "/600/" << part_size_.at(i) << " ";
	++i;
	oss << i << "/700/" << part_size_.at(i) << " ";
	++i;
	oss << i << "/2000/" << part_size_.at(i) << " ";
	//  for (unsigned i = 0; i != part_size_.size(); ++i) {
	//    oss << " index/size:" << i << "/" << part_size_.at(i);
	//  }
	oss << "  totalsize:" << size_;

	//oss << "\n";
	//int begin = Mod(end_ - size_);
	//int cur = begin;
	//int consumed = 0;
	//MCE_INFO(__FILE__ << __LINE__);
	//while (consumed++ < size_) {
	//  FeedPtr feed = feeds_[consumed - 1].feed_;
	//  int stype = feed->SmallType();
	//  oss << " " << consumed - 1 << "/fid:" << feed->feed_id() << "/stype:"
	//      << stype << "/index:" << GetIndex(stype) << "\n";
	//}

	return oss.str();
}

//class FeedWrapper{
//	long feed;
//	long merge;
//	int typeinfo; // 包含 NewsMergeType(2位), MiniMergeType(2位), ReplyType(2位), 空两位，其余24位为type 和 Stype通过mod 100确定大类和小类
//	int time;
//	int weight;//高8位为 friendrank,次高8位为baseWeightId, 低16位为weight
//	int actor;
//
//	int click;
//	int reply;
//	int range;
//	int sortinfo;
//};
void UniqueFeedList::IncView(const set<long> & showfids) {
	MCE_INFO("UniqueFeedList::IncView. size:" << showfids.size());
	int begin = Mod(end_ - size_);
	int cur = begin;
	bool shift = false;
	int inccnt = 0;
	int showsize = showfids.size();
	for (int consumed = 0; consumed < size_; ++consumed, cur = Mod(cur + 1)) {
		UniqueFeed & uniquefeed = unique_feeds_[cur];
		if(showfids.count(uniquefeed.sharedfeed_->feed_id())){
			int oldv = uniquefeed.GetViewedCount();
			uniquefeed.IncViewedCount();
//			MCE_INFO("LiveFeed::IncView. id:" << uniquefeed.sharedfeed_->feed_id()
//					<< " oldv:" << oldv << " newv:" << uniquefeed.GetViewedCount());
			++inccnt;
			if(inccnt >= showsize){
				return;
			}
		}
	}
}

void UniqueFeedList::GetFeedWrappers(const FeedFilterPtr & filter, vector<
		FeedWrapper> & fws, hash_map<long, SharedFeedPtr> & feedmap,int & edm_size) {
	edm_size =0;
	int pos = end_ + COUNT - 1;
	int c = 0;
	for (; c < size_; c++, pos--) {
//		res.push_back(feeds_ + pos % COUNT);

		FeedWrapper fw;
		UniqueFeed & uniquefeed = *(unique_feeds_ + pos % COUNT);
		SharedFeedPtr tmpfeed = uniquefeed.sharedfeed_;
		fw.feed = tmpfeed->feed_id();
		fw.merge = tmpfeed->merge();
		fw.typeinfo = tmpfeed->type_;
		int stype = tmpfeed->SmallType();
//		MCE_INFO("LiveFeed::GetFeedWrappers. fid:" << fw.feed << " view:" << uniquefeed.GetViewedCount());
		if (!filter->FilterType(stype)) {
			continue;
		}
		if (uniquefeed.time_ < 0 && uniquefeed.time_ > time(NULL)) {
			if (filter->show_edm()) {
				if(uniquefeed.time_ < 0 ){
					uniquefeed.time_ == time(NULL) + 8 * 60;
					edm_size++;
				}
			}else{
				continue;//如果不显示edm，就不用发给FeedSort服务
			}
		}
		fw.time = uniquefeed.time_;
		fw.actor = tmpfeed->actor();

		fw.interaction = tmpfeed->click_count_ <= 255 ? tmpfeed->click_count_ : 255;
		fw.interaction |= (tmpfeed->reply_count_ <= 255 ? tmpfeed->reply_count_ : 255) << 8;
		fw.interaction |= (tmpfeed->range_ <= 255 ? tmpfeed->range_ : 255) << 16;


//		fw.click = tmpfeed->click_count_;
//		fw.reply = tmpfeed->reply_count_;
//		fw.range = tmpfeed->range_;

		//	  float weight_id = (uniquefeed.weight_ >> 16) & 0xFF;
		fw.weight = uniquefeed.weight_;
		fws.push_back(fw);
		feedmap[tmpfeed->feed_id()] = tmpfeed;

	}
	return ;



//
//
//	//	SortParamPtr param = new SortParam;
//	edm_size =0;
//	int begin = Mod(end_ - size_);
//	int cur = begin;
//	bool shift = false;
//
//	for (int consumed = 0; consumed < size_; ++consumed, cur = Mod(cur + 1)) {
//		FeedWrapper fw = new FeedWrapper;
//		LiveFeedItem & uniquefeed = feeds_[cur];
//		FeedPtr tmpfeed = feeds_[cur].feed_;
//		fw.feed = tmpfeed->feed_id();
//		fw.merge = tmpfeed->merge();
//		fw.typeinfo = tmpfeed->type_;
//		int stype = tmpfeed->SmallType();
////		MCE_INFO("LiveFeed::GetFeedWrappers. fid:" << fw.feed << " view:" << uniquefeed.GetViewedCount());
//		if (!filter->FilterType(stype)) {
//			continue;
//		}
//		if (uniquefeed.time_ < 0 && uniquefeed.time_ > time(NULL)) {
//			if (filter->show_edm()) {
//				if(uniquefeed.time_ < 0 ){
//					uniquefeed.time_ == time(NULL) + 8 * 60;
//					edm_size++;
//				}
//			}else{
//				continue;//如果不显示edm，就不用发给FeedSort服务
//			}
//		}
//		fw.time = uniquefeed.time_;
//		fw.actor = tmpfeed->actor();
//		fw.click = tmpfeed->click_count_;
//		fw.reply = tmpfeed->reply_count_;
//		fw.range = tmpfeed->range_;
//		//	  float weight_id = (uniquefeed.weight_ >> 16) & 0xFF;
//		fw.weight = uniquefeed.weight_;
//		fws.push_back(fw);
//		feedmap[tmpfeed->feed_id()] = tmpfeed;
//	}
}

/*
 bool LiveFeed::CheckEnter(int uid, LiveFeedItem& item, bool log){
 item.SetEntered();
 }*/

/*
 void LiveFeed::CheckEnterHot(int uid, bool log){
 if(check_entered_>0 && (time(NULL)-check_entered_) < 5*60){
 return;
 }
 bool first_enter = false;
 if(check_entered_ < 0){
 first_enter = true;
 }
 check_entered_ = time(NULL);
 int pos = end_ + COUNT - 1;
 int c = 0;
 for(; c<size_; c++, pos--){
 LiveFeedItem& item = feeds_[pos%COUNT];
 Ice::Long weight = CaculateWeight(uid, item, log);
 int t = time(NULL) - item.feed_->time_;
 if(first_enter){
 t = 0;
 }
 if(item.IsEntered()){
 int leave_hot_weight = 0;
 if(hot_size_ <= 200){
 leave_hot_weight = hot_size_*20;
 }else{
 leave_hot_weight = 4000;
 }
 if(hot_size_>80 && weight < leave_hot_weight){
 item.ClearEntered();
 hot_size_--;
 }
 }else if(weight > t + 3608){

 item.SetEntered();
 hot_size_++;
 }
 }
 }*/
