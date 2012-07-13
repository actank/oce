#include "HotShareController.h"
#include "IDInfo.h"
#include "Config.h"
#include "LogWrapper.h"
#include "UserUrlAdapter.h"
#include "util/cpp/MD5Util.h"
#include <json/json.h>
#include <openssl/md5.h>
#include <iomanip>

using namespace std;
using namespace MyUtil;
using namespace xce::adapter::userurl;
using namespace xce::hotshare;


void GetDataTimer::handle()
{
  MCE_INFO("***********start timer*********");

  HotShareController &hsController = HotShareController::instance();
  
  hsController.LoadDBID();
  hsController.LoadVideo();
}

HotShareController::HotShareController() : is_read_all_id_(true)
{
  curl_code_ = curl_global_init(CURL_GLOBAL_ALL);
}

HotShareController::~HotShareController()
{
  curl_global_cleanup();
}

bool HotShareController::LoadDBID()
{
  if (!is_read_all_id_) 
    return true;

  vector<IDInfo> IDInfoVec;
  
  if ( !hsDB_.GetID(IDInfoVec, hsBuffer_.GetMaxLocalID(), ID_SIZE) )
  {
    MCE_WARN("HotShareController::SetDBID  Message---->    Fail to get IDInfo from DB, maxLocalID:"<<hsBuffer_.GetMaxLocalID());
    return false;
  }

  MCE_INFO("HotShareController::SetDBID   Message---->    IDInfoVec.size: "<<IDInfoVec.size());

  for (vector<IDInfo>::const_iterator iter=IDInfoVec.begin(); iter!=IDInfoVec.end(); ++iter)
  {
    int ret = hsBuffer_.AddID(iter->localID, iter->DBID);
    MCE_INFO("HotShareController::SetDBID   Message---->   localID:"<<iter->localID<<"    DBID:"<<iter->DBID
        <<"   topDBIDPos:"<<hsBuffer_.topDBIDPos <<"    bottomDBIDPos:"<<hsBuffer_.bottomDBIDPos<<"   ret:"<<ret);
  }

  if (IDInfoVec.size() == 0)
    is_read_all_id_ = false;

  return true;
}

bool HotShareController::LoadVideo()
{    
  MCE_INFO("HotShareController::LoadVideo Message---->    maxDBID:"<<hsBuffer_.GetMaxDBID());

  Ice::Long retMaxDBID = 0;
  std::vector<HotShareInfo> hsInfoVec;
  size_t hsInfoVecPos = 0;

  if (!hsDB_.GetHotShare(hsInfoVec, hsBuffer_.GetMaxDBID(), HSDATA_SIZE ,retMaxDBID))
  {
    MCE_WARN("HotShareController::LoadVideo Message---->   Fail to get hsdata from DB");
    return false;
  }
  MCE_INFO("HotShareController::LoadVideo Message---->    hsInfoVec:"<<hsInfoVec.size());

  if (hsInfoVec.size() > 0)
  {
  
    hsBuffer_.SetMaxDBID(retMaxDBID);

    MCE_INFO("HotShareController::LoadVideo Message---->   SetMaxDBID:" << retMaxDBID);
  
    GetUserUrl(hsInfoVec, hsInfoVecPos); 
    GetVideoAddress(hsInfoVec, hsInfoVecPos);  
  }
  return true;
}

bool HotShareController::GetUserUrl(std::vector<HotShareInfo> & hsInfoVec, size_t &hsInfoVecPos)
{
  for (vector<HotShareInfo>::iterator iter=hsInfoVec.begin(); iter!=hsInfoVec.end(); ++iter)
  {
    UserUrlInfoPtr u;
    try
    {
      u = UserUrlAdapter::instance().getUserUrl(iter->user_id);
    }
    catch (Ice::Exception &)
    {
      MCE_WARN("HotShareController::GetUserUrl Message---->    Fail to get users' urls");
      continue;
    }
    if (u)
    {
      iter->head_url = "http://hdn.xnimg.cn/photos/"+u->tinyUrl();
    }
  }

  return true;
}

bool HotShareController::GetVideoAddress(std::vector<HotShareInfo> & hsInfoVec,  size_t &hsInfoVecPos)
{
  if ( hsInfoVec.size() == 0)
    return true;

  if (hsInfoVecPos < hsInfoVec.size())
  {
    if (curl_code_ != CURLE_OK)
    {       
      MCE_WARN("HotShareController::GetVideoAddress  Message---->    Fail to curl_global_init");    
      return false;    
    }

  
    ostringstream ss;
    ss << "http://10.3.17.19:8180/share/" << hsInfoVec[hsInfoVecPos].user_id << "/share/" << hsInfoVec[hsInfoVecPos].share_id;
    //ss << "http://share.ugc.xoa.renren.com:8180/share/" << hsInfoVec[hsInfoVecPos].user_id << "/share/" << hsInfoVec[hsInfoVecPos].share_id;

    std::string urlBuffer;
    CURLcode ret ;
    CURL *curl = curl_easy_init();
    if (curl)
    {
      curl_easy_setopt(curl, CURLOPT_POST, 0);

      curl_easy_setopt(curl, CURLOPT_URL, ss.str().c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&urlBuffer);
      curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, HTTP_TIMEOUT);         
      ret = curl_easy_perform(curl);
    }

    long response_code = 0;
    ret = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    double download_size = 0;
    ret = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &download_size);

    curl_easy_cleanup(curl);

    MCE_WARN("reponse code:"<<response_code);

    MCE_INFO("HotShareController::GetVideoAddress  Message---->      id:"<<hsInfoVec[hsInfoVecPos].id<<"   user_id:"<<hsInfoVec[hsInfoVecPos].user_id
        <<"   share_id:"<<hsInfoVec[hsInfoVecPos].share_id<<"     hsInfoVecPos:"<<hsInfoVecPos<<"    url:"<<ss.str().c_str());

    string url = "";
    if (response_code==200 && download_size>0)
      url = HotShareController::GetSwfAddress(urlBuffer);
    hsInfoVec[hsInfoVecPos].url = url;   
    hsInfoVecPos++;
    GetVideoAddress(hsInfoVec, hsInfoVecPos);
  }
  else
  {    
    size_t validCount = 0;
    //Ice::Long maxLocalID = hsBuffer_.GetMaxDataBufferID();     
    Ice::Long maxLocalID = hsBuffer_.GetMaxLocalID();     
    
    for (vector<HotShareInfo>::iterator iter= hsInfoVec.begin(); iter!= hsInfoVec.end(); ++iter)
    {
      if (iter->url == "")
      {
        MCE_INFO("HotShareController::GetVideoAddress  Message---->    no url   id:"<<iter->id);
        continue;
      }
  
      //iter->local_id = maxLocalID + validCount + 1;
      bool insertID = true;
      int localID = hsBuffer_.GetLocalID(iter->id, insertID);
      if (localID == -1)
        continue;
      iter->local_id = localID;

      if ( insertID )
      {          
        MCE_INFO("HotShareController::GetVideoAddress  Message---->    insert local_id:" <<iter->local_id << "   src_id:"<<iter->id 
            <<"    maxLocalID:"<<maxLocalID<<"    validCount:"<<validCount);
  
        hsDB_.InsertID(iter->local_id, iter->id);
      }

      string xml = VideoXML::OutputXML((*iter), HS_VERSION); 
      MCE_INFO("HotShareController::GetVideoAddress  Message---->    create xml: " <<xml.c_str() );

      MCE_INFO("HotShareController::GetVideoAddress  Message---->    before: topBufferPos:" <<hsBuffer_.topBufferPos <<"   bottomBufferPos:"<<hsBuffer_.bottomBufferPos
          <<"   topDBIDPos:"<<hsBuffer_.topDBIDPos<<"   bottomDBIDPos:"<<hsBuffer_.bottomDBIDPos);
      hsBuffer_.AddXML(xml, iter->id);
  
      MCE_INFO("HotShareController::GetVideoAddress  Message---->   after: topBufferPos:" <<hsBuffer_.topBufferPos <<"   bottomBufferPos:"<<hsBuffer_.bottomBufferPos
          <<"   topDBIDPos:"<<hsBuffer_.topDBIDPos<<"   bottomDBIDPos:"<<hsBuffer_.bottomDBIDPos);
  
      validCount++;
  
      MCE_INFO("HotShareController::GetVideoAddress  Message---->    total:"<<hsInfoVec.size()<<"   validCount:"<<validCount);
    }
  }    

  return true;         
}


size_t HotShareController::WriteCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  std::string * buf = (std::string *)data;
  *buf = std::string((char *)ptr, size * nmemb);

  return size * nmemb;
}

string HotShareController::ParseFlashUrl(const string &data) 
{
  string title = "flash_url\\\":\\\"";
  size_t pos1= data.find(title.c_str(),0);
  if (pos1 == string::npos)
    return "";
  
  size_t pos2 = data.find("\\\",\\\"",pos1);
  if (pos2 == string::npos)
    return "";
       
  return data.substr(pos1+title.length(),  pos2 - pos1 - title.length());
}

string HotShareController::GetSwfAddress(const string & json_text)
{
  Json::Value root;
  Json::Reader root_reader;

  if (!root_reader.parse(json_text, root)) {
    return "";
  }

  Json::Value meta;
  Json::Reader meta_reader;
  if (!meta_reader.parse(root.get("meta", "").asString(), meta)) {
    return "";
  }

  return meta.get("flash_url", "").asString();
}

string HotShareController::GetData(const string &post_body)
{

  vector<Ice::Long> highIDVec, lowIDVec;
  size_t size, type;
  Ice::Long maxLocalID;

  if ( !GetDataFromRequestBody(post_body, highIDVec, lowIDVec, size ,type ,maxLocalID))
    return ""; 

  ostringstream output;  
          
  int tempSize = (int)size;                    

  vector<string>  xmlVec;
  vector<Ice::Long> retLowIDVec, retHighIDVec;

  if (type == VIDEO_TYPE) 
  {

    if (size==0 || size> VIDEO_LIMIT_SIZE)
      size = VIDEO_LIMIT_SIZE;
                                          
    Ice::Long lowID, highID;

    hsBuffer_.GetHotShare(xmlVec, maxLocalID, tempSize, lowID, highID); 
    if (xmlVec.size() > 0)
    {
      tempSize -= (int)(highID - lowID +1);

      retLowIDVec.push_back(lowID);
      retHighIDVec.push_back(highID);      

    }

    MCE_DEBUG("HotShareController::GetData maxid**   Message---->    xmlVec.size:"<<xmlVec.size()<<"    lowID:"<<lowID<<"    highID:"<<highID<<"     tempSize:"<<tempSize <<"   bufferDBID:"<<hsBuffer_.GetMaxDataBufferID()<<"    getmaxLocalID:"<<hsBuffer_.GetMaxLocalID()<<"    getMaxDBID:"<<hsBuffer_.GetMaxDBID() );
                                                          
    size_t count = 0;
    while (tempSize > 0)
    {
      if (lowIDVec.size() <= count || highIDVec.size() <= count )
        break;
     
      const size_t tempXmlVecSize = xmlVec.size(); 
      if ( hsBuffer_.GetHotShare(xmlVec, lowIDVec[count], highIDVec[count], tempSize, lowID, highID)  )
      {     
        if (xmlVec.size()-tempXmlVecSize > 0)     
        {       
          tempSize -= (int)(highID - lowID +1);
                                                                                                                                                      
          retLowIDVec.push_back(lowID);       
          retHighIDVec.push_back(highID);     

          MCE_DEBUG("HotShareController::GetData  Message---->     count:"<<count<<"   lowIDVec:"<<lowIDVec[count]<<"    highID:"<<highIDVec[count]
              <<"   tempSize:"<<tempSize<<"     retLowID:"<<lowID<<"   retHighID:"<<highID);
        }     
        else       
        {
          MCE_DEBUG("HotShareController::GetData  Message---->     count:"<<count<<"   lowIDVec:"<<lowIDVec[count]<<"    highID:"<<highIDVec[count]
              <<"   tempSize:"<<tempSize<<"     retLowID:"<<lowID<<"   retHighID:"<<highID);
          break;
        }
      }
      else
        MCE_DEBUG("HotShareController::GetData  Message---->    Fail to get data to fill hole..    lowIDVec:"<<lowIDVec[count]<<"    highIDVec:"<<highIDVec[count]);
              
      count++;
    }

    output << VideoXML::OutputXML(xmlVec, retLowIDVec, retHighIDVec);
  }
                          
//  MCE_DEBUG("HotShareController::GetData  Message---->      output:\n"<<output.str().c_str());
  return output.str();
                              


}

bool HotShareController::GetDataFromRequestBody(const string &_post_body, vector<Ice::Long> &highIDVec, vector<Ice::Long> &lowIDVec, 
    size_t &size , size_t &type, Ice::Long &maxLocalID )
{
  if (_post_body == "")
    return false;
     
  MCE_DEBUG("HotShareController::GetData  Message---->    post_body:"<<_post_body.c_str());
  highIDVec.clear();
  lowIDVec.clear();
  size = 0;
  maxLocalID = 0;
  type = 0;     

  std::string source , md5;
  
  vector<string> dataVec;
  const char *data = _post_body.c_str();
  boost::split(dataVec, data, boost::is_any_of("&"), boost::token_compress_on);
  if ( dataVec.size() > 0)
  {
    for (vector<string>::const_iterator iter=dataVec.begin(); iter!=dataVec.end(); ++iter)
    {
      if ((*iter).length() > 0)
      {
        size_t pos = (*iter).find_first_of('=');
        if (pos!=string::npos && (*iter).substr(pos+1).length()>0)
        {
          string key = (*iter).substr(0, pos);
          if (key == "highid")
            highIDVec.push_back( atoll(((*iter).substr(pos+1, (*iter).length()-pos-1)).c_str()));
          else if (key == "lowid")
            lowIDVec.push_back( atoll( (*iter).substr(pos+1, (*iter).length()-pos-1).c_str() ));
          else if (key=="size" && size==0)
            size = static_cast<size_t>(atoi((*iter).substr(pos+1, (*iter).length()-pos-1).c_str() ));
          else if (key=="maxid" && maxLocalID==0)
            maxLocalID = atoll( (*iter).substr(pos+1, (*iter).length()-pos-1).c_str());
          else if (key=="type" && type==0)
            type = static_cast<size_t>(atoi((*iter).substr(pos+1, (*iter).length()-pos-1).c_str() ));
          else if (key == "bottom")
            source = (*iter).substr(pos+1, (*iter).length()-pos-1);  
          else if (key == "top")
            md5 = (*iter).substr(pos+1, (*iter).length()-pos-1);  
        }
      }
    }
  }

  if ( !CheckMD5(source, md5) )
   return false;

  if (highIDVec.size() != lowIDVec.size())
    return false;

  return true;
}



bool HotShareController::CheckMD5(string uid, const string & key) 
{
  uid += MD5_KEY;

  unsigned char* md5 = MD5((unsigned char*) uid.c_str(), uid.size(), NULL);
    
  stringstream res;
  for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i)
  {
    res << hex << setw(2) << setfill('0') << (unsigned int) md5[i];
  }
    
  MCE_DEBUG("ClubDispatchKey md5 : " << res.str());
  MCE_DEBUG("ClubDispatchKey key : " << key);
      
  return res.str() == key;
    
}

