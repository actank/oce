/*
  Date:   2011-10-14
  Author: junwu.xiong
  Email:  junwu.xiong@renren-inc.com  
  Function: read recommend page information from socialgraph_recommend_page database  
*/


#include "socialgraph/recommendpage/util/pagedbhelper.h"
#include <iostream>
#include <string>
#include <vector>

#include "DbCxxPool/src/QueryRunner.h"
#include "util/cpp/IntSeqSerialize.h"    

using namespace std;
//using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::socialgraph;

/* read data from recommend_page database and chech results */
int main(int argc, char *argv[])
{

  ConnectionPoolManager::instance().initialize();  

  std::ifstream filein;
  /* read file name from command lien */
  std::cout << "please input the filename to be processed " << std::endl;
  if(argc >= 2 ) {
    std::string filename( argv[1]);
    std::string path("/data/xce/SGProject/bin/pagerecommend/");
    //std::cin << filename;
    filename =  path + filename;
    filein.open( filename.c_str() );
  } else {
    std::cerr << "There has no filename input " << std::endl;
  }


  //filein.open("/data/xce/SGProject/bin/pagerecommend/user_pv.dat");
  if(!filein) {
    std::cout << "can not open file" << std::endl;
  }
  std::string line;
  std::getline(filein, line);

  int total_id_idx = 0;  /* total number of users information have been read */
  while( !line.empty() ) {

    UserPages userpages_item;
    int user_id = 0;
    /* get the user id */
    std::string::size_type idx = line.find('\t');
    if( idx != std::string::npos) {
      user_id =  atoi( ( line.substr(0, idx) ).c_str() );
      userpages_item.setUserId( user_id ); 
      std::cout << "user_id : " << user_id << std::endl;
      
    } else {
    }   
 
    /* query the user's recommended pages infomation */ 
    Statement sql_result;
    std::string table_name("recommend_by_page");
    sql_result << "SELECT * FROM " << table_name << " WHERE user_id = " << user_id;   
    //std::ostringstream oss;
    //oss << "SELECT * FROM " << table_name << " WHERE user_id = " << user_id;
    //std::cout << oss.str() << std::endl;
    PageItemVector pageitem_vector; 
    PageItemHandler pageitem_handler( pageitem_vector ); 
    QueryRunner("socialgraph_recommend_page", CDbRServer).query(sql_result,  pageitem_handler);

    /* check the query result */
    for(PageItemVector::iterator it = pageitem_vector.begin(); it != pageitem_vector.end(); ++it ) {
      std::cout << "page_id = " << (*it).page_id << ", "
                << "page_value = " << (*it).page_value << ", "
                << "page_name = " << (*it).page_name << ", "
                << "description = " << (*it).description << ", "
                << "fans_counter = " << (*it).fans_counter << ", "
                << "common_friend_ids :  " << std::endl; 
      IntVector friend_ids = (*it).common_friend_ids;   
       
      std::cout << "common_friend_number = " << (*it).common_friend_number << std::endl;
      for(IntVector::iterator idx = friend_ids.begin(); ((*idx) > 0);/*idx != friend_ids.end();*/ ++idx) {
            std::cout << *idx << ", ";
       } 
    }

   std::cout << std::endl;
   line.clear();
   std::getline(filein, line);
 
   ++total_id_idx;
   std::cout << total_id_idx << " users' infomation have been read" << std::endl;
  } /* end while */

  filein.close();
  return 0;
}
