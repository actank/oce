#include "socialgraph/recommendpage/exe/rcdpageworker.h"
#include "socialgraph/socialgraphutil/timecost.h"

//using namespace com::xiaonei::xce;
using namespace xce::socialgraph;


//void make_intersection(IntVec::iterator first1, IntVec::iterator end1, IntVec::iterator first2, IntVec::iterator end2, IntVec& result)
void make_intersection(const IntVec &v1, const IntVec &v2, IntVec& result, unsigned int com_num)
{ 
  IntVec::const_iterator it1=v1.begin();
  IntVec::const_iterator it2=v2.begin();
  //while( (it1 != v1.end()) &&  (result.size() < com_num)) {
  //  bool is_com = binary_search(v2.begin(), v2.end(), *it1);
  //  if(is_com) {
  //    result.push_back(*it1);
  //  }
  //  ++it1;
  //}
  
  while(it1 != v1.end() && it2 != v2.end())
    if(*it1 < *it2)
      ++it1;
    else if(*it2 < *it1)
      ++it2;
    else {
      if(result.size() < com_num) {
        result.push_back(*it1);
        ++it1;
        ++it2;
      } else {
        return;
      }
    }

  //while(first1 != end1 && first2 != end2)
  //  if(*first1 < *first2)
  //    ++first1;
  //  else if(*first2 < *first1)
  //    ++first2;
  //  else {
  //    result.push_back(*first1);
  //    ++first1; 
  //    ++first2;
  //  }
}

void getPageInforTask::handle()
{
  PageDbHelper::getPageInforFromDb(page_item_);
}

void getPageFansTask::handle()
{
  PageDbHelper::getPageFansFromDb(page_item_);
}

void getUserFriendTask::handle()
{
  PageDbHelper::getUserFriendFromDb(user_id_, friend_ids_);
}

void setRcdUserPageTask::handle()
{
  RunTime timecost_rcd=RunTime::create("timecost_rcd");
  UserPages userpages_item;
  // get userId 
  int user_id = 0;
  std::string::size_type idx = line_.find('\t');
  if(idx != std::string::npos) {
    user_id = atoi( (line_.substr(0, idx)).c_str() );
    userpages_item.setUserId( user_id );
  } else {
    cout << "can not find user id" << endl;
  }

  //CLOG(INFO) << "query user_id = " << user_id << "'s already focused pages ";
  IntSet focused_page_ids;
  PageDbHelper::getUserPageFromDb(user_id, focused_page_ids);
  //for(IntSet::iterator it=focused_page_ids.begin(); it != focused_page_ids.end(); ++it) {
  //  std::cout << *it << " "; 
  //}   
  //std::cout << std::endl;
  //timecost_rcd.step("get user focused pages");

  //CLOG(INFO) << "query the user's friend relation table and get all friend ids";
  IntVec friend_ids;
  //const static unsigned int kRelationNum = 105;   //usual number of friends of the user
  friend_ids.reserve(UserPages::kRelationNum);
  PageDbHelper::getUserFriendFromDb(user_id,friend_ids);
  //MyUtil::TaskManager::instance().execute( new getUserFriendTask(user_id, friend_ids) );
  //timecost_rcd.step("get user friends");

  //CLOG(INFO) << "get the user's all pages' page_id and page_value";
  PageVec page_items;
  //const static unsigned int kRcdPageNum = 200;   //upbound of recommended pages number to the user 
  page_items.reserve(UserPages::kRcdPageNum);
  std::string::size_type idx_start = idx + 1;
  std::string::size_type idx_end = idx_start;
  while( line_.find(";",idx_start) != std::string::npos ) {
    // get page_id from the file 
    idx_end = line_.find(",", idx_start);
    int page_id = atoi ( line_.substr( idx_start, (idx_end - idx_start) ).c_str() );
    // get page_value from the file 
    idx_start = idx_end + 1;
    idx_end = line_.find(";", idx_start);
    double page_value = atof ( line_.substr( idx_start, (idx_end - idx_start) ).c_str() );
    
    IntSet::iterator it = focused_page_ids.find(page_id);
    //if the used has not focused the page yet 
    if(it == focused_page_ids.end()) {
      //std::cout << "page_id: " << page_id << "not focused" << std::endl;
      std::string init_str("");
      IntVec init_fans;
      IntVec init_ids;
      //Page page_item={0, 0.0, init_str, init_str, 0, init_fans, 0, init_ids};
      Page page_item;
      page_item.page_id = page_id;
      page_item.page_value = page_value;

      //get common page's page_name, fans_counter and description from the PageMap
      PageMap::const_iterator page_map_pos=page_map_.find(page_id);
      if(page_map_pos != page_map_.end()) {
        //Page page_item=boost::lexical_cast<Page>(page_map_.find(page_id));
        page_item.page_name = page_map_pos->second.page_name;
        page_item.description = page_map_pos->second.description;
        page_item.fans_counter = page_map_pos->second.fans_counter;
        page_item.fans_ids = page_map_pos->second.fans_ids;
        //page_item.fans_ids.swap(page_map_pos->second.fans_ids); 
      } else {
        std::cout << "page id is not existed" << std::endl;
      }

      //std::cout << "get common friend ids" << std::endl;
      //IntVec::iterator id_pos = std::find(page_item.fans_ids.begin(), page_item.fans_ids.end(), user_id);
      //const static unsigned int kComDisplayNum = 10;  //upbound of the being displayed common friends number to the user of the page recommended
      page_item.common_friend_ids.reserve(UserPages::kComDisplayNum);
      //make_intersection(friend_ids.begin(), friend_ids.end(),page_item.fans_ids.begin(), page_item.fans_ids.end(), page_item.common_friend_ids);
      make_intersection(friend_ids, page_item.fans_ids, page_item.common_friend_ids, UserPages::kComDisplayNum);
      page_item.common_friend_number=page_item.common_friend_ids.size();
      /*
      std::cout << "common friend size " << page_item.common_friend_ids.size() << "common_friend_ids = ";
      std::string common_test;
      for(IntVec::iterator idx = page_item.common_friend_ids.begin(); idx != page_item.common_friend_ids.end() ; ++idx) {
        std::string friend_idx = boost::lexical_cast<std::string> (*idx);
        //std::cout << "common friend id: " << friend_idx << std::endl;
        common_test = common_test + friend_idx;
        if((idx + 1) != page_item.common_friend_ids.end() )
          common_test = common_test + " ";
      }
      std::cout << common_test << std::endl;
      */

      //push each pageitem to the rcd page vector
      page_items.push_back(page_item);
      /* 
      std::cout << "user_id = " << user_id << std::endl
                << "page_id = " << page_item.page_id << std::endl  
                << "page_value = " << page_item.page_value << std::endl
                << "page_name = " << page_item.page_name << std::endl
                << "description = " << page_item.description << std::endl
                << "fans_counter = " << page_item.fans_counter << std::endl
                << "common_friend_number = " << page_item.common_friend_ids.size() << std::endl
                << "common_friend_ids = ";
     
      std::string common_test;
      for(IntVec::iterator idx = page_item.common_friend_ids.begin(); idx != page_item.common_friend_ids.end() ; ++idx) {
        std::string friend_idx = boost::lexical_cast<std::string> (*idx);
        common_test = common_test + friend_idx;
        if((idx + 1) != page_item.common_friend_ids.end() )
          common_test = common_test + " ";
      }
      std::cout << common_test << std::endl;  
      */
    } else {
      //std::cout << "user_id: " << user_id << " page_id: " << page_id << " already focused" << std::endl;
    }
    // get the next page id idx_start position from the file
    idx_start = idx_end + 1;
    //timecost_rcd.step("get one rcd page");
  } // idx_end while for process all the user's pages 
  
  timecost_rcd.step("get rcd pages for one user over");
  
  //CLOG(INFO) << "************** build page rcd cache data: store user's recommended page infomation to cache *********************";
  if(!page_items.empty()) {
    PageCacheHelper::setRcdPageInforToCache(user_id, page_items);
  }

  ////CLOG(INFO) << "************** build page rcd database: store user's recommended page infomation to DB *********************";
  ////MyUtil::TaskManager::instance().execute( new setRcdPageInforTask( userpages_item.getUserId(), page_item ) );
  if(!page_items.empty()) {
    PageDbHelper::setRcdPageInforToDb(user_id, page_items);
  }

  timecost_rcd.step("build rcd pages for one user over");
 /* 
  CLOG(INFO) << "************** query the user's recommended pages infomation ***********";
  PageVec page_item_vector=page_items;
  PageDbHelper::getRcdPageInforFromDb(user_id, page_item_vector);
  PageVec::iterator it = page_item_vector.end() - 1; 
  std::cout << "page_id = " << (*it).page_id << std::endl
            << "page_value = " << (*it).page_value << std::endl 
            << "page_name = " << (*it).page_name << std::endl
            << "description = " << (*it).description << std::endl
            << "fans_counter = " << (*it).fans_counter << std::endl
            << "common_friend_number = " << (*it).common_friend_number << std::endl
            << "common_friend_ids = ";
  //the default common ids are set to zero when there no common friends, so the
  //following for judgement condition is not idx != friend_ids.end() 
  for(IntVec::iterator idx =  (*it).common_friend_ids.begin(); ((*idx) > 0); ++idx) { 
        std::cout << *idx << " ";
  }
  std::cout << std::endl << std::endl;
  */
}
