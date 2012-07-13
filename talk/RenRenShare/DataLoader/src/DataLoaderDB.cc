#include "DataLoaderDB.h"
#include "QueryRunner.h"
#include "LogWrapper.h"

using namespace std;
using namespace MyUtil;
using namespace com::xiaonei::xce;
using namespace xce::dataloader;

bool DataLoaderDB::GetHotShare(const size_t data_type, Ice::Long &max_db_id, size_t size,  std::vector<HotShareInfo> &hotshare_vector) {
  int type = 0;
  if (data_type == kVideoType)
    type = 10;
  else if (data_type == kAlbumType)
    type = 8;
  else if (data_type == kBlogType)
    type = 11;

  HotShareInfo hs;
  Statement sql;  
  Ice::Long temp_max_db_id = max_db_id;
  sql << "SELECT id, share_id, user_id,  title, url, summary, creation_date, user_name, thumb_url FROM "
    << "hot_share" <<" WHERE stage=0 AND type=" << type <<" AND id>" << max_db_id <<" limit " << size;
  try {
    mysqlpp::StoreQueryResult res = QueryRunner("share_info_new", CDbRServer, "hot_share").store(sql);
    for (size_t i=0; i<res.num_rows(); i++) {
      mysqlpp::Row row = res.at(i);
      hs.id = static_cast<Ice::Long>(row["id"]);
      hs.share_id = static_cast<Ice::Long>(row["share_id"]);
      hs.user_id = static_cast<unsigned int>(row["user_id"]);
      row["title"].to_string(hs.title);
      row["url"].to_string(hs.url);
      row["summary"].to_string(hs.summary);
      row["creation_date"].to_string(hs.creation_date);
      row["user_name"].to_string(hs.user_name);
      row["thumb_url"].to_string(hs.thumb_url);
      if (data_type == kVideoType)
        hs.catagory = "000100010001";
      else if (data_type == kAlbumType)
        hs.catagory = "000100010002";
      else if (data_type == kBlogType)
        hs.catagory = "000100010003";
      else
        hs.catagory = "";

      hotshare_vector.push_back(hs);
      if (hs.id > temp_max_db_id)
        temp_max_db_id = hs.id;
     }
  } catch (std::exception &e) {
    MCE_WARN("DataLoaderDB::GetHotShare: "<< e.what());
    return false;
  } catch (...) {
    MCE_WARN("DataLoaderDB::GetHotShare: exception");
    return false;
  }  
  max_db_id = temp_max_db_id;
  return true;
}


bool DataLoaderDB::GetOnlineVideo(Ice::Long &max_db_id, size_t size, vector<OnlineVideoInfo> &online_video_vector) {
  OnlineVideoInfo ov_info;
  Statement sql;  
  Ice::Long temp_max_db_id = max_db_id;
  sql << "SELECT id, title, thumb_url, page_url, swf_url, catagory, creation_date, release_date FROM "
    << "hotshare_video" <<" WHERE id>" << max_db_id <<" limit " << size;
  try {
    mysqlpp::StoreQueryResult res = QueryRunner("im", CDbRServer, "hotshare_video").store(sql);
    for (size_t i=0; i<res.num_rows(); i++) {
      mysqlpp::Row row = res.at(i);
      Ice::Long temp_id = static_cast<Ice::Long>(row["id"]);
      row["title"].to_string(ov_info.title);
      row["thumb_url"].to_string(ov_info.thumb_url);
      row["page_url"].to_string(ov_info.page_url);
      row["swf_url"].to_string(ov_info.swf_url);
      row["catagory"].to_string(ov_info.catagory);
      row["creation_date"].to_string(ov_info.creation_date);
      row["release_date"].to_string(ov_info.release_date);

      online_video_vector.push_back(ov_info);
      if (temp_max_db_id < temp_id)
        temp_max_db_id = temp_id;
     }
  } catch (std::exception &e) {
    MCE_WARN("DataLoaderDB::GetOnlineVideo: "<< e.what());
    return false;
  } catch (...) {
    MCE_WARN("DataLoaderDB::GetOnlineVideo: exception");
    return false;
  }  
  max_db_id = temp_max_db_id;
  return true;
}
    
bool DataLoaderDB::InsertHotShare(vector<HotShareInfo> &hotshare_vector, size_t limit_size) {
  if (hotshare_vector.size() == 0)
    return true;
  bool ret = true;

  size_t insert_size = 0;
  if (limit_size == 0) {
    insert_size = hotshare_vector.size();
  } else {
    const size_t hotshare_vector_size = hotshare_vector.size();
    insert_size = hotshare_vector_size>limit_size ? limit_size : hotshare_vector_size;
  }

  size_t t = 0;
  while (t < insert_size) {
    size_t temp_insert_size = kHotShareInsertSize>(insert_size-t) ? insert_size-t : kHotShareInsertSize;
    ret = InsertHotSharePart(hotshare_vector, t, temp_insert_size );
    if (!ret)
      break;
    t += temp_insert_size;
  }
  return ret;
}

bool DataLoaderDB::DeleteHotShare(Ice::Long db_id) {
  Statement sql;
  sql << "DELETE FROM "<< "hotshare" <<" WHERE id>"<<db_id;     
  try {
    QueryRunner("im", CDbWServer, "hotshare").store(sql);
  } catch (std::exception &e) {
    MCE_WARN("DataLoaderDB::DeleteHotShare: " << e.what());
    return false;
  } catch (...) {
    MCE_WARN("DataLoaderDB::DeleteHotShare: exception" );
    return false;  
  }
  return true;
}

bool DataLoaderDB::InsertHotSharePart(vector<HotShareInfo> &hotshare_vector, size_t begin, size_t insert_size) {
  if (hotshare_vector.size()==0)
    return true;
  if (begin>hotshare_vector.size())
    return false;
  MCE_DEBUG("DataLoaderDB::InsertHotSharePart   hotshare_vector.size:"<<hotshare_vector.size()<<"    begin:"<<begin<<"    insert_size:"<<insert_size);
  Statement sql;
  sql << "INSERT INTO "<< "hotshare" <<"(share_id, user_id, title, url, summary, creation_date, release_date, user_name, thumb_url, catagory) VALUES ";     
  size_t rest_size = hotshare_vector.size() - begin;
  size_t size = insert_size<rest_size ? insert_size : rest_size;
  for (size_t i=0; i<size; ++i) {
    sql <<"("<< hotshare_vector[i+begin].share_id <<", "<< hotshare_vector[i+begin].user_id <<",'" << hotshare_vector[i+begin].title <<"','" 
      << hotshare_vector[i+begin].url <<"','" <<hotshare_vector[i+begin].summary <<"','" <<hotshare_vector[i+begin].creation_date << "','" 
      <<hotshare_vector[i+begin].release_date <<"','" <<hotshare_vector[i+begin].user_name 
      <<"','" <<hotshare_vector[i+begin].thumb_url <<"','" << hotshare_vector[i+begin].catagory <<"')";
    if (i != size-1)
      sql << ",";
  }
  try {
    QueryRunner("im", CDbWServer, "hotshare").store(sql);
  } catch (std::exception &e) {
    MCE_WARN("DataLoaderDB::InsertHotShare: " << e.what());
    return false;
  } catch (...) {
    MCE_WARN("DataLoaderDB::InsertHotShare: exception" );
    return false;  
  }
  return true;
}
