#include "multicompute.h"

#include "FriendRankCacheAdapter.h"
#include "BuddyByIdReplicaAdapter.h"
#include "PageCommonFansCacheAdapter.h"
#include "FansPageCacheAdapter.h"

#include "base/ptime.h"
#include "base/logging.h"
#include <sstream>

#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>

namespace xce {
namespace buddy {

CMessage::CMessage(int uid): uid_(uid) {
 
}

CMessage::~CMessage() {

}

std::string CMessage::Comb(std::string prefix, int id) {
  std::ostringstream oss;
	oss << prefix << "_" << id;
  return oss.str();
}

void CMessage::Run() {
	Calculate(uid_);	
}

void CMessage::ReverseSort(std::map<int, std::vector<int> >& map1, std::map<int, std::vector<int> >& map2) {
	std::map<int, std::vector<int> >::const_iterator iter1 = map1.begin();
	for (; iter1 != map1.end(); ++iter1) {
		int uid = iter1->first;
		for (std::vector<int>::const_iterator sub_iter1 = iter1->second.begin(); sub_iter1
				!= iter1->second.end(); ++sub_iter1) {
			int pid = *sub_iter1;
			std::map<int, std::vector<int> >::iterator findIter =
					map2.find(pid);
			if (findIter != map2.end()) { //已经有
				findIter->second.push_back(uid);
			} else { //没有 需要增加pair
				std::vector<int> v;
				v.push_back(uid);
				map2.insert(std::make_pair(pid, v));
			}
		}
	}
}

void CMessage::Calculate(int uid) {
	// 1:取到 uid 好友列表 friends
  // 2:取uid + friends 用户的关注Page.map<uid, sequence<pid>>
  // 3:做倒排  排成map<pid, sequence<uid>>
  // 4:过滤page的黑名单   取PageFans服务的批量接口得到黑名单
  // 5:按FriendRank排序! fuck the world
  // 6:搞成数据,保存到Cache中
	using namespace MyUtil;
	using namespace xce::buddy::adapter;
	using namespace xce::buddy;
  PTIME(pt, Comb("Calculate_", uid), true, 0);
	MyUtil::IntSeq friends;  //
	try {
		xce::socialgraph::RankSeq rankSeq = xce::socialgraph::FriendRankCacheAdapter::instance().GetRank(uid);
    xce::socialgraph::RankSeq::const_iterator rsIter = rankSeq.begin();
    for (; rsIter != rankSeq.end(); ++rsIter) {
			friends.push_back(rsIter->userid);
		}
		/*
		BuddyByIdCacheAdapter& buddyAdapter = BuddyByIdCacheAdapter::instance();
		friends = buddyAdapter.getFriendListAsc(uid, 2000);
    */
		if (friends.empty()) {
		//	LOG(INFO) << "uid:" << uid << " friends.size()==0";
			return;
		}
	} catch (...) {
		LOG(LERROR) << "getFriendListAsc(" << uid << ", 2000) exception...";
		return;
	}
	Int2IntSeqMap u2pSeqMap; // 取FansPage服务得到的数据 别忘记了try catch!
	try {
		PageEntryListMap pageEntryListMap = FansPageCacheAdapter::instance().getPagesByUidBatch(friends);
//		LOG(INFO) << "uid:" << uid << " pageEntryListMap.size():" << pageEntryListMap.size();
		PageEntryListMap::const_iterator iter = pageEntryListMap.begin();
		for (; iter != pageEntryListMap.end(); ++iter) {
			int fid = iter->first;
      PageEntryList::const_iterator siter = iter->second.begin();
      MyUtil::IntSeq pages;
      for (; siter != iter->second.end(); ++siter) {
        int pageId = (*siter)->pageId;
				pages.push_back(pageId);
//				LOG(INFO) << "friendid:" << fid << "\t" << "pageid:" << pageId;
			}
			if (pages.empty()) {
				continue;
			}
			else {
				u2pSeqMap.insert(std::make_pair(fid, pages));
			}
		}
	} catch (...) {
		LOG(LERROR) << "FansPageCacheAdapter::instance().getPagesByUidBatch() exception... uid" << uid;
	}

  if (u2pSeqMap.empty())
		return; //好友都没有关注page我就不用计算了...

	Int2IntSeqMap p2uSeqMap;
  ReverseSort(u2pSeqMap, p2uSeqMap);

	//组织原始未过滤的数据 
	
	xce::buddy::PageCommonFans pcf;
	pcf.uid = uid;
	Int2IntSeqMap::const_iterator p2uSeqMapIter = p2uSeqMap.begin();
  for (; p2uSeqMapIter != p2uSeqMap.end(); ++p2uSeqMapIter) {
		int pid = p2uSeqMapIter->first;
		xce::buddy::CommonFans commonFans;
		MyUtil::IntSeq& common = commonFans.common;
	//	MyUtil::IntSeq& uncommon = commonFans.uncommon;
		common.insert(common.begin(), p2uSeqMapIter->second.begin(), p2uSeqMapIter->second.end());
    /*
		for (size_t i = 0; i < friends.size(); ++i) {
      int f = friends[i];
      bool in = false;
			for (size_t j = 0; j < common.size(); ++j) {
				if (common[j] == f) {
					in = true;
          break;
				}
			}
      if (!in)
				uncommon.push_back(f);
		}
		*/
		pcf.dataMap.insert(std::make_pair(pid, commonFans));
	}

	try {
		xce::buddy::adapter::PageCommonFansCacheAdapter::instance().SetData(pcf);
    xce::buddy::CommonFansMap::const_iterator iter = pcf.dataMap.begin();
    std::ostringstream oss;
    oss << "PageCommonFansCacheAdapter::SetData() uid:" << pcf.uid << " pid:";
    for (; iter != pcf.dataMap.end(); ++iter) {
			oss << iter->first << "|";
    }
    LOG(INFO) << oss.str();
	} catch (Ice::Exception e) {
  	LOG(LERROR) << e.what();
	} catch (...) {
		LOG(LERROR) << "PageCommonFansCacheAdapter::SetData() exception...";
	} 

}

}
} // end xce::buddy



