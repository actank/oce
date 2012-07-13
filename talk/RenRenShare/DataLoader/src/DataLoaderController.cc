#include "DataLoaderController.h"
#include <algorithm>
#include <sstream>
#include "LogWrapper.h"
#include "DataLoaderDB.h"

using namespace std;
using namespace MyUtil;
using namespace xce::dataloader;

const char  *kDBIdFilePath = "/data/xce/XNTalk/etc/dbid.conf";
const size_t kOnlineVideoUnitSize = 1;
const size_t kHotShareVideoUnitSize = 5;
const size_t kHotShareAlbumUnitSize = 4;
const size_t kMixUpUnitSize = 9;
const size_t kVideoFetchSize = 500;
const size_t kAlbumFetchSize = 400;
const size_t kOnlineVideoFetchSize = 100;
const size_t kInstantGetDataCycle = 24*6;

void DataLoaderTimer::handle() {
  MCE_INFO("***********start timer*********");

  DataLoaderController::instance().LoadData();
  DataLoaderController::instance().IncreaseCount();
}

DataLoaderController::DataLoaderController() {
  online_video_db_id_ = 0;
  hotshare_db_insert_count_ = 0;
  hotshare_video_db_id_ = hotshare_album_db_id_ = 0; 
  db_fetch_data_count_ = 0;
  db_buffer_finished_ = false;
  hotshare_vector_insert_head_ = 0;
  curl_code_ = curl_global_init(CURL_GLOBAL_ALL);
}

void DataLoaderController::IncreaseCount() {
  if (db_buffer_finished_)
    ++db_fetch_data_count_;

  MCE_DEBUG("DataLoaderController::IncreaseCount:"<<db_fetch_data_count_);
}

DataLoaderController::~DataLoaderController() {
  curl_global_cleanup();
}

bool DataLoaderController::LoadData() {
  if (online_video_db_id_==0 && hotshare_video_db_id_==0 && hotshare_album_db_id_==0 && hotshare_db_insert_count_==0)
    GetDBIdFromFile();

  MCE_DEBUG(" DataLoaderController::LoadData ----> online_video_db_id:"<<online_video_db_id_<<"  hotshare_video_db_id:"<<hotshare_video_db_id_
      <<"   hotshare_album_db_id:"<<hotshare_album_db_id_);
  vector<OnlineVideoInfo> online_video_vector;
  vector<HotShareInfo> hotshare_video_vector, hotshare_album_vector, hotshare_online_video_vector;

//  LoadOnlineVideos(online_video_vector);
//  ShareOnlineVideos(online_video_vector, hotshare_online_video_vector);
  if (db_fetch_data_count_==kInstantGetDataCycle || !db_buffer_finished_) {
    MCE_DEBUG(" DataLoaderController::LoadData ----> ready to read data____ db_fetch_data_count_:" <<db_fetch_data_count_
        <<"   db_buffer_finished_:"<<db_buffer_finished_ );
    db_fetch_data_count_ = 0;

    LoadHotShareAlbum(hotshare_album_vector);
    LoadHotShareVideo(hotshare_video_vector);
    GetSwfUrl(hotshare_video_vector);

    if (!db_buffer_finished_)
      BufferMixUpData(hotshare_video_vector, hotshare_album_vector, hotshare_online_video_vector);
    else
      InstantMixUpData(hotshare_video_vector, hotshare_album_vector, hotshare_online_video_vector);

    if (hotshare_album_vector.size()==0 && hotshare_video_vector.size()==0)
      db_buffer_finished_ = true;
  }

  if (InsertHotShare()) {
    hotshare_db_insert_count_ += hotshare_vector_.size();
    if (!WriteDBIdIntoFile())
      MCE_WARN("DataLoaderController::LoadData ----> fail to  write dbids into file.");

    MCE_DEBUG("DataLoaderController::LoadData  ----> online_video_db_id:"<<online_video_db_id_ 
        <<"     hotshare_video_db_id:"<<hotshare_video_db_id_<<"    hotshare_album_db_id_:"<<  hotshare_album_db_id_
        <<"     hotshare_db_insert_count_:"<<hotshare_db_insert_count_<<"   db_buffer_finished_:"<<db_buffer_finished_ );
  }
  return true;
}

bool DataLoaderController::ShareOnlineVideos(std::vector<OnlineVideoInfo> &online_video_vector, std::vector<HotShareInfo> &hotshare_online_video_vector) {
  //To do
  return true;
}

bool DataLoaderController::InsertHotShare() {
  bool ret = false;
  if (!db_buffer_finished_) {
    ret = DataLoaderDB::InsertHotShare(hotshare_vector_);
    hotshare_vector_.clear();
  } else if (db_fetch_data_count_ < kInstantGetDataCycle) {
    MCE_DEBUG("DataLoaderController::InsertHotShare    hotshare_vecotr.size:" << hotshare_vector_.size()<<"   hotshare_vector_insert_head_:"<<hotshare_vector_insert_head_
        <<"    kInstantGetDataCycle:"<<kInstantGetDataCycle<<"    db_fetch_data_count_:"<<db_fetch_data_count_);
    size_t temp_insert_size = (hotshare_vector_.size()-hotshare_vector_insert_head_) / (kInstantGetDataCycle-db_fetch_data_count_);
    if (temp_insert_size==0 && hotshare_vector_.size()>hotshare_vector_insert_head_)
      temp_insert_size = 1; 
    if (temp_insert_size > 0) {
      ret = DataLoaderDB::InsertHotSharePart(hotshare_vector_, hotshare_vector_insert_head_, temp_insert_size);
      hotshare_vector_insert_head_ += temp_insert_size;
    }
    if (hotshare_vector_.size() <= hotshare_vector_insert_head_) {
      hotshare_vector_.clear();
      hotshare_vector_insert_head_ = 0;
    }
    MCE_DEBUG("DataLoaderController::InsertHotShare    buffer_empty..insert_size:" << temp_insert_size);
  }
  return ret;
}

bool DataLoaderController::DeleteHotShare() {
  return DataLoaderDB::DeleteHotShare();
}

bool DataLoaderController::WriteDBIdIntoFile() {
  DBIdBuffer db_id_buffer;
  db_id_buffer.online_video_db_id_ = online_video_db_id_;
  db_id_buffer.hotshare_video_db_id_ = hotshare_video_db_id_;
  db_id_buffer.hotshare_album_db_id_ = hotshare_album_db_id_;
  db_id_buffer.hotshare_db_insert_count_ = hotshare_db_insert_count_;
  FILE *f;
  if (f=fopen(kDBIdFilePath,"w+")) {
    size_t ret = fwrite(&db_id_buffer, sizeof(DBIdBuffer), 1, f);
    if (ret != 1)
      MCE_WARN("DataLoaderController::WriteDBIdIntoFile  ----> Fail to write dbids into dbid.conf.");
    fclose(f);
    return true;
  } else { 
    MCE_WARN("DataLoaderController::WriteDBIdIntoFile ----> Fail to create dbid.conf.");
  }
  
  return false;
}

bool DataLoaderController::GetDBIdFromFile() {
  FILE *f;
  if (f= fopen(kDBIdFilePath, "r")) {
    DBIdBuffer db_id_buffer;
    size_t ret = fread(&db_id_buffer, sizeof(DBIdBuffer), 1, f);
    if (ret == 1) {
      online_video_db_id_ = db_id_buffer.online_video_db_id_;
      hotshare_video_db_id_ = db_id_buffer.hotshare_video_db_id_;
      hotshare_album_db_id_ = db_id_buffer.hotshare_album_db_id_;
      hotshare_db_insert_count_ = db_id_buffer.hotshare_db_insert_count_;
      MCE_DEBUG("DataLoaderController::GetDBIdFromFile()  ----> online_video_db_id_:"<<online_video_db_id_<<"   hotshare_video_db_id:"<<hotshare_video_db_id_
          <<"   hotshare_album_db_id_:"<<hotshare_album_db_id_<<"    hotshare_db_insert_count_:"<<hotshare_db_insert_count_);
    } else { 
      MCE_WARN("DataLoaderController::GetDBIdFromFile()  ----> Fail to get dbids from dbid.conf.");
    }
    fclose(f);
    return true;
  } else {
    MCE_WARN("DataLoaderController::GetDBIdFromFile()  ----> Fail to open dbid.conf.");
  }
  return false;
}

bool DataLoaderController::LoadOnlineVideos(std::vector<OnlineVideoInfo> &online_video_vector) {
  return DataLoaderDB::GetOnlineVideo(online_video_db_id_, kOnlineVideoFetchSize, online_video_vector);
}

bool DataLoaderController::LoadHotShareVideo(std::vector<HotShareInfo> &hotshare_video_vector) {
  return DataLoaderDB::GetHotShare(kVideoType, hotshare_video_db_id_, kVideoFetchSize, hotshare_video_vector);
}

bool DataLoaderController::LoadHotShareAlbum(std::vector<HotShareInfo> &hotshare_album_vector) {
  return DataLoaderDB::GetHotShare(kAlbumType, hotshare_album_db_id_, kAlbumFetchSize, hotshare_album_vector);
}

size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *data) {
  CurlBuffer *buffer  = (CurlBuffer *)data;
 
  memcpy(buffer->buffer_ptr_+buffer->buffer_size_, ptr, size*nmemb);
  buffer->buffer_size_ += size*nmemb;
  buffer->buffer_ptr_[buffer->buffer_size_] = 0;
 
  return size*nmemb;
}

string DataLoaderController::ParseJSON(const JSONNode &n ) {
  string ret = "";
  JSONNode::const_iterator i = n.begin();
  while (i != n.end()) {
    if (i -> type() == JSON_ARRAY || i -> type() == JSON_NODE) {
       ret = ParseJSON(*i);
    }
    if (ret != "")
      break;  
    std::string node_name = i -> name();
    if (node_name == "meta") {
      ret = i->as_string();
      break;
    }
    ++i;
  }
  return ret;  
}

string DataLoaderController::GetSwfAddress(const std::string &data) {
  string flash_url;
  JSONNode n = libjson::parse(data);
  string meta_content = ParseJSON(n);
  size_t head_pos = meta_content.find("\"flash_url\":\"",0);
  if (head_pos == string::npos)
    return "";
  size_t rear_pos = meta_content.find("\",\"width\"");
  if (rear_pos == string::npos)
    return "";
  string flash_url_raw  = meta_content.substr(head_pos+13,rear_pos-head_pos-13);
  ostringstream ss;
  for (size_t i =0; i<flash_url_raw.length(); ++i)
    if (flash_url_raw[i] != '\\')
      ss << flash_url_raw[i];

  return ss.str();
}

bool DataLoaderController::GetSwfUrl(std::vector<HotShareInfo> &hotshare_video_vector) {
  if (curl_code_ != CURLE_OK) {       
      MCE_WARN("HotShareController::GetVideoAddress  Message---->    Fail to curl_global_init");    
     return false;    
  }

  for (size_t i=0; i<hotshare_video_vector.size(); ++i) {
    ostringstream ss;
//    ss << "http://10.3.17.19:8180/share/" << hotshare_video_vector[i].user_id << "/share/" << hotshare_video_vector[i].share_id;
    ss << "http://share.ugc.xoa.renren.com:8180/share/" << hotshare_video_vector[i].user_id << "/share/" << hotshare_video_vector[i].share_id;

    CurlBuffer url_buffer;
    CURLcode ret ;
    CURL *curl = curl_easy_init();
    if (curl) {
      curl_easy_setopt(curl, CURLOPT_POST, 0);
      curl_easy_setopt(curl, CURLOPT_URL, ss.str().c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&url_buffer);
      curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 5000);         
      ret = curl_easy_perform(curl);
    }
    long response_code = 0;
    ret = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    double download_size = 0;
    ret = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &download_size);

    curl_easy_cleanup(curl);

    string url = "";
    if (response_code==200 && download_size>0)
      url = GetSwfAddress(string(url_buffer.buffer_ptr_, url_buffer.buffer_size_));
    else
      MCE_INFO("DataLoaderController::GetSwfUrl   fail to download.  response code:" << response_code << "   url:"<<ss.str().c_str()); 
    if (url == "")
      MCE_INFO("DataLoaderController::GetSwfUrl   fail to get url.  response code:" << response_code << "   url:"<<ss.str().c_str()); 

    hotshare_video_vector[i].url = url;   
  }
  
  return true;
}

void DataLoaderController::GetMixUpNum(const size_t hotshare_album_rest_size,const size_t hotshare_video_rest_size, 
    size_t &hotshare_album_unit_size, size_t &hotshare_video_unit_size) {
  const size_t base_size = 10;
  if (hotshare_album_rest_size == 0) {
    hotshare_video_unit_size = hotshare_video_rest_size>base_size ? base_size : hotshare_video_rest_size;
    hotshare_album_unit_size = 0;
  } else if (hotshare_video_rest_size == 0) {
    hotshare_album_unit_size =  hotshare_album_rest_size>base_size ? base_size : hotshare_album_rest_size;
    hotshare_video_unit_size = 0;
  } else {
    float temp_ratio = hotshare_video_rest_size*1.0/(hotshare_album_rest_size+hotshare_video_rest_size);
    size_t temp_size = (size_t)(temp_ratio*base_size);
    hotshare_video_unit_size = temp_size>hotshare_video_rest_size? hotshare_video_rest_size: temp_size;
    hotshare_album_unit_size = (10-hotshare_video_unit_size)>hotshare_album_rest_size ? hotshare_album_rest_size : (10-hotshare_video_unit_size);
  }
}

void DataLoaderController::InstantMixUpData(vector<HotShareInfo> &hotshare_video_vector, vector<HotShareInfo> &hotshare_album_vector,
        vector<HotShareInfo> &hotshare_online_video_vector) {
  vector<HotShareInfo> temp_vector;
  size_t hotshare_album_vector_head_pos = 0;
  size_t hotshare_video_vector_head_pos = 0;
  size_t hotshare_online_video_vector_head_pos = 0;
  size_t hotshare_album_rest_size = hotshare_album_vector.size();
  size_t hotshare_video_rest_size = hotshare_video_vector.size();
  size_t online_video_rest_size = hotshare_online_video_vector.size(); 
  size_t hotshare_album_unit_size = 0;
  size_t hotshare_video_unit_size = 0;
  size_t online_video_unit_size = 0;
  size_t count=0, i=0, j=0, k=0;
  bool nothing_left = false;
  while (true) {
    while (true) {
      GetMixUpNum(hotshare_album_rest_size, hotshare_video_rest_size, hotshare_album_unit_size, hotshare_video_unit_size);
      size_t temp_count = 0;
      for (i=0; i<hotshare_album_unit_size&&hotshare_album_vector_head_pos<hotshare_album_vector.size(); ++i,++hotshare_album_vector_head_pos)
        temp_vector.push_back(hotshare_album_vector[hotshare_album_vector_head_pos]);
      temp_count += i, hotshare_album_rest_size -= i;
      for (j=0; j<hotshare_video_unit_size&&hotshare_video_vector_head_pos<hotshare_video_vector.size(); ++hotshare_video_vector_head_pos)
        if (hotshare_video_vector[hotshare_video_vector_head_pos].url.length() > 0) {
          temp_vector.push_back(hotshare_video_vector[hotshare_video_vector_head_pos]);
          ++j;
        }
      temp_count += j, hotshare_video_rest_size = hotshare_video_vector.size()-hotshare_video_vector_head_pos;
      for (k=0; k<online_video_unit_size&&hotshare_online_video_vector_head_pos<hotshare_online_video_vector.size(); ++k,++hotshare_online_video_vector_head_pos)
        temp_vector.push_back(hotshare_online_video_vector[hotshare_online_video_vector_head_pos]);
      temp_count += k, online_video_unit_size -= k;
      count += temp_count;
      if (count >= 10)
        break;
      if (temp_count == 0) {
        nothing_left = true;
        break;
      }
    }
    if (temp_vector.size() > 0) {
      random_shuffle(temp_vector.begin(), temp_vector.end());
      for (vector<HotShareInfo>::const_iterator temp_vector_iter=temp_vector.begin(); temp_vector_iter!=temp_vector.end(); ++temp_vector_iter)
        hotshare_vector_.push_back(*temp_vector_iter);
    }
    if (nothing_left)
      break;
    count = 0;
    temp_vector.clear();
  }
 
  MCE_DEBUG("DataLoaderController::MixUpData  ----> hotshare_vector_.size:" << hotshare_vector_.size());
} 

void DataLoaderController::BufferMixUpData(std::vector<HotShareInfo> &hotshare_video_vector, std::vector<HotShareInfo> &hotshare_album_vector,
        std::vector<HotShareInfo> &hotshare_online_video_vector) {
  if (hotshare_video_vector.size()==0 || hotshare_album_vector.size()==0) 
    return;

  vector<HotShareInfo> temp_vector;
  size_t hotshare_album_vector_head_pos = 0;
  size_t hotshare_video_vector_head_pos = 0;
  size_t hotshare_online_video_vector_head_pos = 0;
  size_t count=0, i=0, j=0, k=0;
  bool nothing_left = false;
  while (true) {
    while (true) {
      size_t temp_count = 0;
      for (i=0; i<kHotShareAlbumUnitSize&&hotshare_album_vector_head_pos<hotshare_album_vector.size(); ++i,++hotshare_album_vector_head_pos)
        temp_vector.push_back(hotshare_album_vector[hotshare_album_vector_head_pos]);
      if (i == 0) {
        nothing_left = true;
        break;
      }
      temp_count += i;
      for (j=0; j<kHotShareVideoUnitSize&&hotshare_video_vector_head_pos<hotshare_video_vector.size(); ++hotshare_video_vector_head_pos)
        if (hotshare_video_vector[hotshare_video_vector_head_pos].url.length() > 0) {
          temp_vector.push_back(hotshare_video_vector[hotshare_video_vector_head_pos]);
          ++j;
        }
      if (j == 0) {
        nothing_left = true;
        break;
      }
      temp_count += j;
      for (k=0; k<kOnlineVideoUnitSize&&hotshare_online_video_vector_head_pos<hotshare_online_video_vector.size(); ++k,++hotshare_online_video_vector_head_pos)
        temp_vector.push_back(hotshare_online_video_vector[hotshare_online_video_vector_head_pos]);
      temp_count += k;
      count += temp_count;
      if (count >= kMixUpUnitSize)
        break;
      if (temp_count == 0) {
        nothing_left = true;
        break;
      }
    }
    if (temp_vector.size() > 0) {
      random_shuffle(temp_vector.begin(), temp_vector.end());
      for (vector<HotShareInfo>::const_iterator temp_vector_iter=temp_vector.begin(); temp_vector_iter!=temp_vector.end(); ++temp_vector_iter)
        hotshare_vector_.push_back(*temp_vector_iter);
    }
    if (nothing_left)
      break;
    count = 0;
    temp_vector.clear();
  }
 
  MCE_DEBUG("DataLoaderController::MixUpData  ----> hotshare_vector_.size:" << hotshare_vector_.size());
  /*
  for (size_t t=0; t<hotshare_vector_.size(); t++)
    MCE_DEBUG("DataLoaderController::MixUpData  ----> id:" << hotshare_vector_[t].id<<"   share_id:"<<hotshare_vector_[t].share_id
        <<"  user_id:"<<hotshare_vector_[t].user_id<<"  title:"<<hotshare_vector_[t].title<<"   url:"<<hotshare_vector_[t].url
       <<"   summary:"<<hotshare_vector_[t].summary<<"  creation_date:"<<hotshare_vector_[t].creation_date<<"   user_name:"<<hotshare_vector_[t].user_name
       <<"   thumb_url:"<<hotshare_vector_[t].thumb_url<<"    catagory:"<<hotshare_vector_[t].catagory );
       */
}
