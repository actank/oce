/*
 * FocusManagerI.h
 *
 *  Created on: May 21, 2010
 *      Author: yejingwei
 */
#include "FocusManagerI.h"
#include "FocusCache.h"
#include "FeedMiniAdapter.h"
#include "FeedContentAdapter.h"
#include <IceUtil/IceUtil.h>
#include "RFeed.h"
#include <utility>
#include "TaskManager.h"
#include <vector>
#include <ctime>
#include <map>
#include <algorithm>
#include "ServiceI.h"
#include "FeedMemcLoaderAdapter.h"
#include "FeedItemCacheByUseridReplicaAdapter.h"
#include <boost/foreach.hpp>
#include <BuddyRelationReplicaAdapter.h>

using namespace MyUtil;
using namespace std;
using namespace xce::feed;
using namespace boost;

//-----------------------------------------------------------------------------------------------

class PrintHelper {
public:
  template<typename T>
  static string Print(const T & container) { //将容器内容打印成字符串
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
};


void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&FocusManagerI::instance(), service.createIdentity(
      "M", ""));
  FocusCache::instance().Init();
  TaskManager::instance().scheduleRepeated(new PrintTask());
}


//-----------------------------------------------------------------------------------------------


void FocusManagerI::CheckLoad(int uid, vector<Ice::Long> & fids, FeedContentDict & dict) {
  vector<Ice::Long> miss_ids;
  for (unsigned i = 0; i != fids.size(); ++i) {
    if (dict.find(fids.at(i)) == dict.end()) {
      miss_ids.push_back(fids.at(i));
    }
  }

  if(miss_ids.empty()){
    return;
  }

  if (!dict.empty()) {
    try {
      FeedMemcLoaderAdapter::instance().LoadFeedSeq(miss_ids);
    } catch (Ice::Exception& e) {
      MCE_WARN("FocusManagerI::CheckLoad --> LoadFeedSeq uid:" << uid << " size:" << miss_ids.size() << " err, " << e );
      return;
    }
    MCE_INFO("FocusManagerI::CheckLoad --> LoadFeedSeq " << uid << " size:" << miss_ids.size());
  }else{
    FeedContentDict dict_loader;
    try {
      dict_loader= FeedMemcLoaderAdapter::instance().GetFeedDict(miss_ids, 100);
    } catch (Ice::Exception& e) {
      MCE_WARN("FocusManagerI::CheckLoad --> GetFeedDict uid:" << uid << " size:" << miss_ids.size() << " err, " << e );
      return;
    }
    MCE_INFO("FocusManagerI::CheckLoad --> GetFeedDict " << uid << " size:" << miss_ids.size() << " res:" << dict_loader.size());
    dict.insert(dict_loader.begin(),dict_loader.end());
  }
}

FeedDataSeq FocusManagerI::GetFocusFeedData(int user, int begin, int limit,
    const ::Ice::Current&) {
  MCE_INFO("FocusManagerI::GetFocusFeedData --> user = " << user);

  //-------------------------------------
  //FocusCache::instance().SetUnreadCount(user);

  //查询user关注的朋友id
  FeedDataSeq result;
  vector<int> targets;
  try {
    set<int> tmp = FocusCache::instance().GetTargets(user);

    if (tmp.empty()) { //该用户没有关注任何好友
      MCE_DEBUG("FocusManagerI::GetFocusFeedData ---> source = " << user << ", do not set focus");
      return result;
    }
    targets.insert(targets.end(),tmp.begin(),tmp.end());
  } catch (std::exception& e) {
    MCE_WARN("FocusManagerI::GetFocusFeedData --> exception :" << e.what());
  }

	MCE_INFO("FocusManagerI::GetFocusFeedData. get targtes. uid:" << user
			<< " begin:" << begin
			<< " limit:" << limit
			<< " targets:" << PrintHelper::Print(targets)
	);
  FocusCache::instance().SetUnreadCount(user,0);

  //white list, by guanghe.ge 2011-12-06
  vector<Ice::Long> feedids;
  FeedItemSeqSeq itemseqseq;
  if (user == 233940269 //葛广贺
   || user == 238489851 //袁飞
   || user == 220678672 //叶经纬
   || user == 40125272 //刘群
   || user == 225092473 //刘迪
   || user == 128487631 //张铁安
   || user == 229406110 //李伟
   || user == 257115362 //闫志东
   || user == 239556923 //赵继承
   || user == 241610785 //康伟
		) {

	  try {
		  MCE_INFO("FocusManagerI::GetFocusFeedData, white list, userid: " << user << " targets.size: " << targets.size() << " begin: " << begin << " limit: " << limit);

		  MyUtil::IntSeq types = getFixedType();
		  FeedMetaSeq metas = FeedItemCacheByUseridReplicaAdapter::instance().getByTypes(targets, begin, limit, types);
		  MCE_DEBUG("FocusManagerI::GetFocusFeedData, white list, userid: " << user << " metas.size: " << metas.size());

		  feedids.reserve(metas.size());
		  for(FeedMetaSeq::iterator it = metas.begin();
				  it != metas.end(); ++it) {
			  feedids.push_back(it->feedid);

			  FeedItem item;
			  item.feed = it->feedid;
			  item.merge = it->mergeid;
			  item.type = it->type;
			  item.time = it->time;
			  item.weight = it->weight;
			  item.actor = it->userid;

		  	  FeedItemSeq items;
			  items.push_back(item);
			  itemseqseq.push_back(items);
		  }
		
		  MCE_DEBUG("FocusManagerI::GetFocusFeedData, white list, userid: " << user << " feedids.size(): " << feedids.size() << " itemseqseq.size: " << itemseqseq.size());

		  FocusCache::instance().UpdateMaxFeed(user,itemseqseq);
	  } catch (Ice::Exception& e) {
		  MCE_WARN("FocusManagerI::GetFocusFeedData, ByUseridReplicaAdapter::get " << e.what());
	  } catch (...) {
		  MCE_WARN("FocusManagerI::GetFocusFeedData, ByUseridReplicaAdapter::get UNKNOWN Exception!");
	  }
  } else {
  //取出关注好友的FeedItem,每个FeedItemSeq包含一个MergeGroup中所有的Feed
  //FeedItemSeqSeq itemseqseq =
  itemseqseq =
      FeedMiniAdapter::instance().GetFeedItemSeqSeqByUsers(targets);
  FocusCache::instance().UpdateMaxFeed(user,itemseqseq);
/*  set<time_t> timeset;
  for(FeedItemSeqSeq::iterator fit = itemseqseq.begin() ; fit != itemseqseq.end() ;++fit){
    const time_t t = fit->at(0).time;
    timeset.insert(t);
  }
  for(set<time_t>::iterator sit = timeset.begin() ; sit != timeset.end() ; ++sit){
    MCE_DEBUG(ctime(&(*sit)));
  }*/

  multimap<int, FeedItemSeq> time2data;
  for (FeedItemSeqSeq::iterator item_seq_itr = itemseqseq.begin(); item_seq_itr
      != itemseqseq.end(); ++item_seq_itr) { //每个FeedItemSeq代表一个MergeGroup,可以生成一个FeedData
    if (item_seq_itr->empty()) {
      MCE_DEBUG("FocusManagerI::GetFocusFeedData --> item_seq empty");
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

  itemseqseq.clear();//清空，以保存排序后的itemseq

  //以时间排序
  //vector<Ice::Long> feedids;
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
	MCE_INFO("FocusManagerI::GetFocusFeedData. uid:" << user << " from feedmini");

  }
  MCE_DEBUG("FocusManagerI::GetFocusFeedData --> feedids size = " << feedids.size());
  if (feedids.empty()) {
    return result;
  }

  //在FeedContent中取FeedId对应的所有FeedContent
  FeedContentDict dict;
  try {
    dict = FeedContentAdapter::instance().getFeedDict(feedids);
  } catch (Ice::Exception& e) {
    MCE_WARN("FocusManagerI::GetFocusFeedData --> FeedContentAdapter.getFeedDict --> exception :" <<e);
  } catch (std::exception& e) {
    MCE_WARN("FocusManagerI::GetFocusFeedData --> FeedContentAdapter.getFeedDict --> exception :" << e.what());
  }

  CheckLoad(user,feedids,dict);
  MCE_DEBUG("FocusManagerI::GetFocusFeedData --> dict size = " << dict.size());
  //根据feedid和feedcontentdict，生成feeddata ;
  try {
    result = BuildFeedData(itemseqseq, dict);
  } catch (Ice::Exception& e) {
    MCE_WARN("FocusManagerI::GetFocusFeedData --> BuildFeedData --> exception :" <<e);
  } catch (std::exception& e) {
    MCE_WARN("FocusManagerI::GetFocusFeedData --> BuildFeedData --> exception :" << e.what());
  }

/*  for(FeedDataSeq::iterator fit = result.begin() ; fit != result.end() ;++fit){
    const time_t t = (*fit)->time;
    MCE_DEBUG(ctime(&t));
  }*/
	MCE_INFO("FocusManagerI::GetFocusFeedData. uid:" << user
			<< " begin:" << begin
			<< " limit:" << limit
			<< " seqseq size:" << itemseqseq.size()
			<< " feedids:" << feedids.size()
			<< " targets:" << PrintHelper::Print(targets)
			<< " dictsize:" << dict.size()
			<< " res:" << result.size()
			);
  return result;
}

FeedDataSeq FocusManagerI::GetFocusFeedDataByStype(int user, int begin,int limit, const vector<int> & stypes, const ::Ice::Current&){
	MCE_INFO("FocusManagerI::GetFocusFeedDataByStype, user: " << user << " begin: " << begin << " limit: " << limit << " types.size: " << stypes.size());
	return GetFocusFeedDataImpl(user,begin,limit,stypes);
}

void FocusManagerI::GetContents(int uid, vector<Ice::Long> & fids, FeedContentDict & dict){
	if(fids.empty())
		return;
  try {
    dict = FeedContentAdapter::instance().getFeedDict(fids);
  } catch (Ice::Exception& e) {
    MCE_WARN("FocusManagerI::GetContents.uid:"<<uid<<"  exception0 :" <<e);
  } catch (std::exception& e) {
    MCE_WARN("FocusManagerI::GetContents.uid:"<<uid<<"  exception1 :" <<e.what());
  }
  CheckLoad(uid,fids,dict);
}

FeedDataSeq FocusManagerI::GetFocusFeedDataImpl(int user, int begin, int limit,const vector<int> stypes) {
  FeedDataSeq result;
  set<int> tmp = FocusCache::instance().GetTargets(user);

  if (tmp.empty()) { //该用户没有关注任何好友
  	MCE_INFO("FocusManagerI::GetFocusFeedDataImpl. uid:" << user
  			<< " begin:" << begin
  			<< " limit:" << limit
  			<< " stypes:" << PrintHelper::Print(stypes)
  			<< ", do not set focus.");
  	return result;
  }
  vector<int> targets(tmp.begin(),tmp.end());
  FocusCache::instance().SetUnreadCount(user,0);

  //by guanghe.ge 2011-12-06
  vector<Ice::Long> feedids;
  FeedItemSeqSeq itemseqseq;

  try {
	  FeedMetaSeq metas = FeedItemCacheByUseridReplicaAdapter::instance().getByTypes(targets, begin, limit,stypes);

	  feedids.reserve(metas.size());
	  for(FeedMetaSeq::iterator it = metas.begin();
			  it != metas.end(); ++it) {
		  feedids.push_back(it->feedid);

		  FeedItem item;
		  item.feed = it->feedid;
		  item.merge = it->mergeid;
		  item.type = it->type;
		  item.time = it->time;
		  item.weight = it->weight;
		  item.actor = it->userid;

		  FeedItemSeq items;
		  items.push_back(item);
		  itemseqseq.push_back(items);
	  }

	  FocusCache::instance().UpdateMaxFeed(user,itemseqseq);
  } catch (Ice::Exception& e) {
	  MCE_WARN("FocusManagerI::GetFocusFeedData, ByUseridReplicaAdapter::get " << e.what());
  } catch (...) {
	  MCE_WARN("FocusManagerI::GetFocusFeedData, ByUseridReplicaAdapter::get UNKNOWN Exception!");
  }

  FeedContentDict dict;
  GetContents(user,feedids,dict);

  result = BuildFeedData(itemseqseq, dict);

  MCE_INFO("FocusManagerI::GetFocusFeedDataImpl. uid:" << user
		  << " begin:" << begin
		  << " limit:" << limit
		  << " stypes:" << PrintHelper::Print(stypes)
		  << " feedids:" << feedids.size()
		  << " dictsize:" << dict.size()
		  << " res:" << result.size()
		  );
  return result;
}



bool FocusManagerI::AddFocuses(int user,
    int target, const ::Ice::Current&) {
  MCE_INFO("FocusManagerI::AddFocuses --> uid:"<<user << ", target:"<<target);
  int ret_code = FocusCache::instance().AddTargets(user, target);
  return ret_code == 0;
}

//成功返回值：添加成功
//失败返回值1：关注数量已达上限，添加失败
//失败返回值2：已关注过该好友，添加失败
//失败返回值3：欲添加的用户非当前用户的好友，添加失败
//失败返回值4：其它原因导致的添加失败

int FocusManagerI::AddFocusWithCheck(int user,
    int target, const ::Ice::Current&) {
  MCE_INFO("FocusManagerI::AddFocusWithCheck --> uid:"<<user << ", target:"<<target);
  int ret_code = 4;//默认的错误值
  xce::buddy::Relationship relation;
  relation.from = user;
  relation.to = target;
  xce::buddy::BuddyDesc desc = xce::buddy::adapter::BuddyRelationCacheAdapter::instance().getRelation(relation);
  if(desc != xce::buddy::Friend){
  	ret_code = 3;//非好友
  } else {
  	ret_code  = FocusCache::instance().AddTargets(user, target);
  }
  MCE_INFO("FocusManagerI::AddFocusWithCheck --> uid:"<<user << ", target:"<<target << " ret_code:" << ret_code);
  return ret_code;
}

void FocusManagerI::DelFocuses(int user,
    int target, const ::Ice::Current& current) {
  MCE_INFO("FocusManagerI::DelFocuses --> uid:"<<user << ", target:"<<target << " ip:" << current.con->toString());
  FocusCache::instance().DelTargets(user, target);
}

MyUtil::IntSeq FocusManagerI::GetFocuses(int user, const Ice::Current&){
  
  MCE_INFO("FocusManagerI::GetFocuses --> uid:"<<user);
  set<int> tmp = FocusCache::instance().GetTargets(user);
  if(tmp.empty()){
    return MyUtil::IntSeq();
  }
  MyUtil::IntSeq res;
  res.insert(res.end(),tmp.begin(), tmp.end());
  return res;
}

FeedDataSeq FocusManagerI::BuildFeedData(FeedItemSeqSeq & itemseqseq,
    FeedContentDict & dict) {
  FeedDataSeq result;
  for (FeedItemSeqSeq::iterator item_seq_itr = itemseqseq.begin(); item_seq_itr
      != itemseqseq.end(); ++item_seq_itr) { //每个FeedItemSeq代表一个MergeGroup,可以生成一个FeedData
    if (item_seq_itr->size() == 0) {
      MCE_DEBUG("FocusManagerI::BuildFeedData --> item_seq empty");
      continue;
    }
    FeedDataPtr feed_data;
    FeedReplyPtr feed_reply;
    ostringstream xml;
    bool data_assigned = false,reply_assigned = false;//表示data和reply是否已经被赋值
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

        if(data_assigned == false){
        	 xml << dict_itr->second->data->xml; //拼接
           feed_data = dict_itr->second->data;
           data_assigned = true;
           if(dict_itr->second->reply){
          	 feed_reply = dict_itr->second->reply;
          	 reply_assigned = true;
           }
        }

//        xml << dict_itr->second->data->xml; //拼接
//        if (data_assigned == false) {
//          feed_data = dict_itr->second->data;
//          data_assigned = true;
//        }
//        if (!(dict_itr->second->reply)) {
//          MCE_DEBUG("Dict reply is null. key = " << dict_itr->first);
//          continue;
//        }
//        if(reply_assigned == false){
//          feed_reply = dict_itr->second->reply;
//          reply_assigned = true;
//        }
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

  MCE_DEBUG("FocusManagerI::BuildFeedData --> FeedDataSeq size = " << result.size());
  return result;
}
int FocusManagerI::GetFocusFeedIncCount(int user, const ::Ice::Current& ) {
  //if(user%5>1){
  //  return 0;
  //}
  vector<int> targets;
  try { 
    set<int> tmp = FocusCache::instance().GetTargets(user);
    if (tmp.empty()) { 
      MCE_INFO("FocusManagerI::GetFocusFeedIncCount ---> source = " << user << ", do not set focus");
      return 0;
    }
    targets.insert(targets.end(),tmp.begin(),tmp.end());
  } catch (std::exception& e) {
      MCE_WARN("FocusManagerI::GetFocusFeedIncCount --> exception :" << e.what());
  }

  if(!FocusCache::instance().IsGetIncCount(user)) {
    int count = FocusCache::instance().GetUnreadCount(user);
    return FocusCache::instance().GetUnreadCount(user);
  }

  FeedItemSeqSeq itemseqseq;        
  try {
    itemseqseq =
            FeedMiniAdapter::instance().GetFeedItemSeqSeqByUsers(targets);
  } catch (Ice::Exception& e) {
    MCE_WARN("FocusManagerI::GetFocusFeedIncCount-->call FeedMinin.GetFeedItemSeqSeqByUsers err"<<e);
  }
  int inc_count = 0;
  inc_count = FocusCache::instance().GetIncCount(user, itemseqseq); 
  MCE_INFO("FocusManagerI::GetFocusFeedIncCount --> " << user << " count:" << inc_count);
  return inc_count;
}

MyUtil::IntSeq FocusManagerI::getFixedType() {
	IntSeq result;
	int type_size = sizeof(TYPE)/sizeof(TYPE[0]);
	result.reserve(type_size);
	for (int i = 0; i < type_size; i++) {
		result.push_back(TYPE[i]);
	}
	return result;
}

//
//FeedDataSeq FocusManagerI::BuildFeedData(FeedItemSeqSeq & itemseqseq,
//    FeedContentDict & dict) {
//  FeedDataSeq result;
//  multimap<int, FeedDataPtr> time2data;
//  for (FeedItemSeqSeq::iterator item_seq_itr = itemseqseq.begin(); item_seq_itr
//      != itemseqseq.end(); ++item_seq_itr) { //每个FeedItemSeq代表一个MergeGroup,可以生成一个FeedData
//    if (item_seq_itr->size() == 0) {
//      MCE_DEBUG("FocusManagerI::BuildFeedData --> item_seq empty");
//      continue;
//    }
//    FeedDataPtr feed_data;
//    FeedReplyPtr feed_reply;
//    ostringstream xml;
//    int time = 0;//记录一个mergegroup中最晚的feed的时间
//    bool assigned = false;//表示data和reply是否已经被赋值
//    for (FeedItemSeq::iterator item_itr = item_seq_itr->begin(); item_itr
//        != item_seq_itr->end(); ++item_itr) {
//      FeedContentDict::iterator dict_itr = dict.find(item_itr->feed);
//      if (dict_itr != dict.end()) {
//        if (!(dict_itr->second)) {
//          MCE_DEBUG("Dict second is null. key = " << dict_itr->first);
//          continue;
//        }
//        if (!(dict_itr->second->data)) {
//          MCE_DEBUG("Dict data is null. key = " << dict_itr->first);
//          continue;
//        }
//        if (!(dict_itr->second->reply)) {
//          MCE_DEBUG("Dict reply is null. key = " << dict_itr->first);
//          continue;
//        }
//        xml << dict_itr->second->data->xml; //拼接
//        if (assigned == false) {
//          feed_data = dict_itr->second->data;
//          feed_reply = dict_itr->second->reply;
//          assigned = true;
//        }
//        time = time > dict_itr->second->data->time ? time
//            : dict_itr->second->data->time;
//      }
//    }
//    if (feed_data) {
//      if (feed_reply) {
//        xml << feed_reply->oldReply << feed_reply->newReply << "<reply_count>"
//            << feed_reply->count << "</reply_count>";
//      }
//      feed_data->xml = xml.str();
//      time2data.insert(make_pair(time, feed_data));
//    }
//  }
//
//  //以时间排序
//  for (multimap<int, FeedDataPtr>::reverse_iterator mit = time2data.rbegin(); mit
//      != time2data.rend(); ++mit) {
//    result.push_back(mit->second);
//  }
//  MCE_DEBUG("FocusManagerI::BuildFeedData --> FeedDataSeq size = " << result.size());
//  return result;
//}

void TypeFilter::Filter(FeedItemSeqSeq & itemseqseq){
	for(FeedItemSeqSeq::iterator itr = itemseqseq.begin(); itr != itemseqseq.end();){
		FeedItemSeq & seq = *itr;
		if(seq.empty()){
			itr = itemseqseq.erase(itr);
			continue;
		}
		int stype = seq.begin()->type & 0xFFFF;
		if(!stypes_.count(stype)){
			itr = itemseqseq.erase(itr);
		}else{
			++itr;
		}
	}
}
