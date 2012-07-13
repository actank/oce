#include <ClearSilver/cgi/cgi.h>
#include "clear_silver_wrap.h"

#include "LogWrapper.h"
#include "Notify/util/tpl_modifiers.h"

using namespace std;
using namespace xce::notify;

static NEOERR * xml_escape_strfunc(const char *src, char **out)
{
  NEOERR *err = STATUS_OK;
  int slen = strlen(src);
  STRING out_s;
  int x;
  char *ptr;

  string_init(&out_s);
  err = string_append (&out_s, "");
  if (err) return nerr_pass (err);
  *out = NULL;

  x = 0;
  while (x < slen)
  {
    ptr = strpbrk(src + x, "&<>\"'");
    if (ptr == NULL || (ptr-src >= slen))
    {   
      err = string_appendn (&out_s, src + x, slen-x);
      x = slen;
    }   
    else
    {   
      err = string_appendn (&out_s, src + x, (ptr - src) - x); 
      if (err != STATUS_OK) break;
      x = ptr - src;
      if (src[x] == '&')
        err = string_append (&out_s, "&amp;");
      else if (src[x] == '<')
        err = string_append (&out_s, "&lt;");
      else if (src[x] == '>')
        err = string_append (&out_s, "&gt;");
      else if (src[x] == '"')
        err = string_append (&out_s, "&quot;");
      else if (src[x] == '\'')
        err = string_append (&out_s, "&apos;");
      x++;
    }
    if (err != STATUS_OK) break;
  }
  if (err)
  {
    string_clear (&out_s);
    return nerr_pass (err);
  }
  *out = out_s.buf;
  return STATUS_OK;
}

std::string CSTemplate::Render(const HdfMap & data) {
  IceUtil::Mutex::Lock lock(render_mutex_);
  if (!ClearHdf()) {
    MCE_WARN("clear hdf handle error");
    return "";
  }
  string empty;
  rendered_.swap(empty);

  HdfMap::const_iterator it = data.begin();
  NEOERR * err = NULL;
  for(; it != data.end(); ++it) {
    err = hdf_set_value(hdf_, it->first.c_str(), it->second.c_str());
  }

  err = cs_render(cs_, (char*)&rendered_, CSTemplate::AppendResult);
  if (err) {
    MCE_WARN("render error");
  }
  return rendered_;
}

CSTemplate::~CSTemplate() {
  if (cs_) {
    cs_destroy(&cs_);
  }
  if (hdf_) {
    hdf_destroy(&hdf_);
  }
} 

bool CSTemplate::LoadFromString(const char * tpl, int size, const HdfMap & init_vars, string * err_desc) {
  bool success = true;
  NEOERR * err = Init(init_vars);
  char * s = new char[size + 1];
  memcpy(s, tpl, size);
  s[size] = '\0';
  err = cs_parse_string(cs_, s, size);
  if (err) {
    STRING str;
    string_init(&str);

    nerr_error_string(err, &str);
    MCE_WARN("Load tpl error : " << str.buf);
    if (err_desc) {
      *err_desc = str.buf;
    }
    string_clear(&str);
    nerr_ignore(&err);
    success = false;
  }
  return success;
}

bool CSTemplate::LoadFromFile(const char * filename, const HdfMap & init_vars, string * err_desc) {
  NEOERR * err = Init(init_vars);
  err = cs_parse_file(cs_, filename);
  if (err) {
    nerr_log_error(err);
  }
  return err == NULL;
}

NEOERR* CSTemplate::AppendResult(void * res, char * data) {
  string & rendered = *(reinterpret_cast<string*>(res));
  rendered += neos_strip(data);
  // rendered += data;
  return NULL;
}

NEOERR* CSTemplate::RFeedTime(const char *in, char **ret) {
  long long t = atoll(in);
  t /= 1000; // 传入的time是毫秒

  if (t < 0) {
    t = 0;
  }
  int buf_len = 0x100;
  char* buf = (char*)malloc(buf_len);
  buf[buf_len - 1] = '\0';

  time_t now = time(0); 
  int diff_minutes = (now - t) / 60;
  if (diff_minutes <= 0) {
    int sec = now - t;
    if( sec <= 0 ) {
      snprintf(buf, buf_len - 1, "刚刚更新");
    }
    else {
      snprintf(buf, buf_len - 1, "%d秒钟前", sec);
    }
  }
  else if (diff_minutes < 60) {
    snprintf(buf, buf_len - 1, "%d分钟前", diff_minutes);
  }
  else {
    struct tm timeinfo;
    struct tm nowtimeinfo;
    localtime_r((const time_t *)&t, &timeinfo);
    localtime_r((const time_t *)&now, &nowtimeinfo);

    int year = nowtimeinfo.tm_year - timeinfo.tm_year;
    int day = nowtimeinfo.tm_yday - timeinfo.tm_yday;
    if(year >= 1) {
      strftime(buf, buf_len - 1, "%Y-%m-%d %H:%M", &timeinfo);
    }
    else {
      if(day < 1) {
        strftime(buf, buf_len - 1, "今天 %H:%M", &timeinfo);
      }
      else if(day == 1) {
        strftime(buf, buf_len - 1, "昨天 %H:%M", &timeinfo);
      }
      else {
        strftime(buf, buf_len - 1, "%m-%d %H:%M", &timeinfo);
      }
    }
  }

  *ret = buf;
  return STATUS_OK;
}


NEOERR * CSTemplate::MaxLength(CSPARSE *parse, CS_FUNCTION *csf, CSARG *args, CSARG *result) {
  NEOERR *err;
  char * text;
  long int max_len = 0;
  
  result->op_type = CS_TYPE_STRING;
  result->s = "";

  
  err = cs_arg_parse(parse, args, "si", &text, &max_len);
  if (err) return nerr_pass(err); 

  string res = HtmlModifyLength(string(text), max_len);

  char * out_buf = (char *)malloc(sizeof(char) * res.size() + 1);
  memcpy(out_buf, res.c_str(), res.size());
  out_buf[res.size()] = '\0';
  result->s = out_buf;
  result->alloc = 1;

  return STATUS_OK;
}

NEOERR * CSTemplate::Init(const HdfMap & init_vars) {
  NEOERR * err = hdf_init(&hdf_);
  for(HdfMap::const_iterator it = init_vars.begin(); it != init_vars.end(); ++it) {
    err = hdf_set_value(hdf_, it->first.c_str(), it->second.c_str());
  }

  if (!err) {
    err = cs_init(&cs_, hdf_);
  }
  if (!err) {
    err = cs_register_esc_strfunc(cs_, "js_escape", cgi_js_escape);
    err = cs_register_esc_strfunc(cs_, "html_escape", cgi_html_escape_strfunc);
    err = cs_register_esc_strfunc(cs_, "xml_escape", xml_escape_strfunc);
    err = cs_register_esc_strfunc(cs_, "x_rfeedtime", RFeedTime);
    err = cs_register_function(cs_, "x_maxlen", 2, MaxLength);
    // err = cs_register_function(cs_, "maxlen", 2, CSTemplate::MaxLength);
  }
  return err;
}

bool CSTemplate::ClearHdf() {
  HDF * t = hdf_obj_child(hdf_);
  while(t) {
    // hdf_dump(t, "sub tree:");
    NEOERR * err = hdf_remove_tree(hdf_, hdf_obj_name(t));
    if (err) {
      MCE_WARN("hdf_remove_tree error");
      return false;
    }
    t = hdf_obj_child(hdf_);
  }
  return true;
}

int CSTemplate::CheckTpl(const string & tpl, string * err_desc) {
  CSTemplate cstemplate;
  bool res = cstemplate.LoadFromString(tpl.c_str(), tpl.size(), HdfMap(), err_desc);
  return res ? 0 : 1;
}

CSTemplateCache::TplVector::TplVector(const string & content, const HdfMap & init_vars) 
    : index_(0)
    , tpl_content_(content) {
  for(int i = 0; i < 5; ++i) {
    CSTemplate * tpl = new CSTemplate;
    bool res = tpl->LoadFromString(content.c_str(), content.size(), init_vars);
    if (res) {
      tpl_vec_.push_back(tpl);
    } else {
      delete tpl;
    }
  }
}

CSTemplateCache::TplVector::~TplVector() {
  for(size_t i = 0; i < tpl_vec_.size(); ++i) {
    delete tpl_vec_[i];
  }
} 

CSTemplate * CSTemplateCache::TplVector::GetTpl() {
  assert(tpl_vec_.size() > 0);
  int i = ++index_;
  if (index_ > 20000) {
    index_ = 0;
  }
  return tpl_vec_[i % tpl_vec_.size()];
}

CSTemplate * CSTemplateCache::GetTemplate(const string & tpl_identifier) {
	IceUtil::RWRecMutex::RLock lock(rw_mutex_);
  MCE_INFO("-" << tpl_identifier << "- cache_ size : " << tpl_cache_.size());

  map<string, TplVector*>::iterator it = tpl_cache_.find(tpl_identifier);
  if (it != tpl_cache_.end()) {
    MCE_INFO("-" << tpl_identifier << "- tpl found!");
    return it->second->GetTpl();
  }
  MCE_INFO("-" << tpl_identifier << "- tpl not found!");
  return NULL;
}

bool CSTemplateCache::AddTemplate(const string & tpl_identifier, const string & tpl_content, const HdfMap & init_vars) {
  TplVector * tpl_vec = new TplVector(tpl_content, init_vars);
  if (tpl_vec && tpl_vec->Size() > 0) {
	  IceUtil::RWRecMutex::WLock lock(rw_mutex_);
    tpl_cache_[tpl_identifier] = tpl_vec;
    return true;
  }
  delete tpl_vec;
  return false;
}

CSTemplateCache::~CSTemplateCache() {
  map<string, TplVector*>::iterator it = tpl_cache_.begin();
  for(; it != tpl_cache_.end(); ++it) {
    delete it->second;
  }
}


