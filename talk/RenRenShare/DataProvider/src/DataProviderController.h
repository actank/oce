#ifndef _DATAPROVIDERCONTROLLER_H
#define _DATAPROVIDERCONTROLLER_H

#include <Ice/Ice.h>
#include <vector>
#include <curl/curl.h>
#include "libjson/libjson.h"
#include "Singleton.h"
#include "TaskManager.h"
#include "DataProviderInfo.h"

namespace xce {
namespace dataprovider {

const size_t kRequestLimitSize = 30;
const std::string kMd5Key = "holyshit";

class CurlBuffer {
  public:
    char *buffer_ptr_;
    size_t buffer_size_;
     
    CurlBuffer() {
      buffer_ptr_ = new char[204800];
      buffer_size_ = 0;
    }
    ~CurlBuffer() {
      delete buffer_ptr_;
    }
};

class DataProviderController: public MyUtil::Singleton<DataProviderController> {
  public:
    DataProviderController();
    ~DataProviderController();

    bool LoadHotShareInfo();
    std::string GetHotShareData(const std::string &post_body);
    std::string GetPhotoListData(const std::string &post_body);

  private:
    bool GetDataFromHotShareRequestBody(const std::string &_post_body, std::vector<Ice::Long> &high_id_vector, std::vector<Ice::Long> &low_id_vector, 
                       size_t &size , size_t &type, Ice::Long &max_id);
    bool GetDataFromPhotoListRequestBody(const std::string &_post_body, std::string &album_id, std::string &owner_id );
    bool GetDataFromDB(Ice::Long max_db_id, size_t fetch_size, std::vector<HotShareInfo> &hotshare_vector);
    std::string GetXMLFromHotShareInfo(std::vector<HotShareInfo> &hotshare_vector);
    bool GetHeadUrl(std::vector<HotShareInfo> &hotshare_vector);
    std::string GetJsonFromUGC(const std::string url);
    void GetAlbumCoverLargeUrl(std::vector<HotShareInfo> &hotshare_vector);
    void GetAlbumInfo(const std::string album_json, std::string &thumb_url1, std::string &description, std::string &owner_id);
    void ParseAlbumJSON(const JSONNode &n,  std::string &thumb_url1, std::string &description, std::string &owner_id);
    bool GetAlbumId(const std::string &url, std::string &album_id, std::string &owner_id);
    bool StoreHotShareIntoBuffer(std::vector<HotShareInfo> &hotshare_vector);
    void GetPhotoList(std::string album_id, std::string owner_id, std::vector<std::string> &photo_list_vector);
    void GetPhotoInfo(const std::string photo_list_json, std::vector<PhotoInfo> &photoInfoVec);
    void ParsePhotoJSON(const JSONNode &n,  std::vector<PhotoInfo> &photoInfoVec) ;
    bool CheckMD5(std::string uid, const std::string & key);
    Ice::Long max_db_id_;
    CURLcode curl_code_;
    CurlBuffer url_buffer_;
};

class DataProviderTimer: public MyUtil::Timer, public MyUtil::Singleton<DataProviderTimer> {
  public:
    DataProviderTimer() : MyUtil::Timer(30000) {}
    void handle();
};

}
}
#endif
