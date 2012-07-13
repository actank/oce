/*
  Date:   2011-10-14
  Author: junwu.xiong
  Email:  junwu.xiong@renren-inc.com  
  Function: build page recommend page infomation database socialgraph_recommend_page by fectching
  page information from page database, page fans information from page_fans databases, users' friends
  infomation from relation_friendrank databases 
  2011-10-20: add multithread pathern in each sub-process
  2011-11-11: first fetch all page and page's fans infomation from db, store it previously and then go on other proccesses
  2011-11-15: fetch each user's already focused pages, store it to STL set and then filter the recommend pages from them
  2011-11-16: test time cost of each module
*/

/*******************************************************************************
1 fetch page infomation and page's fans ids sequence that will be recommended, 
structure it with map and store it to memory;
2 fetch user's already focused pages and filter them from above pages;
3 fetch user's friend ids sequences, make sure who already focused the pages 
will be recommended and display their's name on front;  
4 build being recommended pages' information and store them to db.
********************************************************************************/

#include "socialgraph/recommendpage/exe/rcdpageworker.h"
#include "socialgraph/socialgraphutil/timecost.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <sys/time.h>
#include <boost/lexical_cast.hpp>
#include "util/cpp/IntSeqSerialize.h"
#include "socialgraph/socialgraphutil/clogging.h" 

using namespace std;
using namespace com::xiaonei::xce;
using namespace xce::socialgraph;
using namespace MyUtil;
//using namespace com::xiaonei::xce::socialgraph;

int main(int argc, char **argv)
{

  RunTime timecost_main=RunTime::create("main timecost");
  ConnectionPoolManager::instance().initialize();

  //MyUtil::TaskManager::instance().config( kLevel_Page_Infor, ThreadPoolConfig(4,8) );
  //MyUtil::TaskManager::instance().config( kLevel_Page_Fans, ThreadPoolConfig(4,8) );
  //MyUtil::TaskManager::instance().config( kLevel_User_Friend, ThreadPoolConfig(4,8) );
  //MyUtil::TaskManager::instance().config( kLevel_Page_Common_Fans, ThreadPoolConfig(4,8) );
  //MyUtil::TaskManager::instance().config( kLevel_Rcd_Page_Infor, ThreadPoolConfig(4,8) );

  InitLogger("recommendpage");
  if(argc != 3) {
    CLOG(ERROR) << "Usage: " << argv[0] << " RCD_PAGE_IDS RCD_PAGE_DATA";
    exit(1);
  }

  CLOG(INFO) << "firstly: get all page and page'fans information";
  std::ifstream page_ids_file;
  page_ids_file.open( argv[1] );
  if(!page_ids_file) {
    CLOG(DEBUG) << "Can not open recommended page ids file";
  }

  std::string page_id_line;
  std::getline(page_ids_file, page_id_line);
  PageMap page_map;
  while(!page_id_line.empty()) {
    Page page_item;

    // get page_id from the file 
    int page_id = boost::lexical_cast<int>(page_id_line); 
    page_item.page_id = page_id;

    //get common page's page_name, fans_counter and description from the database
    PageDbHelper::getPageInforFromDb(page_item);
    //MyUtil::TaskManager::instance().execute( new getPageInforTask(page_item) );
    
    //get page's fans from db
    PageDbHelper::getPageFansFromDb(page_item);
    //sort page's fans 
    std::sort(page_item.fans_ids.begin(), page_item.fans_ids.end());    
    //MyUtil::TaskManager::instance().execute( new getPageCommonFansTask(user_id, friend_ids, page_item) );
    //for(IntVec::iterator pos = page_item.fans_ids.begin(); pos != (page_item.fans_ids.begin()+3); ++pos) {
    //  std::cout << *pos << "\t";
    //}
    //std::cout << std::endl<<std::endl;
    //if( page_item.fans_counter == page_item.fans_ids.size() ) {
    //  std::cout <<page_item.fans_counter << ": " <<page_item.fans_ids.size() << std::endl;
    //} else {
    //  std::cout << "get wrong page's fans number" << std::endl;
    //}

    page_map.insert(make_pair(page_id, page_item));
    page_id_line.clear();
    //timecost_main.step("get one page and page's fans");
    std::getline(page_ids_file, page_id_line);
  }
  
  timecost_main.step("get page and page fans information over");

  CLOG(INFO) << "secondly: get users' id, page and value data from file and process one user each time";
  std::ifstream filein;
  filein.open( argv[2] );
  if(!filein) {
    CLOG(DEBUG) << "Can not open user page file";
  }

  //MyUtil::TaskManager::instance().config( kLevel_Rcd_UserPage, ThreadPoolConfig(1,1) );
  MyUtil::TaskManager::instance().config( kLevel_Rcd_UserPage, ThreadPoolConfig(8,10) );
  CLOG(INFO) <<"Starting process user's recommend pages";
  std::string line;
  std::getline(filein, line);

  int total_id_idx = 0;  //index total number of users' page infomation has been processed 
  int task_size=0;
  while(!line.empty()) {

    MyUtil::TaskManager::instance().execute(new setRcdUserPageTask(line, page_map));  
    while((task_size=MyUtil::TaskManager::instance().size(kLevel_Rcd_UserPage)) > 1000){
      sleep(2);
    }
    line.clear();
    std::getline(filein, line);

    //timecost_main.step("1 user processed");
    ++total_id_idx;
    if( total_id_idx % 10000 == 0) {
      timecost_main.step("10000 users processed");
    }
  } 

  while(MyUtil::TaskManager::instance().size(kLevel_Rcd_UserPage) > 0) {
    sleep(60);
  } 

  CLOG(INFO) << total_id_idx << "users have been processed!";

  filein.close();
  return 0;
}
