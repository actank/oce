/*
 * LiveFeedCache.h
 *
 *  Created on: May 5, 2010
 *      Author: antonio
 */

#ifndef LIVEFEEDCACHE_H_
#define LIVEFEEDCACHE_H_

#include <vector>
#include "RFeed.h"
#include "ItemCache.h"
#include "IceLogger.h"
#include "EDMFeed.h"

namespace xce {
namespace feed {

using namespace std;
using namespace MyUtil;

class LiveFeedItem {
public:
  LiveFeedItem(const FeedPtr& feed, int weight, int time) :
    feed_(feed), weight_(weight), time_(time) {
    weight_ = weight_ & 0xFFFF0000;
  }
  LiveFeedItem() {
  }
  ;
  /*inline bool IsTop() const{
   return weight_ & 0x01;
   }
   inline void SetTop(){
   weight_ = weight_ | 0x01;
   }
   inline void ClearTop(){
   weight_ = weight_ & 0xFFFFFFFE;
   }*/
  inline void SetUpdateTime(bool up) {
    if (up) {
      weight_ = weight_ | 0x02;
    } else {
      weight_ = weight_ & 0xFFFFFFFD;
    }
  }
  inline bool IsUpdateTime() const {
    return weight_ & 0x02;
  }

  inline bool IsRandomed() const {
    return weight_ & 0x04;
  }
  inline void SetRandomed() {
    weight_ = weight_ | 0x04;
  }
  inline void ClearRandomed() {
    weight_ = weight_ & 0xFFFFFFFB;
  }

  inline void IncViewedCount() {
    int tmp = (weight_ & 0xFF00) >> 8;
    tmp++;
    if (tmp > 0xFF) {
      tmp = 0xFF;
    }
    weight_ = (weight_ & 0xFFFF00FF) | (tmp << 8);
  }
  inline int GetViewedCount() const {
    return (weight_ & 0xFF00) >> 8;;
  }
  /*inline int GetViewCount(){
   return weight_ & 0x000000FE;
   }
   inline void IncViewCount(){
   int count = GetViewCount();
   count = (count+1 > 0xFE) ? count : count+1;
   weight_ = (weight_&0xFFFFFF01) | (count&0xFE);
   }*/
  FeedPtr feed_;
  mutable int weight_;
  mutable int time_;
};

//class LiveFeedItemContainer {
//public:
//  LiveFeedItemContainer() {
//    for (unsigned i = 0; i < 5; ++i) {
//      key_size_.push_back(0);
//    }
//  }
//  int GetIndex(int stype) {
//    type = (stype / 100) * 100;
//    switch (type) {
//    case 100:
//      return 0;
//    case 500:
//      return 1;
//    case 600:
//      return 2;
//    case 700:
//      return 3;
//    case 2000:
//      return 4;
//    default:
//      return -1;
//    }
//  }
//  int GetSize(int index) {
//    if (index < 0 || index >= key_size_.size()) {
//      MCE_INFO("LiveFeedItemContainer::GetSize. index error:" << index);
//      return -1;
//    }
//    return key_size_[index];
//  }
//  void Add(const LiveFeedItem& item) {
//    ++size_;
//    int stype = item.feed_->SmallType();
//    int index = GetIndex(stype);
//    ++key_size_[index];
//    feeds_[end_] = item;
//    if (total_count_ > 250) {
//      if (index == -1 || GetSize(index) < KEY_COUNT) {
//        RemoveOldestNormal();
//        return;
//      } else {
//        RemoveOldestKey(stype, index);
//      }
//    }
//  }
//  void RemoveOldestKey(int stype, int index) {
//    int begin = end_ - size_;
//    int idx = begin;
//    while (idx < end_) {
//      int cur_stype = feeds_[idx].feed_->SmallType();
//      if (cur_stype != stype) {
//        continue;
//      }
//      --size_;
//      --key_size_.at(index);
//      for (int i = idx; idx != begin; i = (i - 1) % COUNT) {
//        feeds_[i % COUNT] = feeds_[(i - 1) % COUNT];
//      }
//      --size_;
//      break;
//    }
//  }
//
//  void RemoveOldestNormal() {
//    int begin = end_ - size_;
//    int idx = begin;
//    while (idx < end_) {
//      int cur_stype = feeds_[idx].feed_->SmallType();
//      int cur_index = GetIndex(cur_stype);
//      if (cur_index == -1) {
//        for (int i = idx; idx != begin; i = (i - 1) % COUNT) {
//          feeds_[i % COUNT] = feeds_[(i - 1) % COUNT];
//        }
//        --size_;
//        break;
//      }
//    }
//  }
//  void Remove(int stype, int merge) {
//    int begin = -1;
//    int c = 0;
//    int size = size_;
//    for (int i = end_ + COUNT - 1; i >= end_ && c < size; c++, i--) {
//      int cur_stype = feeds_[i % COUNT].feed_->SmallType();
//      if (begin > 0) {
//        if ((cur_stype == stype) && feeds_[i % COUNT].feed_->merge() == merge) {
//          feedids.push_back(feeds_[i % COUNT].feed_->feed_id());
//          feeds_[i % COUNT].feed_ = 0;
//          size_--;
//        } else {
//          feeds_[begin % COUNT] = feeds_[i % COUNT];
//          feeds_[i % COUNT].feed_ = 0;
//          begin--;
//        }
//      } else if ((cur_stype == stype) && feeds_[i % COUNT].feed_->merge()
//          == merge) {
//        feedids.push_back(feeds_[i % COUNT].feed_->feed_id());
//        size_--;
//        begin = i;
//      }
//    }
//    return feedids;
//  }
//
//  string DumpToString() {
//    ostringstream oss;
//    for (unsigned i = 0; i != key_size_.size(); ++i) {
//      oss << " key:" << i << " size:" << key_size_.at(i);
//    }
//    oss << " totalsize:" << size_;
//    return oss.str();
//  }
//  const static int COUNT = 250;
//  static const int KEY_COUNT = 30;
//  int size_;
//  vector<int> key_size_;
//  int end_;
//  LiveFeedItem feeds_[COUNT];
//};

class LiveFeed {
public:
  static const int COUNT = 250;
  static const int RANDOM_COUNT = 1;
  static const int RANDOM_VIEWED_COUNT = 2;
  static const int INIT_VIEWED_COUNT = 2;

  static const int KEY_COUNT = 30;//关键类型的条数
  vector<int> part_size_;

  LiveFeed() :
    size_(0), end_(0), check_entered_(-1) {
    unviewed_count_ = 0;
    memset(feeds_, 0, sizeof(feeds_));

    for (unsigned i = 0; i < 6; ++i) {
      part_size_.push_back(0);
    }
  }
  void Add(int uid, const FeedItem& item, bool updatetime, bool checkenter);
  void Add(int uid, const LiveFeedItem& item, bool updatetime, bool checkenter);
  //void Add(const FeedPtr& feed);
  //void Add(const vector<FeedItem>& items, bool updatetime);
  multimap<Ice::Long, FeedPtr> GetFeedSeq(int begin, int limit);
  vector<FeedPtr> GetFeed();
  vector<LiveFeedItem*> GetLiveFeedItem();
  vector<vector<vector<FeedPtr> > > GetGatheredFeed(int uid, int begin,
      int limit, bool hot, vector<vector<vector<FeedPtr> > >& random,
      int& unviewedcount, int& edm_count);
  vector<vector<vector<FeedPtr> > > GetFeedByType(int uid, const IntSeq& types,
      int begin, int limit, bool hot = false) const;
  vector<vector<vector<FeedPtr> > > GetFeedByStype(int uid,
      const IntSeq& types, int begin, int limit, bool hot = false) const;
  vector<Ice::Long> Remove(int stype, Ice::Long merge);
  vector<Ice::Long> Remove(Ice::Long feed_id);
  void RemoveAll();
  void RemoveExactly(const vector<Ice::Long> & fids);

  //bool                      CheckEnter(int uid, LiveFeedItem& item, bool log = false);
  Ice::Long CaculateWeight(int uid, const LiveFeedItem& live_item, bool log);
  Ice::Long CaculateWeightForSort(int uid, const LiveFeedItem& live_item) const;
  //void                      CheckEnterHot(int uid, bool log);
  //void                      MergeFeed(vector<vector<LiveFeedItem*> >& merge_feeds, vector<bool>& enter_hot_flag, 
  void MergeFeed(int uid, vector<vector<LiveFeedItem*> >& merge_feeds,
      multimap<Ice::Long, int>& order_by_time, vector<int>& viewed_count,
      int begin, bool sort, int& edm_count,bool getcomment) const;

  int UnviewedCount() {
    return unviewed_count_;
  }
  //bool inited(){
  //  return inited_;
  //}
  string DumpToString();
private:
  int GetIndex(int stype);
  int GetSize(int index);
  int IncPartSize(int stype, int step = 1) {
    part_size_.at(GetIndex(stype)) += step;
    return part_size_.at(GetIndex(stype));
  }
  void
  Add2(int uid, const LiveFeedItem& item, bool updatetime, bool checkenter);
  void RemoveOldest();
  //  void RemoveOldestNormal();
  void Remove(int stype, int merge);
  int Mod(int i) {
    return (i + COUNT) % COUNT;
  }
  void ShiftItems(int del_idx, int begin_idx) {
    del_idx = Mod(del_idx);
    begin_idx = Mod(begin_idx);
    for (int i = del_idx; i != begin_idx; i = Mod(i - 1)) {
      feeds_[Mod(i)] = feeds_[Mod(i - 1)];
    }
  }

  //bool          	inited_;
  LiveFeedItem feeds_[COUNT];
  int size_;
  int end_;
  int check_entered_;
  //int             hot_size_;
  int unviewed_count_;
};

class ShiftStatTimer: public Timer, public Singleton<ShiftStatTimer> {
public:
  ShiftStatTimer() :
    Timer(10 * 1000, 0) {
    Clear();
  }
  void Update(int del_idx, int begin_idx, int count) {
    ++add_cnt_;
    if (del_idx == begin_idx) {
      return;
    }
    ++shift_cnt_;
    if (del_idx > begin_idx) {
      shift_item_cnt_ += (del_idx - begin_idx);
    } else {
      shift_item_cnt_ += (del_idx + count - begin_idx);
    }
  }
  void handle() {
    MCE_INFO("ShiftStat: add_cnt_:" << add_cnt_ <<
        " add_with_shift_cnt:" << shift_cnt_ <<
        " shift_item_cnt_:" << shift_item_cnt_ <<
        " avarage shift item cnt:" << (float)shift_item_cnt_ /(float)add_cnt_);
    Clear();
  }
private:
  void Clear(){
    add_cnt_ = shift_cnt_ = shift_item_cnt_ = 0;
  }
  int add_cnt_;
  int shift_cnt_;
  int shift_item_cnt_;
};

}
}

#endif 
