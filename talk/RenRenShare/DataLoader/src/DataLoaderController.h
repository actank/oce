#ifndef _DATALOADERCONTROLLER_H
#define _DATALOADERCONTROLLER_H

#include <Ice/Ice.h>
#include <vector>
#include <curl/curl.h>
#include "libjson/libjson.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "OnlineVideoInfo.h"
#include "HotShareInfo.h"

namespace xce {
namespace dataloader {

class DataLoaderController: public MyUtil::Singleton<DataLoaderController> {
  public:
    DataLoaderController();
    ~DataLoaderController();
    bool LoadData();
    bool DeleteHotShare();
    void IncreaseCount();
  private:
    bool LoadOnlineVideos(std::vector<OnlineVideoInfo> &online_video_vector);
    bool ShareOnlineVideos(std::vector<OnlineVideoInfo> &online_video_vector, std::vector<HotShareInfo> &hotshare_online_video_vector);
    bool LoadHotShareVideo(std::vector<HotShareInfo> &hotshare_video_vector);
    bool LoadHotShareAlbum(std::vector<HotShareInfo> &hotshare_album_vector);
    bool GetSwfUrl(std::vector<HotShareInfo> &hotshare_video_vector);
    void BufferMixUpData(std::vector<HotShareInfo> &hotshare_album_vector, std::vector<HotShareInfo> &hotshare_album_vector,
        std::vector<HotShareInfo> &hotshare_online_video_vector); 
    void InstantMixUpData(std::vector<HotShareInfo> &hotshare_album_vector, std::vector<HotShareInfo> &hotshare_album_vector,
        std::vector<HotShareInfo> &hotshare_online_video_vector); 
    void GetMixUpNum(const size_t hotshare_album_rest_size,const size_t hotshare_video_rest_size, 
        size_t &hotshare_album_unit_size, size_t &hotshare_video_unit_size);
    std::string ParseJSON(const JSONNode &n);
    std::string GetSwfAddress(const std::string &json);
    bool InsertHotShare();
    bool GetDBIdFromFile();
    bool WriteDBIdIntoFile();

    Ice::Long online_video_db_id_;
    Ice::Long hotshare_video_db_id_;
    Ice::Long hotshare_album_db_id_; 
    Ice::Long hotshare_db_insert_count_; 
    std::vector<HotShareInfo> hotshare_vector_;
    size_t hotshare_vector_insert_head_; 
    size_t db_fetch_data_count_;
    bool db_buffer_finished_; 
    CURLcode curl_code_;
};

class DataLoaderTimer: public MyUtil::Timer, public MyUtil::Singleton<DataLoaderTimer> {
  public:
    DataLoaderTimer() : MyUtil::Timer(600*1000) {}
    void handle();
};

class CurlBuffer {
  public:
    char *buffer_ptr_;
    size_t buffer_size_;
     
    CurlBuffer() {
       buffer_ptr_ = new char[102400];
       buffer_size_ = 0;
    }
    ~CurlBuffer() {
       delete buffer_ptr_;
    }
};

struct DBIdBuffer {
  Ice::Long online_video_db_id_;
  Ice::Long hotshare_video_db_id_;
  Ice::Long hotshare_album_db_id_;
  Ice::Long hotshare_db_insert_count_; 
};
}
}
#endif
