#ifndef _HOTSHARECONTROLLER_H
#define _HOTSHARECONTROLLER_H

#include <vector>
#include <curl/curl.h>

#include "HotShareBuffer.h"
#include "HotShareDB.h"
#include "HotShareInfo.h"
#include "VideoXML.h"
#include "Singleton.h"
#include "TaskManager.h"

namespace xce {
namespace hotshare {

const size_t TIMER_FREQ = 60000;
const size_t URLBUFFER_SIZE = 1024 * 2;


class HotShareController : public MyUtil::Singleton<HotShareController>
{
  private:
    bool GetUserUrl(std::vector<HotShareInfo> & hsInfoVec, size_t &hsInfoVecPos);
    bool GetVideoAddress(std::vector<HotShareInfo> & hsInfoVec, size_t &hsInfoVecPos);

    HotShareBuffer hsBuffer_;
    HotShareDB     hsDB_;

    bool is_read_all_id_;

    CURLcode curl_code_;
    static size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *data);
    
    bool GetDataFromRequestBody(const std::string &_post_body, vector<Ice::Long> &highIdVec, vector<Ice::Long> &lowIdVec, 
        size_t &size , size_t &type, Ice::Long &maxLocalId );

    bool CheckMD5(string uid, const string & key) ;
  public:
    HotShareController();
    ~HotShareController();
      
    bool LoadDBID();
    bool LoadVideo();

    std::string GetData(const std::string &post_body);
    static std::string GetSwfAddress(const std::string &data);
    std::string ParseFlashUrl(const std::string & json_text);
};

class GetDataTimer: public MyUtil::Timer, public MyUtil::Singleton<GetDataTimer>
{
  public:
    GetDataTimer() : MyUtil::Timer(TIMER_FREQ) {}
    void handle();
};


}
}
#endif
