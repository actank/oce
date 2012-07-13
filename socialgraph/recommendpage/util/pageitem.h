/*
  Date: 2011-10-27
  Author:   Junwu.xiong 
  Email:    junwu.xiong@renren-inc.com
*/

#ifndef __PAGE_ITEM_H_
#define __PAGE_ITEM_H_

#include <vector>
#include <string>
#include <set>
#include <map>

namespace xce {
namespace socialgraph {

typedef std::vector<int> IntVec;
typedef std::set<int> IntSet;

//recommmoned page infomation database structure 
struct Page {
  Page() 
  {
    page_id = 0;
    page_value = 0.0;
    fans_counter = 0;
    common_friend_number = 0;
  }

  int page_id;
  double page_value;
  std::string page_name;
  std::string description;
  int fans_counter;
  IntVec fans_ids;
  int common_friend_number;
  IntVec common_friend_ids;
};

typedef std::map<int, Page> PageMap;
typedef std::vector<Page> PageVec;

////recommended page information cache structure 
//struct PageCache {
//  PageCache() {
//    page_id = 0;
//    fans_counter = 0;
//  }
//  int page_id;
//  int fans_counter;  
//};
//
//typedef std::vector<PageCache> PageCacheVec;

};
};
#endif
