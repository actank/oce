#include "DataProviderController.h"
#include <sstream>
#include <json/json.h>
#include <openssl/md5.h>
#include <iomanip>
#include "LogWrapper.h"
#include "DataProviderDB.h"
#include "DataProviderBuffer.h"
#include "XMLGenerator.h"
#include "UserUrlAdapter.h"

using namespace std;
using namespace MyUtil;
using namespace xce::dataprovider;
using namespace xce::adapter::userurl;


void DataProviderTimer::handle() {
  MCE_INFO("***********start timer*********");

  DataProviderController::instance().LoadHotShareInfo();
}

DataProviderController::DataProviderController() {
  max_db_id_ = -1;
  curl_code_ = curl_global_init(CURL_GLOBAL_ALL);
}

DataProviderController::~DataProviderController() {
  curl_global_cleanup();
}

string DataProviderController::GetHotShareData(const string &post_body) {
  vector<Ice::Long> high_id_vector, low_id_vector;
  size_t size, type;
  Ice::Long max_id;

  if (!GetDataFromHotShareRequestBody(post_body, high_id_vector, low_id_vector, size, type, max_id))
    return ""; 
  if (type != 2)
    return "";

  MCE_DEBUG("DataProviderController::GetHotShareData  post_body:"<<post_body<<"    high_id_vector.size:"<<high_id_vector.size()
      <<"   low_id_vector.szie:"<<low_id_vector.size()<<"    size:"<<size<<"   type:"<<type<<"    maxi_id:"<<max_id);
  ostringstream output;          
  int temp_size = (int)size;                    
  vector<string> hotshare_xml_vector;
  vector<Ice::Long> ret_low_id_vector, ret_high_id_vector;

  if (size==0 || size>kRequestLimitSize)
    size = kRequestLimitSize  ;
                                          
  if (high_id_vector.size() == 0) {
    Ice::Long ret_low_id=0, ret_high_id=0;
    if (DataProviderBuffer::instance().GetHotShareXML(hotshare_xml_vector, max_id, size, ret_low_id, ret_high_id)) {
      ret_low_id_vector.push_back(ret_low_id);
      ret_high_id_vector.push_back(ret_high_id);
    }
  } else { 
    size_t count = 0;
    while (temp_size > 0) {
      if (high_id_vector.size() <= count || low_id_vector.size() <= count )
        break;

      Ice::Long ret_low_id=0, ret_high_id=0;
      const size_t kTempXmlVecSize = hotshare_xml_vector.size(); 
      if (DataProviderBuffer::instance().GetHotShareXML(hotshare_xml_vector, low_id_vector[count], high_id_vector[count], temp_size, ret_low_id, ret_high_id)) {     
        if (hotshare_xml_vector.size()-kTempXmlVecSize > 0) {       
          temp_size -= (int)(ret_high_id - ret_low_id +1);

          ret_low_id_vector.push_back(ret_low_id);       
          ret_high_id_vector.push_back(ret_high_id);     

          MCE_DEBUG("DataProviderController::GetHotShareData  Message---->     count:"<<count<<"   lowIDVec:"<<low_id_vector[count]<<"    highID:"<<high_id_vector[count]
              <<"   tempSize:"<<temp_size<<"     retLowID:"<<ret_low_id<<"   retHighID:"<<ret_high_id);
        } else {
          MCE_DEBUG("HotShareController::GetData  Message---->     count:"<<count<<"   lowIDVec:"<<low_id_vector[count]<<"    highID:"<<high_id_vector[count]
              <<"   tempSize:"<<temp_size<<"     retLowID:"<<ret_low_id<<"   retHighID:"<<ret_high_id);
          break;
        }
      } 
      count++;
    }
  }

  MCE_DEBUG("DataProviderController::GetHotShareData  Message---->      ret_low_id_vector:"<<ret_low_id_vector.size()<<"    ret_high_id_vector:"<<ret_high_id_vector.size());
  output << XMLGenerator::OutputXMLHotShare(hotshare_xml_vector, ret_low_id_vector, ret_high_id_vector);
//  MCE_DEBUG("DataProviderController::GetHotShareData  Message---->      output:\n"<<output.str());
  return output.str();
}

string DataProviderController::GetPhotoListData(const std::string &post_body) {
  string album_id, owner_id;
  if (!GetDataFromPhotoListRequestBody(post_body, album_id, owner_id) )
    return false;
  return DataProviderBuffer::instance().GetPhoto(atol(album_id.c_str())); 
}

bool DataProviderController::LoadHotShareInfo() {
  vector<HotShareInfo> hotshare_vector;
  if ( !DataProviderDB::GetHotShare(max_db_id_, hotshare_vector) )
    return false;
  GetHeadUrl(hotshare_vector); 
  GetAlbumCoverLargeUrl(hotshare_vector);
  
  for (size_t i=0; i<hotshare_vector.size(); i++) {
    MCE_DEBUG("DataProviderController::LoadHotShareInfo  i:"<<i<<"  id:"<<hotshare_vector[i].id <<"   title:"<<hotshare_vector[i].title 
        << "   url:"<<hotshare_vector[i].url<<"    summary:"<<hotshare_vector[i].summary<<"    creation_date:"<<hotshare_vector[i].creation_date
        <<"    release_date:"<<hotshare_vector[i].release_date<<"    user_name:"<<hotshare_vector[i].user_name << "   thumb_url:"<<hotshare_vector[i].thumb_url
         << "   thumb_url1:"<<hotshare_vector[i].thumb_url1<<"    head_url:"<<hotshare_vector[i].head_url<<"    catagory:"<<hotshare_vector[i].catagory);
  }
 
  StoreHotShareIntoBuffer(hotshare_vector);
  return true;
}

bool DataProviderController::StoreHotShareIntoBuffer(vector<HotShareInfo> &hotshare_vector) {
  for (vector<HotShareInfo>::const_iterator hotshare_vector_iter=hotshare_vector.begin(); hotshare_vector_iter!=hotshare_vector.end(); 
      hotshare_vector_iter++) {
    string hotshare_xml;
    size_t hotshare_item_type = 0;

    if (hotshare_vector_iter->catagory == "000100010001") {
      hotshare_item_type = 1;
      hotshare_xml = XMLGenerator::OutputXMLVideo(*hotshare_vector_iter);
    } else if (hotshare_vector_iter->catagory=="000100010002" && hotshare_vector_iter->thumb_url1!="") {
      hotshare_item_type = 2;
      string album_id, owner_id;
      GetAlbumId(hotshare_vector_iter->url, album_id, owner_id);
      hotshare_xml = XMLGenerator::OutputXMLAlbum(*hotshare_vector_iter, atol(album_id.c_str()));
      
      vector<string> photo_list_vector;
      GetPhotoList(album_id, owner_id, photo_list_vector);
      string photo_list = XMLGenerator::OutputXMLPhotoList(photo_list_vector);
      DataProviderBuffer::instance().AddPhoto(atol(album_id.c_str()), photo_list);
      DataProviderBuffer::instance().AddAlbumXML(hotshare_xml, atol(album_id.c_str()));
    }
    if (hotshare_item_type != 2)
      DataProviderBuffer::instance().AddHotShareXML(hotshare_xml);
  }
  return true;
}

bool DataProviderController::GetAlbumId(const string &url, string &album_id, string &owner_id) {
  size_t getalbum_pos = url.find("getalbum");
  if (getalbum_pos != string::npos) {
    size_t id_pos = url.find("id=");
    if (id_pos == string::npos)
      return false;
    size_t owner_pos = url.find("&owner=");
    if (owner_pos == string::npos)
      return false;
    size_t i = 0;
    for (i=id_pos+3; i<url.length(); ++i) {
      if (url[i]>='0' && url[i]<='9')
        album_id += url[i];
      else
        break;
    }
    for (i=owner_pos+7; i<url.length(); ++i) {
      if (url[i]>='0' && url[i]<='9')
        owner_id += url[i];
      else
        break;
    }
  } else {
    size_t photo_pos = url.find("photo");
    if (photo_pos != string::npos) {
      size_t photo_pos = url.find("photo/");
      if (photo_pos == string::npos)
        return false;
      size_t album_pos = url.find("album-");
      if (album_pos == string::npos)
        return false;
      size_t i = 0;

      for (i=album_pos+6; i<url.length(); ++i) {
        if (url[i]>='0' && url[i]<='9')
          album_id += url[i];
        else
          break;
      }
      for (i=photo_pos+6; i<url.length(); ++i) {
      if (url[i]>='0' && url[i]<='9')
        owner_id += url[i];
      else
        break;
      }
    }
  }
        return true;
}

void DataProviderController::ParsePhotoJSON(const JSONNode &n,  vector<PhotoInfo> &photoInfoVec) { 
  PhotoInfo pi;
  JSONNode::const_iterator i = n.begin();
  while (i != n.end()){
    if (i -> type() == JSON_ARRAY || i -> type() == JSON_NODE){
      ParsePhotoJSON(*i, photoInfoVec);
    }
    std::string node_name = i -> name();
    if (node_name == "headUrl")
      pi.headUrl = i->as_string();
    else if (node_name == "largeUrl")
      pi.largeUrl = i->as_string();
    else if (node_name == "atTitle")
      pi.atTitle = i->as_string();
    else if (node_name == "linkUrl")
      pi.linkUrl = i->as_string();
    else if (node_name == "id")
      pi.photoId = i->as_string();
    ++i;
  }
  if (pi.headUrl!="" && pi.largeUrl!="" && pi.linkUrl!="" && pi.photoId!="")
    photoInfoVec.push_back(pi);
}
  
void DataProviderController::GetPhotoInfo(const string photo_list_json, vector<PhotoInfo> &photoInfoVec) {
  JSONNode n = libjson::parse(photo_list_json);
  ParsePhotoJSON(n, photoInfoVec);
}

void DataProviderController::ParseAlbumJSON(const JSONNode &n,  string &thumb_url1, string &description, string &owner_id) { 
  JSONNode::const_iterator i = n.begin();
  while (i != n.end()){
    if (i -> type() == JSON_ARRAY || i -> type() == JSON_NODE){
      ParseAlbumJSON(*i, thumb_url1, description, owner_id);
    }
    std::string node_name = i -> name();
    if (node_name == "mainUrl")
      thumb_url1 = i->as_string();
    else if (node_name == "description") 
      description = i->as_string();
    else if (node_name == "owner")
      owner_id = i->as_string();
    /*
    else if (node_name == "errorCode")
      thumb_url1 == "error";
    else if (node_name == "message")
      description = i->as_string();
      */
    ++i;
  }
}
  
void DataProviderController::GetAlbumInfo(const string album_json, string &thumb_url1, string &description, string &owner_id) {
  JSONNode n = libjson::parse(album_json);
  ParseAlbumJSON(n, thumb_url1, description, owner_id);
}

void DataProviderController::GetAlbumCoverLargeUrl(vector<HotShareInfo> &hotshare_vector) {
  for (size_t i=0; i<hotshare_vector.size(); ++i) {
    if (hotshare_vector[i].catagory == "000100010002") {
      string owner_id, album_id;
      if (!GetAlbumId(hotshare_vector[i].url, album_id, owner_id)) {
       // MCE_DEBUG("DataProviderController::GetAlbumCoverLargeUrl  ---> thumb_url:"<<hotshare_vector[i].thumb_url <<"   album_id:"<<album_id<<"   owner_id:"<<owner_id);
        continue;
      }
      ostringstream ss;
      ss << "http://gadget.talk.renren.com/photo/" << owner_id <<"/album-" << album_id;
      string thumb_url_json = GetJsonFromUGC(ss.str());
      if (thumb_url_json != "") {
        string thumb_url1,  description, owner_id;
        GetAlbumInfo(thumb_url_json, thumb_url1, description, owner_id);
        MCE_DEBUG("DataProviderController::GetAlbumCoverLargeUrl  ---> thumb_url_json:"<<thumb_url_json
            <<"    hotshare_vecotr.id:"<<hotshare_vector[i].id<<"    thumb_url1:"<<thumb_url1 
            <<"    descrption:"<<description<<"  url:"<<ss.str() );
        hotshare_vector[i].thumb_url1 = thumb_url1;
        hotshare_vector[i].summary = description;
        hotshare_vector[i].owner_id = atoi(owner_id.c_str());
      }
    }
  }
}

void DataProviderController::GetPhotoList(string album_id, string owner_id, vector<string> &photo_list_vector) {
  vector<PhotoInfo> temp_photo_list_vector;
  size_t page_pos = 0;
  const size_t page_size = 20;

  while (true) {
    ostringstream ss;
    ss << "http://gadget.talk.renren.com/photo/"<<owner_id<<"/album-"<<album_id<<"/photo?pattern=page&page="<<page_pos<<"&count="<<page_size;
//    MCE_DEBUG("DataProviderController::GetPhotoList  ---> url:" << ss.str());
    string photo_json = GetJsonFromUGC(ss.str());
    if (photo_json.length() < 10)
      break;
//    MCE_DEBUG("DataProviderController::GetPhotoList  ---> photo_json:" << photo_json);
    GetPhotoInfo(photo_json, temp_photo_list_vector);
//    MCE_DEBUG("DataProviderController::GetPhotoList  ---> temp_photo_list_vector:" << temp_photo_list_vector.size());
    page_pos++;
  }
  for (vector<PhotoInfo>::const_iterator temp_photo_list_vector_iter=temp_photo_list_vector.begin(); temp_photo_list_vector_iter!=temp_photo_list_vector.end();
      temp_photo_list_vector_iter++ ) {
    string xml = XMLGenerator::OutputXMLPhoto(*temp_photo_list_vector_iter);
    if (xml != "")
      photo_list_vector.push_back(xml);  
  }
}

size_t WriteCallback(void *ptr, size_t size, size_t nmemb, void *data) {
  CurlBuffer *buffer  = (CurlBuffer *)data;
  memcpy(buffer->buffer_ptr_+buffer->buffer_size_, ptr, size*nmemb);
  buffer->buffer_size_ += size*nmemb;
  buffer->buffer_ptr_[buffer->buffer_size_] = 0;
  return size*nmemb;
}

string DataProviderController::GetJsonFromUGC(const string url) {
  if (curl_code_ != CURLE_OK) {       
      MCE_WARN("DataProviderController::GetJsonFromUGC  Message---->    Fail to curl_global_init");    
     return "";    
  }

  url_buffer_.buffer_size_ = 0;
  CURLcode ret ;
  CURL *curl = curl_easy_init();
  if (curl) {
//    curl_easy_setopt(curl, CURLOPT_POST, 0);
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&url_buffer_);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 15000);         
    struct curl_slist *slist=NULL;
    slist = curl_slist_append(slist, "user-id:221666396");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    ret = curl_easy_perform(curl);
  }
  long response_code = 0;
  ret = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
  double download_size = 0;
  ret = curl_easy_getinfo(curl, CURLINFO_SIZE_DOWNLOAD, &download_size);

  curl_easy_cleanup(curl);
  string json;
  if (response_code==200 && download_size>0)
    json = string(url_buffer_.buffer_ptr_, url_buffer_.buffer_size_);
  
  return json;
}

bool DataProviderController::GetHeadUrl(std::vector<HotShareInfo> &hotshare_vector) {
  for (vector<HotShareInfo>::iterator iter=hotshare_vector.begin(); iter!=hotshare_vector.end(); ++iter) {
    UserUrlInfoPtr u;
    try {
      u = UserUrlAdapter::instance().getUserUrl(iter->user_id);
    } catch (Ice::Exception &) {
      MCE_WARN("DataProviderController::GetUserUrl Message---->    Fail to get users' urls");
      continue;
    }
    if (u) {
      iter->head_url = "http://hdn.xnimg.cn/photos/"+u->tinyUrl();
    }
  }
  return true;
}

bool DataProviderController::GetDataFromHotShareRequestBody(const string &_post_body, vector<Ice::Long> &high_id_vector, vector<Ice::Long> &low_id_vector, 
    size_t &size , size_t &type, Ice::Long &max_id ) {
  if (_post_body == "")
    return false;
     
  MCE_DEBUG("DataProviderController::GetDataFromHotShareRequestBody(  Message---->    post_body:"<<_post_body.c_str());
  size = 0;
  max_id = 0;
  type = 0;     
  std::string source , md5;
  
  vector<string> data_vector;
  const char *data = _post_body.c_str();
  boost::split(data_vector, data, boost::is_any_of("&"), boost::token_compress_on);
  if ( data_vector.size() > 0) {
    for (vector<string>::const_iterator iter=data_vector.begin(); iter!=data_vector.end(); ++iter) {
      if ((*iter).length() > 0) {
        size_t pos = (*iter).find_first_of('=');
        if (pos!=string::npos && (*iter).substr(pos+1).length()>0) {
          string key = (*iter).substr(0, pos);
          if (key == "highid")
            high_id_vector.push_back( atoll(((*iter).substr(pos+1, (*iter).length()-pos-1)).c_str()));
          else if (key == "lowid")
            low_id_vector.push_back( atoll( (*iter).substr(pos+1, (*iter).length()-pos-1).c_str() ));
          else if (key=="size" )
            size = static_cast<size_t>(atoi((*iter).substr(pos+1, (*iter).length()-pos-1).c_str() ));
          else if (key=="maxid")
            max_id = atoll( (*iter).substr(pos+1, (*iter).length()-pos-1).c_str());
          else if (key=="type")
            type = static_cast<size_t>(atoi((*iter).substr(pos+1, (*iter).length()-pos-1).c_str() ));
          else if (key == "bottom")
            source = (*iter).substr(pos+1, (*iter).length()-pos-1);  
          else if (key == "top")
            md5 = (*iter).substr(pos+1, (*iter).length()-pos-1);  
        }
      }
    }
  }

//  if ( !CheckMD5(source, md5) )
//   return false;
  if (high_id_vector.size() != low_id_vector.size())
    return false;

  return true;
}

bool DataProviderController::GetDataFromPhotoListRequestBody(const string &_post_body, string &album_id, string &owner_id ) {
  if (_post_body == "")
    return false;

  std::string source , md5;
  vector<string> data_vector;
  const char *data = _post_body.c_str();
  boost::split(data_vector, data, boost::is_any_of("&"), boost::token_compress_on);
  if ( data_vector.size() > 0) {
    for (vector<string>::const_iterator iter=data_vector.begin(); iter!=data_vector.end(); ++iter) {
      if ((*iter).length() > 0) {
        size_t pos = (*iter).find_first_of('=');
        if (pos!=string::npos && (*iter).substr(pos+1).length()>0) {
          string key = (*iter).substr(0, pos);
          if (key=="albumid" )
            album_id = (*iter).substr(pos+1, (*iter).length()-pos-1) ;
          else if (key=="ownerid")
            owner_id = (*iter).substr(pos+1, (*iter).length()-pos-1) ;
          else if (key == "bottom")
            source = (*iter).substr(pos+1, (*iter).length()-pos-1);  
          else if (key == "top")
            md5 = (*iter).substr(pos+1, (*iter).length()-pos-1);  
        }
      }
    }
  }
  if (album_id == "")
    return "";
//  if ( !CheckMD5(source, md5) )
//   return false;

  return true;
}

bool DataProviderController::CheckMD5(string uid, const string & key) {
  uid += kMd5Key;
  unsigned char* md5 = MD5((unsigned char*) uid.c_str(), uid.size(), NULL);
    
  stringstream res;
  for (size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
    res << hex << setw(2) << setfill('0') << (unsigned int) md5[i];
  }
    
  MCE_DEBUG("ClubDispatchKey md5 : " << res.str());
  MCE_DEBUG("ClubDispatchKey key : " << key);
      
  return res.str() == key;
}
