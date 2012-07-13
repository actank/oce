/*
 * RedirectionManager.cpp
 *
 *  Created on: 2010-5-21
 *      Author: ld
 */

#include "RedirectionManager.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include "TalkCommon.h"
#include "TalkProxyAdapter.h"
#include "TransferManagerAdapter.h"
#include "TicketAdapter.h"
#include <cmath>
#include <openssl/md5.h>

using namespace xce::notify;
using namespace passport;
using namespace com::xiaonei::talk::adapter;
using namespace com::xiaonei::talk::common;

string Md5(const string& src) {
  char md[32] = { 0 };
  unsigned char md5_buf[16];
  MD5((const unsigned char*) src.c_str(), src.size(), md5_buf);

  //定义一字符窜指针，指向要写入的地址
  char *p = md;

  for (int i = 0; i < 16; i++) {
    sprintf(p, "%02x", md5_buf[i]);
    p += 2;
  }
  //复位指针
  //    p   = md;
  //    if(len <1 || len > 32)
  //        len = 32;
  //    *(p+len)    = '\0';
  return string(md, 32);
}

void MyUtil::initialize() {
  ServiceI& service = ServiceI::instance();
  service.getAdapter()->add(&RedirectionI::instance(), service.createIdentity("M", ""));
  string fcgi_socket = RedirectionI::instance().fcgi_socket();

  FcgiServer * fcgi_server = new FcgiServer(fcgi_socket, 64);
  fcgi_server->RegisterRequestFactory(new RedirectionRequestFactory());
  fcgi_server->Start();
}

RedirectionI::RedirectionI() {
  MCE_DEBUG("RedirectionI::RedirectionI");
  ServiceI& service = ServiceI::instance();

  Ice::PropertiesPtr props = service.getCommunicator()->getProperties();
  _fcgi_socket = props->getPropertyWithDefault("Service." + service.getName()
      + ".FcgiSocket", "0.0.0.0:19005");
  MCE_DEBUG("Fcgi listens on : " << _fcgi_socket);
}

void RedirectionRequest::ReplaceAll(string& src, string replaced_part,
    string replacing_part) {
  string::size_type pos = src.find(replaced_part);
  size_t len = replaced_part.size();
  while (string::npos != pos) {
    src.replace(pos, len, replacing_part);
    pos = src.find(replaced_part, pos);
  }

}

map<string, string> RedirectionRequest::ParseQueryString(string query_string) {

  map < string, string > query_para_map;
  if (0 == query_string.size()) {
    return query_para_map;
  }

  string::size_type pos = query_string.find("&");
  while (pos != string::npos || query_string.size()) {
    string para;

    if (pos != string::npos) {
      para = query_string.substr(0, pos);
      query_string = query_string.substr(pos + 1);
    } else {
      para = query_string;
      query_string = "";
    }

    string::size_type equal_sign_pos = para.find("=");
    string key = para.substr(0, equal_sign_pos);
    string value = para.substr(equal_sign_pos + 1);

    query_para_map.insert(make_pair(key, value));

    pos = query_string.find("&");
  }

  return query_para_map;

}

bool PassportRequest::response() {
  if (!_fcgi_out) {
    MCE_WARN("get notify fcgi_out null");
    return false;
  }
  ostringstream ss;
  string emptyRet = "Content-type: text/html;charset=UTF-8\r\n\r\n";
  ss << emptyRet.c_str();

  bool ret = false;
  int http_uid = 0;
  if (_cookies.find("id") != _cookies.end()) {
    try {
      http_uid = boost::lexical_cast<int>(_cookies["id"]);
    } catch (...) {  
      MCE_WARN("lexical_cast cookie id error : " << _cookies["id"]);
      ret = false;
    }       
  }

  if (http_uid <= 0) {
    MCE_INFO("no session in cookie : " << _query << '-' << _cookie_string);
    ret = false;
  }

  if (_cookies.find("t") == _cookies.end()) {
    MCE_INFO("no ticket in cookie : " << _query << '-' << _cookie_string);
    ret = false;          
  }
    
  int uid = 0;
  try {
    uid = passport::TicketAdapter::instance().verifyTicket(_cookies["t"], "");
  } catch (Ice::Exception& e) {
    MCE_WARN("Ticket " << _cookies["t"] << " verify error : " << e); 
    ret = false;
  }
  if (uid != http_uid) {
    MCE_INFO("cookie ticket error : http_uid != uid : uid"<< uid <<"  http_uid"<<http_uid); 
    ret = false;
  } else {
    ret = true;
  }

  if (ret)
    ss << "1";
  else 
    ss << "0";

  FCGX_PutS(ss.str().c_str(), _fcgi_out);
  return true;
}

bool RedirectionRequest::response() {
  MCE_DEBUG("RedirectionRequest::response");
  string http_host = GetParamFix("HTTP_HOST");
  string request_url = GetParamFix("REQUEST_URI");

  string url = http_host + request_url;
  MCE_INFO("RedirectionRequest::response --> url:" << url);

  string::size_type pos = url.find("gadget.talk.renren.com");

  ReplaceAll(url, "xiaonei.com", "renren.com");
  MCE_DEBUG("RedirectionRequest::response --> replaced url:" << url);

  stringstream ss;

  if (string::npos == pos) {
    ss << "Location: http://" << url << "\r\n\r\n";
    MCE_INFO("RedirectionRequest::response --> NOT ren_ren, ss:" << ss.str());

    FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
    return true;
  }

  string domain = "renren.com";

  string query_string = GetParamFix("QUERY_STRING");
  MCE_DEBUG("RedirectionRequest::response --> QUERY_STRING:" << query_string);

  map < string, string > query_para_map = ParseQueryString(query_string);

  MCE_DEBUG("RedirectionRequest::response --> query_para_map:");
  for (map<string, string>::const_iterator it = query_para_map.begin(); it
      != query_para_map.end(); it++) {
    MCE_DEBUG("RedirectionRequest::response --> " << it->first << "="
        << it->second);
  }

  string prop_host = getProperty("h");
  string prop_ticket = getProperty("t");
  string prop_url = getProperty("u");
  string prop_url_encoded;

  if (query_para_map.count("u")) {
    prop_url_encoded = query_para_map.find("u")->second;
  }
  MCE_DEBUG("RedirectionRequest::response --> prop_url:" << prop_url);
  MCE_DEBUG("RedirectionRequest::response --> prop_url_encoded:"
      << prop_url_encoded);

  ReplaceAll(prop_url, "xiaonei.com", "renren.com");
  ReplaceAll(prop_url_encoded, "xiaonei.com", "renren.com");

  MCE_DEBUG("RedirectionRequest::response --> replaced prop_url:" << prop_url);
  MCE_DEBUG("RedirectionRequest::response --> replaced prop_url_encoded:"
      << prop_url_encoded);

  try {
    std::vector < std::string > vec;
    boost::split(vec, prop_ticket, boost::is_any_of("$"),
        boost::token_compress_on);

    for (vector<string>::const_iterator it = vec.begin(); it != vec.end(); it++) {
      MCE_DEBUG("RedirectionRequest::response --> ticket[" << "]=" << *it);
    }
    if (vec.size() != 5) {
      MCE_WARN("RedirectionRequest::response --> ticket size != 5 ");
      return false;
    }

    if (prop_host != vec[0]) {
      MCE_INFO("RedirectionRequest::response --> prop_host != vec[0]");

      ss << "Location: http://www." << domain << "\r\n\r\n";
      cout << "ss:" << ss.str() << endl;
      FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());

      MCE_INFO(
          "RedirectionRequest::response --> prop_host != vec[0] redirect :http://www.renren.com");
      return false;
    }

    int user_id = 0;
    string endpoint = vec[1];
    long index = 0;

    try {
      user_id = boost::lexical_cast<int>(vec[0]);
      index = boost::lexical_cast<long>(vec[2]);
    } catch (boost::bad_lexical_cast & e) {
      MCE_WARN("RedirectionRequest::response --> bad_lexical_cast : "
          << e.what());
      return false;
    }

    JidPtr jp = new Jid;
    jp->userId = user_id;
    jp->endpoint = endpoint;
    jp->index = index;

    bool flag = TalkProxyAdapter::instance().verify(jp, prop_ticket);
    MCE_INFO("RedirectionRequest::response --> TalkProxyAdapter.verify :"
        << flag << " prop_ticket:" << prop_ticket);

    string t = "";
    int count = 0;
    if (flag) {
      while (!t.size() && count++ < 3) {
        t = TransferManagerAdapter::instance().createTicket(user_id, 60 * 10);
        MCE_DEBUG("RedirectionRequest::response --> user_id:" << user_id);
        int rt = TransferManagerAdapter::instance().verifyTicket(t);
        MCE_DEBUG(
            "RedirectionRequest::response --> TransferManagerAdapter createTicket t="
                << t << " u:" << rt);
      }
    }

    string goto_url = "http://goto." + domain + "/share_redirect?url=";

    string goto_url_encoded =
        "http%3A//goto.renren.com/share_redirect%3Furl%3D";

    if (0 == prop_url.find(goto_url)) {
      prop_url = prop_url.substr(goto_url.size());
      prop_url_encoded = prop_url_encoded.substr(goto_url_encoded.size());

      MCE_DEBUG("RedirectionRequest::response --> para_url after goto:"
          << prop_url);
      MCE_DEBUG("RedirectionRequest::response --> para_url_encoded after goto:"
          << prop_url_encoded);

      prop_url = UriDecode(prop_url);

      MCE_DEBUG("RedirectionRequest::response --> UriDecode prop_url:"
          << prop_url);

      string http_str = "http://";
      if (0 == prop_url.find(http_str)) {
        prop_url = prop_url.substr(http_str.size());
        MCE_DEBUG(
            "RedirectionRequest::response --> UriDecode prop_url after http://  :  "
                << prop_url);
      }

      if (!(prop_url.find(".renren.com") != string::npos || prop_url.find(
          ".kaixin.com") != string::npos || prop_url.find(".xiaonei.com")
          != string::npos)) {

        ss << "Location: http://" << prop_url << "\r\n\r\n";
        MCE_INFO(
            "RedirectionRequest::response --> not renren kaixin xiaonei  redirect:"
                << ss.str());
        FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
        return true;

      }
    }

    if (!t.size()) {
      ss << "Location: http://" << prop_url << "\r\n\r\n";
      MCE_INFO("RedirectionRequest::response --> t.size()==0 redirect:"
          << ss.str());
      FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
      return true;
    }

    ss << "Location: http://passport.renren.com/transfer.do?transfer=" << t
        << "&origURL=" << prop_url_encoded << "\r\n\r\n[]";

    MCE_INFO("RedirectionRequest::response --> t.size()!=0 redirect:"
        << ss.str());
    FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
    return true;

  } catch (exception& e) {

    MCE_INFO("RedirectionRequest::response --> exception:" << e.what());
    ss << "Location: http://www." << domain << "\r\n\r\n";

    MCE_INFO("RedirectionRequest::response --> exception redirect:" << ss.str());
    FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
    return false;
  }
}

bool SafeRedirectionRequest::response() {
  MCE_DEBUG("RedirectionRequest::response");
  string http_host = GetParamFix("HTTP_HOST");
  string request_url = GetParamFix("REQUEST_URI");

  string url = http_host + request_url;
  MCE_INFO("RedirectionRequest::response --> url:" << url);

  string::size_type pos = url.find("gadget.talk.renren.com");

  ReplaceAll(url, "xiaonei.com", "renren.com");
  MCE_DEBUG("RedirectionRequest::response --> replaced url:" << url);

  stringstream ss;

  if (string::npos == pos) {
    ss << "Location: http://" << url << "\r\n\r\n";
    MCE_INFO("RedirectionRequest::response --> NOT ren_ren, ss:" << ss.str());

    FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
    return true;
  }

  string domain = "renren.com";

  string query_string = GetParamFix("QUERY_STRING");
  MCE_INFO("RedirectionRequest::response --> QUERY_STRING:" << query_string);

  map < string, string > query_para_map = ParseQueryString(query_string);

  MCE_DEBUG("RedirectionRequest::response --> query_para_map:");
  for (map<string, string>::const_iterator it = query_para_map.begin(); it
      != query_para_map.end(); it++) {
    MCE_DEBUG("RedirectionRequest::response --> " << it->first << "="
        << it->second);
  }

  //    string prop_host = getProperty("h");
  string prop_ticket = getProperty("t");
  string prop_url = getProperty("u");
  string prop_timestamp = getProperty("i");
  string prop_md5 = getProperty("m");
   //MCE_INFO("prop_url:" << prop_url << ", now:" << time(NULL) << " timestamp:" << prop_timestamp);
  //check time
  int second = (int) time(NULL) - boost::lexical_cast<int>(prop_timestamp);
  if (abs(second) > 60 * 5) {
    ss << "Location: " << prop_url << "\r\n\r\n";
    MCE_INFO("RedirectionRequest::response --> timeout  redirect:" << ss.str());
    FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
    return true;
  }

  string prop_url_encoded;

  if (query_para_map.count("u")) {
    prop_url_encoded = query_para_map.find("u")->second;
  }
  MCE_DEBUG("RedirectionRequest::response --> prop_url:" << prop_url);
  MCE_DEBUG("RedirectionRequest::response --> prop_url_encoded:"
      << prop_url_encoded);

  ReplaceAll(prop_url, "xiaonei.com", "renren.com");
  ReplaceAll(prop_url_encoded, "xiaonei.com", "renren.com");

  MCE_DEBUG("RedirectionRequest::response --> replaced prop_url:" << prop_url);
  MCE_DEBUG("RedirectionRequest::response --> replaced prop_url_encoded:"
      << prop_url_encoded);

  try {
    std::vector < std::string > vec;
    boost::split(vec, prop_ticket, boost::is_any_of("$"),
        boost::token_compress_on);

    for (vector<string>::const_iterator it = vec.begin(); it != vec.end(); it++) {
      MCE_DEBUG("RedirectionRequest::response --> ticket[" << "]=" << *it);
    }
    if (vec.size() != 3) {

      ss << "Location: " << prop_url << "\r\n\r\n";
      MCE_WARN("RedirectionRequest::response --> ticket size != 3, redirect:" << ss.str());
      FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
      return false;
    }

    //      if (prop_host != vec[0]) {
    //        MCE_INFO("RedirectionRequest::response --> prop_host != vec[0]");
    //
    //        ss << "Location: http://www." << domain << "\r\n\r\n";
    //        cout << "ss:" << ss.str() << endl;
    //        FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
    //
    //        MCE_INFO("RedirectionRequest::response --> prop_host != vec[0] redirect :http://www.renren.com");
    //        return false;
    //      }


    int user_id = 0;
    string endpoint = vec[1];
    long index = 0;

    try {
      user_id = boost::lexical_cast<int>(vec[0]);
      index = boost::lexical_cast<Ice::Long>(vec[2]);
    } catch (boost::bad_lexical_cast & e) {
      MCE_WARN("RedirectionRequest::response --> bad_lexical_cast : "
          << e.what());
      ss << "Location: " << prop_url << "\r\n\r\n";
      MCE_WARN("RedirectionRequest::response --> cast uid or index err, redirect:" << ss.str());
      FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
      return false;
    }

    JidPtr jp = new Jid;
    jp->userId = user_id;
    jp->endpoint = endpoint;
    jp->index = index;
    MCE_DEBUG("jid:" << jp->userId << "," << jp->endpoint << "," << jp->index);
    string ticket = TalkProxyAdapter::instance().GetTicket(jp);
    MCE_INFO( "RedirectionRequest::response --> TalkProxyAdapter.GetTicket ticket:" << ticket << " prop_ticket:" << prop_ticket);

    //========检查http 参数 uid 和 TalkProxy 返回的uid是否一致======
    bool uidcheckok = true;
    std::vector<string> proxy_vec;
    boost::split(proxy_vec, ticket, boost::is_any_of("$"), boost::token_compress_on);
    if(!proxy_vec.empty()){
      if(proxy_vec[0] != vec[0]){
        uidcheckok = false;
        MCE_INFO("RedirectionRequest::response--> uidcheck not ok talkproxy uid:" << proxy_vec[0] << " http para uid:" << vec[0]);
      }
    }
    //========检查http 参数 uid 和 TalkProxy 返回的uid是否一致======
    ostringstream os;
    os << ticket << "#" << prop_timestamp;
    if (prop_md5 != Md5(os.str()) || !uidcheckok) {
      ss << "Location: " << prop_url << "\r\n\r\n";
      MCE_INFO("RedirectionRequest::response --> md5 not eq,  redirect:" << ss.str());
      FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
      return true;
    }

    string t = "";
    int count = 0;
   // if (flag) {
      while (!t.size() && count++ < 3) {
        t = TransferManagerAdapter::instance().createTicket(user_id, 60 * 10);
        MCE_DEBUG("RedirectionRequest::response --> user_id:" << user_id);
        int rt = TransferManagerAdapter::instance().verifyTicket(t);
        MCE_DEBUG( "RedirectionRequest::response --> TransferManagerAdapter createTicket t=" << t << " u:" << rt);
      }
    //}

    string goto_url = "http://goto." + domain + "/share_redirect?url=";

    string goto_url_encoded = "http%3A//goto.renren.com/share_redirect%3Furl%3D";

    if (0 == prop_url.find(goto_url)) {
      prop_url = prop_url.substr(goto_url.size());
      prop_url_encoded = prop_url_encoded.substr(goto_url_encoded.size());

      MCE_DEBUG("RedirectionRequest::response --> para_url after goto:"
          << prop_url);
      MCE_DEBUG("RedirectionRequest::response --> para_url_encoded after goto:"
          << prop_url_encoded);

      prop_url = UriDecode(prop_url);

      MCE_DEBUG("RedirectionRequest::response --> UriDecode prop_url:"
          << prop_url);

      string http_str = "http://";
      if (0 == prop_url.find(http_str)) {
        prop_url = prop_url.substr(http_str.size());
        MCE_DEBUG(
            "RedirectionRequest::response --> UriDecode prop_url after http://  :  "
                << prop_url);
      }

      if (!(prop_url.find(".renren.com") != string::npos || prop_url.find(
          ".kaixin.com") != string::npos || prop_url.find(".xiaonei.com")
          != string::npos)) {

        ss << "Location: http://" << prop_url << "\r\n\r\n";
        MCE_INFO(
            "RedirectionRequest::response --> not renren kaixin xiaonei  redirect:"
                << ss.str());
        FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
        return true;

      }
    }

    if (!t.size()) {
      ss << "Location: http://" << prop_url << "\r\n\r\n";
      MCE_INFO("RedirectionRequest::response --> t.size()==0 redirect:"
          << ss.str());
      FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
      return true;
    }

    ss << "Location: http://passport.renren.com/transfer.do?transfer=" << t
        << "&origURL=" << prop_url_encoded << "\r\n\r\n[]";

    MCE_INFO("RedirectionRequest::response --> t.size()!=0 redirect:"
        << ss.str());
    FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
    return true;

  } catch (exception& e) {

    MCE_INFO("RedirectionRequest::response --> exception:" << e.what());
//    ss << "Location: http://www." << domain << "\r\n\r\n";
    ss << "Location: " << prop_url << "\r\n\r\n";
    MCE_INFO("RedirectionRequest::response --> exception redirect:" << ss.str());
    FCGX_FPrintF(_fcgi_out, "%s", ss.str().c_str());
    return false;
  }
}

