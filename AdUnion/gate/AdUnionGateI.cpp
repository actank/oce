#include "AdUnionGateI.h"

#include <cstring>
#include <openssl/md5.h>

#include "ad_logger.h"
#include "ad_cache.h"
#include "url_base64.h"
#include "aes_wrapper.h"

using namespace std;
using namespace xce::ad;

extern const char * g_aes_key;
extern size_t g_aes_key_len;

class GateRequestFactory : public RequestFactory {
public:
  virtual RequestPtr Create(FCGX_Request * r) {
    char * path = FCGX_GetParam("SCRIPT_NAME", r->envp);
    if(path)
    {
      if(strcmp(path, "/union/get.html") == 0) {
        return new GetRequest(r);
      }
      if(strcmp(path, "/union/redirect.html") == 0
          || strcmp(path, "/union/click.html") == 0) {
        return new RedirectRequest(r);
      }
      if(strcmp(path, "/union/nuomi-set") == 0) {
        return new NuomiSetRequest(r);
      }
    }
    return NULL;
  }
};

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&AdUnionGateI::instance(), service.createIdentity("M", ""));

  if (!UnionAdCache::instance().Reload()) {
    MCE_WARN("UnionAdCache load err.");
    return;
  }

  InitAdLogger("ad", "../log/" + ServiceI::instance().getName() + "/ad_log", "INFO");
  InitAdUnionPvLogger("ad_union_pv", "../log/" + ServiceI::instance().getName() + "/ad_pv_log", "INFO");

  TaskManager::instance().scheduleRepeated(new AdLogTimer());

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  string fcgi_socket = props->getPropertyWithDefault("Service." + service.getName() + ".FcgiSocket", "0.0.0.0:9001");
  MCE_DEBUG("Fcgi listens on : " << fcgi_socket);

  // fcgi_server memory managed as RequestFactory ...
  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 512);
  fcgi_server->RegisterRequestFactory(new GateRequestFactory());
  fcgi_server->Start();
}

AdUnionGateI::AdUnionGateI() {
  srand(time(NULL));
}

AdUnionGateI::~AdUnionGateI() {
}

bool AdUnionGateI::Reload(const Ice::Current&) {
  MCE_INFO("AdUnionGateI::Reload");
  UnionAdCache::instance().Reload();
  return true;
}

bool AdUnionGateI::ReloadById(Ice::Int id, const Ice::Current&) {
  return false;
}

bool GetRequest::response() {
  //
  // /union/get.html
  //
  if(!_fcgi_out) {
    MCE_WARN("fcgi_out null");
    return false;
  }

  //std::map<std::string, std::string>::const_iterator ite = param_map_.begin();
  //for(; ite != param_map_.end(); ite ++) {
  //  MCE_DEBUG(ite->first << " = " << ite->second);
  //}
  MCE_DEBUG("Accept Charset: " << GetParamFix("HTTP_ACCEPT_CHARSET"));

  Ice::Int ad_type = getPropertyInt("t", 0);
  Ice::Int tpl_type = getPropertyInt("c", 3);
  MCE_DEBUG("get tpl_type : " << tpl_type);

  Ice::Int slot = -1;
  try {
    slot = boost::lexical_cast<int>(cookie("slot"));
  } catch (boost::bad_lexical_cast & e) {}
  if (slot < 0) {
    slot = rand();
  }
  slot %= 10000;

  int ad_count = getPropertyInt("ac", 1);

  stringstream ss;
  ss << "Content-type: text/html; charset=utf-8\r\nSet-Cookie: slot=" << slot + ad_count << "; path=/union\r\n\r\n";
  FCGX_PutS(ss.str().c_str(), _fcgi_out);

  string appendix;
  if(!getProperty("m").empty()) {
    appendix += "&m=" + getProperty("m");
  }
  if(getPropertyInt("s") >= 0) {
    appendix += "&s=" + getProperty("s");
  }
  if(getPropertyInt("z") >= 0) {
    appendix += "&z=" + getProperty("z");
  }

  string html;
  UnionAdCache::instance().GetAd(ad_type, tpl_type, slot, ad_count, appendix, &html, 
      getPropertyInt("z", 0), getPropertyInt("s", 0), GetRemoteAddr(), GetParamFix("HTTP_REFERER"));
  FCGX_PutS(html.c_str(), _fcgi_out);
  return true;
}

bool RedirectRequest::IsCheat() {
  return false;
}

std::string GetProperty(const std::map<std::string, std::string> & m, const std::string& key) {
  std::map<std::string, std::string>::const_iterator it = m.find(key);
  if (it != m.end()) {
    return it->second;
  }
  MCE_DEBUG("Request::getProperty --> can not find key:" << key);
  return "";
}

Ice::Int GetPropertyInt(const std::map<std::string, std::string> & m, const std::string& key, Ice::Int dft = -1) {
  Ice::Int ret = dft;  
  std::map<std::string, std::string>::const_iterator it = m.find(key);
  if (it != m.end()) {
    try {
      ret = boost::lexical_cast<Ice::Int>(it->second);
    } catch (boost::bad_lexical_cast & e) {
      MCE_WARN("bad_lexical_cast : " << e.what());
    }
  }
  return ret;
}

Ice::Long GetPropertyLong(const std::map<std::string, std::string> & m, const std::string& key, Ice::Long dft = -1) {
  Ice::Long ret = dft;
  std::map<std::string, std::string>::const_iterator it = m.find(key);
  if (it != m.end()) {
    try {
      ret = boost::lexical_cast<Ice::Long>(it->second);
    } catch (boost::bad_lexical_cast & e) {
      MCE_WARN("bad_lexical_cast : " << e.what());
    }
  }
  return ret;
}

bool RedirectRequest::response() {
  //
  // /union/redirect.html
  //
  if(!_fcgi_out) {
    MCE_WARN("fcgi_out null");
    return false;
  }

  string cipher_url = getProperty("c");
  string cipher = url_base64_decode(cipher_url);
  AesWrapper aw((const unsigned char *)g_aes_key, g_aes_key_len);
  int sz = cipher.size();
  unsigned char * plain = aw.Decrypt((unsigned char *)cipher.c_str(), &sz);

  std::map<std::string, std::string> encrypted_props;

  if (!plain) {
    MCE_INFO("cipher NULL");
  } else {
    string plain_url((char*)plain, sz); 
    MCE_DEBUG("encrypted params : " << plain_url);
    free(plain);
    ParseKeyValue(plain_url.c_str(), "&", '=', encrypted_props);
  }

  long ad_id = GetPropertyLong(encrypted_props, "aid");
  
  if (ad_id > 0) {
    UnionAdCache::instance().ClickAd(ad_id, GetPropertyInt(encrypted_props, "c", 3), GetPropertyInt(encrypted_props, "z", 0), 
        GetPropertyInt(encrypted_props, "s", 0), GetRemoteAddr(), IsCheat());
  } else {
    MCE_WARN("error ad id : " << GetProperty(encrypted_props, "aid"));
  }
  string ss;
  
  string dest_url = getProperty("url");
  if (dest_url.find("nuomi.com") != string::npos) {
    MCE_DEBUG("nuomi clicked " << dest_url);
    string site_id = GetProperty(encrypted_props, "s");

    if (!site_id.empty()) {
      if (dest_url.find('?') != string::npos) {
        dest_url += "&s=";
      } else {
        dest_url += "?s=";
      }
      dest_url += site_id;
    }
  } else if (dest_url.find("searchreg") != string::npos) {
    string site_id = GetProperty(encrypted_props, "s");

    if (!site_id.empty()) {
      if (dest_url.find('?') != string::npos) {
        dest_url += "&g=";
      } else {
        dest_url += "?g=";
      }
      dest_url += site_id;
    }
  }

  MCE_DEBUG("dest url : " << dest_url);

  ss = "Location: " + dest_url + "\r\n\r\n";
  MCE_INFO("RedirectRequest::response : " << ss);

  FCGX_PutS(ss.c_str(), _fcgi_out);
  return true;
}

bool NuomiSetRequest::CheckUrl() {
  //m=md5("id+t+h+d+p+u+!$Bt45%8mP")
  if (getProperty("id").empty() || getProperty("h").empty() || getProperty("u").empty()) {
    return false;
  }

  string src = getProperty("id");
  src += getProperty("t");
  src += getProperty("h");
  src += getProperty("d");
  src += getProperty("p");
  src += getProperty("u");
  src += "!$Bt45%8mP";

  unsigned char* md5 = MD5((unsigned char*)src.c_str(), src.size(), NULL);

  stringstream res;
  for(size_t i = 0; i < MD5_DIGEST_LENGTH; ++i) {
    res << hex << setw(2) << setfill('0') << (unsigned int)md5[i];
  }

  string url_md5 = getProperty("m");
  MCE_DEBUG("res : md5[" << src << "]=" << res.str() << " vs. " << url_md5);
  return res.str() == url_md5;
}

bool NuomiSetRequest::response() {
  //
  // /union/nuomi-set
  //
  if(!_fcgi_out) {
    MCE_WARN("fcgi_out null");
    return false;
  }

  stringstream ss;
  ss << "Content-type: text/html; charset=utf-8\r\n\r\n";

  if (!CheckUrl()) {
    MCE_INFO("/union/nuomi-set url error");
    ss << "-1";
    FCGX_FPrintF(_fcgi_out, ss.str().c_str());
    return false;
  }
  Ice::Long goods_id = getPropertyLong("id"); 
  MCE_DEBUG("goods_id : " << goods_id);

  Ice::Int tpl_type = getPropertyInt("t"); 
  MCE_DEBUG("tpl_type : " << tpl_type);
  MCE_DEBUG("tpl_type : " << getProperty("t"));

  string title = getProperty("h"); 
  string content = getProperty("d"); 
  string img_url = getProperty("p"); 
  string dest_url = getProperty("u"); 
  UnionAdCache::instance().UpdateNuomiAd(goods_id, tpl_type, title, content, img_url, dest_url);

  ss << "0";
  FCGX_FPrintF(_fcgi_out, ss.str().c_str());
  return true;
}

