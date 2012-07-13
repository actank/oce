#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "fcgi_request.h"

#include "LogWrapper.h"
#include "passport.h"

using namespace xce::notify;

const char HEX2DEC[256] = {
/*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
/* 0 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 1 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 2 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 3 */0, 1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1,

/* 4 */-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 5 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 6 */-1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 7 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

/* 8 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* 9 */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* A */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* B */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,

/* C */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* D */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* E */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
/* F */-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

//将 sSrc 进行解码，例如把 &amp;&amp;++&#37;&#37 解析成  &&++%% 并返回
std::string Request::UriDecode(const std::string & sSrc) const {
  // Note from RFC1630:  "Sequences which start with a percent sign
  // but are not followed by two hexadecimal characters (0-9, A-F) are reserved
  // for future extension"

  const unsigned char * pSrc = (const unsigned char *) sSrc.c_str();
  const int SRC_LEN = sSrc.length();
  const unsigned char * const SRC_END = pSrc + SRC_LEN;
  const unsigned char * const SRC_LAST_DEC = SRC_END - 2; // last decodable '%'

  char * const pStart = new char[SRC_LEN];
  char * pEnd = pStart;

  while (pSrc < SRC_LAST_DEC) {
    if (*pSrc == '%') {
      char dec1, dec2;
      if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)]) && -1 != (dec2
          = HEX2DEC[*(pSrc + 2)])) {
        *pEnd++ = (dec1 << 4) + dec2;
        pSrc += 3;
        continue;
      }
    }

    *pEnd++ = *pSrc++;
  }

  // the last 2- chars
  while (pSrc < SRC_END)
    *pEnd++ = *pSrc++;

  std::string sResult(pStart, pEnd);
  delete[] pStart;
  return sResult;
}

bool Request::ParseKeyValue(const char * data, const char * key_seps,
    char kv_sep, std::map<std::string, std::string> & pairs) {
  if (!data)
    return false;

  std::vector<std::string> strs;
  boost::split(strs, data, boost::is_any_of(key_seps),
      boost::token_compress_on);
  for (size_t i = 0; i < strs.size(); ++i) {
    size_t pos = strs[i].find_first_of(kv_sep);
    if (pos != std::string::npos) {
      pairs[strs[i].substr(0, pos)] = UriDecode(strs[i].substr(pos + 1));
    } else {
      pairs[strs[i]] = "";
    }
  }
  return true;
}

void Request::jsonEscape(std::string& str) {
  for (std::string::iterator iter = str.begin(); iter != str.end(); ++iter) {
    if ((*iter) == '\'' || (*iter) == '"' || (*iter) == '\\' 
        || (*iter) == '/') {
      iter = str.insert(iter, '\\');
      ++iter;
    }
  }
}

std::string Request::getProperty(const std::string& key) const {
  std::map<std::string, std::string>::const_iterator it = _props.find(key);
  if (it != _props.end()) {
    return it->second;
  }
  MCE_DEBUG("Request::getProperty --> can not find key:" << key);
  return "";
}


Ice::Int Request::GetCookieInt(const std::string& key, Ice::Int dft) const {
  Ice::Int ret = dft;
  std::map<std::string, std::string>::const_iterator it = _cookies.find(key);
  if (it != _cookies.end()) {
    try {
      ret = boost::lexical_cast<Ice::Int>(it->second);
    } catch (boost::bad_lexical_cast & e) {
      MCE_WARN("bad_lexical_cast : " << e.what());
    }
  }
  return ret;
}

Ice::Int Request::getPropertyInt(const std::string& key, Ice::Int dft) const {
  Ice::Int ret = dft;
  std::map<std::string, std::string>::const_iterator it = _props.find(key);
  if (it != _props.end()) {
    try {
      ret = boost::lexical_cast<Ice::Int>(it->second);
    } catch (boost::bad_lexical_cast & e) {
      MCE_WARN("bad_lexical_cast : " << e.what());
    }
  }
  return ret;
}

Ice::Long Request::getPropertyLong(const std::string& key, Ice::Long dft) const {
  Ice::Long ret = dft;
  std::map<std::string, std::string>::const_iterator it = _props.find(key);
  if (it != _props.end()) {
    try {
      ret = boost::lexical_cast<Ice::Long>(it->second);
    } catch (boost::bad_lexical_cast & e) {
      MCE_WARN("bad_lexical_cast : " << e.what());
    }
  }
  return ret;
}

bool Request::response() {

  if(!_fcgi_out)
  {
    MCE_WARN("null fcgi out stream : " << _query << '-' << _cookie_string);
    return false;
  }

  int http_uid = 0;
  if (_cookies.find("id") != _cookies.end()) {
    try {
      http_uid = boost::lexical_cast<int>(_cookies["id"]);
    } catch (...) {
      MCE_WARN("lexical_cast cookie id error : " << _cookies["id"]);
    }
  }

//if (http_uid <= 0) {
//  if (_cookies.find("kl") != _cookies.end()) {
//    try {
//      http_uid = boost::lexical_cast<int>(_cookies["kl"].c_str() + 3);
//    } catch (...) {
//      MCE_WARN("lexical_cast cookie kl error : " << _cookies["kl"]);
//    }
//  }
//}

  if (http_uid <= 0) {
    MCE_INFO("no session in cookie : " << _query << '-' << _cookie_string);
    return false;
  }

  if (_cookies.find("t") == _cookies.end()) {
    MCE_INFO("no ticket in cookie : " << _query << '-' << _cookie_string);
    return false;
  }

#ifdef DEBUG
  _login_uid = http_uid;
  return true;
#endif

  int ticket_uid = TicketPassport::instance().Verify(_cookies["t"]);
  if (http_uid != ticket_uid) {
    MCE_INFO("cookie ticket error : http_uid != ticket_uid : "
        << http_uid << '-' << ticket_uid << '-'
        << _query << '-' << _cookie_string);
    return false;
  }

  _login_uid = http_uid;
  MCE_INFO("verify ok. uid=" << _login_uid);
  return true;
}

string Request::GetParamFix(const std::string & name) const {
  std::map<std::string, std::string>::const_iterator i = param_map_.find(name);
  if (i != param_map_.end()) {
    return i->second;
  }
  return std::string();
}

string Request::GetRemoteAddr() const {
  string addr = GetParamFix("REMOTE_ADDR");
  string forwarded_for = GetParamFix("HTTP_X_FORWARDED_FOR");
  while (!forwarded_for.empty()){
    size_t pos = forwarded_for.find_first_of(',');
    if (forwarded_for.substr(0, pos) != "unknown") {
      addr = forwarded_for.substr(0, pos);
      break;
    } else {
      MCE_INFO("unknown forwarded-for : " << forwarded_for)
    }
    if (pos == string::npos) {
      break;
    }
    if (pos + 2 <= forwarded_for.size()) {
      forwarded_for = forwarded_for.substr(pos + 2);
    }
  }

  return addr;
}

void Request::InitParamMap(FCGX_Request* req) {
  for(int i = 0; req->envp[i]; ++i) {
    std::string s = req->envp[i];
    size_t pos = s.find_first_of('=');
    if (pos > 0 && pos < s.size() - 1) {
      param_map_.insert(make_pair(s.substr(0,pos), s.substr(pos+1)));
    }
  }
}

void Request::Init(FCGX_Request* r) {
  InitParamMap(r); 

  _path = GetParamFix("SCRIPT_NAME");

  //parse query_string
  _query = GetParamFix("QUERY_STRING");
  if (!_query.empty()) {
    ParseKeyValue(_query.c_str(), "&", '=', _props);
  }

  //parse cookie
  _cookie_string = GetParamFix("HTTP_COOKIE");
  if (!_cookie_string.empty()) {
    ParseKeyValue(_cookie_string.c_str(), "; ", '=', _cookies);
  }

  //parse post body
  int content_len = 0;
  try {
    content_len = boost::lexical_cast<Ice::Int>(GetParamFix("CONTENT_LENGTH"));
  } catch (boost::bad_lexical_cast & e) {}

  if (content_len > 0) {
    char * str = new char[content_len + 1];
    int count = FCGX_GetStr(str, content_len, r->in);
    str[content_len] = 0;
    if (count > 0) {
      //_props["post_body"] = UriDecode(str);
      _props["post_body"] = string(str, count);
      _post_body = string(str, count);
      MCE_DEBUG("post body : " << count << " ," << str);
    }
    delete [] str;
  }
}

FcgiTask::FcgiTask(const RequestFactoryPtr & factory)
  : _factory(factory) {
}

void FcgiTask::handle(){
  while(FCGX_Accept_r(&_request) >= 0) {
    RequestPtr req = _factory->Create(&_request);
    if (req) {
      req->response();
    } else {
      MCE_WARN("unhandled request.");
    }
    FCGX_Finish_r(&_request);
  }
}

FcgiThread::FcgiThread(RequestFactoryPtr factory, int sock_fd) 
  : factory_(factory)
  , sock_fd_(sock_fd) {
}


void FcgiThread::run() {
  FCGX_Request request;

  if (FCGX_InitRequest(&request, sock_fd_, 0) != 0) {
    MCE_WARN("FCGX_InitRequest error. fd=" << sock_fd_);
    return;
  }

  while(FCGX_Accept_r(&request) >= 0) {
    RequestPtr req = factory_->Create(&request);
    if (req) {
      try{
        req->response();
      }catch(Ice::Exception& e){
        MCE_WARN("FcgiThread::run err:" << e << ",File:"<<__FILE__ << ",Line:" << __LINE__);
      }catch(std::exception& e){
        MCE_WARN("FcgiThread::run err:" << e.what() << ",File:"<<__FILE__ << ",Line:" << __LINE__);
      }catch(...){
        MCE_WARN("FcgiThread::run err File:"<<__FILE__ << ",Line:" << __LINE__);
      }
    } else {
      MCE_WARN("unhandled request.");
    }

    FCGX_Finish_r(&request); // FCGX_Accept_r里面会调用该函数
  } 

  MCE_WARN("FCGX_Accept_r error.");
} 

FcgiServer::FcgiServer(const std::string & sock, int thread_count, int stack_size) 
  : socket_(sock)
  , thread_count_(thread_count)
  , stack_size_(stack_size) {
}

bool FcgiServer::Start() {
  if (FCGX_Init() != 0) {
    MCE_WARN("FCGX_Init err");
    return false;
  }

  int sock_fd = FCGX_OpenSocket(socket_.c_str(), 100);
  if (sock_fd < 0){
    MCE_WARN("FCGX_OpenSocket err");
    return false;
  }

  for(int i = 0; i< thread_count_; ++i) {
    IceUtil::ThreadPtr t = new FcgiThread(this, sock_fd);
    IceUtil::ThreadControl tc = t->start(stack_size_); // 指定stack size, 默认8M有点大
    tc.detach();
    threads_.push_back(t);
  }
  return true;
}

RequestPtr FcgiServer::Create(FCGX_Request* r) {
  for(size_t i = 0; i < request_factories_.size(); ++i) {
    RequestPtr p = request_factories_[i]->Create(r);
    if (p) {
      return p;
    }
  }

  return NULL;
}

int FcgiServer::RegisterRequestFactory(RequestFactoryPtr factory) {
  if (factory) {
    request_factories_.push_back(factory);
    return 0;
  }
  return -1;
}

