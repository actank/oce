#include "socialgraph/recommendvideo/util/videodbhelper.h"
#include "socialgraph/socialgraphutil/clogging.h"
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace com::xiaonei::xce;
using namespace xce::socialgraph;

void VideoDbHelper::getVideoInforFromDb(VideoVec& video_items, int video_number)
{
  CLOG(INFO) << "get original video source information";
  VideoVec tmp_video_items;
  tmp_video_items.reserve(video_number);
  video_items.reserve(video_number);
  Statement sql_video_infor;
  std::string limit_number( boost::lexical_cast<std::string>(video_number) );
  const char* kQuery_Video_Infor = "SELECT share_id, user_id, title, url, summary, url_md5, user_name, thumb_url \
                                    FROM hot_share WHERE type = 10 AND stage = 0 limit ";
  sql_video_infor << std::string(kQuery_Video_Infor + limit_number);
  QueryVideoDbHandler video_db_handler(tmp_video_items);
  QueryRunner("share_info_new", CDbRServer).query(sql_video_infor, video_db_handler);

  CLOG(INFO) << "filter useless share video urls and  query share video count from url_0~url_99 tables in share_new_0 ~ share_new_9 database instance";
  int share_table_suffix;
  std::string share_table;
  std::string share_pattern;
  int url_table_suffix;
  std::string url_table;
  std::string url_pattern;
  for(VideoVec::iterator it = tmp_video_items.begin(); it != tmp_video_items.end(); ++it) {
    //filter useless share video urls; if the url is invalid, set the share_id to 0
    //std::cout << "share_id" << (*it).share_id << std::endl;
    share_table_suffix = (*it).share_user_id % 100;
    share_table = "share_" + boost::lexical_cast<std::string>(share_table_suffix);
    share_pattern = "share_new_" + boost::lexical_cast<std::string>(share_table_suffix);
    Statement sql_video_share;
    sql_video_share << "SELECT id FROM " << share_table << " WHERE id = " << (*it).share_id;
    mysqlpp::StoreQueryResult result_valid = QueryRunner("share_new", CDbRServer, share_pattern).store(sql_video_share);
    if(result_valid.num_rows() > 0) {     //if the share video id is valid
      //get original video id from url_md5
      Statement sql_video_count;
      (*it).video_id = strtoul( (*it).url_md5.substr(0,10).c_str(), NULL, 16 );
      //std::cout << "video_id" << (*it).video_id << std::endl;
      //get url share count from share url tables 
      url_table_suffix = strtoul( (*it).url_md5.substr(30,2).c_str(), NULL, 16 ) % 100; 
      url_table = "url_" + boost::lexical_cast<std::string>(url_table_suffix);
      url_pattern = "share_new_" + boost::lexical_cast<std::string>(url_table_suffix);
      sql_video_count << "SELECT share_count FROM " << url_table << " WHERE md5 = " << mysqlpp::quote << (*it).url_md5;
      mysqlpp::StoreQueryResult result_video_count = QueryRunner("share_new", CDbRServer, url_pattern).store(sql_video_count);

      if(result_video_count.num_rows() > 0) {
        mysqlpp::Row row = result_video_count.at(0);
        (*it).share_count = (int)row["share_count"];
      }
   
      //store prepared valid recommended video records 
      video_items.push_back(*it);
    }  else {
       //CLOG(DEBUG) << "useless share id: " << (*it).share_id;
       (*it).share_id = 0;  //indicate the share video is not valid
    }
  }
}

//set new recommended video to database
void VideoDbHelper::setRcdVideoInforToDb(UserVideo user_video)
{
  //delete user's old recommended video information
  Statement sql_delete_user_video;
  sql_delete_user_video << "DELETE FROM recommend_by_video WHERE user_id = " << user_video.getUserId();
  //std::cout << user_video.getUserId() << std::endl; 
  QueryRunner("socialgraph_recommend_video", CDbWServer).execute(sql_delete_user_video);

  //bulid user's new video information and video feature data
  Statement sql_insert_user_video;
  const char* kInsert_Rcd_Video_Item = "INSERT INTO recommend_by_video (user_id, video_id, share_id, share_user_id, stage, type, \
                                        title, url, summary, url_md5, share_user_name, thumb_url, share_count) VALUES ";
  sql_insert_user_video << std::string(kInsert_Rcd_Video_Item);
  VideoVec tmp_video_items = user_video.getVideoVec();
  for(VideoVec::iterator it = tmp_video_items.begin(); it != tmp_video_items.end(); ++it) {
    sql_insert_user_video << "(" << user_video.getUserId() << " , ";
    sql_insert_user_video << it->video_id << ", "
                          << it->share_id << ", "
                          << it->share_user_id << ", "
                          << user_video.getStage() << ", "
                          << user_video.getType() << ", " << mysqlpp::quote 
                          << it->title << ", " << mysqlpp::quote
                          << it->url << ", " << mysqlpp::quote   
                          << it->summary << ", " << mysqlpp::quote
                          << it->url_md5 << ", " << mysqlpp::quote
                          << it->share_user_name << ", " << mysqlpp::quote 
                          << it->thumb_url << ", "
                          << it->share_count;
    sql_insert_user_video << ")";
    if( (it + 1) != tmp_video_items.end() ) {
      sql_insert_user_video << ", ";
    }   
  }
  QueryRunner("socialgraph_recommend_video", CDbWServer).execute(sql_insert_user_video);  
}

//get user video data from new recommend video database 
void VideoDbHelper::getRcdVideoInforFromDb(UserVideo& user_video)
{
  //get user video feature data
  Statement sql_user_video;
  sql_user_video << "SELECT * FROM recommend_by_video WHERE user_id = " << user_video.getUserId() << " limit 1";
  mysqlpp::StoreQueryResult result_user_video = QueryRunner("socialgraph_recommend_video").store(sql_user_video);
  if(result_user_video.num_rows() > 0) {
    mysqlpp::Row row = result_user_video.at(0);
    user_video.setUserId( (int)row["user_id"] );
    user_video.setStage( (int)row["stage"] );
    user_video.setType( (int)row["type"] );
  }
  //get user video data  
  Statement sql_rcd_video_infor;
  sql_rcd_video_infor << "SELECT * FROM recommend_by_video WHERE user_id = " << user_video.getUserId();
  VideoVec tmp_video_items;
  QueryRcdVideoDbHandler rcd_video_db_handler( tmp_video_items );
  QueryRunner("socialgraph_recommend_video", CDbRServer).query(sql_rcd_video_infor, rcd_video_db_handler);
  user_video.setVideoVec(tmp_video_items);
}

bool QueryVideoDbHandler::handle(mysqlpp::Row& row) const 
{
  Video video_item;
  video_item.share_id = (long)row["share_id"];
  video_item.share_user_id = (int)row["user_id"];
  video_item.title = std::string(row["title"]);
  video_item.url = std::string(row["url"]);
  video_item.summary = std::string(row["summary"]);
  video_item.url_md5 = std::string(row["url_md5"]);
  video_item.share_user_name = std::string(row["user_name"]);
  video_item.thumb_url = std::string(row["thumb_url"]);
  video_item.share_count = 0;
  
  video_items_.push_back(video_item);
  
  return true;
}

bool QueryRcdVideoDbHandler::handle(mysqlpp::Row& row) const
{
  //std::cout << "I am here now " << std::endl;
  Video video_item;
  video_item.video_id = (long)row["video_id"];
  video_item.share_id = (long)row["share_id"];
  video_item.share_user_id = (int)row["share_user_id"];
  video_item.title = std::string(row["title"]);
  video_item.url = std::string(row["url"]);
  video_item.summary = std::string(row["summary"]);
  video_item.url_md5 = std::string(row["url_md5"]);
  video_item.share_user_name = std::string(row["share_user_name"]);
  video_item.thumb_url = std::string(row["thumb_url"]);
  video_item.share_count = (int)row["share_count"];

  video_items_.push_back(video_item);
  return true;
}
