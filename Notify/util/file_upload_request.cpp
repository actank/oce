#include "file_upload_request.h"

// #include <boost/lexical_cast.hpp>
// #include <boost/algorithm/string.hpp>

#include "LogWrapper.h"

using namespace xce::notify;

string FileUploadRequest::GetLocalFilename() {
  return "uploaded_file";
}

// 读出一个command line, 返回值指向'\r\n' 的'\n'字符
const char * GetLineEnd(const char * buf, size_t len) {
  const char * p = buf + 1; // 首字符肯定不是'\n'

  for(;;) {
    p = (const char *)memchr(p, '\n', len - (p - buf));
    if (!p) {
      break;
    }
    if(*(p - 1) == '\r') {
      break;
    }
    ++ p; // p 指向 '\n' 的下一个字符
  }

  return p;
}

int FindNextPart(const char * str, int content_len, const string & boundary) {
  if (content_len < boundary.size() + 4) {
    return -1;
  }
  const char * p = str;
  if (p[0] != '-' || p[1] != '-') {
    return -2;
  }
  p += 2;
  if (strncmp(p, boundary.c_str(), boundary.size()) != 0) {
    return -3;
  }
  p += boundary.size();
  if (p[0] == '-' && p[1] == '-') {
    // 最后一个part
    return 0;
  }
  if (p[0] != '\r' || p[1] != '\n') {
    return -1;
  }
  p += 2;
  const char * q;
  while((q = GetLineEnd(p, content_len - (p - str)))) {
    if (q - p == 2) {
      return q - str + 1;
    }
    p = q;
  }

  return -1;
}

bool FileUploadRequest::response() {
  if(!_fcgi_out)
  {
    MCE_WARN("null fcgi out stream : " << _query << '-' << _cookie_string);
    return false;
  }

  string content_type = GetParamFix("CONTENT_TYPE");

  stringstream ss;
  ss << "Content-type: text/html\r\n\r\n"  
     << "Request_uri: << " << GetParamFix("REQUEST_URI") << "<br/>"
     << "Content_type: << " << content_type << "<br/>";  

  // get boundary
  string pattern = "boundary=";
  string boundary;
  size_t pos = content_type.find(pattern);
  if (pos != string::npos) {
    boundary = content_type.substr(pos + pattern.size());
  }

  string length = GetParamFix("CONTENT_LENGTH");  
  int content_len = strtol(length.c_str(), NULL, 10);  
  ss << "Content_length: " << content_len << "<br/>";

  if (content_len <= 0) {
    ss << "错误 : CONTENT_LENGTH = " << content_len;
  } else if (GetLocalFilename().empty()) {
    ss << "错误 : GetLocalFilename() 返回文件名无效";
  } else {
    ofstream local_file(GetLocalFilename().c_str());
    const string & body = _props["post_body"];
    if (body.empty()) {
      ss << "错误 : 上传内容为空";
    } else if (!local_file) {
      ss << "错误 : 创建文件" << GetLocalFilename() << "失败";
    } else {
      int pos = FindNextPart(body.c_str(), body.size(), boundary);

      if (body.size() < 10000) {
        MCE_DEBUG("Post Body Content start");
        MCE_DEBUG(body);
        MCE_DEBUG("Post Body Content end");
      }
      if (pos > 0) {
        // "\r\n--[boundary]--\r\n"
        local_file.write(body.c_str() + pos, body.size() - pos - boundary.size() - 8);
        ss << "文件上传成功, 保存为 : " << GetLocalFilename();
      } else {
        ss << "错误 : HTTP BODY 解析失败";
      }
    }
  }

  FCGX_PutS(ss.str().c_str(), _fcgi_out);
  return true;
}


