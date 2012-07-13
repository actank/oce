/*
 * Common.h
 *
 *  Created on: Mar 14, 2011
 *      Author: yejingwei
 */

#ifndef UTIL_H_
#define UTIL_H_
#include "FeedClass.h"
#include "RFeed.h"
#include <map>
#include <set>
#include "Util.h"
#include "LogWrapper.h"
#include "FeedContentAdapter.h"
#include <string>
#include <cmath>
#include "IceLogger.h"

using namespace std;

namespace xce {
namespace feed {



template<typename T>
static string Print(const T & container) {
  ostringstream ios;
  ios << "(";
  bool first_item = true;
  int i = 0;
  for (typename T::const_iterator itr = container.begin(); itr
      != container.end(); ++itr, ++i) {
    if (first_item == true) {
      first_item = false;
    } else {
      ios << ",";
    }
    ios << *itr;
  }
  ios << ")";
  return ios.str();
}

class TimeStat {
  timeval _begin, _end;
  bool _log;
public:
  TimeStat(bool log = false) {
    _log = log;
    reset();
  }
  ~TimeStat() {
    //    if (_log) {
    //      MCE_INFO("######### " << getTime());
    //    }
  }
  void reset() {
    gettimeofday(&_begin, NULL);
  }
  float getTime() {
    gettimeofday(&_end, NULL);
    float timeuse = 1000000 * (_end.tv_sec - _begin.tv_sec) + _end.tv_usec
        - _begin.tv_usec;
    timeuse /= 1000;
    return timeuse;
  }

};

static void MakeTime2Data(FeedItemSeqSeq & itemseqseq, multimap<int,
    FeedItemSeq> & time2data) {
  for (FeedItemSeqSeq::iterator item_seq_itr = itemseqseq.begin(); item_seq_itr
      != itemseqseq.end(); ++item_seq_itr) { //每个FeedItemSeq代表一个MergeGroup,可以生成一个FeedData
    if (item_seq_itr->empty()) {
      MCE_DEBUG("MakeTime2Data item_seq empty");
      //     itemseqseq.erase(item_seq_itr);
      continue;
    }
    int time = 0;//记录一个mergegroup中最晚的feed的时间
    for (FeedItemSeq::iterator item_itr = item_seq_itr->begin(); item_itr
        != item_seq_itr->end(); ++item_itr) {
      time = max(time, item_itr->time);
    }
    time2data.insert(make_pair(time, *item_seq_itr));
  }
}

static void Rank(multimap<int, FeedItemSeq> & time2data, int begin, int limit,
    FeedItemSeqSeq & itemseqseq, vector<Ice::Long> & feedids) {
  //以时间排序
  int index = 0, count = 0;
  for (multimap<int, FeedItemSeq>::reverse_iterator mit = time2data.rbegin(); mit
      != time2data.rend(); ++mit) {
    if (index < begin - 1) {
      ++index;

      continue;
    }
    for (FeedItemSeq::iterator item_itr = mit->second.begin(); item_itr
        != mit->second.end(); ++item_itr) {
      feedids.push_back(item_itr->feed);
    }
    itemseqseq.push_back(mit->second);
    ++count;
    if (count >= limit) {
      break;
    }
  }
}

static bool GetFeedDict(const vector<long> & feedids, FeedContentDict & dict) {
  try {
    dict = FeedContentAdapter::instance().getFeedDict(feedids);
  } catch (Ice::Exception& e) {
    MCE_WARN("GetFeedDict  exception :" <<e);
    return false;
  } catch (std::exception& e) {
    MCE_WARN("GetFeedDict exception :" << e.what());
    return false;
  }
  MCE_INFO("GetFeedDict dict size = " << dict.size());
  return true;
}

static FeedDataSeq BuildFeedData(FeedItemSeqSeq & itemseqseq,
    FeedContentDict & dict) {
  FeedDataSeq result;
  for (FeedItemSeqSeq::iterator item_seq_itr = itemseqseq.begin(); item_seq_itr
      != itemseqseq.end(); ++item_seq_itr) { //每个FeedItemSeq代表一个MergeGroup,可以生成一个FeedData
    if (item_seq_itr->size() == 0) {
      MCE_DEBUG("BuildFeedData --> item_seq empty");
      continue;
    }
    FeedDataPtr feed_data;
    FeedReplyPtr feed_reply;
    ostringstream xml;
    bool data_assigned = false, reply_assigned = false;//表示data和reply是否已经被赋值
    for (FeedItemSeq::iterator item_itr = item_seq_itr->begin(); item_itr
        != item_seq_itr->end(); ++item_itr) {
      FeedContentDict::iterator dict_itr = dict.find(item_itr->feed);
      if (dict_itr != dict.end()) {
        if (!(dict_itr->second)) {
          MCE_DEBUG("Dict second is null. key = " << dict_itr->first);
          continue;
        }
        if (!(dict_itr->second->data)) {
          MCE_DEBUG("Dict data is null. key = " << dict_itr->first);
          continue;
        }
        xml << dict_itr->second->data->xml; //拼接
        if (data_assigned == false) {
          feed_data = dict_itr->second->data;
          data_assigned = true;
        }
        if (!(dict_itr->second->reply)) {
          MCE_DEBUG("Dict reply is null. key = " << dict_itr->first);
          continue;
        }
        if (reply_assigned == false) {
          feed_reply = dict_itr->second->reply;
          reply_assigned = true;
        }
      }
    }
    if (feed_data) {
      if (feed_reply) {
        xml << feed_reply->oldReply << feed_reply->newReply << "<reply_count>"
            << feed_reply->count << "</reply_count>";
      }
      feed_data->xml = xml.str();
      result.push_back(feed_data);
    }
  }

  MCE_DEBUG("BuildFeedData --> FeedDataSeq size = " << result.size());
  return result;
}

static vector<int> Map2Vector(const map<int, vector<int> > & m) {
  vector<int> res;
  for (map<int, vector<int> >::const_iterator i = m.begin(); i != m.end(); ++i) {
    res.insert(res.end(), i->second.begin(), i->second.end());
  }
  return res;
}

static vector<int> Intersect(const set<int> & a, const vector<int> &b) {
  vector<int> res;
  for (vector<int>::const_iterator i = b.begin(); i != b.end(); ++i) {
    if (a.count(*i)) {
      res.push_back(*i);
    }
  }
//  MCE_INFO("Intersect. a:" << Print(a) << " b:" << Print(b) << " res:" << Print(res));
  return res;
}

}
}

#endif /* COMMON_H_ */
